#ifndef _WG_CONTROLLER_H_
#define _WG_CONTROLLER_H_ 


// Unix headers
#include <pthread.h>			// for pthread*

// Local headers
#include "StreamTuple.H"		// for StreamTuple, tuplelist_t
#include "SourcePool.H"			// for SourcePool, sources_t
                                        //     sources_iter_t
#include "StreamBuffer.H"		// for StreamBuffer
#include "Timeval.H"			// for Timeval


using namespace std;
using namespace timeval_tool;

namespace workload_generator{

const long DEFAULT_CHECK_AHEAD_INTERVAL_SEC = 0;
const long DEFAULT_CHECK_AHEAD_INTERVAL_USEC = 10000;

class WGController
{
public:
   WGController();
   WGController(long check_ahead_sec, long check_ahead_usec);
   ~WGController(){}

   void	initialize( SourcePool* pool, StreamBuffer* stream_buffer);

   Timeval&	getLastCheckTime();
   void		setCheckInterval(const Timeval& check_interval);

private: 
   SourcePool*		_source_pool;
   StreamBuffer*	_stream_buffer; 

   pthread_t		_generator_thread;
   Timeval		_last_check_time;
   Timeval		_check_ahead_interval;
   tuplelist_t		_tmp_tuples;
   stream_ids_t		_inactive_sources;
   static void		generatorHandler(WGController* me); 

};

}
#endif
