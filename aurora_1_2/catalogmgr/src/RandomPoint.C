#include "RandomPoint.H"
//#include "stdio.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <exception>
#include <vector>
#include <parseutil.H>
#include <sstream>

/* This function has been borrowed from
http://remus.rutgers.edu/~rhoads/Code/normal.c
Generates a random number in normal distribution
*/

double RandomPoint::gaussrand()
{
  static double V2, fac;
  static int phase = 0;
  double S, Z, U1, U2, V1;
  
  if (phase)
    Z = V2 * fac;
   else
     {
       do {
         U1 = (double)rand() / RAND_MAX;
         U2 = (double)rand() / RAND_MAX;
	 
         V1 = 2 * U1 - 1;
         V2 = 2 * U2 - 1;
         S = V1 * V1 + V2 * V2;
       } while(S >= 1);
       
       fac = sqrt (-2 * log(S) / S);
       Z = V1 * fac;
     }
  
  phase = 1 - phase;
  
  return Z;
}


float RandomPoint::nextPoint()
{
  if ( _distribution == FIXED_DIST )
    return ( point = _mean );
  else if ( _distribution == NORMAL_DIST )
    return ( point = ( _mean + _deviation * gaussrand() ));
  
  exit( 1 );
  // this should not happen at all.
  return -1;
}

RandomPoint::RandomPoint(const char *str )
{
  vector<string> elements = unpackString(str, ",");
  assert(elements.size() <= 3);

  try
    {
      switch (elements.size())
	{
	case 0:
	  _mean         = 0;
	  _deviation    = 0;
	  _distribution = FIXED_DIST;
	  break;

	case 1:
	  _mean         = stringToDouble(elements.at(0));
	  _deviation    = 0;
	  _distribution = FIXED_DIST;
	  break;

	case 2:
	  _mean         = stringToDouble(elements.at(0));
	  _deviation    = stringToDouble(elements.at(1));
	  _distribution = FIXED_DIST;
	  break;

	case 3:
	  _mean         = stringToDouble(elements.at(0));
	  _deviation    = stringToDouble(elements.at(1));

	  assert(! elements.at(2).empty());
	  _distribution = 
	    (elements.at(2).at(0) == 'N') ? NORMAL_DIST : FIXED_DIST;
	  break;
	}
    }
  catch (const exception & e)
    {
      cerr << e.what() << endl;
      assert(false);
    }

  //  cout << " CREATED PARA " << toString() << endl;
}

RandomPoint::RandomPoint( float mean, float dev, int dist )
{
  _mean = mean;
  _deviation = dev;
  _distribution = dist;

  //  cout << endl << " CREATED PARA " << toString() << endl;
}

double RandomPoint::getValue()
{
  return point;
}

string RandomPoint::toString()
{
  ostringstream s;
  s << "RandomPoint: " << point;
  s << "  [ mean : " << _mean;
  s << ", deviation : " << _deviation;
  s << ", distribution : " << _distribution;
  s << " ]";
  
  return s.str();
}
