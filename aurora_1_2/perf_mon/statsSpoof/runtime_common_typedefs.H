#ifndef RUNTIME_COMMON_TYPEDEFS_H
#define RUNTIME_COMMON_TYPEDEFS_H

/*
  This file is meant to hold all typedefs / enums / etc. that pertain to more 
  than just one subsystem, and are too trivial to each get their own file.
 */

#include<vector>
#include<algorithm>

using namespace std;

// Describes a utility curve used by the loadshedder. The vector index gives
// the x-position of the curve point, and the vector value gives the curve 
// height for that x-position. 
//
// These curves are normalized: Every element is expected to be in the 
// domain [0, 1].
typedef vector<double> LoadShedderCurve;


// First value = boxid, second value = port number (0, 1, ...)
typedef pair<int, int> BoxOutputPortId;

#endif
