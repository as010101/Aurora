#include <AuroraNodeImpl.H>
#include <Scheduler.H>
#include <Measurement.H>
#include <util.H>
#include <algorithm>
#include <StorageMgr_Exceptions.H>
#include <vector>
#include <RuntimeGlobals.H>
#include <DelayedDataMgr.H>
#include <LoadShedder.H>
#include <string.h>
#include <iostream>
#include <InputRateMonitorRunnable.H>
#include <TraceLogger.H>
#include <logutil.H>

extern QueueMon* _queue_monitor; // Defined in aurora/sched/src/Shared.C


AuroraNodeImpl::AuroraNodeImpl(string propsFilename)
	throw (exception) :
	_loadGenThrottle(THROTTLE_STOPPED)
{
	_pProps = new PropsFile(propsFilename);

	// For now, we're keeping both the global variables originally used by  the
	// Scheduler, and the RuntimeGlobals' copies.
	//
	// In the long run, however, all global (in the C++ sense) variables should be
	// migrated into RuntimeGlobals, for managability, clarity, and debugging
	// purposes. -cjc
	RuntimeGlobals::setPropsFile(_pProps);

	initApi();
}

AuroraNodeImpl::AuroraNodeImpl(string propsFilename, string prefix, int argc, const char * argv[])
	throw (exception) :
	_loadGenThrottle(THROTTLE_STOPPED)
{
	_pProps = new PropsFile(propsFilename, prefix, argc, argv);

	// In the long run, however, all global (in the C++ sense) variables should be
	// migrated into RuntimeGlobals, for managability, clarity, and debugging
	// purposes. -cjc
	RuntimeGlobals::setPropsFile(_pProps);

	initApi();
}

//===============================================================================

AuroraNodeImpl::~AuroraNodeImpl()
{
	_pDDExporterRunner->join();
	delete _pDDExporterRunner;
	delete _pDDExporter;

	delete _pInputRateMonitorRunnable->getRunnable();
	delete _pInputRateMonitorRunnable;

	delete _pSchedRunner;
	delete _pSchedRunnable;
	delete _pSchedStreamThread;
	delete _pScheduler;
	delete _pCatalogMgr;
	delete _pSchedCatalog;
	delete _pSchedQoSMon;

	printSchedMeasures();
	delete _pSchedMeasure;
  
	delete RuntimeGlobals::getDelayedDataMgr();
	RuntimeGlobals::setDelayedDataMgr(NULL);

	delete RuntimeGlobals::getTraceLogger();
	RuntimeGlobals::setTraceLogger(NULL);

	delete RuntimeGlobals::getPropsFile();
	RuntimeGlobals::setPropsFile(NULL);

	// LoadShedder-related clean-up	BEGIN (tatbul@cs.brown.edu)
	//
	delete _pLoadShedderRunner;
	delete RuntimeGlobals::getLoadShedder();
	RuntimeGlobals::setLoadShedder(NULL);
	//
	// LoadShedder-related clean-up 	END (tatbul@cs.brown.edu)

	// shmid and semid are global variables defined in aurora/sched/src/Shared.C
	cleanup(shmid,semid);

	/*
	printf("ALLSTATS: ??? %lf %lf %lf\n",_measure->getTimeSpentInStreamThread(),
				  						_measure->getTimeSpentInWorkerThread(),
										_measure->getTimeSpentScheduling());
	*/
}

//===============================================================================

void AuroraNodeImpl::shutdown()
	throw (exception)
{
	// This both ensures the binary sem is posted, and tells us if someone else
	// had already posted it.
	bool shutdownAlreadyBegun = _shuttingDown.post();
	if (shutdownAlreadyBegun)
		{
			return;
		}

	// Wake up any pending user calls to deqRecord(...)...
	{
		LockHolder lh(_tupleMon._mtx);
 
		// Wake up any pending user calls to deqRecord(...)...
		_tupleMon._value._shutdown = true;
		_tupleMon._cond.broadcast();
	}

	_pDDExporter->shutdown();

	// Tell everyone in the scheduler to wrap things up...
	__global_stop.setStop();

	// Tell the load generator thread towrap thigs up...
	_loadGenThrottle.setValue(THROTTLE_DONE);

	if (_pLoadShedderRunner != NULL)
		{
			_pLoadShedderRunner->join();		// tatbul@cs.brown.edu
		}

	_pInputRateMonitorRunnable->join();
	_pSchedRunner->join();
	_pSchedStreamThread->join();
}

//===============================================================================

void AuroraNodeImpl::listPorts(vector<int> & in, vector<int> & out) const 
	throw (exception)
{
	in.clear();
	out.clear();
	for (map<int, InputPortBoxInfo>::const_iterator pos =  _inputPortInfoMap.begin();
		 pos !=  _inputPortInfoMap.end(); ++pos)
		{
			in.push_back(pos->first);
		}

	for (map<int, int>::const_iterator pos = _queueIdToOutputPortMap.begin();
		 pos != _queueIdToOutputPortMap.end(); ++pos)
		{
			out.push_back(pos->second);
		}
}

//===============================================================================

