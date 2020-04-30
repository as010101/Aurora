
// author: tatbul@cs.brown.edu

#ifndef _ACCURACYQOS_H
#define _ACCURACYQOS_H

#include <vector>
#include <CommonTypes.H>
#include <TraceLogger.H>

typedef struct {
	int     left_op;    // 0: >, 1: >=
    float   left_val;
    int     right_op;   // 0: <, 1: <=
    float   right_val;
    string  attribute;
} IntervalType;

typedef vector<IntervalType> IntervalVec;
typedef IntervalVec::iterator IntervalVecIter;

typedef struct {
	float x_value;
	float utility;
	float slope;
} Entry;

typedef vector<Entry> EntryVector;
typedef EntryVector::iterator EntryVectorIter;

class LossToleranceQoS 
{

public:

	LossToleranceQoS();
	LossToleranceQoS(EntryVector graph);
	LossToleranceQoS(LossToleranceQoS& qos);
	~LossToleranceQoS() { }

	float getUtility(float x_value);
	float getSlope(float x_value);

	void print();

	EntryVector _graph;
	float		_cursor;

private:

	bool equalFloats(float f1, float f2);
	bool equalDoubles(double d1, double d2);

	TraceLogger *_pLogger;

};

typedef struct {
	float lower_value;
	float upper_value;	// don't use this! this is exclusive
	float utility;
	float relative_frequency;
	float weighted_utility;
	float normalized_utility;
} ValueIntervalEntry;

// vector must be sorted on utility in ascending order
typedef vector<ValueIntervalEntry> IntervalVector;
typedef IntervalVector::iterator IntervalVectorIter;

class ValueQoS
{

public:

	ValueQoS();
	ValueQoS(ValueQoS& qos);
	ValueQoS(int num, float min, float max, IntervalVector v);
	~ValueQoS();

	LossToleranceQoS *computeLTQoS();
	string computeFilterPredicate(float filter_percentage, 
								  float current_percentage, 
								  string attribute,
								  IntervalVec *intervals);

	void sortIntervals();

	void print();

	int				_num_intervals;
	IntervalVector	_intervals;
	
	float 			_min_val;	// possible min for the range
	float			_max_val;	// possible max for the range

private:

	bool equalFloats(float f1, float f2);
	bool equalDoubles(double d1, double d2); 

};

#endif
