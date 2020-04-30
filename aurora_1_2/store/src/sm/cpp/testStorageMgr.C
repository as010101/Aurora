#include <sm/include/StorageMgr.H>
#include <LockHolder.H>
#include <iostream>
#include <sstream>

#include <unistd.h>
#include <sys/time.h>

//===============================================================================

void printUsage()
{
  cout << "testStorageMgr <# frames> <working-dir1> ... <working-dir5>" << endl;
}

//===============================================================================

// Prints to 'cout' the SM's qview stats for each qview in the range:
//    {qbase = 'lowQbaseId',  qview = 'qviewId'} ...
//    {qbase = 'highQbaseId', qview = 'qviewId'}
// 
// The caller *MUST* hold the SM's lock when invoking this method.
void printQviewInfo(StorageMgr & sm, int lowQbaseId, int highQbaseId, int qviewId)
  throw (exception)
{
  if (lowQbaseId > highQbaseId)
    {
      throw SmException(__FILE__, __LINE__, "lowQbaseId > highQbaseId");
    }

  for (int qbaseId = lowQbaseId; qbaseId <= highQbaseId; ++qbaseId)
    {
      cout << "Info for qview {" << qbaseId << ", " << qviewId << "}" << endl;

      size_t numRecsInQview;
      size_t numRecsFramed;
      timeval avgTimestamp;
      sm.getQviewInfo(qbaseId, qviewId, numRecsInQview, numRecsFramed, avgTimestamp);
      
      cout << "   # records in qview: " << numRecsInQview << endl
	   << "   # records framed (at head of qview): " << numRecsFramed << endl;
    }

  cout << endl;
} 

//===============================================================================

// If tvals isn't empty, then it must have a size of 'numRecords'. When tvals is
// empty, this function will call 'gettimeofday(...)' to come up with the 
// timestamps for the tuples. When tvals isn't empty, it explicitely gives the
// timestamps for the records.
void enqRecords(StorageMgr & sm, int superBoxId, int qbaseId, 
		size_t bytesPerRecord, size_t numRecords, char firstLetter,
		const vector<timeval> & tvals)
  throw (exception)
{
  if ((firstLetter < 'A') || (firstLetter > 'Z'))
    {
      throw SmException(__FILE__, __LINE__, "Invalid firstLetter");
    }

  char c = firstLetter;
  size_t tuplesToCopy = numRecords;
  size_t recIdx = 0;

  LockHolder lh(sm);

  while (tuplesToCopy > 0)
    {
      lh.release();
      sm.awaitEnqPossible(superBoxId, qbaseId);
      lh.reacquire();

      size_t numRecs;
      char * pBuffer;
      sm.getEnqBuffer(superBoxId, qbaseId, numRecs, pBuffer);
      assert(numRecs > 0);

      size_t copiedThisRound = 0;
      for (size_t i = 0; (i <numRecs) && (tuplesToCopy > 0); ++i)
	{
	  char * pSlot = pBuffer + (i * bytesPerRecord);

	  if (tvals.empty())
	    {
	      if (gettimeofday(reinterpret_cast<timeval *>(pSlot), NULL) != 0)
		{
		  ostringstream os;
		  os << "gettimeofday(...) failed. errno = " << errno;
		  throw SmException(__FILE__, __LINE__, os.str());
		}
	    }
	  else
	    {
	      memcpy(pSlot, & (tvals[recIdx]), sizeof(timeval));
	    }

	  memset(pSlot + sizeof(timeval), c, bytesPerRecord - sizeof(timeval));

	  --tuplesToCopy;
	  ++copiedThisRound;
	  ++recIdx;

	  c += 1;
	  if (c > 'Z')
	    {
	      c = 'A';
	    }
	}
    
      sm.releaseEnqBuffer(superBoxId, qbaseId, copiedThisRound);
    }
}

//===============================================================================


void deqRecords(StorageMgr & sm, int superBoxId, int qbaseId, int qviewId,
		size_t bytesPerRecord, size_t totalRecsToDeq, char firstLetter)
  throw (exception)
{
  if ((firstLetter < 'A') || (firstLetter > 'Z'))
    {
      throw SmException(__FILE__, __LINE__, "Invalid firstLetter");
    }

  char c = firstLetter;
  size_t tuplesToCopy = totalRecsToDeq;

  LockHolder lh(sm);

  while (tuplesToCopy > 0)
    {
      lh.release();
      sm.awaitDeqPossible(superBoxId, qbaseId, qviewId);
      lh.reacquire();

      size_t numRecsInBuffer;
      unsigned long firstSeqNum;
      const char * pBuffer;

      sm.getDeqBuffer(superBoxId, qbaseId, qviewId, numRecsInBuffer, pBuffer, firstSeqNum);
      assert(numRecsInBuffer > 0);

      size_t copiedThisRound = 0;
      for (size_t i = 0; (i < size_t(numRecsInBuffer)) && (tuplesToCopy > 0); ++i)
	{
	  const char * pSlot = pBuffer + (i * bytesPerRecord);

	  // Maybe do something with the timestamp later...
	  timeval tv;
	  memcpy(& tv, pSlot, sizeof(tv));
	  
	  for (size_t j = sizeof(tv); j < bytesPerRecord; ++j)
	    {
	      if (pSlot[j] != c)
		{
		  ostringstream os;
		  os << "Unexpected character read. Expected '" << c 
		     << "' but found '" << pSlot[j] << "' in tuple offset " << j;
		  throw SmException(__FILE__, __LINE__, os.str());
		}
	    }

	  --tuplesToCopy;
	  ++copiedThisRound;

	  c += 1;
	  if (c > 'Z')
	    {
	      c = 'A';
	    }
	}

      sm.releaseDeqBuffer(superBoxId, qbaseId, qviewId, int(copiedThisRound));
    }
}

