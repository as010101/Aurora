#ifndef _ATTRIBUTE_SOURCE_H_
#define _ATTRIBUTE_SOURCE_H_


// Local headers
#include "DataType.H"			// for DataType
#include "Types.H"			// for parlist_t, distributiontype_t
#include "RandomNumGenerator.H"		// for RandomNumGenerator
#include "Timeval.H"			// for Timeval

using namespace std;
using namespace timeval_tool;

namespace workload_generator{

class AttributeSource
{
public:
   AttributeSource(){}
   AttributeSource(const distribution_t& source_distr, datatype_t type); 

   ~AttributeSource(){};

   void initialize(Timeval first_time);

   distributiontype_t	getSourceType() const ;
   int          getDataSize() const;
   datatype_t	getDataType() const;
   int  getNextData(const Timeval& newtime, char* data);

private:
   int          _data_size;
   datatype_t   _data_type;

   RandomNumGenerator	_randnumber_generator;

   Timeval	_last_time;	// history information
};


}
#endif
