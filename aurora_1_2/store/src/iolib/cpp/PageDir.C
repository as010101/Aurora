#include <iolib/int_include/PageDir.H>
#include <LockHolder.H>
#include <util.H>
#include <iostream>


//=============================================================================

void PageDir::clear(string dirPathname)
  throw (std::exception)
{
  if (dirPathname[dirPathname.size() - 1] != '/')
    {
      dirPathname += '/';
    }

  if (! dirExists(dirPathname))
    {
      throw SmIllegalParameterValueException(__FILE__, __LINE__, "dirPathname doesn't appear to be a directory");   
    }

  // Discover and load in all of the existing page files...
  vector<string> pagefileNames;
  listFilesWithPrefix(dirPathname, "pagefile_", pagefileNames);

  // Load the in-memory representations of the PageFile files.
  unsigned int numPagefiles = pagefileNames.size();
  for (unsigned int i = 0; i < numPagefiles; i++)
    {
      string pagefilePathname = dirPathname +  pagefileNames[i];
      deleteFile(pagefilePathname);
    }
}

//===============================================================================

PageDir::PageDir(string dirpath, 
		 size_t bytesPerPage,
		 int maxConcurrentOps, 
		 BinarySem * pFreePagesBinSem)
  throw (std::exception) :
  _maxConcurrentOps(maxConcurrentOps),
  _currentOpCount(0),
  _fdPool(maxConcurrentOps),
  _storageIsGrowing(false),
  _isShutdown(false),
  _pFreePagesBinSem(pFreePagesBinSem),
  _bytePerPage(bytesPerPage)
{
  if (maxConcurrentOps < 1)
    {
      throw SmIllegalParameterValueException(__FILE__, __LINE__, "maxConcurrentOps < 1");
    }

  _dirpath = dirpath;
  if (_dirpath[_dirpath.size() - 1] != '/')
    {
      _dirpath += '/';
    }

  if (! dirExists(_dirpath))
    {
      throw SmIllegalParameterValueException(__FILE__, __LINE__, "dirpath doesn't appear to be a directory");   
    }

  // Discover and load in all of the existing page files...
  vector<string> pagefileNames;
  listFilesWithPrefix(_dirpath, "pagefile_", pagefileNames);

  // Load the in-memory representations of the PageFile files.
  unsigned int numPagefiles = pagefileNames.size();
  for (unsigned int i = 0; i < numPagefiles; i++)
    {
      // Make sure we've got the correct sequence of pagefile #s in the filenames...
      if (pagefileNames[i] != pagefileNumberToName(i))
	{
	  string errorStr = "Found a problem in the pagefile names sequence.";
	  throw SmException(__FILE__, __LINE__, errorStr);
	}

      // Create the pagefile's in-memory proxy object...
      string pagefilePathname = _dirpath +  pagefileNames[i];
      PageFile * pNewPageFile = new PageFile(pagefilePathname, _bytePerPage, _fdPool);
      _pageFiles.push_back(pNewPageFile);
    }

  _numFreePages = countNumFreePages();
}

//===============================================================================

PageDir::~PageDir()
{
  LockHolder mtxRights(_mtx);

  if (! _isShutdown)
    {
      cerr << "****** PageDir::~PageDir() : Big problem: (! _isShutdown) ******" << endl;
    }

  unsigned int numPagefiles = _pageFiles.size();
  for (unsigned int i = 0; i < numPagefiles; i++)
    {
      delete _pageFiles[i];
      _pageFiles[i] = NULL;
    }
}

//===============================================================================

unsigned long PageDir::getNumFreePages()
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);
  return _numFreePages;
}

//===============================================================================

float PageDir::getFillFraction()
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);

  unsigned long totalFreePages = 0;
  unsigned long totalPageCapacity = 0;

  for (unsigned int i = 0; i < _pageFiles.size(); i++)
    {
      unsigned int allocsWithNoGrowth;
      bool fileGrowthPossible;

      _pageFiles[i]->getCapacityInfo(allocsWithNoGrowth, fileGrowthPossible);
      totalFreePages += allocsWithNoGrowth;

      totalPageCapacity += _pageFiles[i]->getCurrentDataPages();
    }

  if (totalPageCapacity == 0)
    {
      return 1.0;
    }

  unsigned long totalAllocPages = totalPageCapacity - totalFreePages;
  return float(totalAllocPages) / totalPageCapacity;
}

