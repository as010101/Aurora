#ifndef INPUTRATEMONITORRUNNABLE_H
#define INPUTRATEMONITORRUNNABLE_H

#include <Runnable.H>
#include <exception>

class AuroraNodeImpl;

using namespace std;

// This class periodically sweeps over the input arcs of the API, using the
// discovered stats to maintain part of the DelayedDataMgr's stats.
class InputRateMonitorRunnable : public Runnable
{
public:
	InputRateMonitorRunnable(AuroraNodeImpl & ani)
		throw (exception);

	virtual ~InputRateMonitorRunnable()
		throw ();

	virtual void run()
		throw ();

private:
	AuroraNodeImpl & _ani;
};

#endif