//===============================================================================

// For the given superbox, this waits for the step to be runnable, runs it, and
// then marks the step as done. This returns 'true' iff there are more steps 
// still in the superbox.
//
// By 'running a box', we mean:
// 1. It drains all the tuples from each input qview, ensuring that for each 
//    qview the first tuple has all 'A' characters, the second has all 'B'
//    characters, etc. (After 'Z', we start at 'A' again.)
// 2. It fills every output qbase with the number of tuples specified by the
//    step's "number of expected enqueues" specification for that qbase. As with
//    the qviews, the records produces will be all 'A's, then all 'B's, etc.
// 
// Throws an exception if any problem is found, including the content of the 
// tuple data.
//
// *** LOCKING ***
// Do *NOT* already hold the lock on the storage manager when calling this
// function. This function needs to release and acquire the lock on its own.
bool stepRunnerSimpleFill(StorageMgr & sm,
			  int superBoxId, 
			  const map<int, size_t> & qbaseBytesPerRecordMap,
			  const map< pair<int, int>, size_t> & qviewBytesPerRecordMap)
  throw (exception)
{
  LockHolder lh(sm);

  BinarySem readyFlag;
  if (! sm.notifyWhenSuperBoxStepRunnable(superBoxId, readyFlag))
    {
      lh.release();
      readyFlag.awaitPostThenClear();
      lh.reacquire();
    }

  const SuperBoxPlanStep & currentStep = sm.getCurrentSuperBoxStep(superBoxId);

  // Dequeue the old records...
  for (size_t i = 0; i < currentStep.getInputQviewIds().size(); ++i)
    {
      int qbaseId = currentStep.getInputQviewIds()[i].first;
      int qviewId = currentStep.getInputQviewIds()[i].second;

      size_t numRecsInQview, numRecsFramed;
      timeval avgTimestamp;
      sm.getQviewInfo(qbaseId, qviewId, numRecsInQview, numRecsFramed, avgTimestamp);

      if (numRecsInQview > 0)
	{
	  map< pair<int, int>, size_t>::const_iterator pos = 
	    qviewBytesPerRecordMap.find(make_pair(qbaseId, qviewId));
	  if (pos == qviewBytesPerRecordMap.end())
	    {
	      throw SmException(__FILE__, __LINE__, "Couldn't find qviewId record size");
	    }
	  size_t bytesPerRecord = pos->second;
	  
	  lh.release();
	  deqRecords(sm, superBoxId, qbaseId, qviewId, bytesPerRecord, numRecsInQview, 'A');
	  lh.reacquire();
	}
    }

  // Enqueue the new records...
  for (size_t i = 0; i < currentStep.getOutputQbaseIds().size(); ++i)
    {
      int qbaseId = currentStep.getOutputQbaseIds()[i];
      size_t numRecords = currentStep.getNumExpectedEnq()[i];

      map<int, size_t>::const_iterator pos = qbaseBytesPerRecordMap.find(qbaseId);
      if (pos == qbaseBytesPerRecordMap.end())
	{
	  throw SmException(__FILE__, __LINE__, "Couldn't find qbaseId record size");
	}
      size_t bytesPerRecord = pos->second;

      lh.release();
      enqRecords(sm, superBoxId, qbaseId, bytesPerRecord, numRecords, 'A', vector<timeval>());
      lh.reacquire();
    }

  return sm.onSuperBoxStepComplete(superBoxId);
}

//===============================================================================

