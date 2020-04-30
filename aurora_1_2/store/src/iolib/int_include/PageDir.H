#ifndef PageDir_H
#define PageDir_H

#include <StorageMgr_Exceptions.H>
#include <PtMutex.H>
#include <iolib/include/PageAddr.H>
#include <iolib/int_include/PageFile.H>
#include <FdMultiPool.H>
#include <BinarySem.H>
#include <vector>

/*
 This collects all of the individual PageFiles in a specified directory, and 
 provides a facade over that directory as though it was one big page file.
 This is how we offer database bigger than 2GB on filesystems that don't 
 have large file support.
 */
class PageDir
{
public:
  // This deletes all of the files used by an PageDir in the specified
  // directory. Don't execute this while the files are being used by some 
  // PageDir object, or else you'll make someone very unhappy.
  static void clear(string dirPathname)
    throw (std::exception);

  // Discover all of the PageFile files in the specified directory (if there
  // are any), and provide a facade over them / over the directory.
  //
  // If a caller attempts to invoke an operation on this object that would
  // exceed 'maxConcurrentOps', then that operation fails with an exception.
  //
  // If pBinsem != NULL, then its 'post()' method will be called each the
  // number of pages that can be allocated internally in this PageDir goes
  // from zero to non-zero. 
  PageDir(string dirpath, 
	  size_t bytesPerPage,
	  int maxConcurrentOps, 
	  BinarySem * pFreePagesBinSem)
    throw (std::exception);

  virtual ~PageDir();

  // Returns the number of pages that can be allocated from this PageDir
  // without the need for the PageDir to add/expand its presence on disk
  // first.
  unsigned long getNumFreePages()
    throw (std::exception);

  // Returns the fraction (in the range 0...1) of this PageDir's storage that's
  // presently allocated. "1.0" means approximately all pages are presently
  // allocated, whereas "0.0" means approximately no pages is presently
  // allocated.
  // If the PageDir has a total capacity of 0 pages, then this returns "1.0".
  float getFillFraction()
    throw (std::exception);

  // If there was a free page, 'pa' is set to its address, and 'true' is returned.
  // Otherwise, 'false' is returned.
  bool allocPageInternal(PageAddr & pa)
    throw (std::exception);
  
  // Grows the FileDir's presence on disk. There may be only one invocation of 
  // this method at a time.
  void growStorage()
    throw (std::exception,
	   SmFileSystemFullException);

  void freePage(PageAddr & pa)
    throw (std::exception,
	   SmNotAllocatedException);
  
  void readPage(PageAddr & pa,
		char *pBuffer)
    throw (std::exception,
	   SmNotAllocatedException);
  
  void writePage(PageAddr & pa,
		 const char *pBuffer)
    throw (std::exception,
	   SmNotAllocatedException);
    
  // Marks the PageDir as shutdown. Has several effects:
  //   -- All I/O-requiring methods  will throw an exception.
  //   -- Commits the page files' buffers to disk.
  // NOTE: **** DON'T CALL THIS UNTIL THERE ARE NO MORE OUTSTANDING METHOD INVOCATIONS
  //       **** ON THIS OBJECT! 
  //       #### CALLING THIS WHILE THERE'S AN OUTSTANDING READ/WRITE REQUEST CAN
  //       #### SABOTAGE THAT REQUEST, BECAUSE IT WILL CLOSE THE FD USED BY THAT
  //       #### REQUEST!!!
  // When this returns, the PageDir has been quiesced, and committed to disk.
  void shutdown()
    throw (std::exception);

private:
  PageDir(const PageDir & rhs) : _fdPool(0) {assert(false);}
  PageDir & operator= (const PageDir & rhs) {assert(false); return *this;}

  // Given the simple filename of a page file, this returns its index number.
  // If the filename doesn't have an appropriate structure, an exception is thrown.
  string pagefileNumberToName(unsigned int filenum)
    throw (std::exception);

  unsigned long countNumFreePages()
    throw (std::exception);

  PtMutex _mtx; // This must be held to access any of this PageDir's other instance variables.

  int _maxConcurrentOps;
  int _currentOpCount;

  // The index into this vector is the page file's file#.
  vector<PageFile *> _pageFiles;
  FdMultiPool _fdPool;

  // true when and only when there's a pending method invocation to grow storage
  bool _storageIsGrowing;

  // true when and only when the shutdown() method has been called.
  bool _isShutdown;

  // The '/'-terminated path to the directory containing this PageDir's files.
  string _dirpath;

  // Posted to each time the number of pages available for internal allocation goes from 0
  // to non-zero.
  BinarySem * _pFreePagesBinSem;

  // The number of free pages that are available for internal allocation.
  unsigned long _numFreePages;

  size_t _bytePerPage;
};

#endif