void AuroraNodeImpl::enqRecord(int inputPortId, 
							   int streamId, 
							   const void *tupleAddr, 
							   size_t tupleSize)
	throw (exception)
{
	map<int, InputPortBoxInfo>::iterator pos =  _inputPortInfoMap.find(inputPortId);

	if (pos ==  _inputPortInfoMap.end())
		{
			ostringstream os;
			os << "The specified inputPortId value: " << inputPortId << " didn't "
			   << "match any of this AuroraNode's input port IDs";
			throw SmException(__FILE__, __LINE__, os.str());
		}

	int arcId = pos->second._arcId;


	// Verify that the tuple size is appropriate...
	Arcs * pArc = _pSchedCatalog->getArc(arcId);
	TupleDescription * pTupleDesc = pArc->getTupleDescr();  
	int bytesPerTuple = pTupleDesc->getSize() + TUPLE_TIMESTAMP_SIZE + TUPLE_STREAMID_SIZE;

	if (tupleSize != bytesPerTuple)
		{
			ostringstream os;
			os << "The user-supplied buffer was " << tupleSize << " bytes large, but "
			   << "the port to be enqueued into requires a tuple size of " << bytesPerTuple;
			throw SmException(__FILE__, __LINE__, os.str());
		}


	// Go ahead and enqueue it...
	SMInterface smi;
	char * pBuffer = smi.enqueuePin(arcId, 1);

	timeval * pTimestamp = 
		reinterpret_cast<timeval *>(pBuffer + TUPLE_TIMESTAMP_OFFSET);

	if (gettimeofday(pTimestamp, NULL) != 0) 
		{
			throw SmException(__FILE__, __LINE__, 
							  "StreamThread: gettimeofday failed, setting timestamp for new tuple");
		}

	memcpy(pBuffer + TUPLE_STREAMID_OFFSET, & streamId, sizeof(streamId));
  
	memcpy(reinterpret_cast<char *>(pBuffer) + TUPLE_DATA_OFFSET,
		   reinterpret_cast<const char *>(tupleAddr) + TUPLE_DATA_OFFSET,
		   tupleSize - TUPLE_DATA_OFFSET);

	smi.enqueueUnpin(arcId, pBuffer, 1); 

	// Question: should I use SM.SM_free(...) or free(...)  here?
	// Answer  : I am pretty sure that SM_free. it is safer, though SM calls free(..) too.
	smi.SM_free(pBuffer);

	// Update the local input-port stats...
	{
		LockHolder lh(_mtxInputPortInfoMap);
		++ (pos->second._tuplesEnqSinceLastCheck);
	}
}

//===============================================================================

void AuroraNodeImpl::enqBatch(int inputPortId,
							  const void * pUserBuffer,
							  size_t bufferSize,
							  size_t numTuples)
throw (exception)
{
	map<int, InputPortBoxInfo>::iterator pos =  _inputPortInfoMap.find(inputPortId);

	if (pos == _inputPortInfoMap.end())
		{
			ostringstream os;
			os << "The specified inputPortId value: " << inputPortId << " didn't "
			   << "match any of this AuroraNode's input port IDs";
			throw SmException(__FILE__, __LINE__, os.str());
		}

	int arcId = pos->second._arcId;


	// Verify that the tuple size is appropriate...
	Arcs * pArc = _pSchedCatalog->getArc(arcId);
	TupleDescription * pTupleDesc = pArc->getTupleDescr();  
	int bytesPerTuple = pTupleDesc->getSize() + TUPLE_TIMESTAMP_SIZE + TUPLE_STREAMID_SIZE;

	long long totalEnqBytes = bytesPerTuple * numTuples;
	assert(totalEnqBytes < numeric_limits<int>::max());

	if (bufferSize < totalEnqBytes)
		{
			ostringstream os;
			os << "bufferSize = " << bufferSize << ", but caller claimed it contained "
			   << numTuples << " tuples, which we know are supposed to each be "
			   << bytesPerTuple << " bytes long.  The buffer isn't big enough.";
			throw SmException(__FILE__, __LINE__, os.str());
		}

	if (numTuples == 0)
		{
			return;
		}

	// Go ahead and enqueue it...
	SMInterface smi;
	char * pSmBuffer = smi.enqueuePin(arcId, numTuples);

	// Copy the actual tuple content from the user's buffer into SMInterface's buffer.
	memcpy(pSmBuffer, pUserBuffer, totalEnqBytes);

	// Replicate the timestamp into every tuple being enqueued...
	timeval now;
	if (gettimeofday(& now, NULL) != 0) 
		{
			throw SmException(__FILE__, __LINE__, 
							  "StreamThread: gettimeofday failed, setting timestamp for new tuple");
		}

	for (size_t i = 0; i < numTuples; ++i)
		{
			char * pTuple = pSmBuffer + (i * bytesPerTuple);
			memcpy(pTuple, & now, sizeof(now));			
		}

	smi.enqueueUnpin(arcId, pSmBuffer, numTuples); 
	smi.SM_free(pSmBuffer);

	// Update the local input-port stats...
	{
		LockHolder lh(_mtxInputPortInfoMap);
		(pos->second._tuplesEnqSinceLastCheck) += numTuples;
	}
}

//===============================================================================