// Creates a superbox schedule with the specified qualities. 
//
// The collections 'schedule', 'qbaseBytesPerRecordMap', and 
// 'qviewBytesPerRecordMap' will be added to in reflection of the schedule that's
// generated. (If either of the two maps already has an entry for a given key 
// that this function generates, an exception will be thrown if the map indicates
// a different record size than this function generates.)
// Note: This function does *not* clear 'schedule' for appending to it.
//
// Iff 'createSmObjects' is true, then this function will create all qbase and
// qview objecst that are used by the schedule.
//
// All qbases created will have id#s in the range 
//     [firstQbaseId, firstQbaseId + numSteps - 1].
//
// Schedule details:
//    - schedule[0] will just have an output qbase.
//    - schedule[numSteps - 1] will just have an input qview
//    - all other steps will have an input qview and an output qbase.
//    - The qbases/qviews are strung together in the simple linear sequence.
//
// *** LOCKING ***
// Caller must hold the lock on sm when invoking this method.
void makeScheduleLinear(StorageMgr & sm,
			int firstQbaseId, 
			size_t numSteps,
			size_t numPasses,
			size_t bytesPerRecord,
			size_t numRecsEmittedPerStep,
			bool createSmObjects, 
			vector<SuperBoxPlanStep> & schedule,
			map<int, size_t> & qbaseBytesPerRecordMap,
			map< pair<int, int>, size_t> & qviewBytesPerRecordMap)
  throw (exception)
{
  if (numSteps < 1)
    {
      throw SmException(__FILE__, __LINE__, "numSteps < 1");
    }

  if (numPasses < 1)
    {
      throw SmException(__FILE__, __LINE__, "numSteps < 1");
    }

  vector<pair<int, int> > inputQviewIds;
  vector<int> outputQbaseIds;
  vector<size_t> numExpectedEnq;

  // Make the objects...
  if (createSmObjects)
    {
      vector<int> qviewIds;
      qviewIds.push_back(1);

      for (size_t i = 0; i < numSteps; ++i)
	{
	  int qbaseId = firstQbaseId + i; 
	  sm.createQ(qbaseId, qviewIds, bytesPerRecord);
	}
    }

  // Make the schedule...
  for (size_t passNum = 0; passNum < numPasses; ++passNum)
    {
      for (size_t i = 0; i < numSteps; ++i)
	{
	  inputQviewIds.clear();
	  outputQbaseIds.clear();
	  numExpectedEnq.clear();

	  int qbaseId = firstQbaseId + i;


	  vector<pair<int, int> > inputQviewIds;
	  vector<int> outputQbaseIds;
	  vector<size_t> numExpectedEnq;

	  // Setup the qview details...
	  if (i > 0)
	    {
	      pair<int, int> qviewPair(qbaseId - 1, 1);

	      inputQviewIds.push_back(qviewPair);

	      // Setup its record size map...
	      map<pair<int, int>, size_t>::iterator pos = 
		qviewBytesPerRecordMap.find(qviewPair);

	      if (pos != qviewBytesPerRecordMap.end())
		{
		  if (pos->second != bytesPerRecord)
		    {
		      throw SmException(__FILE__, __LINE__, "Record size mismatch");
		    }
		}
	      else
		{
		  qviewBytesPerRecordMap.insert(make_pair(qviewPair, bytesPerRecord));
		}
	    }

	  outputQbaseIds.push_back(qbaseId);
	  numExpectedEnq.push_back(numRecsEmittedPerStep);

	  // Setup its record size map...
	  map<int, size_t>::iterator pos = qbaseBytesPerRecordMap.find(qbaseId);
	  if (pos != qbaseBytesPerRecordMap.end())
	    {
	      if (pos->second != bytesPerRecord)
		{
		  throw SmException(__FILE__, __LINE__, "Record size mismatch");
		}
	    }
	  else
	    {
	      qbaseBytesPerRecordMap.insert(make_pair(qbaseId, bytesPerRecord));
	    }
      
	  // Make the step. Use output qbaseId as the boxId, just for convenience
	  // and debugging...
	  schedule.push_back(SuperBoxPlanStep(outputQbaseIds[0], inputQviewIds, 
					      outputQbaseIds,    numExpectedEnq));
	}
    }
}

//===============================================================================

void test1(unsigned long numPageFrames, string dirname)
  throw (exception)
{
  cout << "*************************************************************" << endl
       << "*                       ENTERING TEST 1                     *" << endl
       << "*************************************************************" << endl
       << endl;

  // Test parameters...
  size_t bytesPerPage          = 65536;
  size_t bytesPerRecord        = 1000;
  size_t maxConcurrency        = 100;
  int    firstQbaseId          = 1;
  size_t numSteps              = 10; //2; //500;
  size_t numPasses             = 1;
  size_t numRecsEmittedPerStep = 2000; //70; //70; //2000;

  StorageMgr * p = new StorageMgr(dirname, bytesPerPage, maxConcurrency, numPageFrames);
  StorageMgr & sm = *p;
  
  try
    {
      map<int, size_t> qbaseBytesPerRecordMap;
      map< pair<int, int>, size_t> qviewBytesPerRecordMap;
	
      {
	LockHolder lh(sm);
	  
	vector<SuperBoxPlanStep> schedule;
	makeScheduleLinear(sm, firstQbaseId, numSteps, numPasses, bytesPerRecord, 
			   numRecsEmittedPerStep, true, schedule, 
			   qbaseBytesPerRecordMap, qviewBytesPerRecordMap);
	  
	sm.scheduleSuperBox(1, schedule);
      }
	
      bool hasMoreSteps = false;
      do 
	{
	  hasMoreSteps = stepRunnerSimpleFill(sm, 1, qbaseBytesPerRecordMap, 
					      qviewBytesPerRecordMap);
	}
      while (hasMoreSteps);
	
      // Cleanup...
      {
	LockHolder lh(sm);
	sm.shutdown(); 
      }
    }
  catch (exception & e)
    {
      cout << "test1: Caught exception " << e.what() << endl;
      throw;
    }

  cout << "*************************************************************" << endl
       << "*                        PASSED TEST 1                      *" << endl
       << "*************************************************************" << endl
       << endl;
}

