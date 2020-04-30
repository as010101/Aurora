#include <Model.H>
#include <LockHolder.H>
#include <ModelObserver.H>

Model::Model()
{
}

//===============================================================================

Model::~Model()
{
}

//===============================================================================


void Model::addObserver(ModelObserver * pObserver)
{
	LockHolder lh(_mtx);
	_observers.push_back(pObserver);
}

//===============================================================================


void Model::setFeederRunState(FEEDER_RUNSTATE newState)
{
	LockHolder lh(_mtx);
	_currentFeederState = newState;

	for (int i = 0; i < _observers.size(); ++i)	{
		_observers[i]->onFeederRunStateChange(_currentFeederState);
	}
}

//===============================================================================


void Model::setEnqDelay(unsigned long delayMilliseconds)
{
	LockHolder lh(_mtx);
	_delayMilliseconds = delayMilliseconds;

	for (int i = 0; i < _observers.size(); ++i)	{
		_observers[i]->onEnqDelayChange(_delayMilliseconds);
	}
}

//===============================================================================

void Model::setAppShutdown(bool doShutdown)
{
	LockHolder lh(_mtx);
	_appShutdown = doShutdown;

	for (int i = 0; i < _observers.size(); ++i)	{
		_observers[i]->onAppShutdownChange(_currentFeederState);
	}
}

//===============================================================================

bool Model::getAppShutdown()
{
	LockHolder lh(_mtx);
	return _appShutdown;
}

//===============================================================================

void Model::addNewOocEvent(const O0Tuple * pTuple)
{
	LockHolder lh(_mtx);
	_oocEventTuples.push_back(pTuple);

	for (int i = 0; i < _observers.size(); ++i)	{
		_observers[i]->onNewOocEvent(pTuple);
	}
}

//===============================================================================

Model::FEEDER_RUNSTATE Model::getFeederRunState()
{
	LockHolder lh(_mtx);
	return _currentFeederState;
}

//===============================================================================

unsigned long Model::getEnqDelay()
{
	LockHolder lh(_mtx);
	return _delayMilliseconds;
}

//===============================================================================

void Model::setNumTuplesEnq(int n)
{
	LockHolder lh(_mtx);
	_numTuplesEnq = n;

	for (int i = 0; i < _observers.size(); ++i)	{
		_observers[i]->onNumTuplesEnqChange(n);
	}
}