//===============================================================================

bool PageDir::allocPageInternal(PageAddr & pa)
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);

  if (_isShutdown)
    {
      throw SmException(__FILE__, __LINE__, "The method PageDir::shutdown() has already been called");
    }

  if (_numFreePages == 0)
    {
      return false;
    }

  for (unsigned int i = 0; i < _pageFiles.size(); i++)
    {
      int pageNum = _pageFiles[i]->allocPageInternal();
      if (pageNum >= 0)
	{
	  pa._pageNum = pageNum;
	  pa._fileNum = i;
	  _numFreePages--;
	  return true;
	}
    }

  // If we got here, then _numFreePages > 0. But if we got here, no free page was found
  // in any of our page files. This should not happen.
  throw SmInternalException(__FILE__, __LINE__, "Couldn't find a free page when we should have been able to");
}

//===============================================================================

void PageDir::growStorage()
  throw (std::exception,
	 SmFileSystemFullException)
{
  //***************************************************************
  // IN CRITICAL SECTION...
  //***************************************************************
  PageFile * pLastPf = NULL;
  unsigned int oldPageFileCount;
 
  {
    LockHolder mtxRights(_mtx);

    if (_isShutdown)
      {
	throw SmException(__FILE__, __LINE__, "The method PageDir::shutdown() has already been called");
      }

    if (_storageIsGrowing)
      {
	throw SmException(__FILE__, __LINE__, "_storageIsGrowing is already true.");
      }

    oldPageFileCount = _pageFiles.size();
    if (oldPageFileCount > 0)
      {
	pLastPf = _pageFiles[_pageFiles.size() - 1];
      }

    _storageIsGrowing = true;
  }

  //***************************************************************
  // OUT OF CRITICAL SECTION...
  //***************************************************************

  // See if we can grow an existing page file. Since we do linear fill, the only
  // one that we might expand is the higest numbered one...
  bool grewExisting = false;

  if (pLastPf != NULL)
    {
      unsigned int allocsWithNoGrowth;
      bool fileGrowthPossible;

      pLastPf->getCapacityInfo(allocsWithNoGrowth, fileGrowthPossible);

      if (fileGrowthPossible)
	{
	  pLastPf->growFile();
	  grewExisting = true;
	}
    }

  if (! grewExisting)
    {
      string nextPfPathname = _dirpath + pagefileNumberToName(oldPageFileCount);

      // Note that we might temporarily exceed the number of allowed concurrent I/O
      // operations by one here, since this doesn't use our FD pool to do its
      // file creation.
      PageFile::createPageFile(nextPfPathname, _bytePerPage);
    }

  //***************************************************************
  // IN CRITICAL SECTION...
  //***************************************************************
  {
    LockHolder mtxRights(_mtx);
    _storageIsGrowing = false;

    if (! grewExisting)
      {
	string nextPfPathname =  _dirpath + pagefileNumberToName(_pageFiles.size());

	PageFile * pNewPf = new PageFile(nextPfPathname, _bytePerPage, _fdPool);

	// Don't start it out without any data capacity...
	pNewPf->growFile();

	_pageFiles.push_back(pNewPf);
      }

    // Don't try to update _numFreePages incrementally, because it's a bit error
    // prone when page files can grow by unpredictable amounts. Just re-reckon
    // from scratch.
    unsigned long oldNumFreePages = _numFreePages;
    _numFreePages = countNumFreePages();

    if ((oldNumFreePages == 0) && (_numFreePages > 0))
      {
	if (_pFreePagesBinSem != NULL)
	  {
	    _pFreePagesBinSem->post();
	  }
      }
  }
}

//===============================================================================

