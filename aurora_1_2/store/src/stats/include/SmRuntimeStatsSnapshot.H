#ifndef SMRUNTIMESTATSSNAPSHOT_H
#define SMRUNTIMESTATSSNAPSHOT_H

#include <sys/time.h>
#include <stats/include/PeriodStat.H>

// An instance of the stats, covering some period of time
class SmRuntimeStatsSnapshot
{
public:
  // This leaves the data uninitialized.
  SmRuntimeStatsSnapshot();

  SmRuntimeStatsSnapshot(timeval firstMoment, 
			 timeval lastMoment,
			 unsigned int numNewPageGrantImmediate,
			 PeriodStat newPageGrantDelays,
			 unsigned int numPageFramingAlreadyFramed,
			 PeriodStat pageFramingDelays);

  virtual ~SmRuntimeStatsSnapshot();

  // Time boundies that this snapshot covers...
  timeval getFirstMoment() const;
  timeval getLastMoment() const;

  // String rendition of this class' public data.
  string getAsString() const
    throw (exception);

  //-----------------------------------------------------------------------------
  // For page allocations...
  //-----------------------------------------------------------------------------

  // Number of page allocations that could be instantly satisfied.
  unsigned int getNumPageAllocImmediate() const;

  // Number of page allocations that required letting another thread run before
  // the requests could be satisfied.
  unsigned int getNumPageAllocDelayed() const;

  // Average time period that a page allocation was waited for. Throws an 
  // exception if there were no delayed page allocations.
  timeval getAvgPageAllocDelay() const
    throw (exception);

  //-----------------------------------------------------------------------------
  // For page framing...
  //-----------------------------------------------------------------------------

  // Number of page framing requests where the page was already framed.
  unsigned int getNumPageFramingAlreadyFramed() const;

  // Number of page framing requests that required letting another thread run 
  // before the respective pages were framed.
  unsigned int getNumPageFramingDelayed() const;

  // Average time period that a page framing was waited for. Throws an exception 
  // if there were no delayed page allocations.
  timeval getAvgPageFramingDelay() const
    throw (exception);

private:
  timeval _firstMoment, _lastMoment;

  unsigned int _numNewPageGrantImmediate;
  PeriodStat _newPageGrantDelays;

  unsigned int _numPageFramingAlreadyFramed;
  PeriodStat _pageFramingDelays; 
};

#endif
