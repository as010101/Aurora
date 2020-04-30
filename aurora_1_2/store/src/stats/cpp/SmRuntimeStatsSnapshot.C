#include <stats/include/SmRuntimeStatsSnapshot.H>
#include <util.H>
#include <sstream>

//===============================================================================

SmRuntimeStatsSnapshot::SmRuntimeStatsSnapshot()
{
}

//===============================================================================

SmRuntimeStatsSnapshot::SmRuntimeStatsSnapshot(timeval firstMoment, 
					       timeval lastMoment,
					       unsigned int numNewPageGrantImmediate,
					       PeriodStat newPageGrantDelays,
					       unsigned int numPageFramingAlreadyFramed,
					       PeriodStat pageFramingDelays)
  : _firstMoment(firstMoment),
    _lastMoment(lastMoment),
    _numNewPageGrantImmediate(numNewPageGrantImmediate),
    _newPageGrantDelays(newPageGrantDelays),
    _numPageFramingAlreadyFramed(numPageFramingAlreadyFramed),
    _pageFramingDelays(pageFramingDelays)
{
}

//===============================================================================

SmRuntimeStatsSnapshot::~SmRuntimeStatsSnapshot()
{
}

//===============================================================================

timeval SmRuntimeStatsSnapshot::getFirstMoment() const
{
  return _firstMoment;
}

//===============================================================================

timeval SmRuntimeStatsSnapshot::getLastMoment() const
{
  return _lastMoment;
}

//===============================================================================

string SmRuntimeStatsSnapshot::getAsString() const
  throw (exception)
{
  timeval elapsedTime = getTimevalDiff(_firstMoment, _lastMoment);
  ostringstream os;
  unsigned int numEvents;

  os << "firstMoment = " << timevalToSimpleString(_firstMoment) << endl
     << "lastMoment = " << timevalToSimpleString(_lastMoment) << endl
     << "elapstedTime = " << timevalToSimpleString(elapsedTime) << endl;

  //-----------------------------------------------------------------------------

  os << "page allocations:" << endl
     << "   # granted immediately: " << _numNewPageGrantImmediate << endl;

  numEvents = _newPageGrantDelays.getNumEvents();
  if (numEvents > 0)
    {
      os << "   # granted after delay: " << numEvents << endl;

      timeval avgDelay = _newPageGrantDelays.getAveragePeriod();
      os << "   Avg. delay: " << avgDelay.tv_sec << " seconds, " 
	 << avgDelay.tv_usec << " microseconds" << endl;
    }
  else
    {
      os << "   # granted after delay: 0" << endl;
    }

  //-----------------------------------------------------------------------------

  os << "page framing needs:" << endl
     << "   # already framed: " << _numPageFramingAlreadyFramed << endl;

  numEvents = _pageFramingDelays.getNumEvents();
  if (numEvents > 0)
    {
      os << "   # framed after delay: " << numEvents << endl;

      timeval avgDelay = _pageFramingDelays.getAveragePeriod();
      os << "   Avg. delay: " << avgDelay.tv_sec << " seconds, " 
	 << avgDelay.tv_usec << " microseconds";
    }
  else
    {
      os << "   # framed after delay: 0";
    }

  return os.str();
}

//===============================================================================

unsigned int SmRuntimeStatsSnapshot::getNumPageAllocImmediate() const
{
  return _numNewPageGrantImmediate;
}

//===============================================================================

unsigned int SmRuntimeStatsSnapshot::getNumPageAllocDelayed() const
{
  return _newPageGrantDelays.getNumEvents();
}

//===============================================================================

timeval SmRuntimeStatsSnapshot::getAvgPageAllocDelay() const
  throw (exception)
{
  return _newPageGrantDelays.getAveragePeriod();
}

//===============================================================================
unsigned int SmRuntimeStatsSnapshot::getNumPageFramingAlreadyFramed() const
{
  return _numPageFramingAlreadyFramed;
}

//===============================================================================
unsigned int SmRuntimeStatsSnapshot::getNumPageFramingDelayed() const
{
  return _pageFramingDelays.getNumEvents();
}

//===============================================================================
timeval SmRuntimeStatsSnapshot::getAvgPageFramingDelay() const
  throw (exception)
{
  return _pageFramingDelays.getAveragePeriod();
}

//===============================================================================