bool AuroraNodeImpl::deqRecord(int & outputPortId, void *tupleAddr, 
							   size_t bufSize)
	throw (exception)
{
	int arcToDequeue;
	bool arcIsChosen = false;

	while (! arcIsChosen)
		{
			LockHolder lh(_tupleMon._mtx);

			if ((_tupleMon._value._shutdown) || (_tupleMon._value._schedWantsShutdown))
				{
					return false;
				}

			// There should definitely be a tuple in one of the output queues.
			// Arbitrarily pick some tuple in some output queue...
			map<int, int> & arcCounts = _tupleMon._value._appArcCounts;

			for (map<int, int>::iterator pos = arcCounts.begin();
				 (pos != arcCounts.end()) && (! arcIsChosen);
				 ++pos)
				{
					if (pos->second > 0)
						{
							arcIsChosen = true;
							arcToDequeue = pos->first;
							-- pos->second;
						}
				}

			if (! arcIsChosen)
				{
					// Wait for something intersting, if we need to...
					_tupleMon._mtx.waitCond(_tupleMon._cond);
				}
		}

	// If flow got here, then an arc was chosen, and it's ID is stored in 
	// arcToDequeue...

	// Set output parameter: outputPortId
	map<int, int>::const_iterator pos = _queueIdToOutputPortMap.find(arcToDequeue);
	if (pos == _queueIdToOutputPortMap.end())
		{
			ostringstream os;
			os << "Internal error: Couldn't find output port for arc id " << arcToDequeue;
			throw SmException(__FILE__, __LINE__, os.str());
		}
	outputPortId = pos->second;

	// Verify that the user's buffer is big enough...
	Arcs * pArc = _pSchedCatalog->getArc(arcToDequeue);
	TupleDescription * pTupleDesc = pArc->getTupleDescr();  
	int bytesPerTuple = pTupleDesc->getSize() + TUPLE_TIMESTAMP_SIZE + TUPLE_STREAMID_SIZE;

	if (bufSize < bytesPerTuple)
		{
			ostringstream os;
			os << "The user-supplied buffer was " << bufSize << " bytes large, but "
			   << "the arc to be dequeued from (" << arcToDequeue << ") has a tuple size of " << bytesPerTuple;
			throw SmException(__FILE__, __LINE__, os.str());
		}

	// Actually get the data...
	SMInterface smi;

	void * pBuffer = smi.dequeuePin(arcToDequeue, 1);

	memcpy(tupleAddr, pBuffer, bytesPerTuple);
	smi.dequeueUnpin(arcToDequeue, 1);
	smi.SM_free(pBuffer);
	return true;
}

//===============================================================================

void AuroraNodeImpl::xStartLoadGenerator()
	throw (exception)
{
	_loadGenThrottle.setValue(THROTTLE_RUNNING);
}

//===============================================================================

void AuroraNodeImpl::xStopLoadGenerator()
	throw (exception)
{
	_loadGenThrottle.setValue(THROTTLE_STOPPED);
}

//===============================================================================

void AuroraNodeImpl::calculateArcPortMaps(QueryNetwork & qn)
	throw (exception)
{
	_inputPortInfoMap.clear();
	_queueIdToOutputPortMap.clear();

	StreamMap      & isMap  = qn.getInputStreams();
	ApplicationMap & appMap = qn.getApplications();
	ArcMap         & arcMap = qn.getArcs();

	vector<int> inputPortBoxIds;
	for (StreamMapIter isPos = isMap.begin(); isPos != isMap.end(); ++isPos)
		{
			inputPortBoxIds.push_back(isPos->first);
		}

	vector<int> outputPortBoxIds;
	for (ApplicationMapIter appMapPos = appMap.begin(); appMapPos != appMap.end(); 
		 ++appMapPos)
		{
			outputPortBoxIds.push_back(appMapPos->first);      
		}
  
	for (ArcMapIter amPos = arcMap.begin(); amPos != arcMap.end(); ++amPos)
		{
			int arcId = amPos->first;
			Arc & a = *(amPos->second);
			assert(arcId == a.getId());

			vector<int>::const_iterator inputPortBoxNumPos = 
				find(inputPortBoxIds.begin(), inputPortBoxIds.end(), a.getSourceId());

			// Maybe the arc is connected to a SourceStream? ...
			if (inputPortBoxNumPos != inputPortBoxIds.end())
				{
					int boxId = *inputPortBoxNumPos;
					_inputPortInfoMap.insert(make_pair(boxId, InputPortBoxInfo(arcId, 0)));
				}
			// Maybe the arc is connected to an Application? ...
			else
				{
					vector<int>::const_iterator outputPortBoxNumPos = 
						find(outputPortBoxIds.begin(), outputPortBoxIds.end(), a.getDestinationId());
	  
					if (outputPortBoxNumPos != outputPortBoxIds.end())
						{
							int boxId = *outputPortBoxNumPos;
							_queueIdToOutputPortMap.insert(make_pair(arcId, boxId));
						} 
				}
		}
  
	assert(inputPortBoxIds.size()  ==  _inputPortInfoMap.size());
	assert(outputPortBoxIds.size() == _queueIdToOutputPortMap.size());
}

//===============================================================================

