#ifndef _STREAM_BUFFER_H_
#define _STREAM_BUFFER_H_

// Linux headers
#include <pthread.h>			// for pthread_*

// Local headers
#include "StreamSender.H"		// for StreamSender
#include "StreamTuple.H"		// for tuplelist_t
#include "eventqueue.h"			// for EventQueue

using namespace std;
namespace workload_generator{

class StreamBuffer
{
public:
   StreamBuffer(){}
   ~StreamBuffer(){}

   void		initialize(StreamSender* sender); 
   void		addTuples(tuplelist_t*);
   void		printQueue();
private: 
   threadtools::EventQueue	_buffer;
   StreamSender*		_sender;
};


}
#endif
