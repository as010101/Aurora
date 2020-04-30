#ifndef _SINGLE_SOURCE_H_
#define _SINGLE_SOURCE_H_

// C++ headers
#include <vector>			// for vector
  
// Local headers
#include "StreamTuple.H"		// for StreamTuple, tuplelist
#include "TimeStampGenerator.H"		// for TimeStampGenerator
#include "AttributeSource.H"		// for AttributeSource
#include "Timeval.H"			// for Timeval
  
using namespace std;
using namespace timeval_tool;

namespace workload_generator{

typedef vector<AttributeSource>		single_sources_t;
typedef vector<AttributeSource>::iterator	single_sources_it_t;

class SingleSource
{
public:
   SingleSource(){}
   SingleSource(
         source_stream_id_t		id, 
         agg_stream_id_t		add_stream_id,
         const distribution_t&		timstamp_generator_distribution,
         single_sources_t		single_sources,
         int				tuple_num);
   ~SingleSource();

   int			getID() const { return _stream_id; }
   agg_stream_id_t&	getAggStreamID() {return _agg_stream_id;}
   int			getTupleNumber() const {return _tuple_num; }
   int			getTupleDataSize() const { return _tuple_data_size;}

   StreamTuple*	getNextTuple();
   void			getTuplesUntil(const Timeval& tm, tuplelist_t &tuples); 
   void			getNTuples(const int tuple_num, tuplelist_t &tuples);  

   bool	active() const {return _generated_tuple_num != _tuple_num;}
   void changeRate(double ratio);
   void	changeTimestampGenerator(
         const distribution_t& timstamp_generator_distribution);

private:
   source_stream_id_t	_stream_id;  // this is for individual source
   agg_stream_id_t	_agg_stream_id; // the ID of the aggStream this 
                                        // individual stream belongs to
   TimeStampGenerator	_timestamp_generator;
   single_sources_t	_single_sources;
   int			_tuple_data_size;

   int 			_tuple_num;
   int			_generated_tuple_num;
};

}
#endif
