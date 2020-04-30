#include <buffercache/include/BufferCache.H>
#include <algorithm>
#include <assert.h>

using namespace std;

BufferCache::BufferCache(string dirPathname, 
			 size_t bytesPerPage,
			 size_t maxConcurrency,
			 size_t numPageFrames,
			 BinarySem & userThreadShouldWork, 
			 BinarySem & lazyWritesComplete)
  throw (exception)
  : _cb(numPageFrames, bytesPerPage, & lazyWritesComplete),
    _maxConcurrency(maxConcurrency),
    _numPageFrames(numPageFrames),
    _bytesPerPage(bytesPerPage),
    _shutdownBegun(false),
    _userThreadShouldWork(userThreadShouldWork),
    _iocbReplies(NULL, 
		 & _workerThreadShouldWork,
		 & _workerThreadShouldWork,
		 "BufferCache._iocbReplies")
{
  // This is to accomodate a very simple algorithm for allocating concurrency to
  // the different kinds of operations...
  if (maxConcurrency < 4)
    {
      throw SmException(__FILE__, __LINE__, "maxConcurrency < 4");
    }

  if (bytesPerPage < 1)
    {
      throw SmException(__FILE__, __LINE__, "bytesPerPage < 1");
    }

  if (numPageFrames < 1)
    {
      throw SmException(__FILE__, __LINE__, "numPageFrames < 1");
    }

  for (size_t i = 0; i < maxConcurrency; i++)
    {
      _iocbPool.push_back(new IOCB());
    }

  _pApd = new AsynchPageDir(dirPathname, bytesPerPage, 
			    maxConcurrency, _iocbReplies);
  
  _pIoRunnable = new BcIoDispatchRunnable(*this);
  _pIoDispatchRunner = new RunnableRunner(* _pIoRunnable);
}

//===============================================================================

BufferCache::~BufferCache()
{
  // If the shutdown hasn't begun, then this destructor is getting invoked either
  // because of a coding error, or because an exception was thrown...
  if (! _shutdownBegun.isPosted())
    {
      cerr << "BufferCache::~BufferCache(): Shutdown not begun, so inducing it now." << endl;
      beginShutdown();
    }
  
  if (! isShutdown())
    {
      cerr << "BufferCache::~BufferCache(): Shutdown not complete. "
	   << "Awaiting shutdown..." << endl;
      awaitShutdown();
      cerr << "BufferCache::~BufferCache(): Shutdown complete." << endl;
    }

  _pIoDispatchRunner->join();
  delete _pIoDispatchRunner;
  delete _pIoRunnable;
  delete _pApd;
}

//===============================================================================

void BufferCache::awakenWorkerThreads()
  throw (exception)
{
  _workerThreadShouldWork.post();
}

//===============================================================================

void BufferCache::beginShutdown()
  throw (exception)
{
  _shutdownBegun.post();
  _workerThreadShouldWork.post();
}

//===============================================================================

bool BufferCache::isShutdown() const
  throw (exception)
{
  return _shutdownComplete.isPosted();
}

//===============================================================================

void BufferCache::awaitShutdown()
  throw (exception)
{
  _shutdownComplete.awaitPost();
}

//===============================================================================

