#ifndef MODELOBSERVER_H
#define MODELOBSERVER_H

#include <Model.H>
#include <O0Tuple.H>

class ModelObserver
{
public:
	// Default implementation of each method is a no-op..
	virtual void onFeederRunStateChange(Model::FEEDER_RUNSTATE newState);
	virtual void onEnqDelayChange(unsigned long delayMilliseconds);
	virtual void onAppShutdownChange(bool doShutdown);
	virtual void onNewOocEvent(const O0Tuple * pTuple);
	virtual void onNumTuplesEnqChange(int n);
};

#endif
