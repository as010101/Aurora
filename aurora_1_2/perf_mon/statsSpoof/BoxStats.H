#ifndef BOXSTATS_H
#define BOXSTATS_H

struct BoxStats
{
	float _cost;

	// Note: This is the selectivity over the *whole* box. I.e., the ratio of 
	// the total number of coming out (regardless of which output port) to the 
	// number of tuples going in.
	float _selectivity;
};

#endif