//===============================================================================

// This tests our ability to save a set of data and then resume its execution.
void test2(unsigned long numPageFrames, string dirname)
  throw (exception)
{
  cout << "*************************************************************" << endl
       << "*                       ENTERING TEST 2                     *" << endl
       << "*************************************************************" << endl
       << endl;

  // Test parameters...
  size_t bytesPerPage          = 65536;
  size_t bytesPerRecord        = 1000;
  size_t maxConcurrency        = 100;
  int    firstQbaseId          = 1;
  size_t numSteps              = 10;
  size_t numPasses             = 2;
  size_t numRecsEmittedPerStep = 2000;


  map<int, size_t> qbaseBytesPerRecordMap;
  map< pair<int, int>, size_t> qviewBytesPerRecordMap;
  vector<SuperBoxPlanStep> masterSchedule; 
  size_t secondRunFirstStep = (numSteps / 2) + 1;
  
  // Step 1: Create the master schedule (which we'll divide later), and the
  // database objects we'll work on...
  try
    {
      StorageMgr sm(dirname, bytesPerPage, maxConcurrency, numPageFrames);

      vector<SuperBoxPlanStep> subSchedule;

      {
	LockHolder lh(sm);
	makeScheduleLinear(sm, firstQbaseId, numSteps, numPasses, bytesPerRecord, 
			   numRecsEmittedPerStep, true, masterSchedule, 
			   qbaseBytesPerRecordMap, qviewBytesPerRecordMap);

	// Produce the subschedule...
	for (size_t i = 1; i < secondRunFirstStep; ++i)
	  {
	    subSchedule.push_back(masterSchedule.at(i-1));
	  }

	sm.scheduleSuperBox(1, subSchedule);
      }

      for (size_t stepNum = 1; stepNum < secondRunFirstStep; ++stepNum)
	{
	  bool hasMoreSteps = stepRunnerSimpleFill(sm, 1, qbaseBytesPerRecordMap, 
						   qviewBytesPerRecordMap);

	  if ((stepNum != (secondRunFirstStep - 1)) && (! hasMoreSteps))
	    {
	      ostringstream os;
	      os << "Ran out of steps prematurely. stepNum = " << stepNum;
	      throw SmException(__FILE__, __LINE__, os.str());
	    }
	  else if ((stepNum ==  (secondRunFirstStep - 1)) && hasMoreSteps)
	    {
	      ostringstream os;
	      os << "Didn't run out of steps when it should have. stepNum = " << stepNum;
	      throw SmException(__FILE__, __LINE__, os.str());
	    }
	}
	
      // Cleanup...
      {
	LockHolder lh(sm);
	sm.shutdown(); 
      }
    }
  catch (exception & e)
    {
      cout << "test2: Caught exception " << e.what() << endl;
      throw;
    }

  // Step 2: Reload from disk, give a revised schedule that picks up where the
  // last one left off, and see if it works...
  try
    {
      StorageMgr sm(dirname, bytesPerPage, maxConcurrency, numPageFrames);

      {
	LockHolder lh(sm);

	masterSchedule.clear();
	makeScheduleLinear(sm, firstQbaseId, numSteps, numPasses, bytesPerRecord, 
			   numRecsEmittedPerStep, false, masterSchedule, 
			   qbaseBytesPerRecordMap, qviewBytesPerRecordMap);

	// Produce the subschedule...
	vector<SuperBoxPlanStep> subSchedule;
	for (size_t i = secondRunFirstStep; i <= masterSchedule.size(); ++i)
	  {
	    subSchedule.push_back(masterSchedule.at(i-1));
	  }

	sm.scheduleSuperBox(1, subSchedule);
      }

      for (size_t stepNum = secondRunFirstStep; stepNum <= masterSchedule.size(); ++stepNum)
	{
	  bool hasMoreSteps = stepRunnerSimpleFill(sm, 1, qbaseBytesPerRecordMap, 
						   qviewBytesPerRecordMap);
	    
	  if ((stepNum != masterSchedule.size()) && (! hasMoreSteps))
	    {
	      ostringstream os;
	      os << "Ran out of steps prematurely. stepNum = " << stepNum;
	      throw SmException(__FILE__, __LINE__, os.str());
	    }
	  else if ((stepNum == masterSchedule.size()) && hasMoreSteps)
	    {
	      ostringstream os;
	      os << "Didn't run out of steps when it should have. stepNum = " << stepNum;
	      throw SmException(__FILE__, __LINE__, os.str());
	    }
	}
	
      // Cleanup...
      {
	LockHolder lh(sm);
	sm.shutdown(); 
      }
    }
  catch (exception & e)
    {
      cout << "test2: Caught exception " << e.what() << endl;
      throw;
    }

  cout << "*************************************************************" << endl
       << "*                        PASSED TEST 2                      *" << endl
       << "*************************************************************" << endl
       << endl;
}