void AuroraNodeImpl::printSchedMeasures()
	throw (exception)
{
	double rateControl    = _pProps->getDouble("Scheduler.rateControl");
	int    kVal           = _pProps->getInt(   "Scheduler.k_val");
	int    burstSize      = _pProps->getInt(   "Scheduler.burst_size");
	double fixedTrainSize = _pProps->getDouble("Scheduler.fixed_train_size");

	// Fudging because PropsFile doesn't currently support 'float' properties...
	double f;
	f = _pProps->getDouble("Scheduler.per_box_cost");
	if (f < numeric_limits<float>::min() || 
		f > numeric_limits<float>::max())
		{
			ostringstream os;
			os << "The property \"Scheduler.per_box_cost\" must have a value that fits"
			   << " in a 'float' data type";
			throw SmException(__FILE__, __LINE__, os.str());
		}
	float perBoxCost = float(f);

	_pSchedMeasure->addMeasurementType( TIME_TOTAL_SCHED_WORKER_THREADS );
	_pSchedMeasure->addMeasurementType( TIME_SCHEDULING_VS_WORKER_THREADS );
	_pSchedMeasure->addMeasurementType( TIME_SPENT_SCHEDULING );
	_pSchedMeasure->addMeasurementType( TIME_SPENT_IN_WORKER_THREADS );
	_pSchedMeasure->addMeasurementType( NUM_SCHEDULING_DECISIONS );
	_pSchedMeasure->addMeasurementType( NUM_BOX_CALLS );
	_pSchedMeasure->addMeasurementType( AVERAGE_LATENCY );
	_pSchedMeasure->addMeasurementType( AVERAGE_QOS );
	_pSchedMeasure->addMeasurementType( AVERAGE_TUPLE_TRAIN_SIZE );
	_pSchedMeasure->addMeasurementType( DISK_IO );
	_pSchedMeasure->addMeasurementType( DISK_READS );
	_pSchedMeasure->addMeasurementType( DISK_WRITES  );
	_pSchedMeasure->addMeasurementType( TOTAL_RUN_TIME );
	_pSchedMeasure->addMeasurementType( NUM_MALLOCS );
	_pSchedMeasure->addMeasurementType( TIME_LOADING_QUEUES );
	_pSchedMeasure->addMeasurementType( TIME_UNLOADING_QUEUES );
	_pSchedMeasure->addMeasurementType( TIME_SPENT_IN_STREAM_THREAD );
	_pSchedMeasure->addMeasurementType( TIME_SPENT_IN_DO_BOX);
	_pSchedMeasure->addMeasurementType( TIME_SPENT_EXECUTING_BOXES);
	_pSchedMeasure->addMeasurementType( GENERAL_PROF_STATS);
	_pSchedMeasure->addMeasurementType( SCHEDULER_PROF_STATS);
	_pSchedMeasure->addMeasurementType( WORKERTHREAD_PROF_STATS);
 	_pSchedMeasure->addMeasurementType( BOX_OVERHEAD);
 	_pSchedMeasure->addMeasurementType( AVG_NUM_SCHEDULABLE_BOXES);


	_pSchedMeasure->addXVariable( INPUT_RATE );
	_pSchedMeasure->addXVariable( APP_DEPTH );
	_pSchedMeasure->addXVariable( APP_WIDTH );
	_pSchedMeasure->addXVariable( MEMORY_SIZE );
	_pSchedMeasure->addXVariable( BOX_COST );
	_pSchedMeasure->addXVariable( QOS_BOUND );
	_pSchedMeasure->addXVariable( APP_COUNT );
	_pSchedMeasure->addXVariable( BOX_COUNT );
	_pSchedMeasure->addXVariable( K_SPANNER_VAL );
	_pSchedMeasure->addXVariable( BURST_SIZE );
	_pSchedMeasure->addXVariable( TRAIN_SIZE );
 	_pSchedMeasure->addXVariable( NUM_BUCKETS );
 	_pSchedMeasure->addXVariable( BEQ_SIZE );

	_pSchedMeasure->setInputRate( rateControl );

	_pSchedMeasure->setBoxCost( perBoxCost );
	_pSchedMeasure->setKSpannerVal( kVal );
	_pSchedMeasure->setBurstSize( burstSize );
	//printf(" MEASURE setting TRAIN SIZE %f\n", fixedTrainSize );
	_pSchedMeasure->setTrainSize( fixedTrainSize );

	_pSchedMeasure->getMemRemainingGraph();
	_pSchedMeasure->outputMeasurements();
}

//===============================================================================

