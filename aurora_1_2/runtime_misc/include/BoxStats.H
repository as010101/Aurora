#ifndef BOXSTATS_H
#define BOXSTATS_H

#include <vector>

using namespace std;

struct BoxStats
{
	// This is the total number of tuples that this box has consumed form any of
	// its input ports during the stats-collection period this BoxStats covers.
	int _numConsumedTuples;

	// Vector has one element per box output arct. The value is the number of
	// tuples that this box has produced on the corresponding output port during
	// the stats-collection period this BoxStats covers.
	//
	// For now, this means that if one output port of a box is the origin or
	// multiple output arcs, we'll see many entries in this vector. Later on we
	// can remove this redundancy.
	//
	// To know which arc_id corresponds to each index into this vector, use these
	// facts:
	// - Each BoxStats object describes some observed behavior of a particular
	//   QBox object. (Information external to this class must exist to know
	//   which QBox object a particular BoxStats object relates to.)
	//
	// - In general, _numProducedTuples[i] corresponds to QBox::_outpuArcId[i]
	//
	// - QBox::_numOutputArcs == _numProducedTuples.size()
	vector<int> _numProducedTuples;

	float _cost;
};

#endif
