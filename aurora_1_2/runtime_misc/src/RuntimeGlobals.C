#include <RuntimeGlobals.H>
#include <LockHolder.H>
#include <StorageMgr_Exceptions.H>

PtMutex           RuntimeGlobals::_s_mtx;
const PropsFile * RuntimeGlobals::_s_pPropsFile;
TraceLogger     * RuntimeGlobals::_s_pTraceLogger;
DelayedDataMgr  * RuntimeGlobals::_s_pHistorian;
LoadShedder		* RuntimeGlobals::_s_pLoadShedder;	// tatbul@cs.brown.edu

//===============================================================================

void RuntimeGlobals::setPropsFile(const PropsFile * p)
	throw (exception)
{
	LockHolder lh(_s_mtx);
	_s_pPropsFile = p;
}

//===============================================================================

void RuntimeGlobals::setTraceLogger(TraceLogger * p)
	throw (exception)
{
	LockHolder lh(_s_mtx);
	_s_pTraceLogger = p;
}

//===============================================================================

void RuntimeGlobals::setDelayedDataMgr(DelayedDataMgr * p)
	throw (exception)
{
	LockHolder lh(_s_mtx);
	_s_pHistorian = p;
}

//===============================================================================

void RuntimeGlobals::setLoadShedder(LoadShedder * p)
	throw (exception)
{
	LockHolder lh(_s_mtx);
	_s_pLoadShedder = p;
}

//===============================================================================

const PropsFile * RuntimeGlobals::getPropsFile(bool throwExceptionIfNull)
	throw (exception)
{
	LockHolder lh(_s_mtx);

	if (throwExceptionIfNull && (_s_pPropsFile == NULL))
		{
			throw SmException(__FILE__, __LINE__, 
							  "RuntimeGlobals::getPropsFile() would have returned NULL");
		}

	return _s_pPropsFile;
}

//===============================================================================

TraceLogger * RuntimeGlobals::getTraceLogger(bool throwExceptionIfNull)
		throw (exception)
{
	LockHolder lh(_s_mtx);

	if (throwExceptionIfNull && (_s_pTraceLogger == NULL))
		{
			throw SmException(__FILE__, __LINE__, 
							  "RuntimeGlobals::getTraceLogger() would have returned NULL");
		}

	return _s_pTraceLogger;
}


//===============================================================================

DelayedDataMgr * RuntimeGlobals::getDelayedDataMgr(bool throwExceptionIfNull)
		throw (exception)
{
	LockHolder lh(_s_mtx);

	if (throwExceptionIfNull && (_s_pHistorian == NULL))
		{
			throw SmException(__FILE__, __LINE__, 
							  "RuntimeGlobals::getDelayedDataMgr() would have returned NULL");
		}

	return _s_pHistorian;
}

//===============================================================================

LoadShedder * RuntimeGlobals::getLoadShedder(bool throwExceptionIfNull)
		throw (exception)
{
	LockHolder lh(_s_mtx);

	if (throwExceptionIfNull && (_s_pLoadShedder == NULL))
		{
			throw SmException(__FILE__, __LINE__,
							  "RuntimeGlobals::getLoadShedder() would have returned NULL");
		}

	return _s_pLoadShedder;
}

//===============================================================================

