#ifndef _STREAM_TUPLE_H_
#define _STREAM_TUPLE_H_


// Local headers
#include "Types.H"			// for agg_stream_id 
#include "Timeval.H"                    //for Timeval

using namespace std;
using namespace timeval_tool;

namespace workload_generator{

class StreamTuple;

typedef list<StreamTuple*> tuplelist_t;
typedef list<StreamTuple*>::iterator tuplelist_iter_t;


class StreamTuple
{
public:
   StreamTuple(
         AggStreamID		agg_stream_id,
         const Timeval&		timestamp, 
         int			data_size);
   ~StreamTuple(); 

   AggStreamID&		getAggStreamID();
   Timeval&		getTimestamp();	
   int			getDataSize() const;
   char*		getDataHandle() const;
   int			getData(char* data, int len);

   bool operator<(StreamTuple &t) {
      return _timestamp < t.getTimestamp();
   }

private:
   AggStreamID		_agg_stream_id;
   Timeval              _timestamp;
   char                 *_data;
   int                  _data_len;
};


}
#endif
