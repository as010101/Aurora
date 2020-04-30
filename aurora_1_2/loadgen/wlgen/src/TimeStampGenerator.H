#ifndef _TIMESTAMPGENERATOR_H_
#define _TIMESTAMPGENERATOR_H_

// C++ headers
#include <ctime>			// for timeval, gettimeofday

// Local headers
#include "RandomNumGenerator.H"		// for RandomNumGenerator
#include "Timeval.H"			// for Timeval

using namespace std;
using namespace timeval_tool;

namespace workload_generator{

const double MIN_INTERVAL = 0.00001;      // minumum interval between ticks
class TimeStampGenerator
{
public:
   TimeStampGenerator(){}
   TimeStampGenerator(const distribution_t& random_number_generator_par);
   ~TimeStampGenerator();

   Timeval&	getTimestamp(); 
   void		setTimeStamp(const timeval &t); 
   void		tick();
   void		changeRatio(double ratio);
   void		changeIntervalGenerator(
         const distribution_t& random_number_generator_par);

protected:
   Timeval		_timestamp;
   RandomNumGenerator*	_interval_generator;
   double		_ratio;
};

}
#endif
