#include <stats/include/PeriodStat.H>
#include <util.H>

//===============================================================================

PeriodStat::PeriodStat()
{
  reset();
}

//===============================================================================

PeriodStat::~PeriodStat()
{
}

//===============================================================================

void PeriodStat::reset()
{
  _numEvents     = 0;
  _totalSec      = 0;
  _totalMicroSec = 0;
}

//===============================================================================

void PeriodStat::onEvent(const timeval & t1, const timeval & t2)
{
  ++ _numEvents;
  _totalSec += (t2.tv_sec - t1.tv_sec);

  if (t2.tv_usec >= t1.tv_usec)
    {
      _totalMicroSec += (t2.tv_usec - t1.tv_usec);
    }
  else
    {
      _totalMicroSec += (1000000 + t2.tv_usec - t1.tv_usec);
      _totalSec -= 1;
    }
}

//===============================================================================

timeval PeriodStat::getAveragePeriod() const
  throw (exception)
{
  return getAvgTimevalBySums(_totalSec, _totalMicroSec, _numEvents);
}

//===============================================================================

unsigned int PeriodStat::getNumEvents() const
{
  return _numEvents;
}

//===============================================================================
