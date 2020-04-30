#ifndef SUPERBOXPACER_H
#define SUPERBOXPACER_H

#include <StorageMgr_Exceptions.H>
#include <BinarySem.H>
#include <sm/include/SuperBoxPlanStep.H>
#include <vector>
#include <map>
#include <queue>

using namespace std;


//*******************************************************************************
// THREAD SAFETY
// This class isn't thread safe in the slightest. Each instance can only be 
// access by one thread at a time.
//*******************************************************************************

// Schedules super box plan steps. We currently do this on a FIFO order to avoid
// starvation.
class SuperBoxPacer
{
public:
  // 'numFrames' is the total number of frames that may be collectively, 
  // concurrently used by all the scheduled super boxes.
  SuperBoxPacer(size_t numFrames)
    throw (exception);

  virtual ~SuperBoxPacer();


  // All the info a SuperBoxPacer keeps on a currently scheduled super box.
  struct SuperBoxPlanInfo
  {
    SuperBoxPlanInfo(const vector<SuperBoxPlanStep> & plan)
      throw (exception);

    // The steps in the super box execution plan. _shedule[0] is the first step
    // to execute.
    vector<SuperBoxPlanStep> _plan;

    // Has a 1:1 correspondance to _schedule. _remainingFrameNeeds[i] gives the
    // maximum number of frames needed to avoid deadlock by any superbox step in
    // the set
    //   { _schedule[i], _schedule[i+1], ..., _schedule[_schedule.size() - 1] }.
    // The prurpose of this is to let us easily know, for any point in the 
    // super box's execution, what's the maximum number of frames it needs in the
    // future. As a super box's execution proceeds, that number may decrease, 
    // allowing more super boxes to be scheduled immediately rather than waiting
    // for this super box to fully complete execution.
    vector<size_t> _remainingFrameNeeds;

    // True iff this super box is active (permitted to run).
    bool _active;

    // index into _schedule giving the current step that's executing or to-be-
    // executed once the step becomes active.
    size_t _currentStep; 

    // When not NULL, this BinarySem is to be posted the next time a the super
    // box has a current step that's made active.
    BinarySem * _pMadeActiveFlag;

    string getDebugInfo() const
      throw (exception);
  };

  // schedule[0] is the first step in the super box plan, etc. It must be true 
  // that plan.size() > 0. The specifeid superBoxId must not be currently 
  // scheduled.
  void submitSuperBoxPlan(int superBoxId, 
			  const vector<SuperBoxPlanStep> & plan)
    throw (exception);

  // Indicates that the 'current' super box plan step has been completed. This 
  // returns 'true' iff there are more steps in this super box plan, or 'false' 
  // iff the super box plan has been fully run (and is therefore no longer 
  // scheduled)
  bool onSuperBoxStepComplete(int superBoxId)
    throw (exception);

  // Indicates that the scheduler is ready to execute the next super box step for
  // the specified super box, and that it's done executing the previous super box 
  // plan step (if there was a previous step).
  //
  // If this returns 'true', then 'readyFlag' hasn't been messed with and the
  // next super box plan step is immediately executable.
  //
  // If this returns 'false', then 'readyFlag.post()' will be called sometime
  // after this method returns. When readyFlag is posted to, the next super box 
  // plan step is executable.
  //
  // NOTE: If this method returns 'false', then this SuperBoxPacer holds a
  // pointer to the readyFlag object until its post()'s it. So it's not
  // valid to delete that BinarySem until this object post()'s it.
  bool awaitNextSuperBoxStep(int superBoxId, BinarySem & readyFlag)
    throw (exception);

  // Give the info for the 'current' super box plan step. This is valid to call 
  // only if there *is* a current super box plan step (i.e., the super box hasn't
  // run to completion), **and** the step is currently executable.
  //
  // The returned reference is valid until the super box's execution is complete
  // (the final call to onSuperBoxComplete(...) for the super box is called.)
  const SuperBoxPlanStep & getCurrentSuperBoxStep(int superBoxId) const
    throw (exception);

  // Gives the schedule and index into the schedule's current step.
  // (Next step if we're in the gap between the last step being done and
  // the next step not yet being awaited).
  const SuperBoxPlanInfo & getSuperBoxInfo(int superBoxId) const
    throw (exception);

  // Lets an outside observer see what super box schedules are currently in 
  // effect. (Meant to let code external to SuperBoxSpacer decide what page 
  // priorities to assign, and what page preallocations to perform.)
  const map<int, SuperBoxPlanInfo> & getSuperBoxPlanInfoMap() const
    throw (exception);

  // Returns the total number of superboxes that have been scheduled but not yet
  // completed.
  size_t getNumScheduledSuperBoxes() const
    throw (exception);

private:
  // Returns the number of frames that aren't currently reserved by active
  // super box steps.
  size_t getNumUnreservedFrames() const
    throw (exception);

  // Analyzes the current step for all registered super box plans, and decides if
  //  any steps need to be made active. If any super box plan being awakened has 
  // a BinarySem registered by the method 'awaitNextSuperBoxStep', this method 
  // will post that BinarySem.
  void updateStepActivations()
    throw (exception);

  size_t _numFrames;

  // SuperBoxes are scheduled in FIFO order. (We might change this policy later.)
  // The super box whose superBoxId is at the front of the queue is the next one
  // to be granted activation.
  queue<int> _superBoxExecOrder;

  // Has one entry for every subbmitted-but-not-yet-complete superbox plan.
  map<int, SuperBoxPlanInfo> _plans;
};

#endif
