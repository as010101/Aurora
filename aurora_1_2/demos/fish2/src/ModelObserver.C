#include <ModelObserver.H>

void ModelObserver::onFeederRunStateChange(Model::FEEDER_RUNSTATE newState)
{
	// no-op
}

void ModelObserver::onEnqDelayChange(unsigned long delayMilliseconds)
{
	// no-op
}

void ModelObserver::onAppShutdownChange(bool doShutdown)
{
	// no-op
}

void ModelObserver::onNewOocEvent(const O0Tuple * pTuple)
{
	// no-op
}


void ModelObserver::onNumTuplesEnqChange(int n)
{
	// no-op
}
