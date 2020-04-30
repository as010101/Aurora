#include <sm/include/SuperBoxPlanStep.H>
#include <sstream>
#include <iostream> // only for debugging

SuperBoxPlanStep::SuperBoxPlanStep(int boxId,
				   const vector<pair<int, int> > & inputQviewIds,
				   const vector<int> & outputQbaseIds,
				   const vector<size_t> & numExpectedEnq)
  throw (exception)  :
  _boxId(boxId),
  _inputQviewIds(inputQviewIds),
  _outputQbaseIds(outputQbaseIds),
  _numExpectedEnq(numExpectedEnq)
{
  // For each input qview, we'll only need to pin one page at a time.
  //
  // For output qbases, we normally need to pin only one page (that on which
  // enqueues occur). However, enqueues can cause a page split, which briefly
  // requires two pages to be pinned.
  _minFramesForNoDeadlock = inputQviewIds.size() + (2 * outputQbaseIds.size());
}

//==============================================================================

SuperBoxPlanStep::SuperBoxPlanStep(const SuperBoxPlanStep & src)
  throw (exception) :
  _minFramesForNoDeadlock(src._minFramesForNoDeadlock),
  _boxId(src._boxId),
  _inputQviewIds(src._inputQviewIds),
  _outputQbaseIds(src._outputQbaseIds),
  _numExpectedEnq(src._numExpectedEnq)
{
}

//==============================================================================

SuperBoxPlanStep::~SuperBoxPlanStep()
{
}

//==============================================================================

int SuperBoxPlanStep::getBoxId() const
{
  return _boxId;
}

//==============================================================================

const vector<pair<int, int> > & SuperBoxPlanStep::getInputQviewIds() const
  throw (exception)
{
  return _inputQviewIds;
}

//==============================================================================

const vector<int> & SuperBoxPlanStep::getOutputQbaseIds() const
  throw (exception)
{
  return _outputQbaseIds;
}

//==============================================================================

const vector<size_t> & SuperBoxPlanStep::getNumExpectedEnq() const
  throw (exception)
{
  return _numExpectedEnq;
}

//==============================================================================

// Returns the minimum number of frames that must be reserved for the execution
// of this train step in order to guarantee that it can always execute.
size_t SuperBoxPlanStep::getMinFramesForNoDeadlock() const
  throw (exception)
{
  return _minFramesForNoDeadlock;
}

//==============================================================================

string SuperBoxPlanStep::getDebugInfo(int indentLevel) const
  throw (std::exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "SuperBoxPlanStep(this = " << this << ")" << endl
     << indentStr << "   _minFramesForNoDeadlock = "      << _minFramesForNoDeadlock << endl
     << indentStr << "   _boxId = "   << _boxId << endl
     << indentStr << "   _inputQviewIds = " << endl;
  
  for (size_t i = 0; i < _inputQviewIds.size(); ++i)
    {
      os << indentStr << "      (" << _inputQviewIds[i].first << ", " 
	 << _inputQviewIds[i].second << ")" << endl;
    }

  os << indentStr << "   _outputQbaseIds = " << endl;
  for (size_t i = 0; i < _outputQbaseIds.size(); ++i)
    {
      os << indentStr << "      " << _outputQbaseIds[i] << endl;
    }

  os << indentStr << "   _numExpectedEnq = " << endl;
  for (size_t i = 0; i < _numExpectedEnq.size(); ++i)
    {
      os << indentStr << "      " << _numExpectedEnq[i] << endl;
    }

  return os.str();
}

//===============================================================================
