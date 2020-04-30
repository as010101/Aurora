#include <sm/include/SuperBoxPacer.H>
#include <sstream>
#include <algorithm>
#include <assert.h>

#include <iostream> // debugging only

SuperBoxPacer::SuperBoxPacer(size_t numFrames)
  throw (exception) :
  _numFrames(numFrames)
{
}

//===============================================================================

SuperBoxPacer::~SuperBoxPacer()
{
}

//===============================================================================

void SuperBoxPacer::submitSuperBoxPlan(int superBoxId, 
				       const vector<SuperBoxPlanStep> & plan)
  throw (exception)
{
  if (plan.empty())
    {
      ostringstream os;
      os << "The specified superbox (" << superBoxId << ") plan is already empty";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  pair < map<int, SuperBoxPacer::SuperBoxPlanInfo>::iterator, bool> rc = 
    _plans.insert(make_pair(superBoxId, plan));

  if (! rc.second)
    {
      ostringstream os;
      os << "The specified superbox (" << superBoxId << ") is already scheduled.";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  SuperBoxPacer::SuperBoxPlanInfo & info = rc.first->second;

  size_t numFramesForNoDeadlock = info._remainingFrameNeeds[0];
  if (numFramesForNoDeadlock > _numFrames)
    {
      _plans.erase(rc.first);
      
      ostringstream os;
      os << "The specified superbox (" << superBoxId << ") has a plan step that "
	 << " will potentially require " << numFramesForNoDeadlock << " frames "
	 << "to run without deadlock. However, the buffer cache only has " 
	 << _numFrames << " frames.";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  _superBoxExecOrder.push(superBoxId);
  updateStepActivations();
}
//===============================================================================

bool SuperBoxPacer::onSuperBoxStepComplete(int superBoxId)
  throw (exception)
{
  map<int, SuperBoxPlanInfo>::iterator pos = _plans.find(superBoxId);

  if (pos == _plans.end())
    {
      ostringstream os;
      os << "The specified super box (" << superBoxId << ") isn't currently scheduled.";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  SuperBoxPlanInfo & pi = pos->second;

  if (! pi._active)
    {
      ostringstream os;
      os << "The specified super box (" << superBoxId << ")'s current step isn't active." ;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // We only do this if we're round-robinning step admissions across all 
  // super boxes. That's not the current policy:
  // pi._active = false;

  if (pi._currentStep == (pi._plan.size() - 1))
    {
      // The super box has run to completion...
      _plans.erase(pos);
      updateStepActivations();
      return false;
    }
  else
    {
      assert(pi._currentStep < pi._plan.size());

      // Make the current step inactive, and designate the next step in the super box (if
      // there is one) as the 'current' step...
      ++pi._currentStep;

      updateStepActivations();
      return true;
    }
}

//===============================================================================

bool SuperBoxPacer::awaitNextSuperBoxStep(int superBoxId, BinarySem & readyFlag)
  throw (exception)
{
  map<int, SuperBoxPlanInfo>::iterator pos = _plans.find(superBoxId);

  if (pos == _plans.end())
    {
      ostringstream os;
      os << "The specified train (" << superBoxId << ") isn't currently scheduled.";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  SuperBoxPlanInfo & pi = pos->second;

  if (pi._active)
    {
      return true;
    }
  else
    {
      pi._pMadeActiveFlag = & readyFlag;
      return false;
    }
}

//===============================================================================

const map<int, SuperBoxPacer::SuperBoxPlanInfo> & 
SuperBoxPacer::getSuperBoxPlanInfoMap() const
  throw (exception)
{
  return _plans;
}

//===============================================================================

const SuperBoxPlanStep & SuperBoxPacer::getCurrentSuperBoxStep(int superBoxId) const
  throw (exception)
{
  map<int, SuperBoxPlanInfo>::const_iterator pos = _plans.find(superBoxId);

  if (pos == _plans.end())
    {
      ostringstream os;
      os << "The specified super box (" << superBoxId << ") isn't currently scheduled.";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  const SuperBoxPlanInfo & pi = pos->second;

  if (! pi._active)
    {
      ostringstream os;
      os << "The specified super box (" << superBoxId << ")'s isn't currently active.";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  return pi._plan[pi._currentStep];
}
//===============================================================================

const SuperBoxPacer::SuperBoxPlanInfo & SuperBoxPacer::getSuperBoxInfo(int superBoxId) const
  throw (exception)
{
  map<int, SuperBoxPlanInfo>::const_iterator pos = _plans.find(superBoxId);

  if (pos == _plans.end())
    {
      ostringstream os;
      os << "The specified super box (" << superBoxId << ") isn't currently scheduled.";
      throw SmException(__FILE__, __LINE__, os.str());
    }

  return pos->second;
}

//===============================================================================

size_t SuperBoxPacer::getNumScheduledSuperBoxes() const
  throw (exception)
{
  return _plans.size();
}

//===============================================================================

size_t SuperBoxPacer::getNumUnreservedFrames() const
  throw (exception)
{
  // This method is a good candidate for optimization. We could just as easily
  // maintain 'numReservedFrames' as an instance variable, which gets updated
  // whenever we make a step become active/inactive. -cjc

  size_t numReservedFrames = 0;

  for (map<int, SuperBoxPlanInfo>::const_iterator pos = _plans.begin();
       pos != _plans.end();
       ++pos)
    {
      const SuperBoxPlanInfo & pi = pos->second;

      if (pi._active)
	{
	  numReservedFrames += pi._remainingFrameNeeds[pi._currentStep];
	}
    }

  if (numReservedFrames > _numFrames)
    {
      throw SmException(__FILE__, __LINE__, "Internal error: numReservedFrames > _numFrames");
    }
  
  return _numFrames - numReservedFrames;
}

//===============================================================================

void SuperBoxPacer::updateStepActivations()
  throw (exception)
{
  // Schedule as many new super boxes as we can reserve frames for...
  size_t numUnreservedFrames = getNumUnreservedFrames();

  bool quit = false;
  while ((numUnreservedFrames != 0) && (! _superBoxExecOrder.empty()) && (! quit))
    {
      // Schedule the next train step in line, if we can reserve enough frames
      // for it...
      int nextSuperBoxToSchedule = _superBoxExecOrder.front();
  
      map<int, SuperBoxPlanInfo>::iterator pos = _plans.find(nextSuperBoxToSchedule);
      if (pos == _plans.end())
	{
	  ostringstream os;
	  os << "Internal error: The specified super box (" << nextSuperBoxToSchedule
	     << ") isn't defined.";
	  throw SmException(__FILE__, __LINE__, os.str());
	}

      SuperBoxPlanInfo & pi = pos->second;

      size_t neededFrames = pi._remainingFrameNeeds[pi._currentStep];

      if (neededFrames < 1)
	{
	  throw SmException(__FILE__, __LINE__, "Encountered a super box step that needs no frames");
	}

      if (neededFrames < numUnreservedFrames)
	{
	  // Make it active...
	  numUnreservedFrames -= neededFrames;
	  pi._active = true;

	  if (pi._pMadeActiveFlag != NULL)
	    {
	      pi._pMadeActiveFlag->post();
	      pi._pMadeActiveFlag = NULL;
	    }

	  _superBoxExecOrder.pop();
	}
      else
	{
	  // We need to stop, because the next superbox to run needs more frames
	  // than are currently available.
	  quit = true;
	}
    }
}

//===============================================================================

SuperBoxPacer::SuperBoxPlanInfo::SuperBoxPlanInfo(const vector<SuperBoxPlanStep> & plan)
  throw (exception) :
  _plan(plan),
  _remainingFrameNeeds(_plan.size()),
  _active(false),
  _currentStep(0),
  _pMadeActiveFlag(NULL)
{
  size_t numSteps = _plan.size();
  if (numSteps > 0)
    {
      size_t maxIdx = numSteps - 1;

      _remainingFrameNeeds[maxIdx] = _plan[maxIdx].getMinFramesForNoDeadlock();

      if (numSteps > 1)
	{
	  for (size_t i = 1; i <= maxIdx; ++i)
	    {
	      size_t idxToFill = maxIdx - i;
	      _remainingFrameNeeds[idxToFill] = max(_remainingFrameNeeds[idxToFill + 1], 
						    _plan[idxToFill].getMinFramesForNoDeadlock());
	    }
	}
    }
}

//===============================================================================

string SuperBoxPacer::SuperBoxPlanInfo::getDebugInfo() const
  throw (exception)
{
  ostringstream os;
  os << "SuperBoxPlanInfo (this = " << this << ")" << endl
     << "   _currentStep = " << _currentStep << endl
     << "   _active = " << _active << endl
     << "   _plan = " << endl;

  for (size_t i = 0; i < _plan.size(); ++i)
    {
      os << "      " << _plan[i].getDebugInfo(9) << endl;
    }
 
  os << "   _remainingFrameNeeds" << endl;
  for (size_t i = 0; i < _remainingFrameNeeds.size(); ++i)
    {
      os << "      " << _remainingFrameNeeds[i] << endl;
    }

  return os.str();
}

//===============================================================================
