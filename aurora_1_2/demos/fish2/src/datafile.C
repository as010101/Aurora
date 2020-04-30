#include <datafile.H>
#include <util.H>
#include <sstream>

string DataFileParagraph::toString()
{
	ostringstream os;
	os << "DataFileParagraph" << endl
	   << "   _sampleTime             = " << timevalToSimpleString(_sampleTime) << endl
	   << "   _printInterval          = " << _printInterval << endl
	   << "   _temperatureCelcius     = " << _temperatureCelcius << endl
	   << "   _pH                     = " << _pH << endl
	   << "   _conductivity_mS_per_cm = " << _conductivity_mS_per_cm << endl
	   << "   _disolvedO2_mg_per_l    = " << _disolvedO2_mg_per_l << endl
	   << endl
	   << "   _singleFishInfo:" << endl;

	for (int i = 0; i < 8; ++i)
		{
			os << "      _singleFishInfo[" << i << "]" << endl
			   << "         _ventilationsPerMinute = " << _singleFishInfo[i]._ventilationsPerMinute << endl
			   << "         _volts                 = " << _singleFishInfo[i]._volts << endl
			   << "         _coughsPerMinute       = " << _singleFishInfo[i]._coughsPerMinute << endl
			   << "         _percentBodyMovement   = " << _singleFishInfo[i]._percentBodyMovement << endl;
		}

	os << "   _numOocVentilatoryRates     = " << _numOocVentilatoryRates << endl
	   << "   _numOocVoltages             = " << _numOocVoltages << endl
	   << "   _numOocCoughRates           = " << _numOocCoughRates << endl
	   << "   _numOocPercentBodyMovements = " << _numOocPercentBodyMovements << endl
	   << "   _numDeadFish                = " << _numDeadFish << endl;

	return os.str();
}
