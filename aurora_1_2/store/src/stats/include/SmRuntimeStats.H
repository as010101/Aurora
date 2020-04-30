#ifndef SMRUNTIMESTATS_H
#define SMRUNTIMESTATS_H

#include <string>
#include <StorageMgr_Exceptions.H>
#include <stats/include/SmRuntimeStatsSnapshot.H>

// Tracks and reports on stats for the Storage Manager's runtime performance.
// This doesn't track persistent stats such as data selectivity.
//
// INSTANTIATION:
// This class uses a reference-counted singleton pattern, so that code all over
// the place can make use of it without us spending our whole lives passing
// pointers around.
//
// THREAD SAFETY:
// None. Only access an instance of this class using one thread at a time.
class SmRuntimeStats
{
public:
  // If you call getInstance(), you're *obligated* to call releaseInstance() an
  // equal number of times. When the numbers of calls match, you may not 
  // consider any previously acquired pointer to a SmRuntimeStats object to be
  // valid...
  static SmRuntimeStats * getInstance();

  static void releaseInstance();
  
  //-----------------------------------------------------------------------------
  // MEMBERS FOR USE BY OBSERVERS OF THE STATISTICS...
  //-----------------------------------------------------------------------------

  // Takes a snapshot of the stats that have accumulated since the last snapshot.
  SmRuntimeStatsSnapshot getSnapshot();
  
  //-----------------------------------------------------------------------------
  // MEMBERS FOR USE BY THE CODE THE STATICS PERTAIN TO...
  //-----------------------------------------------------------------------------

  // A new page needed to be allocated, and the request was immediately 
  // granted.
  void onNewPageGrantImmediate()
    throw (exception);

  // A new page needed to be allocated, but the request was granted only after
  // a necessary delay.
  void onNewPageGrantDelayed(const timeval & t1, const timeval & t2)
    throw (exception);



  // A need existed for a page to be framed, and it turned out that the page
  // was already framed.
  void onFramingAlreadyFramed()
    throw (exception);

  // A need existed for a page to be framed, but the page wasn't framed at the
  // time.
  void onFramingDelayed(const timeval & t1, const timeval & t2)
    throw (exception);


  /*
    THESE ARE HARD TO CALCULATE, BECAUSE THE METHOD THE BUFFERCACHE OFFERS FOR
    AWAITING UNTIL A PAGE CAN BE WRITE-PINNED DOESN'T TELL YOU WHETHER YOU WERE
    AWAITING FRAMING, LOCK AVAILABILITY, OR BOTH. ADD THEM BACK IN LATER IF IT
    REALLY MATTERS.

  // Either the frame was already read-locked, or it could be read-locked 
  // immediately even though it hadn't been read-locked so far.
  void onFrameReadLockGrantImmediate()
    throw (exception);

  // Either the frame was already write-locked, or it could be read-locked 
  // immediately even though it hadn't been write-locked so far.
  void onFrameWriteLockGrantImmediate()
    throw (exception);

  // There was a delay before a read-lock could be granted on a frame.
  void onFrameReadLockGrantDelayed(const timeval & t1, const timeval & t2)
    throw (exception);

  // There was a delay before a write-lock could be granted on a frame.
  void onFrameWriteLockGrantDelayed(const timeval & t1, const timeval & t2)
    throw (exception);
  */

private:
  SmRuntimeStats()
    throw (exception);

  virtual ~SmRuntimeStats();

  // Clears the stats, and resets the baseline time for future rate-based 
  // statistics.
  void resetStats()
    throw (exception);

  static SmRuntimeStats * s_instance;
  static size_t s_refCount;
  timeval _baselineTime;
  
  // All of these stats are implicitely since the last time resetStats() was 
  // called...
  unsigned int _numNewPageGrantImmediate;
  PeriodStat _newPageGrantDelays;

  unsigned int _numPageFramingAlreadyFramed;
  PeriodStat _pageFramingDelays;

  /*
  unsigned int _numFrameReadLockGrantImmediate;
  PeriodStat _frameReadLockGrantDelays;

  unsigned int _numFrameWriteLockGrantImmediate;
  PeriodStat _frameWriteLockGrantDelays;
  */
};

#endif
