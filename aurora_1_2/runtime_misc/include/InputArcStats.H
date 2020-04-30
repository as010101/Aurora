#ifndef INPUTARCSTATS_H
#define INPUTARCSTATS_H

#include <vector>

using namespace std;

struct InputArcStats
{
	// Has 'N' slots, where 'N' is given by the PropsFile property 
	// "LoadShedder.inputRateMaxHistory". The 0th element in the vector is
	// the newest.  When the system starts up, the entire vector is filled with
	// zeros.
	//
	// The slots are updated approximately periodically.  The update rate is 
	// specified by the PropsFile property "LoadShedder.inputRateSamplePeriodMs".
	// This period is given in terms of milliseconds. We'll call this period P.
	//
	// Because this a not running on a realtime operating system, the time 
	// periods used to populate the lots are approximate, and skew can accumulate.
	vector<int> _inputRateHistory;
};

#endif