void BufferCache::dispatchIo()
  throw (exception)
{
  // The best thing to do would be to discover early on how many of each type
  // of operation we'd like to do, and with what priority, and then divide 
  // concurrency appopriately. We have an unplanned-for contention: use of the
  // limited I/O concurrency.
  // This might be simplified if we separate out Alloc/Free requests into their
  // own quota in the APD, but that's not going to happen for a while. -CJC

  // It's ok to laugh at this. It's not the most impressive implementation...
  size_t allocOpQuota = _maxConcurrency / 4;
  // int freeOpQuota  = _maxConcurrency / 4;
  size_t writeOpQuota = _maxConcurrency / 4;

  // Main loop...
  size_t pendingAllocOps = 0;
  size_t pendingFreeOps  = 0;
  size_t pendingReadOps  = 0;
  size_t pendingWriteOps = 0;

  // It's OK to use vectors rather than sets, because we reset the vectors each
  // loop iteration, and we only do at most one operation per PageAddr in a 
  // given loop iteration. So there's no risk of multiple appearances of a
  // PageAddr in this bunch of vectors...
  vector<PageAddr> newlyFramedPages;
  vector<PageAddr> newlyWrittenPages;
  bool pagesNewlyAvailable;

  while (! _shutdownBegun.isPosted())
    {
      newlyFramedPages.clear();
      newlyWrittenPages.clear();
      pagesNewlyAvailable = false;

      // Note that this reacquires the lock each loop iteration.
      {
      LockHolder aHolder(_cb); 

      //-------------------------------------------------------------------------
      // Step 1: Handle I/O results...
      //-------------------------------------------------------------------------
      size_t opsHandled = handleIocbResults(pendingAllocOps,
					    pendingFreeOps,
					    pendingReadOps,
					    pendingWriteOps,
					    newlyFramedPages,
					    newlyWrittenPages,
					    pagesNewlyAvailable);

      assert(pendingAllocOps >= 0);
      assert(pendingFreeOps  >= 0);
      assert(pendingReadOps  >= 0);
      assert(pendingWriteOps >= 0);
      assert((pendingAllocOps + pendingFreeOps 
	      + pendingReadOps + pendingWriteOps 
	      + _iocbPool.size()) == _maxConcurrency);

      if (opsHandled > 0)
	{
	  _userThreadShouldWork.post();
	}

      // Step 2: Handle new allocation requests (right now we're not goint to 
      // handle 'free' requests in our main processing. We'll just do that during
      // shutdown.)
      size_t maxAllocOps = min(_iocbPool.size(), allocOpQuota - pendingAllocOps);
      pendingAllocOps += handlePageAllocRequests(maxAllocOps, pendingAllocOps);

      assert(pendingAllocOps >= 0);
      assert(pendingFreeOps  >= 0);
      assert(pendingReadOps  >= 0);
      assert(pendingWriteOps >= 0);
      assert((pendingAllocOps + pendingFreeOps 
	      + pendingReadOps + pendingWriteOps 
	      + _iocbPool.size()) == _maxConcurrency);

      // Step 3: Handle new lazy write requests...

      size_t maxWriteOps = min(_iocbPool.size(), writeOpQuota - pendingWriteOps);
      pendingWriteOps += handlePageWriteRequests(maxWriteOps);

      assert(pendingAllocOps >= 0);
      assert(pendingFreeOps  >= 0);
      assert(pendingReadOps  >= 0);
      assert(pendingWriteOps >= 0);
      assert((pendingAllocOps + pendingFreeOps 
	      + pendingReadOps + pendingWriteOps 
	      + _iocbPool.size()) == _maxConcurrency);

      // Step 4: Handle new read requests. We'll let this do as much concurrency as
      // we have capacity for...
      size_t maxReadOps = _iocbPool.size();      
      pendingReadOps += handlePageReadRequests(maxReadOps);

      assert(pendingAllocOps >= 0);
      assert(pendingFreeOps  >= 0);
      assert(pendingReadOps  >= 0);
      assert(pendingWriteOps >= 0);
      assert((pendingAllocOps + pendingFreeOps 
	      + pendingReadOps + pendingWriteOps 
	      + _iocbPool.size()) == _maxConcurrency);
      }

      // Step 6: Await more stuff to do...
      _workerThreadShouldWork.awaitPostThenClear();
    }

  assert(pendingAllocOps >= 0);
  assert(pendingFreeOps  >= 0);
  assert(pendingReadOps  >= 0);
  assert(pendingWriteOps >= 0);
  assert((pendingAllocOps + pendingFreeOps 
	  + pendingReadOps + pendingWriteOps 
	  + _iocbPool.size()) == _maxConcurrency);

  // Shut things down...
  newlyFramedPages.clear();
  newlyWrittenPages.clear();
  pagesNewlyAvailable = false;

  {
    LockHolder aHolder(_cb); 
    handleShutdown(pendingAllocOps,
		   pendingFreeOps,
		   pendingReadOps,
		   pendingWriteOps,
		   newlyFramedPages,
		   newlyWrittenPages,
		   pagesNewlyAvailable);
  }
}

