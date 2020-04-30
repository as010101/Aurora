/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: DataManager.H
 *	Asgn: statsSpoof
 *	Date: Sun Apr 20 17:48:06 EDT 2003
 **************************************************************/

#ifndef DataManager_Header
#define DataManager_Header

#include <runtime_common_typedefs.H>
#include <map>

struct StatsImage;

class DataManager {

public:

  DataManager();

  virtual ~DataManager();

  virtual LoadShedderCurve& getLSCurve();

  virtual int getNumApps();

  virtual std::map<int, int>& getArcTuples();

  virtual float getQOS(int appnum);

  virtual void populateData();

protected:

private:
  StatsImage * m_data;

};

#endif
