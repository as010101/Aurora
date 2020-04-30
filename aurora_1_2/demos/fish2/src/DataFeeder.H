#ifndef DATAFEEDER_H
#define DATAFEEDER_H

#include <ModelObserver.H>
#include <Runnable.H>
#include <aurora_cxx.H>
#include <pthread.h>

class DataFileReader;
class Model;

class DataFeeder : public Runnable, public ModelObserver
{
public:
	DataFeeder(AuroraNode * pNode,
			   string dataFilePath,
			   Model * pModel);

	virtual ~DataFeeder();

	void onFeederRunStateChange(Model::FEEDER_RUNSTATE newState);
	void onEnqDelayChange(unsigned long delayMilliseconds);
	void onAppShutdownChange(bool doShutdown);
	void run() throw();

private:
	// If this returns "active", then it's ok to enq a tuple now.
	// If this returns "finished", it's time to shutdown the feeder
	Model::FEEDER_RUNSTATE wait();

	pthread_cond_t _cond;
	pthread_mutex_t _mtx;
	AuroraNode * _pNode;
	DataFileReader * _pReader;
	Model * _pModel;
};

#endif
