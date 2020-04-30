#ifndef SUPERBOXPLANSTEP_H
#define SUPERBOXPLANSTEP_H

#include <StorageMgr_Exceptions.H>
#include <vector>

using namespace std;

class SuperBoxPlanStep
{
public:
  // Note: It's assumed that a given qview id appears at most once in 
  // 'inputQviewIds', and a given qbase id appears at most once in 
  // 'outputQbaseIds'.
  //
  // 'outputQbaseIds' and 'numExpectedEnqueues' are paired: corresponding indices
  // pertain to the same qbase.
  SuperBoxPlanStep(int boxId,
		   const vector<pair<int, int> > & inputQviewIds,
		   const vector<int> & outputQbaseIds,
		   const vector<size_t> & numExpectedEnq)
    throw (exception);

  SuperBoxPlanStep(const SuperBoxPlanStep & src)
    throw (exception);
 
  virtual ~SuperBoxPlanStep();

  int getBoxId() const;

  // First element of pair is the qbaseId of the qbase that the qview is tied to,
  // and second element is the actual qview id.
  const vector<pair<int, int> > & getInputQviewIds() const
    throw (exception);

  const vector<int> & getOutputQbaseIds() const
    throw (exception);

  const vector<size_t> & getNumExpectedEnq() const
    throw (exception);

  // Returns the minimum number of frames that must be reserved for the execution
  // of this train step in order to guarantee that it can always execute.
  size_t getMinFramesForNoDeadlock() const
    throw (exception);

  string getDebugInfo(int indentLevel = 0) const
    throw (std::exception);

  //  TrainStepResources & operator=(const TrainStepResources & rhs);
private:
  size_t _minFramesForNoDeadlock;
  int _boxId;
  vector<pair<int, int> > _inputQviewIds;
  vector<int> _outputQbaseIds;
  vector<size_t> _numExpectedEnq;


};

#endif
