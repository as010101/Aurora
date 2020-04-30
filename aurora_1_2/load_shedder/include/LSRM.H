
// author: tatbul@cs.brown.edu

#ifndef _LSRM_H
#define _LSRM_H

#include <CommonTypes.H>
#include <Predicate.H>

class LSRMEntry;

typedef vector<LSRMEntry*> 	LSRM;
typedef LSRM::iterator 		LSRMIter;

typedef struct 
{
	int			arc_id;		// conceptually, drop will be inserted on this arc 
	double		drop_rate; 	// what fraction of the tuples to drop
	bool		pred_flag;
	IntervalVec	intervals;
	string		predicate_str; // string representation of the predicate
							   // tuples that satisfy this predicate will remain
} DropInfo;

typedef map<int, DropInfo>		DropInfoMap;
typedef DropInfoMap::iterator	DropInfoMapIter;

class LSRMEntry 
{

public:

	LSRMEntry(IntVector app_ids);
	LSRMEntry(LSRMEntry& entry);
	~LSRMEntry();

	bool exists(int arc_id);
	void setCursor(int app_id, float percentage);
	void setAggUtility(double utility);
	double getAggUtility();
	void insertDrop(DropInfo d_info);	// if exists, update the drop rate
	void deleteDrop(int arc_id);
	void updateDropRateBy(int arc_id, double rate);
	void print();

	DropInfoMap	_dropInsertionPlan;	
	FloatMap    _QoSCursors;

private:

	double		_aggUtility;	// tuple utility or value utility based on mode
								// rated to have a value in range [0, 1.0]

	void simplifyANDPredicate(IntervalVec v, IntervalVec u, IntervalVec *r);
	string makeORPredicate(IntervalVec intervals, int offset);
	bool equalFloats(float f1, float f2); 
	bool equalDoubles(double d1, double d2); 

};

#endif
	
