#include "StreamThread.H"
#include "tupleGenerator.H"
#include "Measurement.H"

extern QueueMon* _queue_monitor;

int StreamThread::_tuples_generated = 0;
double StreamThread::_rate_multiplier = 1.0;
int StreamThread::_input_counter = 0;

pthread_cond_t    *StreamThread::_sched_wake_cond  = NULL;
pthread_mutex_t   *StreamThread::_sched_wake_mutex = NULL;

int NextArrival::operator <(const NextArrival &na) const
{
	return _next_t < na._next_t;
}


int NextArrival::operator ==(const NextArrival &na) const
{
	//printf("WTF3 call eq\n");
	return ( _arc_id == na._arc_id );
}


StreamThread::StreamThread(int arc_id, double interarrival,
int num_messages, int train_size,
pthread_mutex_t *tuple_count_mutex,
StreamThreadThrottle & throttle,
PtMutex & mtxInputPortInfoMap,
map<int, InputPortBoxInfo> & inputPortInfoMap ):
_throttle(throttle),
_mtxInputPortInfoMap( mtxInputPortInfoMap ),
_inputPortInfoMap( inputPortInfoMap )
{
	fd = NULL;

	//_arc_id = arc_id;
	_interarrival = interarrival * train_size;	  //this should prob. be done here
	_num_messages = num_messages;
	_train_size = train_size;
	_tuple_count_mutex = tuple_count_mutex;
	setStatus( 1, 0, 0 );
	_sched_wake_interval = 1;

	for (map<int, InputPortBoxInfo>::iterator pos = _inputPortInfoMap.begin();
		pos != _inputPortInfoMap.end(); ++pos)
	_queueIdToInputPortMap[ pos->first ] = pos->second._arcId;
	_num_tuples_to_generate = -1;
};

StreamThread::StreamThread(int input_count,
int max_arc_id,
pthread_mutex_t *tuple_count_mutex,
StreamThreadThrottle & throttle,
PtMutex & mtxInputPortInfoMap,
map<int, InputPortBoxInfo> & inputPortInfoMap ):
_throttle(throttle),
_mtxInputPortInfoMap( mtxInputPortInfoMap ),
_inputPortInfoMap( inputPortInfoMap )
{
	fd = NULL;
	_input_count = input_count;

	_interarrivals    = ( double* )malloc( sizeof( double ) * max_arc_id + 1);
	_destinations     = ( int* )malloc( sizeof( int ) * (max_arc_id + 1));
	_arc_ids          = ( int* )malloc( sizeof( int ) * (max_arc_id + 1));
	_train_sizes      = ( int* )malloc( sizeof( int ) * (max_arc_id + 1));
	_num_messages_arr = ( int* )malloc( sizeof( int ) * (max_arc_id + 1));
	_accumulator      = ( int* )malloc( sizeof( int ) * (max_arc_id + 1));
	bzero( _accumulator, sizeof( int ) * (max_arc_id+1) );

	_tuple_count_mutex = tuple_count_mutex;
	setStatus( 1, 0, 0 );
	_sched_wake_interval = 1;

	for (map<int, InputPortBoxInfo>::iterator pos = _inputPortInfoMap.begin();
		pos != _inputPortInfoMap.end(); ++pos)
	_queueIdToInputPortMap[ pos->first ] = pos->second._arcId;
	_num_tuples_to_generate = -1;
}


// This determines the current mode of work.
// send_to_out -- send the data to the input ports
// write_to_file -- write the data to file
// read_from_file -- read data from a data file.
void StreamThread::setStatus( int send_to_out, int write_to_file,
int read_from_file )
{
	status = 0;
	if ( send_to_out ) status = status | SEND_TO_OUT;
	if ( write_to_file ) status = status | WRITE_TO_FILE;
	if ( read_from_file ) status = status | READ_FROM_FILE;
}


StreamThread::~StreamThread()
{
};

