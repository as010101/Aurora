/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: FishObserver.H
 *	Asgn: src
 *	Date: Wed Jun 25 16:51:34 EDT 2003
 **************************************************************/

#ifndef FishObserver_Header
#define FishObserver_Header

#include <time.h>
#include <sys/time.h>
#include <util.H>
#include <math.h>

#include "ModelObserver.H";
#include "constants.H";

class FishWidget;

class FishObserver : public ModelObserver {

public:

  FishObserver(FishWidget* fish);
  virtual ~FishObserver();

  void onNewOocEvent(const O0Tuple * pTuple);
  void onNumTuplesEnqChange(int n);
  std::string timeToString(const timeval time);

protected:

private:
  FishWidget* m_fish;

};

#endif
