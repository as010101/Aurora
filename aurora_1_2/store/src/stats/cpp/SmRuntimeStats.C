#include <stats/include/SmRuntimeStats.H>
#include <util.H>
#include <algorithm>
#include <sstream>
#include <sys/time.h>


#include <iostream>

SmRuntimeStats * SmRuntimeStats::s_instance = NULL;
size_t           SmRuntimeStats::s_refCount = 0;

//===============================================================================

SmRuntimeStats * SmRuntimeStats::getInstance()
{
  if (s_instance == NULL)
    {
      s_instance = new SmRuntimeStats();
    }

  ++ s_refCount;

  return s_instance;
}

//===============================================================================

void SmRuntimeStats::releaseInstance()
{
  -- s_refCount;

  if (s_refCount == 0)
    {
      delete s_instance;
      s_instance = NULL;
    }
}

//===============================================================================

SmRuntimeStatsSnapshot SmRuntimeStats::getSnapshot()
{
  timeval now;
  gettimeofday(& now, NULL);

  SmRuntimeStatsSnapshot aSnapshot(_baselineTime, now, 
				   _numNewPageGrantImmediate, 
				   _newPageGrantDelays,
				   _numPageFramingAlreadyFramed, 
				   _pageFramingDelays);

  resetStats();
  _baselineTime = now;
  return aSnapshot;
}

//===============================================================================

SmRuntimeStats::SmRuntimeStats()
  throw (exception)
{
  gettimeofday(& _baselineTime, NULL);
  resetStats();
}

//===============================================================================

SmRuntimeStats::~SmRuntimeStats()
{
}

//===============================================================================

void SmRuntimeStats::resetStats()
  throw (exception)
{
  gettimeofday(& _baselineTime, NULL);

  _numNewPageGrantImmediate = 0;
  _newPageGrantDelays.reset();

  _numPageFramingAlreadyFramed = 0;
  _pageFramingDelays.reset();

  /*
  _numFrameReadLockGrantImmediate = 0;
  _frameReadLockGrantDelays.reset();

  _numFrameWriteLockGrantImmediate = 0;
  _frameWriteLockGrantDelays.reset();
  */
}

//===============================================================================

void SmRuntimeStats::onNewPageGrantImmediate()
  throw (exception)
{
  ++ _numNewPageGrantImmediate;
}

//===============================================================================

void SmRuntimeStats::onNewPageGrantDelayed(const timeval & t1, const timeval & t2)
  throw (exception)
{
  _newPageGrantDelays.onEvent(t1, t2);
}

//===============================================================================

void SmRuntimeStats::onFramingAlreadyFramed()
  throw (exception)
{
  ++ _numPageFramingAlreadyFramed;
}

//===============================================================================

void SmRuntimeStats::onFramingDelayed(const timeval & t1, const timeval & t2)
  throw (exception)
{
  _pageFramingDelays.onEvent(t1, t2);
}

//===============================================================================

/*
void SmRuntimeStats::onFrameReadLockGrantImmediate()
  throw (exception)
{
  ++ _numFrameReadLockGrantImmediate;
}

//===============================================================================

void SmRuntimeStats::onFrameReadLockGrantDelayed(const timeval & t1, const timeval & t2)
  throw (exception)
{
  _frameReadLockGrantDelays.onEvent(t1, t2);
}

//===============================================================================

void SmRuntimeStats::onFrameWriteLockGrantImmediate()
  throw (exception)
{
  ++ _numFrameWriteLockGrantImmediate;
}

//===============================================================================

void SmRuntimeStats::onFrameWriteLockGrantDelayed(const timeval & t1, const timeval & t2)
  throw (exception)
{
  _frameWriteLockGrantDelays.onEvent(t1, t2);
}
*/
//===============================================================================