//===============================================================================

class Test3WorkerRunnable : public Runnable
{
public:
  Test3WorkerRunnable(int superBoxId,
		      StorageMgr & sm,
		      int firstQbaseId, 
		      size_t numSteps,
		      size_t numPasses,
		      size_t bytesPerRecord,
		      size_t numRecsEmittedPerStep)
    : _superBoxId(superBoxId),
      _sm(sm),
      _firstQbaseId(firstQbaseId), 
      _numSteps(numSteps),
      _numPasses(numPasses),
      _bytesPerRecord(bytesPerRecord),
      _numRecsEmittedPerStep(numRecsEmittedPerStep)
  {
  }

  //-----------------------------------------------------------------------------

  virtual void run()
    throw ()
  {
    cout << ">>> BEGINNING RUN OF THREAD " << pthread_self() << endl;
    try
      {
	map<int, size_t> qbaseBytesPerRecordMap;
	map< pair<int, int>, size_t> qviewBytesPerRecordMap;
	
	{
	  LockHolder lh(_sm);
	  
	  vector<SuperBoxPlanStep> schedule;
	  makeScheduleLinear(_sm, _firstQbaseId, _numSteps, _numPasses, 
			     _bytesPerRecord, _numRecsEmittedPerStep, true, 
			     schedule, qbaseBytesPerRecordMap, 
			     qviewBytesPerRecordMap);
	
	  _sm.scheduleSuperBox(_superBoxId, schedule);
	}
	
	bool hasMoreSteps = false;
	do 
	  {
	    hasMoreSteps = stepRunnerSimpleFill(_sm, _superBoxId, qbaseBytesPerRecordMap, 
						qviewBytesPerRecordMap);
	  }
	while (hasMoreSteps);
      }
    catch (exception & e)
      {
	cout << "Thread " << pthread_self() << " caught exception " << e.what() << endl;
	cloneAndSetRunException(e);
      }
    cout << ">>> ENDING RUN OF THREAD " << pthread_self() << endl;
  }

  //-----------------------------------------------------------------------------

private:
  int _superBoxId;
  StorageMgr & _sm;
  int _firstQbaseId;
  size_t _numSteps;
  size_t _numPasses;
  size_t _bytesPerRecord;
  size_t _numRecsEmittedPerStep;
};

//===============================================================================

class Test3StatsMonRunnable : public Runnable
{
public:
  Test3StatsMonRunnable(StorageMgr & sm,
		      BinarySem & quitFlag)
    : _sm(sm),
      _quitFlag(quitFlag)
  {
  }

  //-----------------------------------------------------------------------------

  virtual void run()
    throw ()
  {
    while (! _quitFlag.isPosted())
      {
	SmRuntimeStatsSnapshot stats;

	{
	  LockHolder lh(_sm);
	  stats = _sm.getRuntimeStatsSnapshot();
	}

	cout << "SM runtime state: " << endl << stats.getAsString() << endl << endl;
	sleep(1);
      }
  }

private:
  StorageMgr & _sm;
  BinarySem & _quitFlag;
};

//===============================================================================

