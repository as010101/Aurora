#include <iolib/include/AsynchPageDir.H>
#include <iolib/int_include/ApdMasterRunner.H>
#include <iostream>

//=============================================================================

void AsynchPageDir::clear(string dirPathname)
  throw (std::exception)
{
  PageDir::clear(dirPathname);
}

//=============================================================================

AsynchPageDir::AsynchPageDir(string dirPathname, 
			     size_t bytesPerPage,
			     int maxConcurrency,
			     ClosableFifoQueue<IOCB *> & iocbReplyQueue)
  throw (std::exception) :
  _isShutdown(false),
  _iocbReplyQueue(iocbReplyQueue)
{
  _pApdEngine = new ApdMasterRunner(dirPathname, bytesPerPage, maxConcurrency, iocbReplyQueue);
  _pThreadRunner = new RunnableRunner(* _pApdEngine);
}

//=============================================================================

AsynchPageDir::~AsynchPageDir()
{
  delete _pThreadRunner;
  delete _pApdEngine;
}

//=============================================================================

void AsynchPageDir::submitOp(IOCB * pOp)
  throw (std::exception)
{
  if (_isShutdown)
    {
      throw SmException(__FILE__, __LINE__, "This object has already been shutdown.");
    }

  ApdMasterRunner * pApdEngine = dynamic_cast<ApdMasterRunner *>(_pThreadRunner->getRunnable());

  switch (pOp->_opType)
    {
    case IOCB::PAGE_ALLOC:
      pApdEngine->getAllocOpReqQueue().enqueue(pOp);
      break;
    case IOCB::PAGE_FREE:
      pApdEngine->getFreeOpReqQueue().enqueue(pOp);
      break;
    case IOCB::PAGE_READ:
    case IOCB::PAGE_WRITE:
      pApdEngine->getRwOpReqQueue().enqueue(pOp);
      break;
    default:
      throw SmInternalException(__FILE__, __LINE__, "Invalid op type in IOCB");
    }
}

//=============================================================================

void AsynchPageDir::shutdown()
  throw (std::exception)
{
  if (_isShutdown)
    {
      throw SmException(__FILE__, __LINE__, "This object has already been shutdown.");
    }

  _isShutdown = false;

  // Indicate to the ApdMasterRunner that no more IOCB requests will be coming. 
  // This will trigger its eventual quiescence...
  ApdMasterRunner * pApdEngine = dynamic_cast<ApdMasterRunner *>(_pThreadRunner->getRunnable());
  pApdEngine->getAllocOpReqQueue().close();
  pApdEngine->getFreeOpReqQueue() .close();
  pApdEngine->getRwOpReqQueue()   .close();

  // Wait for the ApdMasterRunner to terminate itself.
  _pThreadRunner->join();
 
  std::exception * pException = pApdEngine->getRunException();

  if (pException != NULL)
    {
      SmException * pSmException = dynamic_cast<SmException *>(pException);
      if (pSmException != NULL)
	{
	  SmException e(*pSmException);
	  pApdEngine->deleteRunException();
	  throw e;
	}
      else
	{
	  std::exception e(*pException);
	  pApdEngine->deleteRunException();
	  throw e;
	}
    }
}

//=============================================================================