void StreamThread::addInput( int arc_id, double interarrival,int num_messages,
int train_size )
{
	
	//pair<set<NextArrival>::iterator,bool> res;
	// here's the place to add the inputs!
	// what do I do with num_messages??
	// printf("StreamThread: adding input %d with inter %f and counter %d\n", arc_id, interarrival, _input_counter );

	_arc_ids[ _input_counter ] = arc_id;
	// do I even need those?? NextArrival structures can keep track...?
	_interarrivals[ _input_counter ] = interarrival;
	_num_messages_arr[ _input_counter ] = num_messages;
	_train_sizes[ _input_counter ] = train_size;

	//printf("StreamThread innited arrays\n");

	NextArrival *na = new NextArrival;

	na->_interarrival = interarrival;
	na->_next_t = 0.0;
	na->_arc_id = arc_id;
	na->_num_messages_remain = num_messages;	  // even this is in na structure?
	// yes, I think so. it's not the same as total count, this one decreases!
	na->_seq_id = _input_counter;

	//printf("StreamThread inited NextArrival object\n");
	//printf(" WTF3 before %d insert arc %d\n", allInputs.size(), na->_arc_id );
	allInputs.insert( *na );
	//printf(" WTF3 after  %d\n", allInputs.size() );

	_input_counter++;

	//printf(" StreamThread: finished adding input %d\n", arc_id );
}


void StreamThread::setRate(double r)
{
	_interarrival = r;
}


double StreamThread::getRate(double r)
{
	return _interarrival;
}


double StreamThread::getRateMultiplier()
{
	double rm;
	pthread_mutex_lock(_tuple_count_mutex);
	rm =  _rate_multiplier;
	pthread_mutex_unlock(_tuple_count_mutex);
	return rm;
}


void StreamThread::setRateMultiplier(double rm)
{
	perror("StreamThread: rate multiplier is currently disabled due to reconstructions\n");
	exit( 1 );

	pthread_mutex_lock(_tuple_count_mutex);
	_rate_multiplier = rm;
	pthread_mutex_unlock(_tuple_count_mutex);
}


int StreamThread::getTuplesGenerated()
{
	int tg;
	pthread_mutex_lock(_tuple_count_mutex);
	tg =  _tuples_generated;
	pthread_mutex_unlock(_tuple_count_mutex);
	return tg;
}


void StreamThread::incrementTuplesGenerated(int incr)
{
	pthread_mutex_lock(_tuple_count_mutex);
	_tuples_generated += incr;
	pthread_mutex_unlock(_tuple_count_mutex);
}