void AuroraNodeImpl::initScheduler()
    throw (exception)
{
	_pSchedMeasure = new Measurement(& _tupleMon);
	_measure = _pSchedMeasure; // sched/src/global.H defines this, so we need to set it
	_sched_measure_output_file = _pProps->getStringWithDefault("Scheduler.measure_output_file", "DEFAULT");
	_measure->setOutputFilename((char *)_sched_measure_output_file.c_str());

	_pSchedCatalog = new Catalog();
	_catalog = _pSchedCatalog; // sched/src/global.H defines this, so we need to set it

	if (_pProps->getBool("Scheduler.experiment_mode"))
		{
			_pSchedCatalog->setExperimentFlag(true);
		}

	_pSchedCatalog->loadFromDB(_pQueryNetwork);

	//_pSchedCatalog->printArcs();

	// Note: These also initialize the global variables (defined in 
	// aurora/sched/src/Shared.C): shmid and semid
	// Note: 'initialize_shared_mem' MUST be called sometime *after* the statement 
	// immediately above this one, becuase it relies on knowing the number of arcs
	// in the network.
	initialize_shared_mem(SHM_KEY);
	initialize_semaphore(SEMKEY);

	// NOTE: I'm probably missing ALL of the stuff for creating the StreamThreads, 
	// which are (I think) used for load generation.

	//this has to come before the scheduler start, since SMI uses
	// that number for memory
	_pSchedMeasure->setMemorySize(_pProps->getULong("Scheduler.mem_size"));

	// NOTE: For some reason, these used to be called only when the catalog was 
	// generated by this program. I had to move this out of that code, because this
	// program is now ignorant of whether or not a catalog was generated 
	// programatically.
	// 
	// And besides, it's not clear to me why you'd only want to call these methods
	// when the catalog was auto-generated. It seems reasonable to call this even
	// when a human created the _pCatalogMgr->  -cjc, 22 Feb 03.
	_pSchedMeasure->setNumApps(_pQueryNetwork->getNumberOfApplications());
	_pSchedMeasure->setAppCount(_pQueryNetwork->getNumberOfApplications());
	_pSchedMeasure->setBoxCount( _catalog->getNumBoxes() );
	_pSchedMeasure->setQoSBound(_pCatalogMgr->getQoSBound() );

	// Set the scheduler's stop type/condition...
	string stopType = _pProps->getString("Scheduler.stop_type");
	_pSchedMeasure->setStopType(const_cast<char *>(stopType.c_str()));

	if (stopType == "TUPLES_WRITTEN")
		{
			_pSchedMeasure->
				setStopCondTuplesWritten(_pProps->getUInt("Scheduler.stop_cond_tuples_written"));
		}
	else if (stopType == "TUPLES_WRITTEN_TO_MEMORY")
		{
			_pSchedMeasure->
				setStopCondTuplesWrittenToMemory(_pProps->getUInt("Scheduler.stop_cond_tuples_written_to_memory"));
		}
	else if (stopType == "TIME_STOP")
		{
			_pSchedMeasure->
				setStopCondTimeStop(_pProps->getInt("Scheduler.stop_cond_timed_stop"));
		}

	// Fudging because PropsFile doesn't currently support 'float' properties...
	double f;
	f = _pProps->getDouble("Scheduler.per_box_cost");
	if (f < numeric_limits<float>::min() || 
		f > numeric_limits<float>::max())
		{
			ostringstream os;
			os << "The property \"Scheduler.per_box_cost\" must have a value that fits"
			   << " in a 'float' data type";
			throw SmException(__FILE__, __LINE__, os.str());
		}
	float perBoxCost = float(f);

	f = _pProps->getDouble("Scheduler.per_box_selectivity");
	if (f < numeric_limits<float>::min() || 
		f > numeric_limits<float>::max())
		{
			ostringstream os;
			os << "The property \"Scheduler.per_box_selectivity\" must have a value that fits"
			   << " in a 'float' data type";
			throw SmException(__FILE__, __LINE__, os.str());
		}
	float perBoxSelectivity = float(f);

	for ( int i = 0; i <= _pSchedCatalog->getMaxBoxId(); i++ )
		{
			Boxes *b = _pSchedCatalog->getBox(i);
			if ( b != NULL )
				{
					if ( perBoxCost >= 0 )
						b->setCost(perBoxCost);
					if ( perBoxSelectivity >= 0 )
						b->setSelectivity(perBoxSelectivity);
					//printf("b->setSelectivity: %f\n",b->getSelectivity());
				}
		}

	int arcs = _pSchedCatalog->getMaxArcId()+1;
	//printf("DEBUG:SMI: Allocated 2X %d\n", sizeof( int ) * arcs );
	memory_in_use = 0;
	pthread_mutex_init( &memory_mutex, NULL );
	queueAccessTm = ( int *) malloc( sizeof( int ) * arcs );
	bzero( queueAccessTm, sizeof( int ) * arcs );
	tuplesOnDisk = new int[ arcs ];
	bzero( tuplesOnDisk, arcs * sizeof( int ) );

	pthread_mutex_init( &__box_work_set_mutex, NULL );

	_pSchedStreamThread = new StreamThread(_pSchedCatalog->getNumInputs(), 
										   _pSchedCatalog->getMaxArcId(), 
										   & _tuple_count_mutex, 
										   _loadGenThrottle,
										   _mtxInputPortInfoMap,
										   _inputPortInfoMap );

	_pSchedMeasure->setStreamThread(_pSchedStreamThread);

	bool tpbMode = _pProps->getBoolWithDefault
		("Scheduler.one_thread_per_box", false);
	if ( tpbMode )
		_catalog->initTPBSignalConditions();

	/* prfered one */

 	_pSchedMeasure->setNumBuckets(_pProps->getIntWithDefault("Scheduler.num_buckets", 5));

	_pScheduler = new Scheduler(_pProps->getInt("Scheduler.max_queue_size"),
								_pProps->getInt("Scheduler.max_num_worker_threads"),
							    _pProps->getInt("Scheduler.num_buckets"),
								& _tupleMon);

	int sched_beq_size = _pProps->getIntWithDefault("Scheduler.box_execution_queue_size", 10);
 	_pSchedMeasure->setBEQSize(sched_beq_size);
 	_pScheduler->setBEQSize(sched_beq_size);
 
 	_pSchedMeasure->setSecondsPerRand(_pProps->getDoubleWithDefault("Scheduler.seconds_per_rand", 1.3e-07)); 
 
	/* old one 
>>>>>>> 1.33
	if ( _sched_num_worker_threads_opt == false )
		_sched_num_worker_threads = _pProps->getInt("Scheduler.max_num_worker_threads");
	if ( _sched_num_buckets_opt == false )
			_sched_num_buckets = _pProps->getInt("Scheduler.num_buckets");
	_pScheduler = new Scheduler(_pProps->getInt("Scheduler.max_queue_size"),
								_sched_num_worker_threads,
							    _sched_num_buckets,
								& _tupleMon);
	*/
	_pSchedQoSMon = new QoSMonitor( 2, _pQueryNetwork );
	_qos = _pSchedQoSMon;
	_pSchedQoSMon->run();

	// Set the scheduler's major scheduling method...
	string schedBy = _pProps->getString("Scheduler.sched_by");
	if (schedBy == "BOX_SCHEDULING")
		{
			_pScheduler->setSchedBy(SCHED_BY_BOX);
		}
	else if (schedBy == "APP_SCHEDULING")
		{
			_pScheduler->setSchedBy(SCHED_BY_APP);
		}
  
	// Set the scheduler's schedule type...
	if ((schedBy == "BOX_SCHEDULING") || (schedBy == "APP_SCHEDULING"))
		{
			string priorityAssignment = _pProps->getString("Scheduler.priority_assignment");
			if (priorityAssignment == "SLOPE")
				{
					if (schedBy == "BOX_SCHEDULING")
						_pScheduler->setBoxScheduleType(SLOPE_SLACK_TYPE);
					else // schedBy == "APP_SCHEDULING"
						_pScheduler->setAppScheduleType(APP_SLOPE_SLACK_TYPE);
				}
			else if (priorityAssignment == "RR")
				{
					if (schedBy == "BOX_SCHEDULING")
						_pScheduler->setBoxScheduleType(BOX_RR_TYPE);
					else // schedBy == "APP_SCHEDULING"
						_pScheduler->setAppScheduleType(APP_RR_TYPE);
				}
			else if (priorityAssignment == "RANDOM")
				{
					if (schedBy == "BOX_SCHEDULING")
						_pScheduler->setBoxScheduleType(BOX_RANDOM_TYPE);
					else // schedBy == "APP_SCHEDULING"
						_pScheduler->setAppScheduleType(APP_RANDOM_TYPE);
				}
			else if (priorityAssignment == "LQF")
				{
					if (schedBy == "BOX_SCHEDULING")
						_pScheduler->setBoxScheduleType(BOX_LQF_TYPE);
					else // schedBy == "APP_SCHEDULING"
						_pScheduler->setAppScheduleType(APP_LQF_TYPE);
				}
			else if (priorityAssignment == "BUCKETING")
				{
					if (schedBy == "BOX_SCHEDULING")
						_pScheduler->setBoxScheduleType(BOX_BUCKETING_TYPE);
					else // schedBy == "APP_SCHEDULING"
						_pScheduler->setAppScheduleType(APP_BUCKETING_TYPE);
				}
			else
				{
					throw SmException(__FILE__, __LINE__,  
									  "The property \"Scheduler.priority_assignment\" has an illegal value");
				}
		}
	else // default scheduling
		{
			_pScheduler->setSchedBy(SCHED_BY_BOX);
			_pScheduler->setBoxScheduleType(BOX_RR_TYPE);
			_pScheduler->setAppScheduleType(APP_RANDOM_TYPE);
		}

	///////////////////////////////////////////////////////////////////////////////
	// BEGIN: Code for starting up the StreamThreads
	///////////////////////////////////////////////////////////////////////////////

	// Jeff's Stuff
	//  vector<StreamThread *>	input_arcs;
	//Arcs *a;
	//printf("GOT TO A numInputs : %i\n", _pSchedCatalog->getNumInputs());
	//StreamThread *st = NULL;
	if (stopType == "TUPLES_GENERATED_STOP")
		{
		int numTuples = _pProps->getInt("Scheduler.stop_cond_num_tuples_to_generate");
		_pSchedStreamThread->setNumTuplesToGenerate(numTuples);
		}

	bool rateFlag = _pProps->getBool("Scheduler.rate_flag");

	double per_input_interval=0.0;

	if (rateFlag)
		{
			// multiply _sched_rate_control by num inputs (its not a rate .. rather its
			// an interval (1/rate)
			per_input_interval = _pScheduler->getNetworkCost();
			double rateControl    = _pProps->getDouble("Scheduler.rateControl");

			if ( rateControl > 0.0 )
				{
					per_input_interval = per_input_interval/rateControl;
				}
			else
				{
					// multiply _sched_rate_control by num inputs (its not a rate .. rather its
					// an interval (1/rate)
					per_input_interval = _pScheduler->getNetworkCost();

					if ( _sched_rate_control > 0.0 )
					{
						per_input_interval = per_input_interval/_sched_rate_control;
					}
					else
					{
						per_input_interval = fabs (_sched_rate_control);
						//printf("c per_input_interval: %f\n",per_input_interval);
					}
					double total_input_rate = (1.0/per_input_interval)*_pSchedCatalog->getNumInputs();
					//printf("per_input_interval: %f\n",per_input_interval);
					//printf("total_input_rate (tuples per second): %f\n", total_input_rate);

					//per_input_interval = fabs(rateControl); // Trunk Version
				}
			double total_input_rate = (1.0/per_input_interval)*_pSchedCatalog->getNumInputs();
		}

	int schedBurstSize = _pProps->getInt("Scheduler.burst_size");

	for ( int i = 0; i < _pSchedCatalog->getNumInputs(); i++ )
		{
			Inputs *inputs = _pSchedCatalog->getInput(i);
			{
				if (rateFlag)
					{
						_pSchedStreamThread->addInput( inputs->getArcId(), per_input_interval,
													   inputs->getNumTuples(), schedBurstSize ); 
					}
				else
					{
						_pSchedStreamThread->addInput( inputs->getArcId(), inputs->getRate(), 
													   inputs->getNumTuples(), schedBurstSize ); 
					}
			}
		}
  
	_pSchedStreamThread->setSchedulerWakeCondition( _pScheduler->getWakeCondition() );
	_pSchedStreamThread->setSchedulerWakeMutex( _pScheduler->getWakeMutex() );
	_pSchedStreamThread->setSchedulerWakeInterval( _pProps->getInt("StreamThread.sched_wake_interval"));

	///////////////////////////////////////////////////////////////////////////////
	// END: Code for starting up the StreamThreads
	///////////////////////////////////////////////////////////////////////////////

	// April 2003 - The Rebirth of QueueMon.
	if (_pProps->getBoolWithDefault("Scheduler.queue_mon_flag", false))
		{
			_queue_monitor = new QueueMon(_pQueryNetwork, _pSchedStreamThread, _pSchedCatalog);
			_queue_monitor->start(5);
			sleep(2);
	    
			// Block everyone right here until the viewer says "run". We'll just use busy-waiting right now
			while (!_queue_monitor->isRunning()) 
				sleep(1);
		}


	string traversalType = _pProps->getStringWithDefault("Scheduler.traversal_type", 
														 "MIN_COST");

	if (traversalType == "MIN_COST")
		{
			_pScheduler->setBoxTraversalType(MIN_COST);
		}
	else if (traversalType == "MIN_LATENCY")
		{
			_pScheduler->setBoxTraversalType(MIN_LATENCY);
		}
	else if (traversalType == "MIN_MEMORY")
		{
			_pScheduler->setBoxTraversalType(MIN_MEMORY);
		}
	else
		{
			throw SmException(__FILE__, __LINE__,  
							  "The property \"Scheduler.traversal_type\" has an illegal value");
		}

	if (_pProps->getBool("Scheduler.top_k_spanner_flag"))
		{
			int kVal = _pProps->getInt("Scheduler.k_val");
			_pScheduler->setTopKSpanner(kVal);
		}

	double fixedTrainSize = _pProps->getDouble("Scheduler.fixed_train_size");
	_pScheduler->setFixedTrainSize(fixedTrainSize);
	_pSchedMeasure->setStartTime();
	_pScheduler->setFixedTrainSize(fixedTrainSize);
	_pSchedMeasure->setStartTime();
}