void PageDir::freePage(PageAddr & pa)
  throw (std::exception,
	 SmNotAllocatedException)
{
  LockHolder mtxRights(_mtx);

  if (_isShutdown)
    {
      throw SmException(__FILE__, __LINE__, "The method PageDir::shutdown() has already been called");
    }

  typedef unsigned int uint;
  if ((pa._fileNum < 0) && (uint(pa._fileNum) >= _pageFiles.size()))
    {
      throw SmException(__FILE__, __LINE__, "(pa._fileNum < 0) && (pa._fileNum >= _pageFiles.size())");
    }

  _pageFiles[pa._fileNum]->freePage(pa._pageNum);
  _numFreePages++;

  if (_numFreePages == 1)
    {
      // We just transitioned from 0 to 1 free page.
      if (_pFreePagesBinSem != NULL)
	{
	  _pFreePagesBinSem->post();
	}
    }
}

//===============================================================================
  
void PageDir::readPage(PageAddr & pa,
		       char *pBuffer)
  throw (std::exception,
	 SmNotAllocatedException)
{
  if (pBuffer == NULL)
    {
      throw SmException(__FILE__, __LINE__, "pBuffer == NULL");
    }

  //***************************************************************
  // IN CRITICAL SECTION...
  //***************************************************************
  PageFile * pPageFile = NULL;

  {
    LockHolder mtxRights(_mtx);

    if (_isShutdown)
      {
	throw SmException(__FILE__, __LINE__, "The method PageDir::shutdown() has already been called");
      }

    typedef unsigned int uint;
    if ((pa._fileNum < 0) && (uint(pa._fileNum) >= _pageFiles.size()))
      {
	throw SmException(__FILE__, __LINE__, "(pa._fileNum < 0) && (pa._fileNum >= _pageFiles.size())");
      }

    pPageFile = _pageFiles[pa._fileNum];
  }

  //***************************************************************
  // OUT OF CRITICAL SECTION...
  //***************************************************************
  pPageFile->readPage(pa._pageNum, pBuffer);
}

//===============================================================================
  
void PageDir::writePage(PageAddr & pa,
			const char *pBuffer)
  throw (std::exception,
	 SmNotAllocatedException)
{
  if (pBuffer == NULL)
    {
      throw SmException(__FILE__, __LINE__, "pBuffer == NULL");
    }

  //***************************************************************
  // IN CRITICAL SECTION...
  //***************************************************************
  PageFile * pPageFile = NULL;

  {
    LockHolder mtxRights(_mtx);

    if (_isShutdown)
      {
	throw SmException(__FILE__, __LINE__, "The method PageDir::shutdown() has already been called");
      }

    typedef unsigned int uint;
    if ((pa._fileNum < 0) && (uint(pa._fileNum) >= _pageFiles.size()))
      {
	throw SmException(__FILE__, __LINE__, "(pa._fileNum < 0) && (pa._fileNum >= _pageFiles.size())");
      }

    pPageFile = _pageFiles[pa._fileNum];
  }

  //***************************************************************
  // OUT OF CRITICAL SECTION...
  //***************************************************************
  pPageFile->writePage(pa._pageNum, pBuffer);
}

//===============================================================================

void PageDir::shutdown()
  throw (std::exception)
{
  LockHolder mtxRights(_mtx);
  if (_isShutdown)
    {
      return;
    }

  for (unsigned int i = 0; i < _pageFiles.size(); i++)
    {
      _pageFiles[i]->shutdown();
    }

  // Once we do this, no more I/O activity at all can occur in this PageDir...
  _fdPool.close();

  _isShutdown = true;
}

//===============================================================================

string PageDir::pagefileNumberToName(unsigned int filenum)
  throw (std::exception)
{
  // I'm sure there's a better place to enforce that we don't have more than
  // 10,000 page files in this directory, but we can clean that up later.
  if (filenum > 9999)
    {
      throw SmException(__FILE__, __LINE__, "filenum > 9999");
    }

  char numString[5];
  snprintf(numString, sizeof(numString), "%.4u", filenum);

  string filename = "pagefile_";
  filename += numString;
  filename += ".dat";

  return filename;
}

//===============================================================================

unsigned long PageDir::countNumFreePages()
    throw (std::exception)
{
  unsigned long returnVal = 0;
  for (unsigned int i = 0; i < _pageFiles.size(); i++)
    {
      unsigned int allocsWithNoGrowth;
      bool fileGrowthPossible;

      _pageFiles[i]->getCapacityInfo(allocsWithNoGrowth, fileGrowthPossible);
      returnVal += allocsWithNoGrowth;
    }

  return returnVal;
}

//===============================================================================
