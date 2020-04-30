#ifndef AURORANODEIMPL_H
#define AURORANODEIMPL_H

#include <string>
#include <exception>
#include <map>
#include <vector>
#include <AuroraNode.H>
#include <PropsFile.H>
#include <QueryNetwork.H>
#include <Scheduler.H>
#include <CatalogManager.H>
#include <SchedulerRunnable.H>
#include <RunnableRunner.H>
#include <StreamThreadThrottle.H>
#include <AppTupleMonitor.H>
#include <TraceLogger.H>
#include <LoadShedder.H>
#include <ExportDelayedData.H>
#include <LoadShedder.H>
#include <PtMutex.H>
#include <InputPortBoxInfo.H>

using namespace std;

// All of this public methods correspond to the same-signatured methods in the
// AuroraNode class. Look there for documentation.

class AuroraNodeImpl : public AuroraNode
{
public:
	AuroraNodeImpl(string propsFilename)
		throw (exception);

	AuroraNodeImpl(string propsFilename, string prefix, int argc, const char * argv[])
		throw (exception);

	virtual ~AuroraNodeImpl();

	virtual void shutdown()
		throw (exception);

	// NOTE: Be careful: These to *not* necessarily correlate to the labels such
	// as "i0", "i1", and "o0" that appear in the GUI on the i/o ports.
	virtual void listPorts(vector<int> & in, vector<int> & out) const
		throw (exception);

	virtual void enqRecord(int inputPortId,
						   int streamId, 
						   const void *tupleAddr, 
						   size_t tupleSize)
		throw (exception);

	virtual void enqBatch(int inputPortId,
						  const void * pBuffer,
						  size_t bufferSize,
						  size_t numTuples)
		throw (exception);

	virtual bool deqRecord(int & outputPortId, void *tupleAddr, size_t bufSize)
		throw (exception);

	virtual bool deqBatch(int & outputPortId, int & numTuples, void *pBuffer, size_t bufSize)
		throw (exception);

	virtual void xStartLoadGenerator()
		throw (exception);

	virtual void xStopLoadGenerator()
		throw (exception);

	virtual bool xHasSchedulerRequestedShutdown()
		throw (exception);

 	virtual void schedulerFinishedWait();

	virtual TraceLogger & xGetTraceLogger()
		throw (exception);


	bool isDeqTupleAvailable()
		throw (exception);

	size_t getTupleSizeByOutputPort(int portNum) const
		throw (exception);

	//---------------------------------------------------------------------------
	// Methods that are public only so that helper threads can access them...
	//---------------------------------------------------------------------------

	// Keeps the DelayedDataMgr's up to date, to the extent that this class is
	// responsible for doing so.
	//
	// Returns sometime after _shuttingDown has been posted.
	//
	// The DelayedDataMgr object and the RuntimeGlobals object must exist during
	// the entire time this method is invoked.
	void maintainDdmStats()
		throw (exception);

private:
	const PropsFile    * _pProps;
	CatalogManager     * _pCatalogMgr;
	QueryNetwork       * _pQueryNetwork;
	Scheduler          * _pScheduler;
	Catalog            * _pSchedCatalog;
	Measurement        * _pSchedMeasure;
	StreamThread       * _pSchedStreamThread;
	SchedulerRunnable  * _pSchedRunnable;
	RunnableRunner     * _pSchedRunner;
	LoadShedder	       * _pLoadShedder;
	RunnableRunner     * _pLoadShedderRunner;
	RunnableRunner     * _pInputRateMonitorRunnable;

	// Note: This points to the same object as the variable _qos that's declared in
	// sched/src/global.H. This alias exists to help us remember that it the
	// QoSMonitor object must be cleaned up when this AuroraNodeImpl is cleaning 
	// up.
	QoSMonitor         * _pSchedQoSMon;

	// Controls when _pSchedStreamThread is actually doing work. Also is used 
	// during shutdown to tell _pSchedStreamThread that it's time to shut down.
	StreamThreadThrottle _loadGenThrottle;

	// Key = input port ID (presently just the box ID of the input port)
	map<int, InputPortBoxInfo> _inputPortInfoMap;

	// Hold this mutex whenever accessing the _inputPortInfoMap's dependent data.
	PtMutex _mtxInputPortInfoMap; 



	// Key = arcId for an arc that's connected to an output port.
	// Value = output port ID (presently just the box ID of the input port)
	map<int, int> _queueIdToOutputPortMap;

        // Key = output port ID
        // Value = # bytes (including std. header) for tuples on that port.
        map<int, size_t> _outputPortToTupleSizeMap;

	// Becomes posted whenn shutdown() is called.
	BinarySem _shuttingDown;

	// This is how the Scheduler lets us know that there are tuples available
	// on the output queues.
	//
	// NOTE: This can also be broadcast to if this AuroraNodeImpl has received
	// the 'shutdown' command. In such a case, the '_shutingDown' instance variable
	// will become 'true' prior to the corresponding broadcast of 
	// _tupleMon._cond.
	AppTupleMonitor _tupleMon;

	// Never NULL after the c'tor has returned. The fact that no logging is desired
	// is expressed in the attributes of the TraceLogger, not by _pLogger == NULL.
	TraceLogger * _pLogger;
  
	ExportDelayedData * _pDDExporter;
	RunnableRunner * _pDDExporterRunner;

	// Part of sched/src/main.C's code...
	pthread_mutex_t _tuple_count_mutex;
	// Populates _inputPortToQueueIdMap and _outputPortToQueueIdMap
	void calculateArcPortMaps(QueryNetwork & qn)
		throw (exception);

	// The bulk of the API initialization.  Happens after the PropsFile has been
	// instantiated and is pointed to by _pProps.
	void initApi()
		throw (exception);

	// Initializes all of the global variables and objects from 
	// /aurora/sched/src/... that must be initialized prior to calling 
	// Scheduler::start().
	//
	// Requires the following instance variables to already be setup:
	//   _pQueryNetwork
	//   _pProps
	//
	// Also sets up the instance variables:
	//   _pSchedCatalog
	//   _pScheduler
	//   _pSchedMeasure
	//   _pSchedQoSMon
	//   _pSchedStreamThread
	//
	// This method start's neither _pSchedStreamThread nor a thread to run 
	// _pScheduler->start(), but does leave the AuroraNodeImpl in good shape to 
	// actually launch those threads whenever it's desired.
	void initScheduler()
		throw (exception);

	// Prints to stdout the Scheduler measurements.
	// Requires _pProps and _pSchedMeasure to be already instiantiated.
	void printSchedMeasures()
		throw (exception);

	//	int 			_sched_beq_size;
	//  	int 			_sched_num_buckets;
	//  	double 			_sched_per_box_cost;
	string			_sched_measure_output_file;
	double			_sched_rate_control;
};

#endif