//===============================================================================

void AuroraNodeImpl::maintainDdmStats()
	throw (exception)
{
	unsigned long sleepPeriod = RuntimeGlobals::getPropsFile()->
		getUInt("LoadShedder.inputRateSamplePeriodMs") * 1000;

	unsigned int numHistorySlots = RuntimeGlobals::getPropsFile()->
		getUInt("LoadShedder.inputRateMaxHistory");

	DelayedDataMgr & ddm = * RuntimeGlobals::getDelayedDataMgr();

	while (! _shuttingDown.isPosted())
		{
			usleep(sleepPeriod);

			StatsImage & si = ddm.getWritableImage();
			LockHolder lh(_mtxInputPortInfoMap);

			// Shift the old history to mave room for the new entry.
			// 
			// (yes, we could skip all this using a circular buffer, but that 
			// takes a while to debug sometimes.  We can swith techniques for 
			// storing this info, if/when it's known to be worthwhile)
			for (map<int, InputPortBoxInfo>::iterator pos = _inputPortInfoMap.begin();
				 pos != _inputPortInfoMap.end();
				 ++pos)
				{
					// This is in fact boxId, though an uninitiated one
					// might think its arc id. Box id (of the input port!)
					// is used as a key.
					int boxId = pos->first;

					vector<int> & history = si._inputArcsStats[boxId]._inputRateHistory;

					// Quickly shift the contents of slots 
					// [0]...[numHistorySlots-2]  into
					// [1]...[numHistorySlots-1]

					if (numHistorySlots > 0)
						{
							for (size_t i = numHistorySlots - 1; 
								 i >= 1; --i)
								{
									history[i] = history[i-1];
								}

							history[0] = pos->second._tuplesEnqSinceLastCheck;
						}

					pos->second._tuplesEnqSinceLastCheck = 0;
				}
			ddm.releaseWritableImage();
		}
}

