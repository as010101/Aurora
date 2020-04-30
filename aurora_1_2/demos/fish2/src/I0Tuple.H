#ifndef I0TUPLE_H
#define I0TUPLE_H


#include <time.h>
#include <sys/time.h>
#include <string>
#include <sstream>
#include <util.H>
using namespace std;
struct I0Tuple { 

  timeval  _sys_timestamp  __attribute__ ((packed));
  int  _sys_streamId  __attribute__ ((packed));

  int  _printerInterval  __attribute__ ((packed));
  timeval  _timeOfSample  __attribute__ ((packed));
  float  _temperatureCelcius  __attribute__ ((packed));
  float  _pH  __attribute__ ((packed));
  float  _conductivity_mS_per_cm  __attribute__ ((packed));
  float  _disolvedO2_mg_per_l  __attribute__ ((packed));
  char  _is_ooc[1]  __attribute__ ((packed));
  int  _last_good_printerInterval  __attribute__ ((packed));
  int  _num_ooc_vent_rates  __attribute__ ((packed));
  int  _num_ooc_voltages  __attribute__ ((packed));
  int  _num_ooc_cough_rates  __attribute__ ((packed));
  int  _num_ooc_percent_body_movements  __attribute__ ((packed));
  int  _num_dead  __attribute__ ((packed));


  string toString() const
{    char* pBuffer = new char[1+1];
     ostringstream os;
     os << "I0Tuple (this = " << this << ")" << endl
        << "   _sys_timestamp = " << timevalToSimpleString(_sys_timestamp) << endl
        << "   _sys_streamId = " << _sys_streamId << endl;
     os << "  _printerInterval = " << _printerInterval << endl;
     os << "  _timeOfSample = " << timevalToSimpleString(_timeOfSample) << endl;
     os << "  _temperatureCelcius = " << _temperatureCelcius << endl;
     os << "  _pH = " << _pH << endl;
     os << "  _conductivity_mS_per_cm = " << _conductivity_mS_per_cm << endl;
     os << "  _disolvedO2_mg_per_l = " << _disolvedO2_mg_per_l << endl;
     memcpy(pBuffer, _is_ooc, 1);
     pBuffer[1] = '\0'; 
     os << "   _is_ooc = \"" << pBuffer << "\""  << endl;
     os << "  _last_good_printerInterval = " << _last_good_printerInterval << endl;
     os << "  _num_ooc_vent_rates = " << _num_ooc_vent_rates << endl;
     os << "  _num_ooc_voltages = " << _num_ooc_voltages << endl;
     os << "  _num_ooc_cough_rates = " << _num_ooc_cough_rates << endl;
     os << "  _num_ooc_percent_body_movements = " << _num_ooc_percent_body_movements << endl;
     os << "  _num_dead = " << _num_dead;
     delete[] pBuffer;
     return os.str();
  }
 
};
#endif
