#include <iolib/include/AsynchPageDir.H>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <vector>
#include <set>
#include <sstream>
#include <map>
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
	  if (_maxDelayMs > 0)
	    {
	      unsigned delayMs = _minDelayMs + (random() % delayRange);
	      usleep(1000 * delayMs);
	    }
	  
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

// Returns the number of milliseconds required to specified number of a/w/r/f op
// sequences.
int benchmarkAllOps(string dirpath, size_t bytesPerPage, int numOpsSequences, int maxConcurrency, 
		    int minMsBetweenAllocs, int maxMsBetweenAllocs)
  throw (std::exception)
{
  PtMutex rightToUseApd;

  // Setup our AsynchPageDir...
  BinarySem closedAndEmptySem(false);
  ClosableFifoQueue<IOCB*>  iocbReplyQueue(NULL, NULL, & closedAndEmptySem, "testAsynchPageDir.iocbReplyQueue");

  AsynchPageDir apd(dirpath, bytesPerPage, maxConcurrency, iocbReplyQueue);

  // Generate our test pattern data...
  const int numPatterns = 5;
  char * testPatterns[numPatterns];
  for (int i = 0; i < numPatterns; i++)
    {
      testPatterns[i] = new char[bytesPerPage];
      memset(testPatterns[i], int('a') + int(i % numPatterns), bytesPerPage);
    }

  // Begin the paced generation of 50 page allocations...
  PageAllocator pa(PageAllocator(apd, rightToUseApd, numOpsSequences, 
				 minMsBetweenAllocs, maxMsBetweenAllocs));

  struct timeval startTime;
  int rc = gettimeofday(& startTime, NULL);
  if (rc != 0)
    {
      throw SmException(__FILE__, __LINE__, "Failed call to gettimeofday(...)");
    }

  RunnableRunner allocRunner(pa);

  // Begin the process of, for each allocated page: write it, read it, free it...
  int allocsSeen = 0;
  int freesSeen = 0;
  int readsSeen = 0;
  int writesSeen = 0;

  // Key = # of r/w/g ops already running when the r/w op began execution
  // Dependent value = # of r/w ops encountering this level of preexisting concurrency.
  map<int, int> concurrencyMap;

  while (freesSeen < numOpsSequences)
    {
      IOCB * pIOCB = iocbReplyQueue.dequeueBlocking();
      if (pIOCB->_pException != NULL)
	{
	  cout << "Found an exception on IOCB: " << pIOCB->debugStr() << endl;
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

	  concurrencyMap[pIOCB->_opsRunningWhenStarted]++;

	  pIOCB->_opType = IOCB::PAGE_READ;
	  pIOCB->_pBuffer = new char[bytesPerPage];
	  {
	    LockHolder h(rightToUseApd);
	    apd.submitOp(pIOCB);
	  }
	  break;
	case IOCB::PAGE_READ:
	  readsSeen++;

	  concurrencyMap[pIOCB->_opsRunningWhenStarted]++;

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
	  throw SmException(__FILE__, __LINE__, "Got back an IOCB of unrecognized _opType");
	}
    }


  struct timeval endTime;
  rc = gettimeofday(& endTime, NULL);
  if (rc != 0)
    {
      throw SmException(__FILE__, __LINE__, "Failed call to gettimeofday(...)");
    }

  // Dump our results
  /*
  cout << "Concurrency Histogram: (Num. r/w/g ops already running, # r/w ops in this scenario)" << endl;
  for (map<int, int>::iterator pos = concurrencyMap.begin(); 
       pos != concurrencyMap.end(); 
       ++pos)
    {
      cout << pos->first << ", " << pos->second << endl;
    }
  cout << endl;
  */

  allocRunner.join();

  // Cleanup...
  apd.shutdown();

  closedAndEmptySem.awaitPostThenClear();
  if (! iocbReplyQueue.isClosed())
    {
      throw SmException(__FILE__, __LINE__, "reply queue wasn't closed when it should be.");
    }

  long secDiff = endTime.tv_sec - startTime.tv_sec;
  long usecDiff = endTime.tv_usec - startTime.tv_usec;
  long msecDiff = (secDiff * 1000) + (usecDiff / 1000);
  return int(msecDiff);
}

//*******************************************************************************

void printUsage()
{
  cout << "Usage: benchmarkAsynchPageDir <dirname>" << endl;
}

//*******************************************************************************

void runRepitions(string dirpath, size_t bytesPerPage, int numOpsSequences, 
		  int maxConcurrency, int minMsBetweenAllocs, 
		  int maxMsBetweenAllocs, int numRepitions)
{
  cout << "========================================================================" << endl
       << "maxConcurrency = " << maxConcurrency << endl
       << "Page size = " << bytesPerPage << " bytes" << endl
       << "Total op sequences per run: " << numOpsSequences << endl
       << "========================================================================" << endl
       << endl;

  double totalBytes = 0;
  double totalMs = 0;

  for (int i = 1; i <= numRepitions; i++)
    {
      AsynchPageDir::clear(dirpath);

      long runMs = benchmarkAllOps(dirpath, bytesPerPage, numOpsSequences, 
				       maxConcurrency, minMsBetweenAllocs, 
				       maxMsBetweenAllocs);

      long rwBytes = numOpsSequences * 2 * bytesPerPage;
    
      totalBytes += rwBytes;
      totalMs += runMs;

      /*
      double roundKB = double(rwBytes) / 1024;
      double roundSeconds = double(runMs) / 1000;
      cout << "\trun #" << i << ": " << runMs << " milliseconds" << endl
	   << "\tTotal r/w bytes = " << rwBytes << endl
	   << "\tAvg. xfer rate = " << int(round(roundKB / roundSeconds)) << " KB/s" << endl << endl;
      */
    }

  double totalKB = double(totalBytes) / 1024;
  double totalSeconds = double(totalMs) / 1000;
  cout << "\t*** Avg. xfer rate for all runs: " << int(round(totalKB / totalSeconds)) << " KB/s ***" << endl << endl;
}

//*******************************************************************************

int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      printUsage();
      return 1;
    }

  try
    {
      int minDelayMs = 0;
      int maxDelayMs = 0;
      int totalOpSequences = 2500;

      size_t bytesPerPage = 512;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 1024;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 2048;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 4096;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 8192;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 16384;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 32768;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 65536;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);

      bytesPerPage = 131072;
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 1,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 2,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 5,    minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 10,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 20,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 50,   minDelayMs, maxDelayMs, 10);
      runRepitions(argv[1], bytesPerPage, totalOpSequences, 100,  minDelayMs, maxDelayMs, 10);
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
