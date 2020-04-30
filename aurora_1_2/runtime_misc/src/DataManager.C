/*************************************************************************
 *    NAME: Akash Hasmukh Parikh
 *    USER: ahp
 *    FILE: DataManager.cpp
 *    DATE: Fri Apr 25  2:32:08 2003
 *************************************************************************/
#include "DataManager.H"
#include "ImportDelayedData.H"
#include <iostream>

/*************************************************************************
 * Function Name: DataManager::DataManager
 * Parameters: 
 * Effects: 
 *************************************************************************/

DataManager::DataManager()
{
  if(pthread_mutex_init(&_stats_mtx, NULL))
  {
	printf("pthread_mutex_init failed\n");
  }

  _idd = NULL;

  if(pthread_cond_init(&_data_cond, NULL))
  {
	printf("pthread_cond_init failed\n");
  }

  if(pthread_mutex_init(&_data_mtx, NULL))
  {
	printf("pthread_mutex_init failed\n");
  }

  _hasData = false;
  srand48(time(NULL));
}


/*************************************************************************
 * Function Name: DataManager::~DataManager
 * Parameters: 
 * Effects: 
 *************************************************************************/

DataManager::~DataManager()
{
  /* destroy stats information */

  if(pthread_mutex_destroy(&_stats_mtx))
  {
	printf("pthread_mutex_destroy failed\n");
  }

  if(pthread_mutex_destroy(&_data_mtx))
  {
	printf("pthread_mutex_destroy failed\n");
  }

  if(pthread_cond_destroy(&_data_cond))
  {
	printf("pthread_cond_destroy failed\n");
  }
}


/*************************************************************************
 * Function Name: DataManager::readStatsImage
 * Parameters: 
 * Returns: const StatsImage&
 * Effects: 
 *************************************************************************/
const StatsImage&
DataManager::readStatsImage()
{
  pthread_mutex_lock(&_stats_mtx);
  return _stats;
}


/*************************************************************************
 * Function Name: DataManager::writeStatsImage
 * Parameters: 
 * Returns: StatsImage&
 * Effects: 
 *************************************************************************/
StatsImage&
DataManager::writeStatsImage()
{
  pthread_mutex_lock(&_stats_mtx);
  return _stats;
}


/*************************************************************************
 * Function Name: DataManager::releaseStatsImage
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
DataManager::releaseStatsImage()
{
  pthread_mutex_unlock(&_stats_mtx);
}


/*************************************************************************
 * Function Name: DataManager::releaseStatsImage
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
DataManager::printStatsImage()
{
	cout << _stats.toString() << endl;
}


/*************************************************************************
 * Function Name: DataManager::setLoadShedderValue
 * Parameters: float value
 * Returns: void
 * Effects: 
 *************************************************************************/
void
DataManager::setLoadShedderValue(float value)
{
  _idd->send_load_shedder_value(value);
}


/*************************************************************************
 * Function Name: DataManager::setImportDelayedData
 * Parameters: ImportDelayedData *idd
 * Returns: void
 * Effects: 
 *************************************************************************/
void
DataManager::setImportDelayedData(ImportDelayedData *idd)
{
  _idd = idd;
}



/*************************************************************************
 * Function Name: DataManager::releaseStatsImage
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
string
DataManager::getQueryNetwork()
{
  return _query_network;
}



/*************************************************************************
 * Function Name: DataManager::releaseStatsImage
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
DataManager::setQueryNetwork(string qn)
{
  _query_network = qn;
}



/*************************************************************************
 * Function Name: DataManager::blockingHasData
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
DataManager::blockingHasData()
{
	if( !_hasData) {
	  pthread_mutex_lock(&_data_mtx);

	  while(!_hasData) {
		pthread_cond_wait(&_data_cond, &_data_mtx);
	  }

	  pthread_mutex_unlock(&_data_mtx);
	}
}


/*************************************************************************
 * Function Name: DataManager::nonblockingHasData
 * Parameters: 
 * Returns: bool
 * Effects: 
 *************************************************************************/
bool
DataManager::nonblockingHasData()
{
  return _hasData;
}


/*************************************************************************
 * Function Name: DataManager::updateHasData
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
DataManager::updateHasData()
{
  if(!_hasData) {
	_hasData = true;
	pthread_cond_broadcast(&_data_cond);
  }
}

//===============================================================================

StatsImage::StatsImage()
		throw (exception)
{
	// no-op on purpose
}
