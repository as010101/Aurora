#ifndef O0TUPLE_H
#define O0TUPLE_H


#include <time.h>
#include <sys/time.h>
#include <string>
#include <sstream>
#include <util.H>
using namespace std;
struct O0Tuple { 

  timeval  _sys_timestamp  __attribute__ ((packed));
  int  _sys_streamId  __attribute__ ((packed));

  int  _ooc_printerInterval  __attribute__ ((packed));
  float  _ooc_temperatureDelta1hour  __attribute__ ((packed));
  float  _ooc_temperatureDelta2hour  __attribute__ ((packed));
  float  _ooc_temperatureDelta4hour  __attribute__ ((packed));
  float  _ooc_phDelta1hour  __attribute__ ((packed));
  float  _ooc_phDelta2hour  __attribute__ ((packed));
  float  _ooc_phDelta4hour  __attribute__ ((packed));
  float  _ooc_condDelta1hour  __attribute__ ((packed));
  float  _ooc_condDelta2hour  __attribute__ ((packed));
  float  _ooc_condDelta4hour  __attribute__ ((packed));
  float  _ooc_o2Delta1hour  __attribute__ ((packed));
  float  _ooc_o2Delta2hour  __attribute__ ((packed));
  float  _ooc_o2Delta4hour  __attribute__ ((packed));
  timeval  _ooc_timeOfSample  __attribute__ ((packed));
  float  _ooc_temperatureCelcius  __attribute__ ((packed));
  float  _ooc_pH  __attribute__ ((packed));
  float  _ooc_conductivity_mS_per_cm  __attribute__ ((packed));
  float  _ooc_disolvedO2_mg_per_l  __attribute__ ((packed));
  int  _ooc_last_good_printerInterval  __attribute__ ((packed));
  int  _ooc_num_ooc_vent_rates  __attribute__ ((packed));
  int  _ooc_num_ooc_voltages  __attribute__ ((packed));
  int  _ooc_num_ooc_cough_rates  __attribute__ ((packed));
  int  _ooc_num_ooc_percent_body_movements  __attribute__ ((packed));
  int  _ooc_num_dead  __attribute__ ((packed));
  int  _good_printerInterval  __attribute__ ((packed));
  float  _good_temperatureDelta1hour  __attribute__ ((packed));
  float  _good_temperatureDelta2hour  __attribute__ ((packed));
  float  _good_temperatureDelta4hour  __attribute__ ((packed));
  float  _good_phDelta1hour  __attribute__ ((packed));
  float  _good_phDelta2hour  __attribute__ ((packed));
  float  _good_phDelta4hour  __attribute__ ((packed));
  float  _good_condDelta1hour  __attribute__ ((packed));
  float  _good_condDelta2hour  __attribute__ ((packed));
  float  _good_condDelta4hour  __attribute__ ((packed));
  float  _good_o2Delta1hour  __attribute__ ((packed));
  float  _good_o2Delta2hour  __attribute__ ((packed));
  float  _good_o2Delta4hour  __attribute__ ((packed));
  timeval  _good_timeOfSample  __attribute__ ((packed));
  float  _good_temperatureCelcius  __attribute__ ((packed));
  float  _good_pH  __attribute__ ((packed));
  float  _good_conductivity_mS_per_cm  __attribute__ ((packed));
  float  _good_disolvedO2_mg_per_l  __attribute__ ((packed));
  int  _good_num_ooc_vent_rates  __attribute__ ((packed));
  int  _good_num_ooc_voltages  __attribute__ ((packed));
  int  _good_num_ooc_cough_rates  __attribute__ ((packed));
  int  _good_num_ooc_percent_body_movements  __attribute__ ((packed));
  int  _good_num_dead  __attribute__ ((packed));


