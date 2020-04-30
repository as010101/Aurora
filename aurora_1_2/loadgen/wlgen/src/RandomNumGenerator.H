#ifndef _RANDOMNUMGENERATOR_H_
#define _RANDOMNUMGENERATOR_H_

// C++ headers
#include <cmath>		// for drand48()
#include <fstream>		// for fstream

// Loacal headers
#include "Types.H"


// This class define an object that can generate random numbers 
// according to the distribution type and some parameters.
// The getRandNum() function will return a random number each time it is
// called.
//
// If the parameters are not valid, only zero will be returned by
// getRandNum().
//
// For CONSTANT distribution, there is one parameter, which is the value
// of the constant. 
//
// For UNIFORM distribution, there are two parameters. The first one is
// the lower bound, the second one is the upper bound. 
//
// For NORMAL distribution, there are two parameters. The first one is
// the mean value, the second one is the standard deviation. 
//
// For EXPONENTIAL distribution, there is one parameter, which is the
// mean value. 
//
// For LINEAR distribution, there are four parameter. They are 
// initial_value, step, lower_bound, upper_bound. Let count be the
// number of times that getRandNum() is called, the the random number 
// returned by getRandNum() is 
// min(max(lower_bound, initial_value + count * step), upper_bound).
//
// For RANDOMWALK distribution, there are five parameter. They are
// initial_value, step, prob, lower_bound, upper_bound. 
// Each time, the value generated is remembered. Each time getRandNum()
// is called, it return (last_value + step) with probability prob and 
// (last_value - step) with probbility (1-prob). If the value is outside
// of the boundary, it will change the sign of the incremented value.
//
// For RANDOMDIRECTIONWALK distribution, there are five parameter. They are
// initial_value, step, prob, lower_bound, upper_bound. 
// Each time getRandNum() is called, with probability prob, the sign of
// step will be changed and (last_value + step) is returned.
//

using namespace std;
namespace workload_generator{

class RandomNumGenerator
{
public:
   RandomNumGenerator(){} 
   RandomNumGenerator(const distribution_t& par);
   ~RandomNumGenerator(); 

   // This function returns a random number 
   double		getRandNum();
   double		getRandNum(double interval);
   distributiontype_t	type() const {return _type;}


   // This function can check whether the parameters of a given distribution
   // is valid
   static bool		checkDistribution(const distribution_t& distr);

   // This function returns whether the random numbergenerator is valid
   bool			valid() const { return _valid;}

private:
   distributiontype_t 	_type;
   parlist_t		_parameters;

   int i;

   bool			_valid;

   // for input file only
   fstream*		_input_file;
   bool			_first_time;
   bool			_open;


   double		nextRand(double interval);

   double		normal(double m, double s);  // m: mean, s: std
   double		exponential(double m);       // m: mean
   double		linear(double interval);
   double		randomWalk(double interval);
   double		randomDirectionWalk(double interval);
   double		inputFile();
};


}
#endif
