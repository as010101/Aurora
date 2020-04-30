/*************************************************************************
 *    NAME: Andrea Michelle Fein
 *    USER: afein
 *    FILE: FishObserver.C
 *    DATE: Wed Jun 25 16:52:17 2003
 *************************************************************************/
#include "FishObserver.H"
#include "FishWidget.H"

#include <iostream>

using namespace std;
/*************************************************************************
 * Function Name: FishObserver::FishObserver
 * Parameters: QApplication app
 * Effects: 
 *************************************************************************/

FishObserver::FishObserver(FishWidget* fish)
{
  m_fish = fish;
}


/*************************************************************************
 * Function Name: FishObserver::~FishObserver
 * Parameters: 
 * Effects: 
 *************************************************************************/

FishObserver::~FishObserver()
{
}


/*************************************************************************
 * Function Name: FishObserver::onNewOocEvent
 * Parameters: const O0Tuple * pTuple
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishObserver::onNewOocEvent(const O0Tuple * pTuple)
{
  struct FishSlidingWindows alertWindows = {
    timeToString(pTuple->_ooc_timeOfSample),
    pTuple->_ooc_temperatureDelta1hour,
    pTuple->_ooc_temperatureDelta2hour,
    pTuple->_ooc_temperatureDelta4hour,
    pTuple->_ooc_phDelta1hour,
    pTuple->_ooc_phDelta2hour,
    pTuple->_ooc_phDelta4hour,
    pTuple->_ooc_condDelta1hour,
    pTuple->_ooc_condDelta2hour,
    pTuple->_ooc_condDelta4hour,
    pTuple->_ooc_o2Delta1hour,
    pTuple->_ooc_o2Delta2hour,
    pTuple->_ooc_o2Delta4hour
  };

  struct FishSlidingWindows nonAlertWindows = {
    timeToString(pTuple->_good_timeOfSample),
    pTuple->_good_temperatureDelta1hour,
    pTuple->_good_temperatureDelta2hour,
    pTuple->_good_temperatureDelta4hour,
    pTuple->_good_phDelta1hour,
    pTuple->_good_phDelta2hour,
    pTuple->_good_phDelta4hour,
    pTuple->_good_condDelta1hour,
    pTuple->_good_condDelta2hour,
    pTuple->_good_condDelta4hour,
    pTuple->_good_o2Delta1hour,
    pTuple->_good_o2Delta2hour,
    pTuple->_good_o2Delta4hour
  };
  
  struct AlertEvent ae = {
    alertWindows,
    nonAlertWindows,
    pTuple->_ooc_printerInterval,
    pTuple->_good_printerInterval,
    pTuple->_ooc_num_ooc_vent_rates,
    pTuple->_ooc_num_ooc_cough_rates,
    pTuple->_ooc_num_ooc_voltages,
    pTuple->_ooc_num_ooc_percent_body_movements,
    pTuple->_ooc_num_dead,
    pTuple->_good_num_ooc_vent_rates,
    pTuple->_good_num_ooc_cough_rates,
    pTuple->_good_num_ooc_voltages,
    pTuple->_good_num_ooc_percent_body_movements,
    pTuple->_good_num_dead,
    pTuple->_ooc_temperatureCelcius,
    pTuple->_ooc_pH,
    pTuple->_ooc_conductivity_mS_per_cm,
    pTuple->_ooc_disolvedO2_mg_per_l,
    pTuple->_good_temperatureCelcius,
    pTuple->_good_pH,
    pTuple->_good_conductivity_mS_per_cm,
    pTuple->_good_disolvedO2_mg_per_l
  };

  m_fish->addAlert(ae);
}


/*************************************************************************
 * Function Name: FishObserver::onNumTuplesEnqChange
 * Parameters: int n
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishObserver::onNumTuplesEnqChange(int n) 
{
  m_fish->updateProgressDisplay(n);
}


/*************************************************************************
 * Function Name: FishObserver::timeToString
 * Parameters: const timeval time
 * Returns: string
 * Effects: 
 *************************************************************************/
string
FishObserver::timeToString(const timeval time)
{
  return timevalToHumanString(time);
}

