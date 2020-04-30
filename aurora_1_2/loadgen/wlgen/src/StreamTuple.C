#include "StreamTuple.H"

using namespace std;
namespace workload_generator{

StreamTuple::StreamTuple(
         AggStreamID		agg_stream_id,
         const Timeval&		timestamp, 
         int			data_size):
      _agg_stream_id(agg_stream_id),
      _timestamp(timestamp),
      _data_len(data_size)
{
   assert(data_size > 0);
   _data = new char[_data_len];
}

StreamTuple::~StreamTuple()
{ 
   delete[] _data;
}

AggStreamID& StreamTuple::getAggStreamID() 	
{
   return _agg_stream_id; 
}

Timeval& StreamTuple::getTimestamp()
{ 
   return _timestamp; 
}

int StreamTuple::getDataSize() const	
{ 
   return _data_len; 
}

char* StreamTuple::getDataHandle() const
{ 
   return _data; 
}

int StreamTuple::getData(char* data, int len)
{
         if (len < _data_len) {
            return 0;
         }
         memcpy(data, _data, _data_len);
         return _data_len;
}

}
