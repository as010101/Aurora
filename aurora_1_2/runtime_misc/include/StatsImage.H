#ifndef STATSIMAGE_H
#define STATSIMAGE_H

#include <map>
#include <BoxStats.H>
#include <InputArcStats.H>
#include <AppArcStats.H>
#include <runtime_common_typedefs.H>
#include <exception>
#include <string>

using namespace std;

class QueryNetwork;
class Box;

struct StatsImage
{
	// Just lets each member's default c'tor do its thing
	StatsImage()
		throw (exception);

	// Initializes the StatsImage s.t. for each arc mentioned in 'qn', 
	// _numTuplesOnArcs has an entry with a tuple-count of -1.
	StatsImage(QueryNetwork & qn)
		throw (exception);

	// map key = boxId/arcId, as appropriate
	map<int, BoxStats>      _boxesStats;
	map<int, int>           _numTuplesOnArcs; // number of tuples currently residing in each arc

	map<int, string>		_dropsOnArcs; 	  // drop info for arcs that currently have drops on them

	map<int, InputArcStats> _inputArcsStats;
	map<int, AppArcStats>   _appArcsStats;

	// The prediction of the load shedder for, given a certain depth of
	// load shedding, how much overall utility will be exhibited in the query
	// network's outputs.
	LoadShedderCurve _utilityVsLsPrediction;

	string toString();
};

#endif
