#ifndef SCHEDULERRUNNABLE_H
#define SCHEDULERRUNNABLE_H

#include <Runnable.H>
#include <Scheduler.H>
#include <StorageMgr_Exceptions.H>

/**
 This class just provides a thread to run the Scheduler::start() method.
 */
class SchedulerRunnable : public Runnable
{
public:
  // The object referred to by 's' must remain valid for the entire time this
  // SchedulerRunnable exists.
  //
  // Before this SchedulerRunnable's run() method is called, 's' must be properly
  // intialized s.t. it's valid to call s.start().
  SchedulerRunnable(Scheduler & s)
    throw (exception);

  virtual ~SchedulerRunnable();

  // This method will execute s.start(). Normally, this method will return 
  // withing a finite amount of time after (__global_stop.getStop()) becomes 
  // true.
  virtual void run() throw();

private:
  Scheduler & _sched;
};

#endif
