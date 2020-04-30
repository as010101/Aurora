#ifndef APDMASTERRUNNER_H
#define APDMASTERRUNNER_H

#include <Runnable.H>
#include <PtThreadPool.H>
#include <StorageMgr_Exceptions.H>
#include <iolib/include/IOCB.H>
#include <iolib/int_include/PageDir.H>
#include <iolib/int_include/RwOpRunner.H>
#include <iolib/int_include/PdGrowthRunner.H>
#include <ClosableFifoQueue.H>
#include <vector>

/* 
   An instance of this class does all of the high-level scheduling and
   organization of an AsynchPageDir's intenral activity.

   It runs actively until it's 3 input queues are all closed and empty.

   Sometime after that, when it's completed all of its outstanding request
   processing and has settled the files, it will close the reply queueu.

   Once the reply queue is closed and empty, this object's work will be done
   and it will return from its run() method.
 */
class ApdMasterRunner : public Runnable
{
public:
  ApdMasterRunner(string pageDirPath,
		  size_t bytesPerPage,
		  int maxIoConcurrency,
		  ClosableFifoQueue<IOCB *> & iocbReplyQueue)
    throw (std::exception);

  virtual ~ApdMasterRunner();

  ClosableFifoQueue<IOCB*> & getAllocOpReqQueue();
  ClosableFifoQueue<IOCB*> & getFreeOpReqQueue();
  ClosableFifoQueue<IOCB*> & getRwOpReqQueue();

  void run() 
    throw ();

private:
  // Fully deals with the results of all Runnables that the
  // thread pool has completed the execution of.
  // 
  // If 'workUntilClosedAndEmpty', then this will execute
  // until the op-reply-queue is closed and empty.
  //
  // Otherwise, this will return as soon as the op-reply-queue
  // is empty.
  void handleCompletedThreadOps()
    throw (std::exception);

  void handleNewFreeReqs()
    throw (std::exception);

  void handleNewAllocReqs()
    throw (std::exception);

  void handleNewRwReqs()
    throw (std::exception);

  void handleCompletedRwOp(RwOpRunner * pOp)
    throw (std::exception);

  void handleCompletedPdGrowth(PdGrowthRunner * pOp)
    throw (std::exception);

  bool allInputQueuesEmptyAndClosed()
    throw (std::exception);

  BinarySem _attentionNeededBinSem;
  PageDir _pageDir;

  ClosableFifoQueue<IOCB*> _allocOpReqs;
  ClosableFifoQueue<IOCB*> _freeOpReqs;
  ClosableFifoQueue<IOCB*> _rwOpReqs;
  ClosableFifoQueue<IOCB*> & _opReplies;

  int _numPendingThreadOps;
  int _maxIoConcurrency;
  ClosableFifoQueue<Runnable*> _completedThreadOps;
  bool _growthOpRunning;

  PtThreadPool _threadPool;

  // Just a pool to store RwOpRunner objects that aren't currently in use,
  // in case one is needed.
  vector<RwOpRunner *> _idleRwOpRunners;
};

#endif