void *StreamThread::run()
{
	int temp;									  //n
	//int _queue;
	float r;
	timeval *t = new timeval;
	time_t start_t;
	//char buff[MAXBUFF + 1];
	TupleDescription *tuple_descr;
	int num_fields, field_offset;				  //, c=0;
	double last_time = 0.0, inter_time;
	NextArrival na;

	unsigned int seed = time( NULL );			  //_arc_id;

	if ( (status & ( WRITE_TO_FILE | READ_FROM_FILE )) > 0 )
		;										  //sprintf( queueFilename, "input.%05d", _arc_id );

	if ( (status & WRITE_TO_FILE) > 0 )
		fd = fopen( queueFilename, "w+b" );

	if ( (status & READ_FROM_FILE) > 0 )
		fd = fopen( queueFilename, "rb" );

	if ( fd == NULL && ( (status&( WRITE_TO_FILE | READ_FROM_FILE)) > 0 ) )
	{
		;										  //printf("StreamThread: Arc %d input I/O Failure\n", _arc_id );
		exit( 1 );
	}

	// Wait until either the user wants us to generate load, or wants us to
	// shut down.
	// The purpose of this barrier is to make sure that all of the other
	// Scheduler objects have been initialized before we proceed.
	_throttle.awaitValueNotEquals(THROTTLE_STOPPED);

	int total_tuple_count = 0;
	timeval chunk_start;
	gettimeofday(&chunk_start,NULL);

	bool done = false;
	struct itimerval first;
	init_etime(&first);
	double secs;
	secs = get_etime(&first);

	while ( (allInputs.size() > 0) && (! done))
	{
		// Wait until either the user wants us to generate load, or wants us to
		// shut down...
		StreamThreadThrottleState tState =
			_throttle.getValueWhenNotEquals(THROTTLE_STOPPED);

		if (tState == THROTTLE_DONE)
		{
			done = true;
			continue;
		}
		assert(tState == THROTTLE_RUNNING);

		if ( _num_tuples_to_generate > 0 )
		{
				if ( total_tuple_count >= _num_tuples_to_generate )
				{
						_measure->testStopCond();
						done = true;
						continue;
				}
		}

		set<NextArrival>::iterator iter;
		for ( iter = allInputs.begin(); iter != allInputs.end(); iter++ )
		{
			//printf("WTF3 arc: %d nextt: %f inter: %f\n",(*iter)._arc_id, (*iter)._next_t, (*iter)._interarrival );
			;
		}

		// note that access to na is not mutexed, because there is only
		// one stream thread and na object is local to its run method
		na = *( allInputs.begin() );

		// take out the next arrival object.
		// REALLY has to be done like that, since allInputs is a multiset!
		//printf("WTF3 size now was %d\n", allInputs.size() );
		allInputs.erase( allInputs.begin() );
		//printf("WTF3 size now is %d\n", allInputs.size() );
		//printf("StreamThread(?) for arc: %i    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ %d\n", na._arc_id, _train_sizes[ na._seq_id ] );
		//_arc_id,_train_size);

		SMInterface SM;

		char* ptr;

		(void) time (&start_t);
		Arcs *arc = _catalog->getArc( na._arc_id );

		tuple_descr=arc->getTupleDescr();

		num_fields = tuple_descr->getNumOfFields();

		// tell the queue monitor (if in use) how many messages there will be
		if (_queue_monitor != NULL)
		{
			_queue_monitor->setNumMessages( _num_messages_arr[ na._seq_id ] );
			//_num_messages);
		}
		
		// printf("_interarrival: %f and as of now remain %d\n", na._interarrival, na._num_messages_remain);
		//	for(int i=0; i<(_num_messages/_train_size); i++)
		if ( na._num_messages_remain > 0 )
		{
			// If the queue monitor is in use, check if we are paused
			if (_queue_monitor != NULL)
			{
				while (!_queue_monitor->isRunning()) sleep(1);
			}
			for ( int tuple = 0; tuple < _train_sizes[ na._seq_id ]; tuple++ )
			{
				//printf("[StreamThread] Tuple %i/%i, train sz=%d tuple is %d\n", tuple, _train_sizes[ na._seq_id ], _train_sizes[ na._seq_id ], tuple );
				// Remember: tuples now implicitly include a special timestamp and streamid field

				if ( (status & SEND_TO_OUT) > 0 )
				{
					ptr = SM.enqueuePin( na._arc_id, 1 );//_train_sizes[na._seq_id] );
					//cout << " Called enqueuePin on " << na._arc_id << endl;
				}
				// why did I even have train size here? It's IN the train loop.
				else
					// the sizeof(int) comes from stream id (hard coded now!)
					{

					ptr = ( char * ) malloc( _train_size * (tuple_descr->getSize() +
						TUPLE_DATA_OFFSET + sizeof(int)));
					}

				assert(ptr != NULL);

				// get the current time stamp
				//(void) time (&t);

				// this is where we would read input from file!.
				// else do it the usual, oldfashioned way :)
				// New! More precision, seconds and microseconds
				if ( (status & READ_FROM_FILE) > 0 )
				{
					temp = fread( t, TUPLE_STREAMID_OFFSET, 1, fd );
					if ( temp <= 0 )
					{
												  //_arc_id );
						printf( "ST: Read failed miserably %d\n", -1 );
						exit( -1 );
					}
					//printf(" ST: READ: %d\n", t->tv_sec );
				}
				else
				if ( (status & SEND_TO_OUT) > 0 )
					if (gettimeofday(t, NULL) != 0)
				{
					perror("StreamThread: gettimeofday failed, setting timestamp for new tuple");
					exit(-1);
				}
				// do a little adjustment. accumulator array carries the
				// diffs that are too small to sleep on. Timestamp is adjusted
				// to make sure that timestamps compensate for non-slept time
				//else  (didn't work )
				//t->tv_usec += _accumulator[ na._arc_id ];

				//printf("YYY:StreamThreadArc%d: t1: %i  t2: %i\n", na._arc_id, t->tv_sec,t->tv_usec);

				if ( (status & WRITE_TO_FILE) > 0 )
				{
					if ( fwrite( t, TUPLE_STREAMID_OFFSET, 1, fd ) <= 0 )
						//printf(" SMI: WROTE: %ld\n", t->tv_sec );
						//else
					{
						printf("SMI: I/O write failure\n");
						exit( 1 );
					}
				}
				// and now place it in the tuple. NOTE the hard coded
				// assumption that the tuple has enough space in its timestamp
				// field for whatever space this variable 't' takes up
				memcpy(ptr, t, sizeof( timeval ));

				// Now, the streamid. For now, totally hard coded to be "1"
				int stream_id = 1;
				*((int*)(ptr+TUPLE_STREAMID_OFFSET)) = stream_id;

				//printf("%dSMI: Generating (or reading) a tuple [{ts, streamid}, %d fields] : {%d, %d} \n", na._arc_id, num_fields, t, stream_id);
				// To allow printing it via tupleGenerator
				string tupleFormat = "ti";		  // Start with the timestamps and streamids

				// random delay right now
				//sleep(rand_r(&seed)%8);
				char MITRE_HACK_TYPE = 'S'; // PART OF THE MITRE HACK STUFF
				char MITRE_HACK_LASTTANK_OR_SOLDIER = 'S'; // PART OF THE MITRE HACK STUFF
				
				for (int f=0; f<num_fields; f++)
				{
					field_offset = TUPLE_DATA_OFFSET + tuple_descr->getFieldOffset(f);
					// here we read the file... and go to next field.
					if ( (status & READ_FROM_FILE) > 0 )
					{
						fread( ptr+field_offset, tuple_descr->getFieldSize( f ), 1, fd );
						if ( temp <= 0 )
						{
							printf("ST: Read failed miserably %d\n",na._arc_id);
							exit( -1 );
						}
						//printf( "SMI: READ from file %d\n", *(ptr+field_offset ) );
						continue;				  // skip to end of filed generation
					}
												  // generate random tuple from 1 to 9
					r = rand_r(&seed) % 100 + 1 + 0.01;
					switch(tuple_descr->getFieldType(f))
					{
						case INT_TYPE:



						  /**
						     MITRE DEMOS JUNE 25 2003 HACK
						     na._arc_id 0 is (prn (int 0-100), latitude (int 0-100), longitude (int 0-100), timestamp (timeval)
						     na._arc_id 5 is (prn, type ("SPEED_, HULL__, TURRET, FUEL__, AMMO__, ENGINE, BLOOD_, HEART_"), value (float varies for each), timestamp (timeval))

						  */
						  if (na._arc_id == 0) {
						    if (f == 0)
						      *((int*)(ptr+field_offset)) = 1+rand_r(&seed)%100; // PRN
						    else if (f == 1)
						      *((int*)(ptr+field_offset)) = 1+rand_r(&seed)%100; // LATITUDE
						    else if (f == 2)
						      *((int*)(ptr+field_offset)) = 1+rand_r(&seed)%100; // LONGITUDE
						    else
						      *((int*)(ptr+field_offset)) = (int)r; // default
						  } else if (na._arc_id == 20) {
						    if (f == 0) {
						      int x =  1+rand_r(&seed)%200; // PRN (biased for < 50 [a.k.a. tanks])
						      if (x > 100) x = 1+rand_r(&seed)%50;
						      *((int*)(ptr+field_offset)) = x;
						      // set whether we generated a soldier (50 to 100) or a tank (< 50)
						      if (x < 50) MITRE_HACK_LASTTANK_OR_SOLDIER = 'T';
						      else MITRE_HACK_LASTTANK_OR_SOLDIER = 'S';
						    } else
						      *((int*)(ptr+field_offset)) = (int)r; // default
						  } else {
						    cout << "[StreamThread] MITRE JUNE Hack enabled, yet unexpected arc ids! Quitting now." << endl;
						    exit(1);
						  }
						  // Uncomment me if removing mitre hack
						  // *((int*)(ptr+field_offset)) = (int)r;
						  tupleFormat += "i";
						  //printf(", %d",  (int)r);
						  break;
						case FLOAT_TYPE:
						  if (na._arc_id == 20) {
						    // Value for the 'type'
						    float turrval;
						    switch(MITRE_HACK_TYPE) {
						    case 'S':
						      *((float*)(ptr+field_offset)) = (float) (1.0+rand_r(&seed)%60);// VALUE FOR SPEED (0.0 to 60.0)
						      break;
						    case 'H':						      
						      *((float*)(ptr+field_offset)) = (float) (1.0+rand_r(&seed)%360);// VALUE FOR HULL (0.0 to 360.0)
						      break;
						    case 'T':
						      turrval = (float) (1.0+rand_r(&seed)%360);// VALUE FOR TURRET (0.0 to 360.0)
						      *((float*)(ptr+field_offset)) = turrval;
						      //if (turrval > 90) {*((float*)(ptr+field_offset)) = (1.0+rand_r(&seed)%20); }
						      //else { *((float*)(ptr+field_offset)) = (turrval > 90.0) ? turrval - 90.0 : turrval; }
						      break;
						    case 'F':
						      *((float*)(ptr+field_offset)) = (float) (1.0+rand_r(&seed)%100);// VALUE FOR FUEL (0.0 to 100.0)
						      break;
						    case 'A':
						      *((float*)(ptr+field_offset)) = (float) (1.0+rand_r(&seed)%100);// VALUE FOR AMMO (0.0 to 100.0)
						      break;
						    case 'E':
						      *((float*)(ptr+field_offset)) = (float) (1.0-rand_r(&seed)%2);// VALUE FOR ENGINE (0.0 or 1.0)
						      break;
						    case 'B':
						      *((float*)(ptr+field_offset)) = (float) (1.0+rand_r(&seed)%150);// VALUE FOR BLOOD PRESSURE (0.0 to 150.0)
						      break;
						    case 'R':
						      *((float*)(ptr+field_offset)) = (float) (1.0+rand_r(&seed)%120);// VALUE FOR HEART RATE (0.0 to 120.0)
						      break;
						    default:
						      cout << "[StreamThread] MITRE JUNE Hack enabled, yet unexpected MITRE_HACK_TYPE for float value. Quitting now." << endl;
						      exit(1);
						      break;
						    }
						  }
						  else
							*((float*)(ptr+field_offset)) = (float)r;
							tupleFormat += "f";
							//printf(", %f",  (float)r);
							break;
						case DOUBLE_TYPE:
							*((double*)(ptr+field_offset)) = (double)r;
							tupleFormat += "f";
							//printf(", %f",  (double)r);
							break;
						case STRING_TYPE:
						{
						  if (na._arc_id == 20 && f == 1) {
						    char* MITRE_HACK_CHOSEN_TYPE = "______";

						    if (MITRE_HACK_LASTTANK_OR_SOLDIER == 'S') { // SOLDIER CODE
						      // Pick a type (remember to set MITRE_HACK_TYPE to the appropriate char for the float value to match)
						      char* MITRE_HACK_TYPES[] = { "SPEED_", "AMMO__", "BLOOD_", "HEART_" };
						      unsigned int r2d2 = 1+rand_r(&seed)%3;
						      MITRE_HACK_CHOSEN_TYPE = MITRE_HACK_TYPES[r2d2]; // any other, even turret!
						      {
							MITRE_HACK_TYPE = MITRE_HACK_CHOSEN_TYPE[0];
							if (MITRE_HACK_TYPE == 'H') { // differentiate between HULL__ and HEART_
							  if (MITRE_HACK_CHOSEN_TYPE[1] == 'U') MITRE_HACK_TYPE = 'H';
							  else MITRE_HACK_TYPE = 'R';
							}
						      }
						    } else { // TANK CODE
						      // Pick a type (remember to set MITRE_HACK_TYPE to the appropriate char for the float value to match)
						      char* MITRE_HACK_TYPES[] = { "SPEED_", "HULL__", "TURRET", "FUEL__", "AMMO__", "ENGINE" };
						      // choose one, and for each char, set it [bias for TURRET]
						      unsigned int r2d2 = 1+rand_r(&seed)%10;
						      if (r2d2 > 5) MITRE_HACK_CHOSEN_TYPE = MITRE_HACK_TYPES[2]; // turret
						      else MITRE_HACK_CHOSEN_TYPE = MITRE_HACK_TYPES[r2d2]; // any other, even turret!
						      {
							MITRE_HACK_TYPE = MITRE_HACK_CHOSEN_TYPE[0];
							if (MITRE_HACK_TYPE == 'H') { // differentiate between HULL__ and HEART_
							  if (MITRE_HACK_CHOSEN_TYPE[1] == 'U') MITRE_HACK_TYPE = 'H';
							  else MITRE_HACK_TYPE = 'R';
							}
						      }
						    }
						    // Generate the string
						    unsigned int char_offset_r2d2 = 0;
						    for (int si = 0; si < 6; si++)
						      {
							char c = (char) MITRE_HACK_CHOSEN_TYPE[si];
							*(ptr + field_offset + char_offset_r2d2) = c;
							char_offset_r2d2++;
							tupleFormat += "c";
						      }
						    
						    break;
						  }
						  // NORMAL CODE
						  int chars = tuple_descr->getFieldSize(f);
						  int char_offset = 0;
						  //printf(", ");
						  for (int si = 0; si < chars; si++)
						    {
						      // This generates a char between a and d (d = ascii code 97)
						      char c = (char) (rand() % 4) + 97;
						      *(ptr + field_offset + char_offset) = c;
						      //printf("%c",c);
						      char_offset++;
						      tupleFormat += "c";
						    }
						}
						break;
						case TIMESTAMP_TYPE:	  // a timeval
							timeval *ts = new timeval;
							gettimeofday(ts,NULL);
							memcpy(ptr+field_offset, t, sizeof( timeval ));
							tupleFormat += "t";
							break;
					}
					if ( (status & WRITE_TO_FILE) > 0 )
					{
						fwrite( ptr+field_offset, tuple_descr->getFieldSize( f ), 1, fd );
						//printf( "SMI: WROTE to file %d\n", *(ptr+field_offset ) );
					}
				}
				//printTuple(ptr,tupleFormat.c_str());
				incrementTuplesGenerated(1);
				na._num_messages_remain--;

				total_tuple_count++;
				//if ( (total_tuple_count % _sched_wake_interval) == 0 )
				//{
				//}

				if ( (total_tuple_count % 1000) == 0 )
				{
					static double last_tuples_per_sec = 0;
					int step = 1000;
					if ( last_tuples_per_sec > 10000 )
						step = 100000;
					else if ( last_tuples_per_sec > 1000 )
						step = 10000;
					if ( (total_tuple_count % step) == 0 )
					{
						timeval chunk_stop;
						gettimeofday(&chunk_stop,NULL);
						double total_time = (chunk_stop.tv_sec + (chunk_stop.tv_usec*1e-6)) -
							(chunk_start.tv_sec + (chunk_start.tv_usec*1e-6));
						last_tuples_per_sec = total_tuple_count/total_time;
						fprintf(stderr,"generating %f tuples per second : %f seconds per tuple\n",
							total_tuple_count/total_time,total_time/total_tuple_count);
					}
				}

				if ( (status & SEND_TO_OUT) > 0 )
				{
					//printf(" StreamThread-------------------------------------->ENQUEUE, ST, into queue %d ( III%d )\n", na._arc_id, -1 );
					SM.enqueueUnpin(na._arc_id, ptr, 1);
					//printf(" StreamThread-------------------------------------->ENQUEUE2, ST, into queue %d ( III%d )\n", na._arc_id, -1 );
					// report one message generated
				}
				//printf("StreamThread: queue monitor is about to %f\n", _queue_monitor);
				if (_queue_monitor != NULL)
				{
					_queue_monitor->tickMessage();
				}
				SM.SM_free(ptr);
				if ( total_tuple_count % _sched_wake_interval == 0 )
				{
					//printf("SIGNALING the scheduler tuples sent: %i\n",total_tuple_count);
					pthread_mutex_lock(_sched_wake_mutex);
					pthread_cond_broadcast(_sched_wake_cond);
					pthread_mutex_unlock(_sched_wake_mutex);
				}

				// update the tuples count (for Stats Object)
				{
					//LockHolder lh(_mtxInputPortInfoMap);
					++ (( _inputPortInfoMap.find( _queueIdToInputPortMap[ na._arc_id ] ) )->second)._tuplesEnqSinceLastCheck;
					//++ (pos->second._tuplesEnqSinceLastCheck);
				}

			}

			// this is the sleeping part...
			/* 
						  double rm;
						 if ( i % 5 == 0 )
						{
							rm = getRateMultiplier();
							_interarrival = (_interarrival/rm);//*_train_size;
							}
			*/
			//printf(" StreamThread: about to compute sleeping time\n");
			//printf(" WTF3? ARC=%d, _interarr %f, Train=%d inter_time sleep %f\n", na._arc_id , na._interarrival, _train_sizes[ na._seq_id ], na._next_t - last_time );

			inter_time = na._next_t - last_time;

			struct timeval timeout;
			//timeout.tv_sec = (int)_interarrival;
			timeout.tv_sec = (int)inter_time;
			//timeout.tv_usec = (int)((_interarrival - (int)_interarrival) * 1000000);
			timeout.tv_usec = (int)((inter_time - (int)inter_time) * 1000000);

			// This appears to be a Richard / Eddie specific setting for inter-arrival time,
			// so I'm commenting it out for the general populance. -cjc
			 timeout.tv_sec = 0;
			 timeout.tv_usec = 200000;

 			// Old junk, forget stream thread rate control...
			//if (_queue_monitor != NULL) timeout.tv_usec = _queue_monitor->getusecRate();
			//

			//printf("arc*in %d _inter arrival: %f\n", na._arc_id, inter_time );
			// move the clock to the next arrival time.

			last_time = na._next_t;
			na._next_t += ( na._interarrival * _train_sizes[ na._seq_id ] );
			allInputs.insert( na );

			//int precision_msec = 50000;
			int precision_msec = 100000;

			//if ( inter_time > 0.0 )
			//{
			//cerr << "StreamThread: go to sleep..." << inter_time << " tim " << timeout.tv_usec << endl;
			if ( (1000000 * timeout.tv_sec + timeout.tv_usec)+_accumulator[ na._arc_id ] > precision_msec )
			{  // sleep soundly and safely. and reduce the accumulated time
				// we assume that precision is A LOT less than a second
				// something like .5 second might break this code. 
				timeout.tv_usec += _accumulator[ na._arc_id ];
				select(0,0,0,0,&timeout);
				//for ( int z = 0; z < _input_count; z++ )
				//	_accumulator[ z ] -= _accumulator[ na._arc_id ];
				_accumulator[ na._arc_id ] = 0;
			}
			else
				//for ( int z = 0; z < _input_count; z++ )
				_accumulator[ na._arc_id ] += timeout.tv_usec;
		}
		//_measure->incrementTimeSpentInStreamThread(get_etime(&first)-secs);
	}
	_measure->setTimeSpentInStreamThread(get_etime(&first)-secs);

	if ( (status & ( WRITE_TO_FILE )) > 0 ) fclose( fd );
	return (void *)0;
}


void *StreamThread::entryPoint(void *pthis)
{
	StreamThread *pt = (StreamThread*)pthis;
	pt->run();
	return (void *)0;
}


void StreamThread::start()
{
	pthread_create(&_thread, 0, entryPoint, this);
}


void StreamThread::join()
{
	pthread_join(_thread, NULL);
}


void StreamThread::init_etime(struct itimerval *first)
{
	first->it_value.tv_sec = 1000000;
	first->it_value.tv_usec = 0;
	setitimer(ITIMER_VIRTUAL,first,NULL);
}


double StreamThread::get_etime(struct itimerval *first)
{
	struct itimerval curr;
	getitimer(ITIMER_VIRTUAL,&curr);
	return (double)(
		(first->it_value.tv_sec + (first->it_value.tv_usec*1e-6)) -
		(curr.it_value.tv_sec + (curr.it_value.tv_usec*1e-6)));

	//return (double)(
	//(first->it_value.tv_sec - curr.it_value.tv_sec) +
	//(first->it_value.tv_usec - curr.it_value.tv_usec)*1e-6);
}