//===============================================================================

size_t AuroraNodeImpl::getTupleSizeByOutputPort(int portNum) const
	throw (exception)
{
	map<int, size_t>::const_iterator pos = _outputPortToTupleSizeMap.find(portNum);

	if (pos == _outputPortToTupleSizeMap.end())
		{
			ostringstream os;
			os << "AuroraNodeImpl::getTupleSizeByOutputPort: " << portNum 
			   << " isn't a valid output port number";

			throw SmException(__FILE__, __LINE__, os.str());
		}

	return pos->second;
}

//===============================================================================

bool AuroraNodeImpl::isDeqTupleAvailable()
	throw (exception)
{
	LockHolder lh(_tupleMon._mtx);
	AppTupleInfo & ati = _tupleMon._value;

	for (map<int, int>::const_iterator pos = ati._appArcCounts.begin();
		 pos != ati._appArcCounts.end();
		 ++pos)
		{
			if (pos->second > 0)
				{
					return true;
				}
		}

	return false;
}

//===============================================================================

/*void AuroraNodeImpl::schedulerFinishedWait()
{
	pthread_mutex_lock(__global_stop.getSchedFinishedMutex());
	pthread_cond_wait(__global_stop.getSchedFinishedWaitCond(), __global_stop.getSchedFinishedMutex());
	pthread_mutex_unlock(__global_stop.getSchedFinishedMutex());

}*/

//===============================================================================

bool AuroraNodeImpl::xHasSchedulerRequestedShutdown()
	throw (exception)
{
	LockHolder lh(_tupleMon._mtx);
	return _tupleMon._value._schedWantsShutdown;
}

//===============================================================================

TraceLogger & AuroraNodeImpl::xGetTraceLogger()
	throw (exception)
{
	return * _pLogger;
}

//===============================================================================

