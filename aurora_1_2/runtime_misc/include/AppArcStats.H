#ifndef APPARCSTATS_H
#define APPARCSTATS_H

#include <vector>

using namespace std;

struct AppArcStats
{
	// Index into vector = histogram bucket #. The meanings of each bucket # are
	// defined elsewhere in the runtime.
	//
	// The sum of all the values in the histogram should be very close to 1.0, 
	// but since it's a float, equality with 1.0 isn't required.
	vector<float> _valueHistogram;    // NOTE: NOT YET BEING MAINTAINED

	// The recent utility on this output as perceived by the load shedder. 
	// Whichever QoS graph is currently in use by the load shedder defines
	// the meaning of this utility value.
	//double _qosUtility;    // NOTE: NOT YET BEING MAINTAINED
	// no more needed (tatbul@cs.brown.edu)

	// expected values for the accuracy BEGIN (tatbul@cs.brown.edu)
	// 
	float _accuracy;

	double _accuracyUtility;

	float  _outputValue;		// assumption: QoS is on a float attribute
								// NOTE: NOT YET BEING MAINTAINED

	double _outputValueUtility;	// NOTE: NOT YET BEING MAINTAINED
	//
	// expected values for the accuracy END (tatbul@cs.brown.edu)

	// Before a meaningful value has been collected, this field has some value
	// < 0.  Afterwards, it's kept up to date with the QosMonitor's avg. delay
	// value for this arc.
	double _avgLatency; // in josh's code was _qosLatency
	
	double _avgLatencyUtility; // in josh's code was _qosUtility
};


#endif
