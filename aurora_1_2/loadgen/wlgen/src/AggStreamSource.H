#ifndef _AGG_STREAM_SOURCE_H_
#define _AGG_STREAM_SOURCE_H_

// C++ headers
#include <vector>		// for vector
  
// Local headers
#include "Types.H"		// for {datatype_t, distributiontype_t, 
				//      distribution_t, agg_stream_id_t} 

using namespace std;

namespace workload_generator{

struct AttributeDef
{
   datatype_t			attribute_data_type; // data type 
   distributiontype_t		source_type;         // source type
   vector<distribution_t>	source_parameters;   // This is a list of 
                                  // distributions which decide the 
                                  // the parameters of the source distribution.
                                  // Since the the prameters may be
                                  // different for each of the sources,
                                  // thus they are specified by a
                                  // distribution instead of real valued
                                  // parameters.
};

class AggStreamSource
{
public:
   AggStreamSource();
   agg_stream_id_t		id;   // this is the aggreate stream id 
   int				num_streams; // number of individual streams
   int				num_tuples;  // number of tuples will 
                                             // be generated for each 
                                             // individual stream.  
                                             // -1 for infinit stream

   distributiontype_t		rate_type; 	// The distribution 
                                                // type for the 
                                                // interarrival time
   vector<distribution_t>	rate_parameters; 

   vector<AttributeDef>		attributes;


   // for debug
   void				print();

private:
   void	printDistribution(distribution_t& distr);
   void	printDistributionVector(vector<distribution_t>& distr_v);
   void	printAttibueDef(AttributeDef& attr_def);
   void	printDistributionType(distributiontype_t type, bool is_source);
   void	printDateType(attr_type_t type);	
};


}
#endif
