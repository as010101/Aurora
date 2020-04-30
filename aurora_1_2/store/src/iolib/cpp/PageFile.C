#include <iolib/int_include/PageFile.H>
#include <util.H>
#include <FdMultiPoolLease.H>
#include <LockHolder.H>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <math.h>
#include <sstream>

#include <iostream> // just for debugging

//=========================================================================================

void PageFile::createPageFile(string filename, 
			      size_t bytesPerPage)
      throw (std::exception)
{
  if (bytesPerPage < 1)
    {
	throw SmIllegalParameterValueException(__FILE__, __LINE__, "bytesPerPage < 1");
    }

  int fd = openFileWithFlagsAndPerms(filename,
				     O_RDWR | O_CREAT | O_EXCL, 
				     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

  // Initialize the page allocation bitmap within the file. (One byte covers 8 pages, at
  // one bit per page.)
  // The c'tor initialized the map to 'false', which is what we want in this case.
  SerializableBitSet pageAllocBitmap(getMaxDataPages(bytesPerPage));
  pageAllocBitmap.save(fd);

  // Pad the file to our first data block boundary, so that PageFile::PageFile's checks
  // and calculations are easier to do...
  off_t oldFileLen = getFileLen(fd);

  // To keep our data pages block-alligned, we start them at the first
  // block that follows our bitmap. (lseek treats 0, not 1, as the
  // first byte number in a file.)
  off_t firstDataPageOffset = off_t(getNumBitmapPages(bytesPerPage) * bytesPerPage);

  size_t bytesToAdd = firstDataPageOffset - oldFileLen;
  assert(bytesToAdd > 0);

  char * pFiller = new char[bytesToAdd];
  memset(pFiller, '.', bytesToAdd);
  writeData(fd, oldFileLen, pFiller, bytesToAdd);
  delete[] pFiller;

  close(fd);
}

//=========================================================================================

PageFile::PageFile(string filename, 
		   size_t bytesPerPage, 
		   FdMultiPool & fdPool)
  throw (std::exception)
  : _pageAllocBitmap(getMaxDataPages(bytesPerPage)),
    _filename(filename),
    _fdPool(fdPool),
    _isShutdown(false),
    _isGrowing(false),
    _bytesPerPage(bytesPerPage),
    _maxDataPages(getMaxDataPages(bytesPerPage)),
    _numBitmapPages(getNumBitmapPages(bytesPerPage)),
    _firstDataByteOffset(getNumBitmapPages(bytesPerPage) * bytesPerPage)
{
  if (bytesPerPage < 1)
    {
	throw SmIllegalParameterValueException(__FILE__, __LINE__, "bytesPerPage < 1");
    }

  _fdPool.registerFile(_filename, O_RDWR);
  
  FdMultiPoolLease fdLease(_fdPool, _filename);

  int fd = fdLease.getFd();

  // Make sure that the file is a believable size: 
  //   SM_FIRST_DATA_PAGE_OFFSET + (n * bytesPerPage) bytes, where n >= 0...
  off_t dataBytes =  getFileLen(fd) - _firstDataByteOffset;

  if (dataBytes < 0)
    {
      throw SmException(__FILE__, __LINE__, 
				  "The file wasn't even SM_FIRST_DATA_PAGE_OFFSET bytes in size!");
    }

  if ((dataBytes % _bytesPerPage) != 0)
    {
      ostringstream os;
      os << "The file size had a problem: (dataBytes % _bytesPerPage) != 0" << endl
	 << "   dataBytes     = " << dataBytes << endl
	 << "   _bytesPerPage = " << _bytesPerPage << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  _currentDataPageCapacity = (dataBytes / _bytesPerPage);

  // Load the page allocation map from disk...
  _pageAllocBitmap.load(fd);
}

//=========================================================================================

PageFile::~PageFile()
{
  if (! _isShutdown)
    {
      cerr << "PageFile::~PageFile(): ! _isShutdown" << endl;
    }
}

//=========================================================================================

void PageFile::getCapacityInfo(unsigned int & allocsWithNoGrowth,
			       bool & fileGrowthPossible)
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);

  int pagesInUse = 0;
  for (int i = 0; i < _currentDataPageCapacity; i++)
    {
      pagesInUse += int(_pageAllocBitmap.testBit(i));
    }

  allocsWithNoGrowth = _currentDataPageCapacity - pagesInUse;

  fileGrowthPossible = (_currentDataPageCapacity < getPageCountAfterNextGrowth()); 
}

