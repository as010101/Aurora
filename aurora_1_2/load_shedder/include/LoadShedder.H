
// author: tatbul@cs.brown.edu

#ifndef _LOADSHEDDER_H
#define _LOADSHEDDER_H

#include <global.H>
#include <Predicate.H>
#include <DelayedDataMgr.H>
#include <TraceLogger.H>
#include <QBox.H>
#include <Runnable.H>
#include <LSRM.H>
#include <Parameters.H>
#include <CommonTypes.H>
#include <DropLocations.H>
#include <map>
#include <vector>
#include <string>
#include <assert.h>
#include <pthread.h>

class LoadShedder : public Runnable
{

public:

	LoadShedder(string mode, unsigned int period, string attribute);
	~LoadShedder(); 

	void run() throw();

	void setUtilityLevel(double utility);

	void fillLSInfo(int box_id, QBoxDropInfoMap *ls_info);

	bool isALegalNetwork();

private: 

	bool isLegal(int box_type);
	bool checkBoxLegality();
	void fillNetTopology();								// Kothai
	void formatLTQoS(EntryVector *graph);
	void processApps();									// Kothai
	void assignApps(int box_id, int app_index);			// Kothai
	void setDropLocations();							// Kothai
	void setAppCombinations();							// Kothai
	void setLocations();								// Kothai
	void collectArcs(int loc_index, BoxTop current_box);// Kothai
	bool isSplit(int box_id);
	void assignRateCoefs();								// Kothai
	double computeRateCoef(ArcTop arc);
	void assignLoadCoefs();								// Kothai
	double computeLoadCoef(ArcTop arc);
	void assignQoS();									// Kothai

	void completeValueQoS(int app_id);
	void fillLGRatios();
    void quickSort(RatioEntryVector *v, int p, int r);
	int  partition(RatioEntryVector *v, int p, int r);

    bool fillDropEntry(LSRMEntry *entry, IntVector app_ids, float goal_per);	
	void moveDropBox(LSRMEntry *entry, DropInfo d_info);
	bool fillFilterEntry(LSRMEntry *entry, IntVector app_ids, float goal_per,
						 bool first_entry);					
	void moveFilterBox(LSRMEntry *entry, DropInfo d_info);
	Predicate *makeORPredicate(PredicateVector preds, int offset);
	string makeORPredicate(PredicateStrVector preds, int offset);
	string makeORPredicate(IntervalVec intervals, int offset);
	void simplifyORPredicate(IntervalVec *interval);
	string formatAttribute(string attribute, int arc_id);
	string reFormatAttribute(string attribute);

	bool constructLSRM();

	int lockMutex(pthread_mutex_t *mutex);
	int tryLockMutex(pthread_mutex_t *mutex);
	int unlockMutex(pthread_mutex_t *mutex);

	void printCurve();
	void printNetTopology();
	void printLGRatios();

	bool equalDoubles(double d1, double d2);
	bool equalFloats(float f1, float f2);

	// data about the query network
	//
	ArcMap 				_arc_map;
	NetTopology			_topology;
	DropLocationsVector	_locations;
	RatioEntryVector	_lg_entries;
	bool				_legal;

	// run-time stats
	//
	StatsImage	*_current_stats;

	// data produced by the load shedder
	//
	LSRM				_map;
	int					_level;		// current LS level that achieves _utility
	float				_utility; 	// current utiliy to maintain (set by PM)

	// parameters to the load shedder
	//
	LSMode				_mode;
	unsigned int		_period;
	string				_attribute;

	// synchronization variables
	//
	pthread_mutex_t		*_ls_level_mutex;
	pthread_mutex_t		*_lsrm_mutex;

	// pointers to global objects
	//
	DelayedDataMgr		*_pDelayedDM;
	TraceLogger			*_pLogger;

};

#endif
	
