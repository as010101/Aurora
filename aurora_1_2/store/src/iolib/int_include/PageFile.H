#ifndef PageFile_H
#define PageFile_H

#include <iolib/include/IOCB.H>
#include <StorageMgr_Exceptions.H>
#include <sm_int_constants.H>
#include <SerializableBitSet.H>
#include <FdMultiPool.H>
#include <PtMutex.H>
#include <string>


/*
  Allowed concurrency:
  - As many concurrent invocations of the following methods as desired:
     - getCapacityInfo(...)
     - allocPageInternal(...)
     - freePage(...)
     - readPage(...)
     - writePage(...)
    Plus *at most* one invocation of growFile().
 --OR--
   A single invocation of shutdown().
 */
class PageFile
{
public:
  static void createPageFile(string filename, size_t bytesPerPage)
    throw (std::exception);
  
  // Opens an existing page file, and wraps it with an instance of this class.
  // Note: bytesPerPage must be the exact same value that was used when creating the
  // page file on disk.
  PageFile(string filename, 
	   size_t bytesPerPage, 
	   FdMultiPool & fdPool)
    throw (std::exception);
  
  virtual ~PageFile();

  void getCapacityInfo(unsigned int & allocsWithNoGrowth,
		       bool & fileGrowthPossible)
    throw (std::exception);

  // Returns the current number of data pages this file can handle (doesn't account
  // for what it could handle if grown further).
  unsigned int getCurrentDataPages()
    throw (std::exception);

  // Returns the maximum number of data pages this file can handle, when grown
  // fully...
  int getMaxDataPages()
    throw (std::exception);

  // Returns the page # if successful, or -1 if it failed because there was
  // no page available.
  int allocPageInternal()
    throw (std::exception);
  
  // Grows the file. There may be only one invocation of this method at a time.
  // Throws an exception if it's not successful, because you could have called
  // getCapacityInfo(...) before calling this method to discover if it shoudl work.
  void growFile()
    throw (std::exception,
	   SmFileFullException,
	   SmFileSystemFullException);

  void freePage(int pageId)
    throw (std::exception,
	   SmNotAllocatedException);
  
  void readPage(int pageId,
		char *pBuffer)
    throw (std::exception,
	   SmNotAllocatedException);
  
  void writePage(int pageId,
		 const char *pBuffer)
    throw (std::exception,
	   SmNotAllocatedException);
    
  // Marks the PageFile as shutdown. Has several effects:
  //   -- All I/O-requiring operations will throw an exception.
  //   -- Commits the file's buffers to disk.
  void shutdown()
    throw (std::exception);
  

  string getDebugInfo()
    throw (std::exception);
private:
  // Calculates the number of data pages (not including allocation bitmap pages)
  // can fit into a 2 GB file, assuming that the file will also need an
  // appropriate number of allocation bitmap pages.
  static long getMaxDataPages(long bytesPerPage)
    throw (std::exception);

  // Calculates the number of allocation bitmap pages a 2GB page file should
  // contain, assuming that the pages are 'bytesPerPage' in size.
  // (getMaxDataPages(x) + getNumBitmapPages(x)) <= (2 GB / bytesPerPage)
  static long getNumBitmapPages(long bytesPerPage)
    throw (std::exception);

  bool isPageAllocated(int pageId) const
    throw (std::exception);

  void setPageAllocated(int pageId, bool isAllocated)
    throw (std::exception);

  // Tries to find a free page within the already allocated file. If successful,
  // this method returns the page ID. If unsuccessful, this returns -1.
  int findFreePage();

  // Attempts to grow the file on the filesystem by the specified number of pages.
  // Iff the filesystem is too full, the TooFullException is thrown.
  /* I THINK THIS IS UNUSED. WE CAN KILL IT IF IT IS...
  void growFileByPageCount(int numNewPages)
    throw (std::exception,
	   SmFileFullException,
	   SmFileSystemFullException);
  */

  int getPageCountAfterNextGrowth()
    throw (std::exception);


  // Number of data pages the file is presently capable of storing without being
  // grown on the filesystem.
  int  _currentDataPageCapacity; 

  // The nth bit = 1 implies that the nth page in the file is allocated. Pages are 
  // numbered starting with #0.
  // NOTE: We assume that (SM_MAX_FILE_PAGES % 8) == 0.
  SerializableBitSet _pageAllocBitmap;

  string _filename;
  FdMultiPool & _fdPool;
  bool _isShutdown;
  bool _isGrowing;
  size_t _bytesPerPage;
  long _maxDataPages;
  long _numBitmapPages;

  // Offset into the file where the first data page's first byte appears. The first byte in
  // the file has an offset of 0, since that's how lseek(...) expects addresses.
  off_t _firstDataByteOffset;

  // This must be held whenever accessing this PageFile's instance variables, except
  // for _fdPool - That can accessed even when _mtx isn't held.
  PtMutex _mtx;
};

#endif
