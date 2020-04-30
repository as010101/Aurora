#include <DelayedDataMgr.H>
#include <StorageMgr_Exceptions.H>
#include <RuntimeGlobals.H>
#include <LockHolder.H>

#include <algorithm>

using namespace std;

//===============================================================================

DelayedDataMgr::DelayedDataMgr(QueryNetwork & qn)
	throw (exception)
{
  // euh at least next time have <<endl at the end, not cout!
  //cout << "New DelayedDataManager " << cout;
	_shutdownBegun = false;
	_pCheckedOutStats = NULL;
	_pCurrentStats = NULL;

	// Seed the system with null data...
	_pCurrentStats = new StatsImage(qn);
	_staleImageRefCounts.insert(make_pair(_pCurrentStats, 1));
}

//===============================================================================

DelayedDataMgr::~DelayedDataMgr()
	throw (exception)
{
	LockHolder lh(_mtx);

	// Clean up the old stats images. We're not certain about how this class will
	// be used, to complain if we find anything in this map.
	for (map<StatsImage *, int>::iterator pos = _staleImageRefCounts.begin();
		 pos != _staleImageRefCounts.end();
		 ++ pos)
		{
			delete pos->first;
		}
}

//===============================================================================

void DelayedDataMgr::run()
	throw ()
{
}

//===============================================================================

void DelayedDataMgr::shutdown()
{
	_shutdownBegun = true;
}

//===============================================================================

void DelayedDataMgr::addChangeObserver(BinarySem * pFlagSem)
	throw (exception)
{
	LockHolder lh(_mtx);

	if (_shutdownBegun)
		{
			assert(false);
			RuntimeGlobals::getTraceLogger()->log("runtime.critical", "Shutdown already begun");
			abort();
		}

	bool success = _changeObservers.insert(pFlagSem).second;
	if (! success)
		{
			assert(false);
			RuntimeGlobals::getTraceLogger()->log("runtime.critical", "Adding a change observer that's already there");
			abort();
		}
}

//===============================================================================

void DelayedDataMgr::removeChangeObserver(BinarySem * pFlagSem)
	throw (exception)
{
	LockHolder lh(_mtx);

	if (_shutdownBegun)
		{
			assert(false);
			RuntimeGlobals::getTraceLogger()->log("runtime.critical", "Shutdown already begun");
			abort();
		}

	if (_changeObservers.erase(pFlagSem) == 0)
		{
			assert(false);
			RuntimeGlobals::getTraceLogger()->log("runtime.critical", "Removing a change observer that's wasn't there");
			abort();
		}
}

//===============================================================================

StatsImage & DelayedDataMgr::getWritableImage()
	throw (exception)
{
	_activeImageAccess.lock();

	{
		LockHolder lh(_mtx);
		
		// Clone the old "current" image.
		_pCheckedOutStats = new StatsImage(* _pCurrentStats);

		return * _pCheckedOutStats;
	}
}

//===============================================================================

void DelayedDataMgr::releaseWritableImage()
	throw (exception)
{
	LockHolder lh(_mtx);

	// Since the old _pCurrentStats image will no longer hold that role, reduce
	// its ref-count accordingly...
	decrementRefCount(_pCurrentStats);


	// Install the brand-new StatsImage, that the writer just finished modifying,
	// to be the "current" image that readers will draw from...
	_pCurrentStats = _pCheckedOutStats;
 	_pCheckedOutStats = NULL;
	_staleImageRefCounts.insert(make_pair(_pCurrentStats, 1));


	// Open the door for some other updater to do work.
	_activeImageAccess.unlock();

	// Tell everyone waiting on a new version that it is, in fact, available...
	for (set<BinarySem*>::iterator pos = _changeObservers.begin();
		 pos != _changeObservers.end(); 
		 ++ pos)
		{
			(*pos)->post();
		}
}

//===============================================================================

StatsImage * DelayedDataMgr::getReadableImage()
	throw (exception)
{
	LockHolder lh(_mtx);
	incrementRecCount(_pCurrentStats);
	return _pCurrentStats;
}

//===============================================================================

void DelayedDataMgr::releaseReadableImage(const StatsImage * pImage)
	throw (exception)
{
	LockHolder lh(_mtx);
	decrementRefCount(const_cast<StatsImage *>(pImage));
}

//===============================================================================

void DelayedDataMgr::decrementRefCount(StatsImage * pImage)
{
	map<StatsImage *, int>::iterator pos = 
		_staleImageRefCounts.find(pImage);

	assert(pos != _staleImageRefCounts.end());

	-- (pos->second);

	if (pos->second == 0)
		{
			delete pos->first;
			_staleImageRefCounts.erase(pos);
		}
}

//===============================================================================

void DelayedDataMgr::incrementRecCount(StatsImage * pImage)
{
	map<StatsImage *, int>::iterator pos = 
		_staleImageRefCounts.find(pImage);

	assert(pos != _staleImageRefCounts.end());

	++ (pos->second);
}

//===============================================================================

