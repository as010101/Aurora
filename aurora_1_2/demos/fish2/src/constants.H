/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: constants.H
 *	Asgn: fishgui
 *	Date: Wed Jun 18 10:20:38 EDT 2003
 **************************************************************/

#ifndef constants_Header
#define constants_Header

#include <time.h>
#include <sys/time.h>
// time to string in util.H

struct FishSlidingWindows
{
  //timeval_t _timestamp;
  std::string _timestamp;
  float _tempDelta1;
  float _tempDelta2;
  float _tempDelta4;
  float _phDelta1;
  float _phDelta2;
  float _phDelta4;
  float _condDelta1;
  float _condDelta2;
  float _condDelta4;
  float _o2Delta1;
  float _o2Delta2;
  float _o2Delta4;
};

struct AlertEvent
{
  FishSlidingWindows _alertWindows;
  FishSlidingWindows _nonAlertWindows;
  int _alertPrint;
  int _nonAlertPrint;
  int _alertNumOocVentilatoryRates;
  int _alertNumOocCoughRates;
  int _alertNumOocVoltages;
  int _alertNumOocPercentBodyMovements;
  int _alertNumDead;
  int _nonAlertNumOocVentilatoryRates;
  int _nonAlertNumOocCoughRates;
  int _nonAlertNumOocVoltages;
  int _nonAlertNumOocPercentBodyMovements;
  int _nonAlertNumDead;
  float _alertTemp;
  float _alertPH;
  float _alertConductivity;
  float _alertO2;
  float _nonAlertTemp;
  float _nonAlertPH;
  float _nonAlertConductivity;
  float _nonAlertO2;
};


#endif
