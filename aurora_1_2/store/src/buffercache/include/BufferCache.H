#ifndef BUFFERCACHE_H
#define BUFFERCACHE_H

using namespace std;

#include <iolib/include/AsynchPageDir.H>
#include <buffercache/include/BufferCacheControlBlock.H>
#include <buffercache/include/BcIoDispatchRunnable.H>
#include <PtMutex.H>
#include <exception>

using namespace std;

class BufferCache
{
public:
  /**
     This opens an AsynchPageDir in the specified directory, 
     with the characteristics specified in this c'tor's 
     parameters.

     The total buffer memory used by this object will be
     "bytesPerPage * numPageFrames". Additional memory will
     be used for bookkeeping, but it's likely to be much
     smaller than the amount of memory used for the buffers.

     _userThreadShouldWork is posted to whenever this BufferCache things that
     the user might be interested in the state of this cache. The general
     intention for this is to signal when:
     - pages arrive in memory, or
     - requested page allocations become available, or
     - a request to shutdown the buffer cache has been satisfied
   */
  BufferCache(string dirPathname, 
	      size_t bytesPerPage,
	      size_t maxConcurrency,
	      size_t numPageFrames,
	      BinarySem & userThreadShouldWork, 
	      BinarySem & lazyWritesComplete)
    throw (exception);

  virtual ~BufferCache();

  // This is how you interact with this BufferCache, pretty much...
  BufferCacheControlBlock _cb;
  
  // The user must call this whenever he's done something to _cb that might
  // possibly require worker thread attention. It's OK to call this method
  // regardless of whether or not this BufferCache's lock is currently held.
  // It's only illegal to call this after beginShutdown() has been invoked.
  void awakenWorkerThreads()
    throw (exception);

  // Call this when you're totally done using this object. Once this is called, 
  // the results of interacting with _cb are undefined. Calling this method 
  // finalizes all data to disk.
  // Once this method has been invoked, the only valid thing to do to this object
  // is to delete it or call "isShutdown()". When shutdown is complete, the
  // user-supplied BinarySem will be posted.
  void beginShutdown()
    throw (exception);

  // Remember that 'userThreadShouldWork' will be posted to when this goes 
  // from false to true.
  bool isShutdown() const
    throw (exception);

  // Blocks until the shutdown procedure is complete
  void awaitShutdown()
    throw (exception);

private:
  size_t _maxConcurrency;
  size_t _numPageFrames;
  size_t _bytesPerPage;
  //  BufferCacheObserver * _pObserver;

  // By using thread-safe BinarySems here instead of bool variables, we can 
  // implement a two-flag handshake between the user and the worker threads
  // for shutdown control that doesn't need an over-arching mutex. This lets us
  // avoid a certain design that potentially caused deadlock.
  BinarySem _shutdownBegun;
  BinarySem _shutdownComplete;

  // Each of these is posted whenever it seems like the worker/user thread might
  // want to check out the state of this buffer cache.
  BinarySem _workerThreadShouldWork;
  BinarySem & _userThreadShouldWork;

  friend class BcIoDispatchRunnable;

  BcIoDispatchRunnable * _pIoRunnable;
  RunnableRunner * _pIoDispatchRunner;

  AsynchPageDir * _pApd;
  ClosableFifoQueue<IOCB *> _iocbReplies;

  // This contains the addresses of the IOCBs that aren't currently in use.
  vector<IOCB *> _iocbPool;


  // Invoked by the I/O Dispatch Runner thread. Does all the real, hard work of
  // this class.
  void dispatchIo()
    throw (exception);

  // Handles all replies currently in _iocbReplies.
  // Returns the number of ops handled.
  // The calling thread must hold the lock on _cb when calling this method.
  // This *reduces* each parameter by the number of replies handled of that
  // type.
  size_t handleIocbResults(size_t & numAllocOps,
			   size_t & numFreeOps,
			   size_t & numReadOps,
			   size_t & numWriteOps,
			   vector<PageAddr> & newlyFramedPages,
			   vector<PageAddr> & newlyWrittenPages,
			   bool & pagesNewlyAvailable)
    throw (exception);

  // Issues up to 'maxOps' page allocation requests, depending on the state of
  // _cb._availPages.
  // pendingAllocOps is the number of page allocatio ops currently pending.
  // Returns the number of IOCB requests this method issued.
  size_t handlePageAllocRequests(size_t maxOps,
				 size_t pendingAllocOps)
    throw (exception);

  // Issues up to 'maxOps' page read requests, depending on the state of
  // _cb.
  // Returns the number of IOCB requests this method issued.
  size_t handlePageWriteRequests(size_t maxOps)
    throw (exception);

  // Issues up to 'maxOps' page lazy write requests, depending on the state of
  // _cb.
  // Returns the number of IOCB requests this method issued.
  size_t handlePageReadRequests(size_t maxOps)
    throw (exception);

  // Does all the hard work of shutting the system down...
  void handleShutdown(size_t pendingAllocOps,
		      size_t pendingFreeOps,
		      size_t pendingReadOps,
		      size_t pendingWriteOps,
		      vector<PageAddr> & newlyFramedPages,
		      vector<PageAddr> & newlyWrittenPages,
		      bool & pagesNewlyAvailable)
    throw (exception);
};

#endif 
