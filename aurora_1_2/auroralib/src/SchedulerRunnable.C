#include <SchedulerRunnable.H>

//===============================================================================

SchedulerRunnable::SchedulerRunnable(Scheduler & s)
  throw (exception) :
  _sched(s)
{
}

//===============================================================================

SchedulerRunnable::~SchedulerRunnable()
{
}

//===============================================================================

void SchedulerRunnable::run() throw()
{
  _sched.start();
}

//===============================================================================
