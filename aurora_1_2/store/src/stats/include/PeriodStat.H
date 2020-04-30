#ifndef PERIODSTAT_H
#define PERIODSTAT_H

#include <sys/time.h>
#include <StorageMgr_Exceptions.H>

// Manages statistics for a single event that has an associated wait-time.
class PeriodStat
{
public:
  PeriodStat();
  virtual ~PeriodStat();

  void reset();
  void onEvent(const timeval & t1, const timeval & t2);

  // Returns the avg. elapsed delay period. Throw an exception if no events
  // have occured since the last reset.
  timeval getAveragePeriod() const
    throw (exception);

  unsigned int getNumEvents() const;

private:
  unsigned int _numEvents;
  unsigned long long _totalSec;
  unsigned long long _totalMicroSec;
};

#endif
