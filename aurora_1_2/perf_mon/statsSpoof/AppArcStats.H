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
	vector<float> _valueHistogram;

	// The recent utility on this output as perceived by the load shedder. 
	// Whichever QoS graph is currently in use by the load shedder defines
	// the meaning of this utility value.
	float _qosUtility;
};

#endif

