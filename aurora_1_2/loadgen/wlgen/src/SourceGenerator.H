#ifndef _STREAM_SOURCE_GENERATOR_H_
#define _STREAM_SOURCE_GENERATOR_H_

// Local headers
#include "SourcePool.H"			// for SourcePool, sources_t
#include "AggStreamSource.H"		// for AggStreamSource
#include "Types.H"			// for {source_stream_id_t, parlist_t
					//      distribution_t}
#include "StreamIDGenerator.H"          // for StreamIDGenerator
#include "RandomNumGenerator.H"		// for RandomNumGenerator

using namespace std;
namespace workload_generator{

typedef vector<RandomNumGenerator> rand_num_generator_vect_t;

class SourceGenerator
{
public:
   SourceGenerator(){}
   ~SourceGenerator(){}

   void	initialize(SourcePool* source_pool); 

   bool	newSources(AggStreamSource& agg_source);

   bool changeSourceRate(AggStreamSource& agg_source);

private: 
   SourcePool*	_source_pool;

   bool newSingleSource(
         AttributeDef&			attrdef, 
         vector<RandomNumGenerator>&	randnum_generators,
         single_sources_t&		single_sources);
};

}
#endif 