//===============================================================================

size_t BufferCache::handleIocbResults(size_t & numAllocOps,
				      size_t & numFreeOps,
				      size_t & numReadOps,
				      size_t & numWriteOps,
				      vector<PageAddr> & newlyFramedPages,
				      vector<PageAddr> & newlyWrittenPages,
				      bool & pagesNewlyAvailable)
  throw (exception)
{
  size_t opsHandled = 0;
  IOCB * pIOCB;
  while (_iocbReplies.dequeueNonBlocking(pIOCB))
    {
      ++opsHandled;

      // Free up the IOCB early, in case we throw an exception. I'm doing this to
      // debug a problem where the pool runs out of free IOCBs when it's not
      // supposed to be empty...
      assert(pIOCB != NULL);
      PageAddr pa     = pIOCB->_pageAddr;
      IOCB::OPCODE op = pIOCB->_opType;

      if (pIOCB->_pException != NULL)
	{
	  throw SmException(*(pIOCB->_pException));
	  _iocbPool.push_back(pIOCB);
	}
      else
	{
	  _iocbPool.push_back(pIOCB);
	}      

      switch (op)
	{
	case IOCB::PAGE_ALLOC:
	  _cb._availPages.addPage(pa);
	  pagesNewlyAvailable = true;
	  --numAllocOps;
	  break;
	case IOCB::PAGE_FREE:
	  // No action needed.
	  --numFreeOps;
	  break;
	case IOCB::PAGE_READ:
	  _cb.onFrameLoadComplete(pa);
	  newlyFramedPages.push_back(pa);
	  --numReadOps;
	  break;
	case IOCB::PAGE_WRITE:
	  _cb.onFrameLazyWriteComplete(pa);
	  newlyWrittenPages.push_back(pa);
	  --numWriteOps;
	  break;
	default:
	  throw SmException(__FILE__, __LINE__, "Invalid IOCB optype");
	}
    }

  return opsHandled;
}

//===============================================================================

size_t BufferCache::handlePageAllocRequests(size_t maxOps,
					    size_t pendingAllocOps)
  throw (exception)
{
  AvailablePageSet & aps = _cb._availPages;
  size_t numPagesWanted = 
    max((aps.getDesiredPageCount() - 
	 aps.getCurrentPageCount() - 
	 pendingAllocOps), 
	size_t(0));

  size_t numOps = min(numPagesWanted, maxOps);

  for (size_t i = 0; i < numOps; ++i)
    {
      assert(_iocbPool.size() > 0);
      IOCB * pIOCB = _iocbPool.back();
      _iocbPool.pop_back();

      pIOCB->_opType = IOCB::PAGE_ALLOC;
      _pApd->submitOp(pIOCB);
    }

  return numOps;
}

//===============================================================================

size_t BufferCache::handlePageWriteRequests(size_t maxOps)
  throw (exception)
{
  vector<PageAddr> pages;
  vector<char *> frames;

  _cb.beginFrameLazyWriteOps(maxOps, pages, frames);

  size_t numOps = pages.size();
  assert(pages.size() <= maxOps);

  for (size_t i = 0; i < numOps; ++i)
    {
      assert(! _iocbPool.empty());
      IOCB * pIOCB = _iocbPool.back();
      _iocbPool.pop_back();

      pIOCB->_opType = IOCB::PAGE_WRITE;
      pIOCB->_pageAddr = pages[i];
      pIOCB->_pBuffer  = frames[i];

      _pApd->submitOp(pIOCB);
    }

  return numOps;
}

