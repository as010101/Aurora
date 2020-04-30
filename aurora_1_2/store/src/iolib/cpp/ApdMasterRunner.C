#include <iolib/int_include/ApdMasterRunner.H>
#include <typeinfo>
#include <sstream>
#include <iostream>
#include <assert.h>

//===============================================================================

ApdMasterRunner::ApdMasterRunner(string pageDirPath,
				 size_t bytesPerPage,
				 int maxIoConcurrency,
				 ClosableFifoQueue<IOCB *> & iocbReplyQueue)
  throw (std::exception) :
  _attentionNeededBinSem(false),
  _pageDir(pageDirPath, bytesPerPage, maxIoConcurrency, & _attentionNeededBinSem),
  _allocOpReqs(NULL, & _attentionNeededBinSem, & _attentionNeededBinSem, "ApdMasterRunner._allocOpReqs"),
  _freeOpReqs(NULL, & _attentionNeededBinSem, & _attentionNeededBinSem, "ApdMasterRunner._freeOpReqs"),
  _rwOpReqs(NULL, & _attentionNeededBinSem, & _attentionNeededBinSem, "ApdMasterRunner._rwOpReqs"),
  _opReplies(iocbReplyQueue),
  _numPendingThreadOps(0),
  _maxIoConcurrency(maxIoConcurrency),
  _completedThreadOps(NULL, & _attentionNeededBinSem, & _attentionNeededBinSem, "ApdMasterRunner._completedThreadOps"),
  _growthOpRunning(false),
  _threadPool(1, maxIoConcurrency, _completedThreadOps)
{
  if (maxIoConcurrency < 1)
    {
      throw SmIllegalParameterValueException(__FILE__, __LINE__, 
						       "(maxIoConcurrency < 1)");
    }
}

//===============================================================================

ApdMasterRunner::~ApdMasterRunner()
{
  while (_idleRwOpRunners.size() > 0)
    {
      RwOpRunner * p = _idleRwOpRunners.back();
      delete p;
      _idleRwOpRunners.pop_back();
    }
}

//===============================================================================

ClosableFifoQueue<IOCB*> & ApdMasterRunner::getAllocOpReqQueue()
{
  return _allocOpReqs;
}

//===============================================================================

ClosableFifoQueue<IOCB*> & ApdMasterRunner::getFreeOpReqQueue()
{
  return _freeOpReqs;
}

//===============================================================================

ClosableFifoQueue<IOCB*> & ApdMasterRunner::getRwOpReqQueue()
{
  return _rwOpReqs;
}

//===============================================================================

void ApdMasterRunner::run() 
  throw ()
{
  try
    {
      // Loop until there will definitely be no more requests submitted to
      // this ApdMasterRunner to handle.
      while (! allInputQueuesEmptyAndClosed())
	{
	  handleCompletedThreadOps();
	  handleNewFreeReqs();
	  handleNewAllocReqs();
	  handleNewRwReqs();
	  _attentionNeededBinSem.awaitPostThenClear();
	}

      // OK, now we're getting no new requests from the user. It's time to finish
      // up our existing work, and then clean up...
      _threadPool.quiesce();

      while (! _completedThreadOps.isClosed())
	{
	  handleCompletedThreadOps();

	  // When the thread pool has quiesced, it will close() the _completedThreadOps
	  // object. When _completedThreadOps is closed and empty, it will post the
	  // _attentionNeededBinSem flag.
	  _attentionNeededBinSem.awaitPostThenClear();
	}

      _pageDir.shutdown();
      _opReplies.close();      
    }
  catch (std::exception & e)
    {
      cloneAndSetRunException(e);
    }
  catch (...)
    {
      SmException * pException = 
	new SmException(__FILE__, __LINE__, 
				  "ApdMasterRunner::run(): Caught an exception that wasn't a std::exception");
      setRunException(pException);
    }
}

//===============================================================================

void ApdMasterRunner::handleCompletedThreadOps()
  throw (std::exception)
{
  while (! _completedThreadOps.isEmpty())
    {
      Runnable * pRunnable = NULL;
      bool success = _completedThreadOps.dequeueNonBlocking(pRunnable);
      if (! success)
	{
	  throw SmInternalException(__FILE__, __LINE__, "_opReplies seemed empty when it shouldn't be");
	}

      if (typeid(*pRunnable) == typeid(RwOpRunner))
	{
	  RwOpRunner * pRwOpRunner = dynamic_cast<RwOpRunner*>(pRunnable);
	  handleCompletedRwOp(pRwOpRunner);
	}
      else if (typeid(*pRunnable) == typeid(PdGrowthRunner))
	{
	  PdGrowthRunner * pGrowthRunner = dynamic_cast<PdGrowthRunner*>(pRunnable);
	  handleCompletedPdGrowth(pGrowthRunner);
	}
      else
	{
	  ostringstream msgStream;
	  msgStream << "_completedThreadOps produced a runnable of the wrong type: "
		    << typeid(*pRunnable).name() << endl;
      	  throw SmInternalException(__FILE__, __LINE__, msgStream.str());
	}
    }
}

