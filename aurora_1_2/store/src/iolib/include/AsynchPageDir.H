#ifndef ASYNCHPAGEDIR_H
#define ASYNCHPAGEDIR_H

#include <string>
#include <StorageMgr_Exceptions.H>
#include <ClosableFifoQueue.H>
#include <RunnableRunner.H>
#include <iolib/include/IOCB.H>
#include <PtMutex.H>
#include <PtCondition.H>


using namespace std;

class ApdMasterRunner;

/*
  NOTE: THIS CLASS IS NOT SAFE FOR CONCURRENT EXECUTIONS OF ITS PUBLIC METHODS.
 */
class AsynchPageDir
{
public:
  // This deletes all of the files used by an AsynchPageDir in the specified
  // directory. Don't execute this while the files are being used by some 
  // AsynchPageDir object, or else you'll make someone very unhappy.
  static void clear(string dirPathname)
    throw (std::exception);

  // Once the responseQueue has been closed by this object, it's safe to delete
  // this object.
  //
  // If the APD has already used the specified directory (and that directly hasn't
  // been subsequently cleaned out), then bytesPerPage *MUST* be the same value
  // that is was before.
  AsynchPageDir(string dirPathname, 
		size_t bytesPerPage,
		int maxConcurrency,
		ClosableFifoQueue<IOCB *> & iocbReplyQueue)
    throw (std::exception);

  virtual ~AsynchPageDir();

  // Submits an IOCB. The specified IOCB may not be modified by the caller
  // until it reappears in the response queue.
  // This will delete the IOCB's existing _pException, if there is one.
  // This fails if the AsynchPageDir has already been closed.
  // It's illegal to have the same IOCB submitted > once at a time.
  void submitOp(IOCB * pOp)
    throw (std::exception);

  // Disables future op submissions. Once the pending ops have completed...
  // (1) the response queue's close() method will be called.
  // (2) This object will be ready for deletion.
  //
  // It's ok to delete this AsynchPageDir once the responseQueue is both closed and empty.
  //
  // Note: The intended shutdown sequence is as follows...
  //  (1) Perform the last submitOp(...) calls if you want to.
  //  (2) Call AsynchPageDir::shutdown().
  //  (3) Retrieve all of the IOCB results from the reply queue until it's empty.
  //  (4) Delete this AsynchPageDir object.
  void shutdown()
    throw (std::exception);

private:
  RunnableRunner * _pThreadRunner;
  ApdMasterRunner * _pApdEngine;
  bool _isShutdown;
  ClosableFifoQueue<IOCB *> & _iocbReplyQueue;
};

#endif
