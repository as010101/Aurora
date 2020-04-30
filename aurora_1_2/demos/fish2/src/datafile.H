#ifndef DATAFILE_H
#define DATAFILE_H

#include <sys/time.h>
#include <time.h>
#include <string>

using namespace std;

struct SingleFishReading
{
	float _ventilationsPerMinute;
	float _volts;
	float _coughsPerMinute;
	float _percentBodyMovement;
};

struct DataFileParagraph
{
	timeval _sampleTime;
	int     _printInterval;
	
	// These are the most recent water readings, and are taken from line #2 or #3
	// of each paragraph, depending on PI#.  This is just to counterace an 
	// artifact of the software that produces the data files...
	float _temperatureCelcius;
	float _pH;
	float _conductivity_mS_per_cm;
	float _disolvedO2_mg_per_l;
	
	SingleFishReading _singleFishInfo[8];

	int _numOocVentilatoryRates;
	int _numOocVoltages;
	int _numOocCoughRates;
	int _numOocPercentBodyMovements;
	int _numDeadFish;

	string toString();
};

#endif
