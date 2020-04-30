#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <PtMutex.H>
#include <O0Tuple.H>

using namespace std;

class ModelObserver;

class Model
{
public:
	typedef enum FEEDER_RUNSTATE {
		FEEDER_RUNSTATE_PAUSED,
		FEEDER_RUNSTATE_ACTIVE,
		FEEDER_RUNSTATE_FINISHED
	};

	Model();
	virtual ~Model();

	void addObserver(ModelObserver * pObserver);

	void setFeederRunState(FEEDER_RUNSTATE newState);
	FEEDER_RUNSTATE getFeederRunState();

	void setEnqDelay(unsigned long delayMilliseconds);
	unsigned long getEnqDelay();

	// Whenn true, the user has requested that the application shut down
	void setAppShutdown(bool doShutdown);
	bool getAppShutdown();

	// Passes ownership of the O0Tuple to this Model
	void addNewOocEvent(const O0Tuple * pTuple);

	// Specifies the number of tuples that have been enqueued into the query
	// network so far.
	void setNumTuplesEnq(int n);

private:
	vector<ModelObserver*> _observers;
	unsigned long _delayMilliseconds;
	FEEDER_RUNSTATE _currentFeederState;
	bool _appShutdown;
	vector<const O0Tuple*>_oocEventTuples;
	int _numTuplesEnq;

	PtMutex _mtx;
};

#endif