//===============================================================================

size_t BufferCache::handlePageReadRequests(size_t maxOps)
  throw (exception)
{
  vector<PageAddr> pages;
  vector<char *> frames;

  _cb.beginFrameLoadOps(maxOps, pages, frames);


  // debug code
  set<PageAddr> foo;
  for (size_t i = 0; i < pages.size(); i++)
    {
      assert(foo.find(pages[i]) == foo.end());
      foo.insert(pages[i]);
    }

  size_t numOps = pages.size();
  assert(pages.size() <= maxOps);

  for (size_t i = 0; i < numOps; ++i)
    {
      assert(! _iocbPool.empty());
      IOCB * pIOCB = _iocbPool.back();
      _iocbPool.pop_back();

      pIOCB->_opType = IOCB::PAGE_READ;
      pIOCB->_pageAddr = pages[i];
      pIOCB->_pBuffer  = frames[i];

      _pApd->submitOp(pIOCB);
    }

  return numOps;
}

//===============================================================================

void BufferCache::handleShutdown(size_t pendingAllocOps,
				 size_t pendingFreeOps,
				 size_t pendingReadOps,
				 size_t pendingWriteOps,
				 vector<PageAddr> & newlyFramedPages,
				 vector<PageAddr> & newlyWrittenPages,
				 bool & pagesNewlyAvailable)
  throw (exception)
{
  if (_cb._framedPages.atLeastOnePageUserLocked())
    {
      throw SmException(__FILE__, __LINE__, 
				  "Trying to shutdown while at least one page has a user-lock");
    }

  // Keep up the lazy writing until there's no more to do, and also let all
  // preexisting ops finish up...
  size_t pendingOps = 
    pendingAllocOps + pendingFreeOps + 
    pendingWriteOps + pendingReadOps;

  bool mightNeedMoreLazyWrites = true;

  // Note our abandonment of the quota system now. The only class of new
  // I/O ops we'll issue is "write".
  while ((pendingOps > 0) || mightNeedMoreLazyWrites)
    {
      // Handle pending ops that have completed...
      handleIocbResults(pendingAllocOps,
			pendingFreeOps,
			pendingReadOps,
			pendingWriteOps,
			newlyFramedPages,
			newlyWrittenPages,
			pagesNewlyAvailable);

      pendingOps = 
	pendingAllocOps + pendingFreeOps + 
	pendingWriteOps + pendingReadOps;

      // Issue new lazy-write ops. This is limited only by maxOps, and the need 
      // to actually lazy write any pages. So when this tells us there are no
      // pages to lazy write, we've issued all of the lazy write ops we need to.
      if (mightNeedMoreLazyWrites && 
	  //	  (pendingOps > 0) && 
	  (pendingOps < _maxConcurrency))
	{
	  int newLazyWriteOps = 
	    handlePageWriteRequests(_maxConcurrency - pendingOps);

	  pendingWriteOps += newLazyWriteOps;
	  
	  mightNeedMoreLazyWrites = (newLazyWriteOps > 0);
	}

      pendingOps = 
	pendingAllocOps + pendingFreeOps + 
	pendingWriteOps + pendingReadOps;

      if (pendingOps > 0)
	{
	  // Recall that _iocbReplies will post to this when it goes from empty
	  // to non-empty. Our earlier call to handleIocbResults empties out the
	  // queue, and since we have pending ops, it must become non-empty
	  // eventually.
	  _workerThreadShouldWork.awaitPostThenClear();
	}
    }

  // Quiesce storage...
  _pApd->shutdown();

  while (! _iocbReplies.isClosed())
    {
      _workerThreadShouldWork.awaitPostThenClear();
    }

  if (! _iocbReplies.isEmpty())
    {
      throw SmException(__FILE__, __LINE__, "Got unexpected IOCB reply");
    }

  _shutdownComplete.post();
  _userThreadShouldWork.post(); 
}

//===============================================================================