//=========================================================================================

unsigned int PageFile::getCurrentDataPages()
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);
  return _currentDataPageCapacity;
}

//=========================================================================================

int PageFile::getMaxDataPages()
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);
  return _maxDataPages;
}

//=========================================================================================

int PageFile::allocPageInternal()
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);

  if (_isShutdown)
    {
      throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
    }

  int pageId = findFreePage();
  if (pageId != -1)
    {
      setPageAllocated(pageId, true);
    }

  return pageId;
}

//=========================================================================================
  
void PageFile::growFile()
  throw (std::exception,
	 SmFileFullException,
	 SmFileSystemFullException)
{
  int numNewPages;

  // Segregate this method into two different regions of time that the mutex is held, so
  // that other methods can be invoked while this one has to grow the file...
  LockHolder mtxRights(_mtx);

  // *********** IN CRITICAL SECTION *************
  if (_isGrowing)
    {
      throw SmException(__FILE__, __LINE__, "_isGrowing was already true");
    }

  // Is the file already at our self-imposed maximum size?
  if (_currentDataPageCapacity == _maxDataPages)
    {
      ostringstream msgStream;
      msgStream << "_currentDataPageCapacity == _maxDataPages:" << endl
		<< "\t_currentDataPageCapacity = " << _currentDataPageCapacity << endl
		<< "\t_maxDataPages = " << _maxDataPages << endl;
      throw SmFileFullException(__FILE__, __LINE__, msgStream.str());
    }

  // Make sure that even with a small growth factor, we at least get SOME growth. This mostly
  // just matters when the file is very small.
  int idealDataPageCount = getPageCountAfterNextGrowth();
  if (idealDataPageCount == _currentDataPageCapacity)
    {
      ostringstream msgStream;
      msgStream << "idealDataPageCount == _currentDataPageCapacity:" << endl
		<< "\tidealDataPageCount = " << idealDataPageCount << endl
		<< "\t_currentDataPageCapacity = " << _currentDataPageCapacity << endl;
      throw SmFileFullException(__FILE__, __LINE__, msgStream.str());
    }

  numNewPages = idealDataPageCount - _currentDataPageCapacity;
  _isGrowing = true;

  mtxRights.release();
  // *********** OUTSIDE CRITICAL SECTION *************

  try
    {
      FdMultiPoolLease fdLease(_fdPool, _filename);
      int fd = fdLease.getFd();

      size_t oldFileLen = getFileLen(fd);
      size_t newFileLen = oldFileLen + (numNewPages * _bytesPerPage);

      // For debugging only, to speed unit testing...
      //      setFileLen(fd, newFileLen, true);
      setFileLen(fd, newFileLen, false);
    }
  catch(...)
    {
      _isGrowing = false;
      throw;
    }

  mtxRights.reacquire();
  // *********** IN CRITICAL SECTION *************

  _isGrowing = false;
  _currentDataPageCapacity += numNewPages;
}

//=========================================================================================

void PageFile::freePage(int pageId)
  throw (std::exception,
	 SmNotAllocatedException)
{
  LockHolder mtxRights(_mtx);

  if (_isShutdown)
      {
	throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
      }

  if (! isPageAllocated(pageId))
    {
      ostringstream msgStream;
      msgStream <<  "The specified page isn't currently marked as allocated: " << pageId;
      throw SmNotAllocatedException(__FILE__, __LINE__, msgStream.str());
    }

  setPageAllocated(pageId, false);
}

//=========================================================================================

