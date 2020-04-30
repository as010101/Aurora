
// author: tatbul@cs.brown.edu

#ifndef _DROP_LOCATIONS_H
#define _DROP_LOCATIONS_H

#include <vector>
#include <CommonTypes.H>

using namespace std;

class DropLocation;

typedef vector<DropLocation *> DropLocationsVector;
typedef DropLocationsVector::iterator DropLocationsIter;

class DropLocation 
{

public:

	DropLocation();
	DropLocation(DropLocation& dl);
	~DropLocation(); 

	void compareAppIds(IntVector v, BoolVector *result);
	bool isSymmetricSubset(IntVector v, IntVector u);
	bool isSubset(IntVector v, IntVector u);
	void addArc(int arc_id);
	void unionAppIds(IntVector app_ids1, IntVector app_ids2);
	void setArcIds(IntVector arc_ids);
	void setAppIds(IntVector app_ids);
	void print();

	IntVector 			_app_ids;	// applications affected
	IntVector 			_arc_ids;	// potential points for drop insertion
	LossToleranceQoS  	*_qos; 		// aggregate loss tolerance QoS

};

#endif