void test3(unsigned long numPageFrames, string dirname)
  throw (exception)
{
  cout << "*************************************************************" << endl
       << "*                       ENTERING TEST 3                     *" << endl
       << "*************************************************************" << endl
       << endl;

  size_t bytesPerPage          = 65536;
  size_t bytesPerRecord        = 1000;
  size_t maxConcurrency        = 100;
  size_t numThreads            = 10;
  size_t stepsPerThread        = 20;
  size_t passesPerThread       = 1;
  size_t numRecsEmittedPerStep = 1000;

  try
    {
      StorageMgr sm(dirname, bytesPerPage, maxConcurrency, numPageFrames);

      BinarySem quitFlag;
      RunnableRunner statsMonRunner(* (new Test3StatsMonRunnable(sm, quitFlag)));

      vector<Runnable *> runnables;
      for (size_t i = 0; i < numThreads; ++i)
	{
	  int firstQbaseId = i * stepsPerThread;

	  runnables.push_back(new Test3WorkerRunnable(i, 
						      sm, 
						      firstQbaseId, 
						      stepsPerThread, 
						      passesPerThread, 
						      bytesPerRecord,
						      numRecsEmittedPerStep));
	}

      vector<RunnableRunner *> runners;
      for (size_t i = 0; i < numThreads; ++i)
	{
	  runners.push_back(new RunnableRunner(* (runnables[i])));
	}

      for (size_t i = 0; i < numThreads; ++i)
	{
	  runners[i]->join(); 
	}

      quitFlag.post();
      statsMonRunner.join();
      delete statsMonRunner.getRunnable();

      for (size_t i = 0; i < numThreads; ++i)
	{
	  size_t idx = (numThreads - 1) - i;
	  assert(runners[idx]->getRunnable() == runnables[idx]);
	  assert(idx == (runners.size()   - 1));
	  assert(idx == (runnables.size() - 1));

	  exception * pException = runnables[idx]->getRunException();
	  if (pException != NULL)
	    {
	      SmException * pSmException = 
		dynamic_cast<SmException *>(pException);

	      if (pSmException != NULL)
		{
		  throw SmException(* pSmException);
		}
	      else
		{
		  throw exception(*pException);
		}
	    }

	  delete runnables[idx];
	  runnables.pop_back();

	  delete runners[idx];
	  runners.pop_back();
	}
      {
	LockHolder lh(sm);
	sm.shutdown(); 
      }
    }
  catch (exception & e)
    {
      cout << "test3: Caught exception " << e.what() << endl;
      throw;
    }

  cout << "*************************************************************" << endl
       << "*                        PASSED TEST 3                      *" << endl
       << "*************************************************************" << endl
       << endl;
}

//===============================================================================

// This verifies that getNonEmptyQviews() works properly.
void test4(unsigned long numPageFrames, string dirname)
  throw (exception)
{
  cout << "*************************************************************" << endl
       << "*                       ENTERING TEST 4                     *" << endl
       << "*************************************************************" << endl
       << endl;

  size_t bytesPerPage          = 65536;
  size_t bytesPerRecord        = 1000;
  size_t maxConcurrency        = 100;

  try
    {
      {
	StorageMgr sm(dirname, bytesPerPage, maxConcurrency, numPageFrames);
	LockHolder lh(sm);

	const set<pair<int, int> > & nonEmptyQviews = sm.getNonEmptyQviews();

	if (! nonEmptyQviews.empty())
	  {
	    throw SmException(__FILE__, __LINE__, "! nonEmptyQviews.empty()"); 
	  }

	// Setup a qbase with two qviews, and another qbase that we're not going
	// to use:
	// qbaseId = 1, qviewIds = {2, 3}
	// qbaseId = 4, qviewIds = {5}
	vector<int> qviewIds;

	qviewIds.push_back(2);
	qviewIds.push_back(3);
	sm.createQ(1, qviewIds, bytesPerRecord);

	qviewIds.clear();
	qviewIds.push_back(5);
	sm.createQ(4, qviewIds, bytesPerRecord);

	// Schedule a superbox whose first step will give us access to all of the
	// objects we need for our test...
	vector<pair<int, int> > inputQviews;
	vector<int> outputQbases;

	inputQviews.push_back(make_pair(1, 2));
	inputQviews.push_back(make_pair(1, 3));
	inputQviews.push_back(make_pair(4, 5));
	outputQbases.push_back(1);
	outputQbases.push_back(4);

	vector<SuperBoxPlanStep> schedule;
	vector<size_t> numExpectedEnq;

	int superBoxId = 1;
	numExpectedEnq.push_back(1);
	schedule.push_back(SuperBoxPlanStep(1, inputQviews, outputQbases, 
					    numExpectedEnq));
	sm.scheduleSuperBox(superBoxId, schedule);

	BinarySem readyFlag;
	if (! sm.notifyWhenSuperBoxStepRunnable(superBoxId, readyFlag))
	  {
	    lh.release();
	    readyFlag.awaitPostThenClear();
	    lh.reacquire();
	  }

	// Ensure that enqueueing a tuple makes the corresponding qviews become
	// non-empty...
	{
	  lh.release();
	  sm.awaitEnqPossible(superBoxId, 1);
	  lh.reacquire();
	}

	size_t numRecs;
	char * pEnqBuffer;
	sm.getEnqBuffer(superBoxId, 1, numRecs, pEnqBuffer);

	gettimeofday(reinterpret_cast<timeval *>(pEnqBuffer), NULL);
	sm.releaseEnqBuffer(superBoxId, 1, 1);

	bool found1_2 = false;
	bool found1_3 = false;

	for (set<pair<int, int> >::const_iterator pos = nonEmptyQviews.begin();
	     pos != nonEmptyQviews.end(); 
	     ++pos)
	  {
	    if ((*pos) == make_pair(1, 2))
	      {
		found1_2 = true;
	      }
	    else if ((*pos) == make_pair(1, 3))
	      {
		found1_3 = true;
	      }
	    else
	      {
		ostringstream os;
		os << "Found unexpected qview: <" << pos->first << ", " << pos->second << ">";
		throw SmException(__FILE__, __LINE__, os.str());
	      }
	  }
	
	if ((! found1_2) || (! found1_3))
	  {
	    throw SmException(__FILE__, __LINE__, "Didn't find at least one expected qview");
	  }

	// Ensure that dequeuing a tuple makes the corresponding qview become
	// empty...
	{
	  lh.release();
	  sm.awaitDeqPossible(superBoxId, 1, 3);
	  lh.reacquire();
	}

	sm.releaseDeqBuffer(superBoxId, 1, 3, 1);

	if (nonEmptyQviews.size() != 1)
	  {
	    throw SmException(__FILE__, __LINE__, "nonEmptyQviews.size() != 1");
	  }

	set<pair<int, int> >::const_iterator pos = nonEmptyQviews.begin();
	assert(pos != nonEmptyQviews.end());
	
	if ((*pos) != make_pair(1, 2))
	  {
	    ostringstream os;
	    os << "Found unexpected qview: <" << pos->first << ", " << pos->second << ">";
	    throw SmException(__FILE__, __LINE__, os.str());
	  }

	// Cleanup...
	sm.onSuperBoxStepComplete(superBoxId);
	sm.shutdown();
      }


      // Verify that the set of non-empty qviews is properly restored upon 
      // reload...
      {
	StorageMgr sm(dirname, bytesPerPage, maxConcurrency, numPageFrames);
	LockHolder lh(sm);

	const set<pair<int, int> > & nonEmptyQviews = sm.getNonEmptyQviews();

	if (nonEmptyQviews.size() != 1)
	  {
	    throw SmException(__FILE__, __LINE__, "nonEmptyQviews.size() != 1");
	  }

	set<pair<int, int> >::const_iterator pos = nonEmptyQviews.begin();
	assert(pos != nonEmptyQviews.end());
	
	if ((*pos) != make_pair(1, 2))
	  {
	    ostringstream os;
	    os << "Found unexpected qview: <" << pos->first << ", " << pos->second << ">";
	    throw SmException(__FILE__, __LINE__, os.str());
	  }

	// Cleanup...
	sm.shutdown();
      }
    }
  catch (exception & e)
    {
      cout << "test4: Caught exception " << e.what() << endl;
      throw;
    }

  cout << "*************************************************************" << endl
       << "*                        PASSED TEST 4                      *" << endl
       << "*************************************************************" << endl
       << endl;
}