void PageFile::readPage(int pageId,
			char *pBuffer)
  throw (std::exception,
	 SmNotAllocatedException)
{
  // ********** INSIDE CRITICAL SECTION **************
  {
    LockHolder mtxRights(_mtx);

    if (pBuffer == NULL)
      {
	throw SmIllegalParameterValueException(__FILE__, __LINE__, "pBuffer == NULL");
      }

    if (_isShutdown)
      {
	throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
      }
    
    if (! isPageAllocated(pageId))
      {
	throw SmNotAllocatedException(__FILE__, __LINE__, 
						"The specified page isn't currently marked as allocated.");
      }
  }

  // ********** OUSIDE CRITICAL SECTION **************
  off_t fileOffset = _firstDataByteOffset + (pageId * _bytesPerPage);

  FdMultiPoolLease fdLease(_fdPool, _filename);
  readData(fdLease.getFd(), fileOffset, pBuffer, _bytesPerPage);
}

//=========================================================================================

void PageFile::writePage(int pageId,
			 const char *pBuffer)
  throw (std::exception,
	 SmNotAllocatedException)
{
  // ********** INSIDE CRITICAL SECTION **************
  {
    LockHolder mtxRights(_mtx);

    if (_isShutdown)
      {
	throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
      }

    if (pBuffer == NULL)
      {
	throw SmIllegalParameterValueException(__FILE__, __LINE__, "pBuffer == NULL");
      }
    
    if (! isPageAllocated(pageId))
      {
	throw SmNotAllocatedException(__FILE__, __LINE__, 
						"The specified page isn't currently marked as allocated.");
      }
  }

  // ********** OUSIDE CRITICAL SECTION **************
  off_t fileOffset = _firstDataByteOffset + (pageId * _bytesPerPage);
  FdMultiPoolLease fdLease(_fdPool, _filename);
  writeData(fdLease.getFd(), fileOffset, pBuffer, _bytesPerPage);
}    

//=========================================================================================

void PageFile::shutdown()
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);
  
  if (! _isShutdown)
    {
      FdMultiPoolLease fdLease(_fdPool, _filename);
      int fd = fdLease.getFd();

      // Commit the page allocation map to disk...
      setFileOffset(fd, 0, SEEK_SET);
      _pageAllocBitmap.save(fd);
      
      //closeFile(fd); // This gets closed when the FdMultiPool is closed.
      _isShutdown = true;
    }
}

//=========================================================================================

string PageFile::getDebugInfo()
  throw (std::exception)
{
  unsigned int allocsWithNoGrowth;
  bool fileGrowthPossible;

  getCapacityInfo(allocsWithNoGrowth, fileGrowthPossible);

  ostringstream os;
  os << "\tBytes per page:       " << _bytesPerPage << endl
     << "\t# Bitmap pages:       " << _numBitmapPages << endl
     << "\tMax # Data pages:     " << _maxDataPages << endl
     << "\tCurrent # data pages: " << _currentDataPageCapacity 
     << "\tallocsWithNoGrowth:   " << allocsWithNoGrowth
     << "\tfileGrowthPossible:   " << fileGrowthPossible
     << endl;
  return os.str();
}

//=========================================================================================

long PageFile::getMaxDataPages(long bytesPerPage)
  throw (std::exception)
{
  /*
  // Ignore any fraction of the file that can't hold a whole page...
  long long maxFileUsableBytes = (((long long) SM_MAX_FILE_BYTES) / bytesPerPage) * bytesPerPage;

  long long maxFileBits = maxFileUsableBytes * 8;
  long bitsPerPage = bytesPerPage * 8;

  // We count on integer division to provide our floor function...
  long maxDataPages = long(maxFileBits / (bitsPerPage + 1)); 

  return maxDataPages;
  */

  unsigned int bitmapPagesNeeded = 0;
  unsigned int dataPagesProposed = 0;
  bool justUppedBitmapPages = false;

  while ((bytesPerPage * (bitmapPagesNeeded + dataPagesProposed)) < SM_MAX_FILE_BYTES)
    {
      dataPagesProposed++;
      if ((bitmapPagesNeeded * bytesPerPage) < dataPagesProposed)
	{
	  bitmapPagesNeeded++;
	  justUppedBitmapPages = true;
	}
      else
	{
	  justUppedBitmapPages = false;
	}
    }

  dataPagesProposed--;
  if (justUppedBitmapPages)
    {
      bitmapPagesNeeded--;
    }

  return dataPagesProposed;
}

