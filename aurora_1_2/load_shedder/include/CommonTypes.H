
// author: tatbul@cs.brown.edu

#ifndef _COMMONTYPES_H
#define _COMMONTYPES_H

#include <Predicate.H>
#include <Parameters.H>
#include <AccuracyQoS.H> 
#include <vector>
#include <list>
#include <map>

using namespace std;

typedef enum {DropLS, FilterLS, NoLS} LSMode;

typedef vector<double> LSCurve;

typedef map<int, int> IntMap;
typedef IntMap::iterator IntMapIter;

typedef map<int, float> FloatMap;
typedef FloatMap::iterator FloatMapIter;

typedef vector<int> IntVector;
typedef IntVector::iterator IntVectorIter;

typedef vector<bool> BoolVector;
typedef BoolVector::iterator BoolVectorIter;

typedef vector <Predicate*> PredicateVector;
typedef PredicateVector::iterator PredicateVectorIter;

typedef vector <string> PredicateStrVector;
typedef PredicateStrVector::iterator PredicateStrVectorIter;

typedef enum {INPUT, BOX, ARC, APP} IdType;

typedef struct {
    int     id;
    IdType  type;
} Id;

typedef list<Id> IdList;
typedef IdList::iterator IdListIter;

typedef struct {
    Id      id;
    IdList  next;
} InputTop;

typedef struct {
    Id      id;
    Id      prev;
    Id      next;
    double  rate_coef;	// of the stream that flows on this arc
	double  load_coef;	// of a drop box if inserted on this arc
} ArcTop;

typedef struct {
    Id          id;
    IdList      prev;
    IdList      next;
    IntVector   to_apps;
} BoxTop;

class LossToleranceQoS;
class ValueQoS;

typedef struct {
    Id					id;
    Id					prev;
	LossToleranceQoS 	*lt_qos;
	ValueQoS			*val_qos;
	//float				lowest_value;	// from histograms
} AppTop;
// If lt_qos is specified in the GUI, then val_qos must be NULL.
// Also, in fillNetTopology(), lt_qos must be converted to slope-based
// representation from GUI's point-based representation.
// If val_qos is specified in the GUI, then lt_qos must be NULL.
// In this case, we derive the lt_qos from the val_qos.

typedef map<int, InputTop> InputTopMap;
typedef InputTopMap::iterator InputTopMapIter;

typedef map<int, ArcTop> ArcTopMap;
typedef ArcTopMap::iterator ArcTopMapIter;

typedef map<int, BoxTop> BoxTopMap;
typedef BoxTopMap::iterator BoxTopMapIter;

typedef map<int, AppTop> AppTopMap;
typedef AppTopMap::iterator AppTopMapIter;

typedef struct {
    InputTopMap inputs;
    ArcTopMap   arcs;
    BoxTopMap   boxes;
    AppTopMap   apps;
} NetTopology;

typedef struct {
    int 	loc_id;			// index in the DropLocations vector
    float 	max_per;		// max percentage for the function piece
    float 	min_per;		// min percentage for the function piece
    float 	slope;			// of the lt_qos function piece
    double 	lg_ratio;
} RatioEntry;

typedef vector<RatioEntry> RatioEntryVector;
typedef RatioEntryVector::iterator RatioEntryVectorIter;

#endif