  string toString() const
{    char* pBuffer = new char[0+1];
     ostringstream os;
     os << "O0Tuple (this = " << this << ")" << endl
        << "   _sys_timestamp = " << timevalToSimpleString(_sys_timestamp) << endl
        << "   _sys_streamId = " << _sys_streamId << endl;
     os << "  _ooc_printerInterval = " << _ooc_printerInterval << endl;
     os << "  _ooc_temperatureDelta1hour = " << _ooc_temperatureDelta1hour << endl;
     os << "  _ooc_temperatureDelta2hour = " << _ooc_temperatureDelta2hour << endl;
     os << "  _ooc_temperatureDelta4hour = " << _ooc_temperatureDelta4hour << endl;
     os << "  _ooc_phDelta1hour = " << _ooc_phDelta1hour << endl;
     os << "  _ooc_phDelta2hour = " << _ooc_phDelta2hour << endl;
     os << "  _ooc_phDelta4hour = " << _ooc_phDelta4hour << endl;
     os << "  _ooc_condDelta1hour = " << _ooc_condDelta1hour << endl;
     os << "  _ooc_condDelta2hour = " << _ooc_condDelta2hour << endl;
     os << "  _ooc_condDelta4hour = " << _ooc_condDelta4hour << endl;
     os << "  _ooc_o2Delta1hour = " << _ooc_o2Delta1hour << endl;
     os << "  _ooc_o2Delta2hour = " << _ooc_o2Delta2hour << endl;
     os << "  _ooc_o2Delta4hour = " << _ooc_o2Delta4hour << endl;
     os << "  _ooc_timeOfSample = " << timevalToSimpleString(_ooc_timeOfSample) << endl;
     os << "  _ooc_temperatureCelcius = " << _ooc_temperatureCelcius << endl;
     os << "  _ooc_pH = " << _ooc_pH << endl;
     os << "  _ooc_conductivity_mS_per_cm = " << _ooc_conductivity_mS_per_cm << endl;
     os << "  _ooc_disolvedO2_mg_per_l = " << _ooc_disolvedO2_mg_per_l << endl;
     os << "  _ooc_last_good_printerInterval = " << _ooc_last_good_printerInterval << endl;
     os << "  _ooc_num_ooc_vent_rates = " << _ooc_num_ooc_vent_rates << endl;
     os << "  _ooc_num_ooc_voltages = " << _ooc_num_ooc_voltages << endl;
     os << "  _ooc_num_ooc_cough_rates = " << _ooc_num_ooc_cough_rates << endl;
     os << "  _ooc_num_ooc_percent_body_movements = " << _ooc_num_ooc_percent_body_movements << endl;
     os << "  _ooc_num_dead = " << _ooc_num_dead << endl;
     os << "  _good_printerInterval = " << _good_printerInterval << endl;
     os << "  _good_temperatureDelta1hour = " << _good_temperatureDelta1hour << endl;
     os << "  _good_temperatureDelta2hour = " << _good_temperatureDelta2hour << endl;
     os << "  _good_temperatureDelta4hour = " << _good_temperatureDelta4hour << endl;
     os << "  _good_phDelta1hour = " << _good_phDelta1hour << endl;
     os << "  _good_phDelta2hour = " << _good_phDelta2hour << endl;
     os << "  _good_phDelta4hour = " << _good_phDelta4hour << endl;
     os << "  _good_condDelta1hour = " << _good_condDelta1hour << endl;
     os << "  _good_condDelta2hour = " << _good_condDelta2hour << endl;
     os << "  _good_condDelta4hour = " << _good_condDelta4hour << endl;
     os << "  _good_o2Delta1hour = " << _good_o2Delta1hour << endl;
     os << "  _good_o2Delta2hour = " << _good_o2Delta2hour << endl;
     os << "  _good_o2Delta4hour = " << _good_o2Delta4hour << endl;
     os << "  _good_timeOfSample = " << timevalToSimpleString(_good_timeOfSample) << endl;
     os << "  _good_temperatureCelcius = " << _good_temperatureCelcius << endl;
     os << "  _good_pH = " << _good_pH << endl;
     os << "  _good_conductivity_mS_per_cm = " << _good_conductivity_mS_per_cm << endl;
     os << "  _good_disolvedO2_mg_per_l = " << _good_disolvedO2_mg_per_l << endl;
     os << "  _good_num_ooc_vent_rates = " << _good_num_ooc_vent_rates << endl;
     os << "  _good_num_ooc_voltages = " << _good_num_ooc_voltages << endl;
     os << "  _good_num_ooc_cough_rates = " << _good_num_ooc_cough_rates << endl;
     os << "  _good_num_ooc_percent_body_movements = " << _good_num_ooc_percent_body_movements << endl;
     os << "  _good_num_dead = " << _good_num_dead;
     delete[] pBuffer;
     return os.str();
  }
 
};
#endif
