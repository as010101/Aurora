#ifndef RUNTIMEGLOBALS_H
#define RUNTIMEGLOBALS_H

#include <exception>

class PropsFile;
class TraceLogger;
class DelayedDataMgr;
class LoadShedder;	// tatbul@cs.brown.edu

#include <PtMutex.H>
#include <PropsFile.H>
#include <TraceLogger.H>
#include <LoadShedder.H>
#include <DelayedDataMgr.H>

using namespace std;

class RuntimeGlobals
{
public:
	//---------------------------------------------------------------------------
	// Methods that should only be called by whatever thread sets up this
	// set of globals. General consumers should never call these methods.
	//
	// Generally, the way to "unset" one of these pointers is by setting it to
	// NULL.
	//---------------------------------------------------------------------------

	static void setPropsFile(const PropsFile * p)
		throw (exception);

	static void setTraceLogger(TraceLogger * p)
		throw (exception);

	static void setDelayedDataMgr(DelayedDataMgr * p)
		throw (exception);

	static void setLoadShedder(LoadShedder * p)
		throw (exception);	// tatbul@cs.brown.edu

	//---------------------------------------------------------------------------
	// Methods anyone can call.
	// The pointer is valid until a set...(...) call (see above) changes it...
	//---------------------------------------------------------------------------

	static const PropsFile * getPropsFile(bool throwExceptionIfNull = true)
		throw (exception);

	static TraceLogger * getTraceLogger(bool throwExceptionIfNull = true)
		throw (exception);

	static DelayedDataMgr * getDelayedDataMgr(bool throwExceptionIfNull = true)
 		throw (exception);

	static LoadShedder * getLoadShedder(bool throwExceptionIfNull = true)
		throw (exception);	// tatbul@cs.brown.edu
	
private:
	static PtMutex _s_mtx;

	static const PropsFile      * _s_pPropsFile;
	static       TraceLogger    * _s_pTraceLogger;
	static       DelayedDataMgr * _s_pHistorian;
	static		 LoadShedder	* _s_pLoadShedder;	// tatbul@cs.brown.edu
};

#endif