bool AuroraNodeImpl::deqBatch(int & outputPortId, int & numTuplesToDeq, void *pBuffer, size_t bufSize)
		throw (exception)
{
	int arcToDequeue;
	int numTuplesAvailable;
	int bytesPerTuple;
	bool arcIsChosen = false;

	while (! arcIsChosen)
		{
			LockHolder lh(_tupleMon._mtx);

			if ((_tupleMon._value._shutdown) || (_tupleMon._value._schedWantsShutdown))
				{
					return false;
				}

			// There should definitely be a tuple in one of the output queues.
			// Arbitrarily pick some tuple in some output queue...
			map<int, int> & arcCounts = _tupleMon._value._appArcCounts;

			for (map<int, int>::iterator pos = arcCounts.begin();
				 (pos != arcCounts.end()) && (! arcIsChosen);
				 ++pos)
				{
					arcToDequeue = pos->first;
					numTuplesAvailable = pos->second;

					Arcs * pArc = _pSchedCatalog->getArc(arcToDequeue);
					TupleDescription * pTupleDesc = pArc->getTupleDescr();  
					bytesPerTuple = pTupleDesc->getSize() + TUPLE_TIMESTAMP_SIZE + TUPLE_STREAMID_SIZE;

					if (bufSize < bytesPerTuple)
						{
							ostringstream os;
							os << "The user-supplied buffer was " << bufSize << " bytes large, but "
							   << "the arc to be dequeued from has a tuple size of " << bytesPerTuple;
							throw SmException(__FILE__, __LINE__, os.str());
						}

					int maxTuplesInBuffer = bufSize / bytesPerTuple;
					numTuplesToDeq = min(maxTuplesInBuffer, numTuplesAvailable);

					if (numTuplesToDeq > 0)
						{
							pos->second -= numTuplesToDeq;
							arcIsChosen = true;
						}
				}

			if (! arcIsChosen)
				{
					// Wait for something intersting, if we need to...
					_tupleMon._mtx.waitCond(_tupleMon._cond);
				}
		}

	// Set output parameter: outputPortId
	map<int, int>::const_iterator pos = _queueIdToOutputPortMap.find(arcToDequeue);
	if (pos == _queueIdToOutputPortMap.end())
		{
			ostringstream os;
			os << "Internal error: Couldn't find output port for arc id " << arcToDequeue;
			throw SmException(__FILE__, __LINE__, os.str());
		}
	outputPortId = pos->second;



	// Actually get the data...
	SMInterface smi;
	void * pSmBuffer = smi.dequeuePin(arcToDequeue, numTuplesToDeq);

	memcpy(pBuffer, pSmBuffer, bytesPerTuple * numTuplesToDeq);
	smi.dequeueUnpin(arcToDequeue, numTuplesToDeq);
	smi.SM_free(pSmBuffer);
	return true;
}

//===============================================================================

void AuroraNodeImpl::initApi()
	throw (exception)
{
	int rc = pthread_mutex_init(& _tuple_count_mutex, NULL);
	if (rc != 0)
		{
			perror( "/////   AuroraNodeImpl::AuroraNodeImpl");
			cerr << "/////         rc = " << rc << endl << endl;
			abort();
		}

	_pLogger = new TraceLogger(* _pProps);
	RuntimeGlobals::setTraceLogger(_pLogger);

	string catalogDir = _pProps->getString("CatalogMgr.directory");

	//cout << "catalogDir: " << catalogDir << endl;
	_pCatalogMgr = new CatalogManager(catalogDir);
	_pQueryNetwork = _pCatalogMgr->load();

	calculateArcPortMaps(* _pQueryNetwork);

	// Put off setting the DelayedDataMgr until now, because we need a QueryNetwork
	// object to construct it. Set the other RuntimeGlobals earlier, however, so 
	// that the TraceLogger and PropsFile are available ASAP to our initialization
	// code.
	RuntimeGlobals::setDelayedDataMgr(new DelayedDataMgr(* _pQueryNetwork));

	// No other thread is using this yet, so we can avoid the locking/broadcast 
	// issues...
	for (map<int, int>::const_iterator pos = _queueIdToOutputPortMap.begin();
		 pos != _queueIdToOutputPortMap.end();
		 ++pos)
		{
			_tupleMon._value._appArcCounts.insert(make_pair(pos->first, 0));
		}
	initScheduler();

  // Populate _outputPortToTupleSizeMap  (must happen after initScheduler())...
  for (map<int, int>::const_iterator pos = _queueIdToOutputPortMap.begin();
	   pos != _queueIdToOutputPortMap.end(); ++pos)
	  {
		  int portId = pos->second;
		  int arcId = pos->first;

		  Arcs * pArc = _pSchedCatalog->getArc(arcId);
		  TupleDescription * pTupleDesc = pArc->getTupleDescr();  
		  size_t bytesPerTuple = pTupleDesc->getSize() + TUPLE_TIMESTAMP_SIZE + TUPLE_STREAMID_SIZE;

		  _outputPortToTupleSizeMap[portId] = bytesPerTuple;
	  }

	// LoadShedder-related initializations	BEGIN (tatbul@cs.brown.edu)
	//
	// Note: Properties are to be set here!!
	_pLoadShedder = new LoadShedder(_pProps->getString("LoadShedder.mode"), 
									_pProps->getUInt("LoadShedder.period"), 
									_pProps->getString("LoadShedder.attribute")); 
	RuntimeGlobals::setLoadShedder(_pLoadShedder);
	//
	// LoadShedder-related initializations    END (tatbul@cs.brown.edu)

	_pSchedStreamThread->start();

	_pSchedRunnable = new SchedulerRunnable(* _pScheduler);

	_pSchedRunner = new RunnableRunner(* _pSchedRunnable); 

	_pDDExporter = new ExportDelayedData(10000, 10);
	_pDDExporterRunner = new RunnableRunner(* _pDDExporter);

	_pInputRateMonitorRunnable = 
		new RunnableRunner(* (new InputRateMonitorRunnable(*this)));

	// activate the load shedder thread (note: must be activated after 
	// Stats and Scheduler threads)
	if (((_pProps->getString("LoadShedder.mode") == "DropLS") ||
		 (_pProps->getString("LoadShedder.mode") == "FilterLS")) &&
		(_pLoadShedder->isALegalNetwork()))
		{
			_pLoadShedderRunner = new RunnableRunner(* _pLoadShedder);
		}
	else
		{
			_pLoadShedderRunner = NULL;
		}
}

//===============================================================================

void AuroraNodeImpl::schedulerFinishedWait()
{
	int rc;
	rc = pthread_mutex_lock(__global_stop.getSchedFinishedMutex());
	assert(rc);

	rc = pthread_cond_wait(__global_stop.getSchedFinishedWaitCond(), __global_stop.getSchedFinishedMutex());
	assert(rc);

	rc = pthread_mutex_unlock(__global_stop.getSchedFinishedMutex());
	assert(rc);
}

//===============================================================================