//===============================================================================

void ApdMasterRunner::handleNewFreeReqs()
  throw (std::exception)
{
  while (! _freeOpReqs.isEmpty())
    {
      IOCB * pIOCB = NULL;
      bool success = _freeOpReqs.dequeueNonBlocking(pIOCB);
      if (! success)
	{
	  throw SmInternalException(__FILE__, __LINE__, "_freeOpReqs seemed empty when it shouldn't be");
	}

      try
	{
	  if (pIOCB->_pException != NULL)
	    {
	      delete pIOCB->_pException;
	      pIOCB->_pException = NULL;
	    }
	  
	  _pageDir.freePage(pIOCB->_pageAddr);
	}
      catch (SmException & e)
	{
	  pIOCB->_pException = new SmException(e);
	}
      catch (std::exception & e)
	{
	  pIOCB->_pException = new SmException(__FILE__, __LINE__, e.what());
	}

      _opReplies.enqueue(pIOCB);
    }
}

//===============================================================================

void ApdMasterRunner::handleNewAllocReqs()
  throw (std::exception)
{
  // Loop until either there are no more page-allocation requests pending, or
  // it would fail because the PageDir has no internally free pages to offer us...
  while ((! _allocOpReqs.isEmpty()) && (_pageDir.getNumFreePages() > 0))
    {
      IOCB * pIOCB = NULL;
      bool success = _allocOpReqs.dequeueNonBlocking(pIOCB);
      if (! success)
	{
	  throw SmInternalException(__FILE__, __LINE__, "_freeOpReqs seemed empty when it shouldn't be");
	}

      try
	{
	  if (pIOCB->_pException != NULL)
	    {
	      delete pIOCB->_pException;
	      pIOCB->_pException = NULL;
	    }

	  _pageDir.allocPageInternal(pIOCB->_pageAddr);
	}
      catch (SmException & e)
	{
	  pIOCB->_pException = new SmException(e);
	}
      catch (std::exception & e)
	{
	  pIOCB->_pException = new SmException(__FILE__, __LINE__, e.what());
	}

      _opReplies.enqueue(pIOCB);
    }

  // If this is a good time, grow the PageDir's storage...
  if (! _growthOpRunning)
    {
      if ((((! _allocOpReqs.isEmpty()) && (_pageDir.getNumFreePages() == 0)) ||
	   (_pageDir.getFillFraction() >= SM_PAGE_DIR_GROWTH_TRIGGER)))
	{
	  // Note that this can't cause us to exceed the maximum allowed degree of
	  // concurrency, because our thread pool will just queue up those Runnables
	  // that would be excessively concurrent...
	  _growthOpRunning = true;
	  _numPendingThreadOps++;
	  _threadPool.schedule(new PdGrowthRunner(_pageDir));
	}
    }
}

//===============================================================================

void ApdMasterRunner::handleNewRwReqs()
  throw (std::exception)
{
  while ((! _rwOpReqs.isEmpty()) && (_numPendingThreadOps < _maxIoConcurrency))
    {
      IOCB * pIOCB = NULL;
      bool success = _rwOpReqs.dequeueNonBlocking(pIOCB);
      if (! success)
	{
	  throw SmInternalException(__FILE__, __LINE__, "_rwOpReqs seemed empty when it shouldn't be");
	}

      if (pIOCB->_pException != NULL)
	{
	  delete pIOCB->_pException;
	  pIOCB->_pException = NULL;
	}

      pIOCB->_opsRunningWhenStarted = _numPendingThreadOps;
      _numPendingThreadOps++;

      RwOpRunner * pRunner;
      if (_idleRwOpRunners.size() > 0)
	{
	  pRunner = _idleRwOpRunners.back();
	  _idleRwOpRunners.pop_back();
	}
      else
	{
	  pRunner = new RwOpRunner(_pageDir);
	}

      pRunner->setOp(pIOCB);
      _threadPool.schedule(pRunner);
    }
}

//===============================================================================

void ApdMasterRunner::handleCompletedRwOp(RwOpRunner * pOp)
  throw (std::exception)
{
  assert(pOp != NULL);
  _numPendingThreadOps--;

  IOCB * pIOCB = pOp->getOp();
  _opReplies.enqueue(pIOCB);  

  pOp->setOp(NULL);
  _idleRwOpRunners.push_back(pOp);
}

//===============================================================================

void ApdMasterRunner::handleCompletedPdGrowth(PdGrowthRunner * pOp)
  throw (std::exception)
{
  assert(pOp != NULL);

  _numPendingThreadOps--;
  _growthOpRunning = false;
  delete pOp;
}

//===============================================================================

bool ApdMasterRunner::allInputQueuesEmptyAndClosed()
  throw (std::exception)
{
  return ((_allocOpReqs.isEmpty()) && (_allocOpReqs.isClosed()) &&
	  (_freeOpReqs.isEmpty())  && (_freeOpReqs.isClosed())  &&
	  (_rwOpReqs.isEmpty())    && (_rwOpReqs.isClosed()));
}
