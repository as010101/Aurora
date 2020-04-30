#ifndef _STREAMIDGENERATOR_H_
#define _STREAMIDGENERATOR_H_

// Local header
#include "Types.H"		// for source_stream_id_t

using namespace std;
namespace workload_generator{

class StreamIDGenerator
{
public:
   static source_stream_id_t getID(
         source_stream_id_t agg_stream_id, 
         int count)
   {
      source_stream_id_t id = agg_stream_id << 16;
      id += count;
      return id; 
   }
};

}
#endif
