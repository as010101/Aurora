#ifndef RANDOM_POINT_H
#define RANDOM_POINT_H

/*
  Author: Alexander Rasin
  Date: Nov 3, 2002
*/

#include <string>

#define NORMAL_DIST        0
#define FIXED_DIST         1

using namespace std;

class RandomPoint
{
public:
  RandomPoint( float mean, float dev, int dist );
  RandomPoint::RandomPoint(const char *str );
  ~RandomPoint() {}

  float nextPoint();
  double getValue();
    

  string toString();
  
private:

  double gaussrand();
  float _mean;
  float _deviation;
  int _distribution;
  
  double point;
};

#endif