//=========================================================================================

long PageFile::getNumBitmapPages(long bytesPerPage)
  throw (std::exception)
{
  /*
  long long maxDataPages = getMaxDataPages(bytesPerPage);
  long bitsPerPage = bytesPerPage * 8;

  long numBitmapPages = long(ceil(((double) maxDataPages) / bitsPerPage));

  return numBitmapPages;
  */


  unsigned int bitmapPagesNeeded = 0;
  unsigned int dataPagesProposed = 0;
  bool justUppedBitmapPages = false;

  while ((bytesPerPage * (bitmapPagesNeeded + dataPagesProposed)) < SM_MAX_FILE_BYTES)
    {
      dataPagesProposed++;
      if ((bitmapPagesNeeded * bytesPerPage) < dataPagesProposed)
	{
	  bitmapPagesNeeded++;
	  justUppedBitmapPages = true;
	}
      else
	{
	  justUppedBitmapPages = false;
	}
    }

  dataPagesProposed--;
  if (justUppedBitmapPages)
    {
      bitmapPagesNeeded--;
    }

  return bitmapPagesNeeded;

}

//=========================================================================================

bool PageFile::isPageAllocated(int pageId) const
    throw (std::exception)
{
  if (_isShutdown)
    {
      throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
    }

  if ((pageId < 0) || (pageId >= _maxDataPages))
    {
      throw SmIllegalParameterValueException(__FILE__, __LINE__, 
					   "(pageId < 0) || (pageId >=  _maxDataPages)");
    }

  return _pageAllocBitmap.testBit(pageId);
}

//=========================================================================================

void PageFile::setPageAllocated(int pageId, bool isAllocated)
  throw (std::exception)
{
  if (_isShutdown)
    {
      throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
    }

  if ((pageId < 0) || (pageId >= _maxDataPages))
    {
      throw SmIllegalParameterValueException(__FILE__, __LINE__, 
					   "(pageId < 0) || (pageId >=  _maxDataPages)");
    }

  _pageAllocBitmap.setBit(pageId, isAllocated);
}

//=========================================================================================

int PageFile::findFreePage()
{
  if (_isShutdown)
    {
      throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
    }

  // If the bitmap ha NO free pages, then we can stop looking right away...
  if (_pageAllocBitmap.getBitCountForValue(false) == 0)
    {
      return -1;
    }

  int lowestFreePage = _pageAllocBitmap.getFirstIdxForValue(false);
  if (lowestFreePage < _currentDataPageCapacity)
    {
      return lowestFreePage;
    }
  else
    {
      return -1;
    }
}

//========================================================================================

/*
void PageFile::growFileByPageCount(int numNewPages)
  throw (std::exception,
	 SmFileFullException,
	 SmFileSystemFullException)
{
  if (_isShutdown)
    {
      throw SmClosedException(__FILE__, __LINE__, "PageFile.shutdown() has already been called.");
    }

}
*/

//========================================================================================

int PageFile::getPageCountAfterNextGrowth()
  throw (std::exception)
{  
  bool exponentialGrowth = true;
 
  int maxNewTotalPages;
  if (exponentialGrowth)
    {
      maxNewTotalPages = max(_currentDataPageCapacity + 100, 
			     _currentDataPageCapacity + 
			     int(_currentDataPageCapacity * SM_DATA_FILE_GROWTH_FACTOR));
    }
  else // linear growth
    {
      maxNewTotalPages = max(_currentDataPageCapacity + 100, 
			     int(_currentDataPageCapacity * SM_DATA_FILE_GROWTH_FACTOR));
    }
  
  return min(long(maxNewTotalPages), _maxDataPages);
}

//========================================================================================
