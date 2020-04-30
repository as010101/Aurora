/**************************************************************
*	Name: Akash Hasmukh Parikh (ahp)
*	File: DataManager.h
*	Asgn: perfmon
*	Date: Fri Apr 25 02:21:12 EDT 2003
**************************************************************/

#ifndef __DataManager_Header__
#define __DataManager_Header__

#include <StatsImage.H>
#include <pthread.h>
#include <string>

class ImportDelayedData;

class DataManager {

public:

  DataManager();
  virtual ~DataManager();

  /* blocks until it can lock the mutex and then returns a const reference to
   * the stats */
  const StatsImage& readStatsImage();

  /* blocks until i can lock the mutex for then returns a reference to stats
   * object */
  StatsImage& writeStatsImage();

  void releaseStatsImage();
  void printStatsImage();

  void setImportDelayedData(ImportDelayedData *idd);
  void setLoadShedderValue(float value);

  void blockingHasData();
  bool nonblockingHasData();
  void updateHasData();

  void setQueryNetwork(string qn);
  string getQueryNetwork();

protected:

private:
  pthread_cond_t	_data_cond;
  pthread_mutex_t	_data_mtx;
  pthread_mutex_t	_stats_mtx;
  StatsImage 		_stats;
  string			_query_network;
  ImportDelayedData	*_idd;

  
  bool				_hasData;

};

#endif
