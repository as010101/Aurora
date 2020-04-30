#ifndef STATSIMAGE_H
#define STATSIMAGE_H

#include <map>
#include <BoxStats.H>
#include <InputArcStats.H>
#include <AppArcStats.H>
#include <FilterBoxOutputArcStats.H>
#include <runtime_common_typedefs.H>

struct StatsImage
{
	// map key = boxId/arcId, as appropriate
	map<int, BoxStats>      _boxesStats;
	map<int, InputArcStats> _inputArcsStats;
	map<int, AppArcStats>   _appArcsStats;
        map<int, int>           _numTuplesOnArcs; 
        // number of tuples currently residing in each arc

	// Has one entry for every filter box output port.
	map<BoxOutputPortId, FilterBoxOutputArcStats> _filterOutputsStats;

	// The prediction of the load shedder for, given a certain depth of
	// load shedding, how much overall utility will be exhibited in the query
	// network's outputs.
	LoadShedderCurve _utilityVsLsPrediction;
};

#endif
