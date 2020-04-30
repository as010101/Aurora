#include <iolib/include/AsynchPageDir.H>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <sstream>
#include <Runnable.H>
#include <RunnableRunner.H>
#include <PtMutex.H>
#include <LockHolder.H>
#include <PtThreadPool.H>
#include <sm_int_constants.H>


//*******************************************************************************

// Used in test #3
class PageAllocator : public Runnable
{
public:
     PageAllocator(AsynchPageDir & apd,
		   PtMutex & rightToUseApd,
		   int numPagesToAllocate,
		   int minDelayMs,
		   int maxDelayMs);

     void run() throw();

private:
     AsynchPageDir & _apd;
     PtMutex & _rightToUseApd;
     int _numPagesToAllocate;
     int _minDelayMs;
     int _maxDelayMs;
};


PageAllocator::PageAllocator(AsynchPageDir & apd,
			     PtMutex & rightToUseApd,
			     int numPagesToAllocate,
			     int minDelayMs,
			     int maxDelayMs)
     : _apd(apd),
       _rightToUseApd(rightToUseApd),
       _numPagesToAllocate(numPagesToAllocate),
       _minDelayMs(minDelayMs),
       _maxDelayMs(maxDelayMs)
{
}

void PageAllocator::run()
     throw ()
{
     try
     {
	  int delayRange = _maxDelayMs - _minDelayMs + 1;

	  for (int i = 0; i < _numPagesToAllocate; i++)
	  {
	       unsigned delayMs = _minDelayMs + (random() % delayRange);
	       usleep(1000 * delayMs);
	  
	       IOCB * pIOCB = new IOCB();
	       pIOCB->_opType = IOCB::PAGE_ALLOC;
	       {
		    LockHolder h(_rightToUseApd);
		    _apd.submitOp(pIOCB);
	       }
	  }
     }
     catch (std::exception & e)
     {
	  cout << "!!! PageAllocator: Caught std::exception: " << e.what() << endl;
	  cloneAndSetRunException(e);
     }
     catch (...)
     {
	  cout << "!!! PageAllocator: Caught ..." << endl;
	  SmException * pException =
	       new SmException(__FILE__, __LINE__, 
					 "PageAllocator::run(): Caught an exception that wasn't a std::exception");
	  setRunException(pException);
     }
}

//*******************************************************************************

void printUsage()
{
     cout << "Usage: testAsynchPageDir <dirname> <bytes-per-page>" << endl;
}

//*******************************************************************************