//===============================================================================

// Verify that cxn points seem to work ok...
void test5(unsigned long numPageFrames, string dirname)
{
  cout << "*************************************************************" << endl
       << "*                       ENTERING TEST 5                     *" << endl
       << "*************************************************************" << endl
       << endl;

  const size_t bytesPerPage    = 65536;
  const size_t bytesPerRecord  = 1000;
  const size_t maxConcurrency  = 100;
  const size_t tuplesPerPage   = bytesPerPage / bytesPerRecord;

  try
    {
      {
	StorageMgr sm(dirname, bytesPerPage, maxConcurrency, numPageFrames);
	LockHolder lh(sm);

	// Set up our initialy topology...
	// Qbase 1 has a cxn point, and we'll attach a cxn point qview to that
	// point.
	vector<int> qviewIds;

	bool limitTuplesByNum = true;
	size_t maxNumTuples = 500;
	bool limitTuplesByAge = false;
	unsigned int maxTupleAgeSeconds = 0;

	sm.createQ(1, qviewIds, bytesPerRecord, true);
	sm.setCxnPointHistorySpec(1, limitTuplesByNum, maxNumTuples, 
				  limitTuplesByAge, maxTupleAgeSeconds);

	// Set up and run our first execution plan. We need to have two plans, 
	// because the cxn point must not be connected to during the enqueueing 
	// less we void the test...
	int boxId;
	vector<pair<int, int> > inputQviewIds;
	vector<int> outputQbaseIds;
	vector<size_t> numExpectedEnq;

	boxId = 1;
	outputQbaseIds.push_back(1);
	numExpectedEnq.push_back(20);
	SuperBoxPlanStep step1(boxId, inputQviewIds, outputQbaseIds, numExpectedEnq);

	vector<SuperBoxPlanStep> schedule;
	schedule.push_back(step1);
	sm.scheduleSuperBox(1, schedule);

	BinarySem readyFlag;
	if (! sm.notifyWhenSuperBoxStepRunnable(1, readyFlag))
	  {
	    lh.release();
	    readyFlag.awaitPostThenClear();
	    lh.reacquire();
	  }

	size_t numRecords = 600;
	vector<timeval> tvals;
        lh.release();
	enqRecords(sm, 1, 1, bytesPerRecord, numRecords, 'A', tvals);
        lh.reacquire();
	
	sm.onSuperBoxStepComplete(1);

	size_t numRecsInQview;
	size_t numRecsFramed;
	timeval avgTimestamp;

	sm.getQviewInfo(1, 0, numRecsInQview, numRecsFramed, avgTimestamp);
	cout << "Cxn point: numRecsInQview = " << numRecsInQview << ", numRecsFramed = " << numRecsFramed << endl;


	size_t minExpectedTuples = maxNumTuples;
	size_t maxExpectedTuples = maxNumTuples + tuplesPerPage - 1;

	sm.forceCxnPointPruning(1); // Do this so we can be confident truncation works right...

	if ((numRecsInQview < minExpectedTuples) || 
	    (numRecsInQview > maxExpectedTuples))
	  {
	    ostringstream os;
	    os << "Cxn point qview was expected to have between " 
	       << minExpectedTuples << " and " << maxExpectedTuples
	       << " tuples, but it actually had " << numRecsInQview << " tuples.";
	    throw SmException(__FILE__, __LINE__, os.str());
	  }

	// Setup and run our second step. Ensure that we get the right set of
	// tuples from the cxn point...
	limitTuplesByNum = true;
	maxNumTuples = 100;
	limitTuplesByAge = false;
	maxTupleAgeSeconds = 0;

	sm.createCxnPointQView(1, 1, limitTuplesByNum, maxNumTuples, 
			       limitTuplesByAge, maxTupleAgeSeconds);

	boxId = 2;
	inputQviewIds.push_back(make_pair(1, 1));
	outputQbaseIds.clear();
	numExpectedEnq.clear();
	SuperBoxPlanStep step2(boxId, inputQviewIds, outputQbaseIds, numExpectedEnq);

	schedule.clear();
	schedule.push_back(step2);
	sm.scheduleSuperBox(1, schedule);

	// Run our plan, ensuring that we get the right number of tuples from the
	// connection point...
	if (! sm.notifyWhenSuperBoxStepRunnable(1, readyFlag))
	  {
	    lh.release();
	    readyFlag.awaitPostThenClear();
	    lh.reacquire();
	  }

	sm.getQviewInfo(1, 1, numRecsInQview, numRecsFramed, avgTimestamp);

	minExpectedTuples = maxNumTuples;
	maxExpectedTuples = maxNumTuples + tuplesPerPage - 1;

	if ((numRecsInQview < minExpectedTuples) || 
	    (numRecsInQview > maxExpectedTuples))
	  {
	    ostringstream os;
	    os << "Cxn point qview was expected to have between " 
	       << minExpectedTuples << " and " << maxExpectedTuples
	       << " tuples, but it actually had " << numRecsInQview << " tuples.";
	    throw SmException(__FILE__, __LINE__, os.str());
	  }

	// Determined experimentally. Complicated to predict because cxn point
	// query results are now page-part aligned, rether than being strictly
	// limited by the history spec.
	//
	// This might seem like a non-test because we calculated 'firstLetter'
	// experimentally, but at least 'deqRecords(...)' still ensures that the
	// output tuples have proper letter-order.
	char firstLetter = 'N'; 

        lh.release();
	deqRecords(sm, 1, 1, 1, bytesPerRecord, maxNumTuples, firstLetter);
        lh.reacquire();

	sm.onSuperBoxStepComplete(1);

	// Clean up...
	sm.deleteQView(1, 1);
	sm.deleteQ(1);
	sm.shutdown();
      }
    }
  catch (exception & e)
    {
      cout << "test5: Caught exception " << e.what() << endl;
      throw;
    }

  cout << "*************************************************************" << endl
       << "*                        PASSED TEST 5                      *" << endl
       << "*************************************************************" << endl
       << endl;
}


//===============================================================================


int main(int argc, char* argv[])
{
  if (argc != 7)
    {
      printUsage();
      return 1;
    }

  char * endPtr;
  unsigned long numFrames = strtoul(argv[1], & endPtr, 10);
  if (*endPtr != '\0')
    {
      ostringstream os;
      os << "First parameter didn't parse as an unsigned long";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  string dirname1 = argv[2];
  string dirname2 = argv[3];
  string dirname3 = argv[4];
  string dirname4 = argv[5];
  string dirname5 = argv[6];

  try
    {
      test1(numFrames, dirname1);
      test2(numFrames, dirname2);
      test3(numFrames, dirname3);
      test4(numFrames, dirname4);
      test5(numFrames, dirname5);
    }
  catch (exception & e)
    {
      cout << "main: Caught exception: " << e.what() << endl;
      return 1;
    }
  catch (...)
    {
      cout << "main: Caught some excetpion not derived from 'exception'" << endl;
      return 1;
    }
}

//===============================================================================
