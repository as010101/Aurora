#ifndef DELAYEDDATAMGR_H
#define DELAYEDDATAMGR_H

#include <exception>
#include <StatsImage.H>
#include <PtMutex.H>
#include <BinarySem.H>
#include <Runnable.H>
#include <FifoCriticalSection.H>
#include <set>

using namespace std;

/**
   Provides a double-buffered scheme for certain volatile Aurora runtime 
   metadata. Specifically designed for data whose consumers can tolerate
   a small amount of staleness in the data.
 */
class DelayedDataMgr : public Runnable
{
public:
	DelayedDataMgr(QueryNetwork & qn)
		throw (exception);

	virtual ~DelayedDataMgr()
		throw (exception);

	// As defined in the Runnable class.
	virtual void run()
		throw ();

	// Call this before destroying the object, but after all threads are done 
	// invoking this object's methods. Calling this will result in the 
	// eventual return from the run() method.
	void shutdown();

	//---------------------------------------------------------------------------
	// Methods for users of this object...
	//---------------------------------------------------------------------------
	// Adds pFlagSem to the set of BinarySem objects that will be post()'ed 
	// whenever a new StatsImage becomes available for read-access. Throws an
	// exception if it's already in that set.
	void addChangeObserver(BinarySem * pFlagSem)
		throw (exception);

	// Removes pFlagSem from the set of observers. If it's not already in the
	// set, an exception is thrown.
	void removeChangeObserver(BinarySem * pFlagSem)
		throw (exception);


	// This blocks until the caller is allowed to modify the write-image of the
	// stats. Until the caller calls 'releaseWritableImage()', no other call to
	// this method will succeed, and readers won't see a new version of the 
	// stats.
	//
	// This lock isn't reentrant, but we're also not necessarily checking for
	// self-deadlock. So just keep your life simple, and use if this way:
	//
	//     DelayedDataMgr * pDdm = RuntimeStats::getDelayedDataMgr();
	//     pDdm->getWritableImage();
	//     ...
	//     pDdm->releaseWritableImage();
	//
	// It's guaranteed that maps in the returned StatsImage will contain all
	// appropriate entries given the current network topology.
	StatsImage & getWritableImage()
		throw (exception);

	// Call this once for every call to getWritableImage(). Behavior isn't 
	// defined if you call this more than you should, or from a different thread 
	// than the call to getWritableImage() was made on.
	void releaseWritableImage()
		throw (exception);


	// Use this to get access to the potentially-stale-but-hopefully-not-by-much
	// StatsImage. Note that it's a const pointer: Don't override that.
 	//
	// Once you call this, you're also obligated to call releaseReadableImage().
	// Until you do so, never versions of the stats image can't be made public.
	//
	// It's guaranteed that maps in the returned StatsImage will contain all
	// appropriate entries given the current network topology.
	StatsImage * getReadableImage()
		throw (exception);

	// (Same caveats as stated in the comments of releaseWritableImage())
	//
	// Provide the same pointer you acquired using getReadableImage. We realy on
	// that for ref counting.
	void releaseReadableImage(const StatsImage * pImage)
		throw (exception);

private: 
	// Reduces the reference count of the given StatsImage in 
	// _staleImageRefCounts by 1. If that reduces the count to 0, then the entry
	// is deleted from the map and the StatsImage object is also deleted.
	//
	// Behavior not defined if pImage isn't in the map.
	void decrementRefCount(StatsImage * pImage);

	// Opposite of decrementRefCount. The map must already contain an entry
	// for pImage.
	void incrementRecCount(StatsImage * pImage);

	// When not NULL, this is the copy of the stats image that some write has
	// checked out for modification.
	StatsImage * _pCheckedOutStats;

	// The version of the stats that the reader's should acquire. When not NULL,
	// there's a corresponding entry in _staleImageRefCounts.
	StatsImage * _pCurrentStats;

	FifoCriticalSection _activeImageAccess;

	// For each stale stats image, this tracks the total number of read-locks 
	// that are held on it. When it goes to 0, a StatsImage is deleted.
	//
	// An entry in this map being pointed to by _pCurrentStats counts as a 
	// reference and affects the ref-count accordingly.
	map<StatsImage *, int> _staleImageRefCounts;

	set<BinarySem*> _changeObservers;
	bool _shutdownBegun;
	PtMutex _mtx;
};

#endif