int main(int argc, char* argv[])
{
     if (argc != 3)
     {
	  printUsage();
	  return 1;
     }

     string pageDir = argv[1];
     int bytesPerPage = atoi(argv[2]);

     try
     {
	  //=========================================================================
	  // Test 1: Basic startup and shutdown...
	  //=========================================================================

	  cout << "Test 1: Beginning" << endl << endl;

	  {
	       BinarySem closedAndEmptySem(false);
	       ClosableFifoQueue<IOCB*>  iocbReplyQueue(NULL, NULL, & closedAndEmptySem, "testAsynchPageDir.iocbReplyQueue");

	       cout << "\tAbout to instantiate an AsynchPageDir" << endl;
	       AsynchPageDir apd(pageDir, bytesPerPage, 100, iocbReplyQueue);
	
	       // Give the system a chance to get a little busy w/PageDir active growth...
	       sleep(2);

	       cout << "\tAbout to shutdown the AsynchPageDir" << endl;
	       apd.shutdown();

	       cout << "\tAbout to wait on the reply queue's binary semaphore." << endl;
	       closedAndEmptySem.awaitPostThenClear();
	       if (! iocbReplyQueue.isClosed())
	       {
		    throw SmException(__FILE__, __LINE__, "Test 1 failed: reply queue wasn't closed when it should be.");
	       }
	  }

	  cout << "Test 1: Success" << endl << endl;
	

	  //=========================================================================
	  // Test 2: Test growth operations...
	  //=========================================================================

	  cout << "Test 2: Beginning" << endl << endl;

	  {
	       BinarySem closedAndEmptySem(false);
	       ClosableFifoQueue<IOCB*>  iocbReplyQueue(NULL, NULL, & closedAndEmptySem, "testAsynchPageDir.iocbReplyQueue");

	       cout << "\tAbout to instantiate an AsynchPageDir" << endl;
	       AsynchPageDir apd(argv[1], bytesPerPage, 100, iocbReplyQueue);
	
	       vector<IOCB *> allocOps;
	       for (int i = 0; i < 1000; i++)
	       {
		    IOCB * pIOCB = new IOCB();
		    pIOCB->_opType = IOCB::PAGE_ALLOC;
		    allocOps.push_back(pIOCB);
		    apd.submitOp(pIOCB);
	       }

	       // Get all of the IOCB replies.
	       // Make sure that...
	       // (A) There are at least 1000 of them.
	       // (B) They correspond to the submitted ops
	       // (C) None of them has an exception.
	       set<IOCB *> completedAllocOps;
	       for (int i = 0; i < 1000; i++)
	       {
		    IOCB * pIOCB = iocbReplyQueue.dequeueBlocking();
		    if (pIOCB->_pException != NULL)
		    {
			 ostringstream s;
			 s << "An IOCB reply had an exception: " << pIOCB->_pException->what();
			 throw SmException(__FILE__, __LINE__, s.str());
		    }
		    completedAllocOps.insert(pIOCB);
	       }

	       if (completedAllocOps.size() != 1000)
	       {
		    throw SmException(__FILE__, __LINE__, "completedAllocOps.size() != 1000");
	       }

	       for (int i = 0; i < 1000; i++)
	       {
		    if (completedAllocOps.find(allocOps[i]) == completedAllocOps.end())
		    {
			 throw SmException(__FILE__, __LINE__, "At least one submitted alloc op wasn't completed");
		    }
	       }

	       // In one shot, test our ability to (A) resubmit IOCBs as a different op type,
	       // and (B) free pages.
	       for (int i = 0; i < 1000; i++)
	       {
		    IOCB * pIOCB = allocOps[i];
		    pIOCB->_opType = IOCB::PAGE_FREE;
		    apd.submitOp(pIOCB);
	       }

	       set<IOCB *> completedFreeOps;
	       for (int i = 0; i < 1000; i++)
	       {
		    IOCB * pIOCB = iocbReplyQueue.dequeueBlocking();
		    if (pIOCB->_pException != NULL)
		    {
			 ostringstream s;
			 s << "An IOCB reply had an exception: " << pIOCB->_pException->what();
			 throw SmException(__FILE__, __LINE__, s.str());
		    }
		    completedFreeOps.insert(pIOCB);
	       }

	       if (completedFreeOps.size() != 1000)
	       {
		    throw SmException(__FILE__, __LINE__, "completedFreeOps.size() != 1000");
	       }

	       for (int i = 0; i < 1000; i++)
	       {
		    if (completedFreeOps.find(allocOps[i]) == completedAllocOps.end())
		    {
			 throw SmException(__FILE__, __LINE__, "At least one submitted free op wasn't completed");
		    }
	       }

	       // Cleanup...
	       cout << "About to shutdown the AsynchPageDir" << endl;
	       apd.shutdown();

	       cout << "About to wait on the reply queue's binary semaphore." << endl;
	       closedAndEmptySem.awaitPostThenClear();
	       if (! iocbReplyQueue.isClosed())
	       {
		    throw SmException(__FILE__, __LINE__, "Test 2 failed: reply queue wasn't closed when it should be.");
	       }
	  }

	  cout << "Test 2: Success" << endl << endl;

	  //=========================================================================
	  // Test 3: Massive content test. allocs/free/reads/writes
	  //=========================================================================

	  cout << "Test 3: Beginning" << endl << endl;

	  {
	       PtMutex rightToUseApd;

	       // Setup our AsynchPageDir...
	       BinarySem closedAndEmptySem(false);
	       ClosableFifoQueue<IOCB*>  iocbReplyQueue(NULL, NULL, & closedAndEmptySem, "testAsynchPageDir.iocbReplyQueue");

	       cout << "\tAbout to instantiate an AsynchPageDir..." << endl;
	       AsynchPageDir apd(argv[1], bytesPerPage, 100, iocbReplyQueue);

	       // Generate our test pattern data...
	       cout << "\tAbout to generate test pattern..." << endl;
	       const int numPatterns = 5;
	       char * testPatterns[numPatterns];
	       for (int i = 0; i < numPatterns; i++)
	       {
		    testPatterns[i] = new char[bytesPerPage];
		    memset(testPatterns[i], int('a') + int(i % numPatterns), bytesPerPage);
	       }

	       // Begin the paced generation of 50 page allocations...
	       cout << "\tStarting the page allocator..." << endl;
	       PageAllocator pa(PageAllocator(apd, rightToUseApd, 500, 1, 200));
	       RunnableRunner allocRunner(pa);

	       // Begin the process of, for each allocated page: write it, read it, free it...
	       int allocsSeen = 0;
	       int freesSeen = 0;
	       int readsSeen = 0;
	       int writesSeen = 0;
    
	       cout << "\tAbout to handle returned IOCBs..." << endl;

	       while (freesSeen < 500)
	       {
		    IOCB * pIOCB = iocbReplyQueue.dequeueBlocking();
		    if (pIOCB->_pException != NULL)
		    {
			 cout << "Test 3 failed: Found an exception on IOCB: " << pIOCB->debugStr() << endl;
			 throw *(pIOCB->_pException);
		    }

		    switch (pIOCB->_opType)
		    {
		    case IOCB::PAGE_ALLOC:
			 allocsSeen++;
			 pIOCB->_opType = IOCB::PAGE_WRITE;
			 pIOCB->_pBuffer = testPatterns[(pIOCB->_pageAddr._pageNum) % numPatterns];
			 {
			      LockHolder h(rightToUseApd);
			      apd.submitOp(pIOCB);
			 }
			 break;
		    case IOCB::PAGE_WRITE:
			 writesSeen++;
			 pIOCB->_opType = IOCB::PAGE_READ;
			 pIOCB->_pBuffer = new char[bytesPerPage];
			 {
			      LockHolder h(rightToUseApd);
			      apd.submitOp(pIOCB);
			 }
			 break;
		    case IOCB::PAGE_READ:
			 readsSeen++;
			 if (memcmp(pIOCB->_pBuffer, 
				    testPatterns[(pIOCB->_pageAddr._pageNum) % numPatterns], 
				    bytesPerPage) != 0)
			 {
			      throw SmException(__FILE__, __LINE__, "Test 3 failed: The test pattern didn't survive storage");
			 }

			 delete pIOCB->_pBuffer;
			 pIOCB->_pBuffer = NULL;

			 pIOCB->_opType = IOCB::PAGE_FREE;
			 {
			      LockHolder h(rightToUseApd);
			      apd.submitOp(pIOCB);
			 }
			 break;
		    case IOCB::PAGE_FREE:
			 freesSeen++;
			 break;
		    default:
			 throw SmException(__FILE__, __LINE__, "Test 3 failed: Got back an IOCB of unrecognized _opType");
		    }
	       }

	       cout << "Heave I/O load complete:" << endl
		    << "\tallocs seen = " << allocsSeen << endl
		    << "\twrites seen = " << writesSeen << endl
		    << "\treads seen = " << readsSeen << endl
		    << "\tfrees seen = " << freesSeen << endl << endl;
  
	       allocRunner.join();

	       // Cleanup...
	       cout << "About to shutdown the AsynchPageDir" << endl;
	       apd.shutdown();

	       cout << "About to wait on the reply queue's binary semaphore." << endl;
	       closedAndEmptySem.awaitPostThenClear();
	       if (! iocbReplyQueue.isClosed())
	       {
		    throw SmException(__FILE__, __LINE__, "Test 3 failed: reply queue wasn't closed when it should be.");
	       }
	  }

	  cout << "Test 3: Success" << endl << endl;

	  //=========================================================================

	  cout << "############### TEST COMPLETE #################" << endl;
     }
     catch (SmException & e)
     {
	  cout << "Caught exception: " << typeid(e).name() << endl
	       << "\tLocation = " << e.getLocation() << endl
	       << "\tLine num = " << e.getLineNum() << endl
	       << "\tDescription = " << e.getDescription() << endl;
	  return 1;
     }
     catch (std::exception & e)
     {
	  cout << "Caught an exception: " << e.what() << endl;
	  return 1;
     }
     catch (...)
     {
	  cout << "Caught some exception other than a std::exception!" << endl;
	  return 1;
     }
}
