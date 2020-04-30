/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: DataManager.C
 *    DATE: Sun Apr 20 17:54:33 2003
 *************************************************************************/
#include "DataManager.H"
#include "StatsImage.H"

/*************************************************************************
 * Function Name: DataManager::DataManager
 * Parameters: 
 * Effects: 
 *************************************************************************/

DataManager::DataManager()
{
  m_data = new StatsImage;
  AppArcStats apps;
  srand(std::clock());
  int numApps = 1 + (int)(12.0*rand()/(RAND_MAX+1.0));
  for (int i = 0; i < numApps; i++) {
    m_data->_appArcsStats[i] = apps;
    m_data->_appArcsStats[i]._qosUtility = rand() % 100;
  }  
  m_data->_utilityVsLsPrediction.push_back(1.0);
  m_data->_utilityVsLsPrediction.push_back(.8);
  m_data->_utilityVsLsPrediction.push_back(.6);
}


/*************************************************************************
 * Function Name: DataManager::~DataManager
 * Parameters: 
 * Effects: 
 *************************************************************************/

DataManager::~DataManager()
{
  delete m_data;
}

/*************************************************************************
 * Function Name: DataManager::getLSCurve
 * Parameters: 
 * Returns: double*
 * Effects: 
 *************************************************************************/
LoadShedderCurve&
DataManager::getLSCurve()
{
  return m_data->_utilityVsLsPrediction;
}


/*************************************************************************
 * Function Name: DataManager::getNumApps
 * Parameters: 
 * Returns: int
 * Effects: 
 *************************************************************************/
int
DataManager::getNumApps()
{
  return m_data->_appArcsStats.size();
}


/*************************************************************************
 * Function Name: DataManager::getQOS
 * Parameters: int appnum
 * Returns: float
 * Effects: 
 *************************************************************************/
float
DataManager::getQOS(int appnum)
{
  return m_data->_appArcsStats[appnum]._qosUtility;
}

/*************************************************************************
 * Function Name: DataManager::getArcTuples
 * Parameters: 
 * Returns: map of number of tuples in each arc, keyed by arc id
 * Effects: 
 *************************************************************************/
std::map<int, int>& 
DataManager::getArcTuples()
{
  return m_data->_numTuplesOnArcs;
}


/*************************************************************************
 * Function Name: DataManager::populateData
 * Parameters: 
 * Returns: 
 * Effects: Fills the structures with 'random' data
 *************************************************************************/
void 
DataManager::populateData()
{
  int numApps = getNumApps();
  for (int i = 0; i < numApps; i++) {
    int diff = rand() % 5 - 2;
    m_data->_appArcsStats[i]._qosUtility+=diff;
    if (m_data->_appArcsStats[i]._qosUtility < 0)
      m_data->_appArcsStats[i]._qosUtility = 0;
    if (m_data->_appArcsStats[i]._qosUtility > 100)
      m_data->_appArcsStats[i]._qosUtility = 100;
  }
  for (int i = 1; i < m_data->_utilityVsLsPrediction.size(); i++) {
    double diff = (rand() % 3 - 1) * 0.01;
    m_data->_utilityVsLsPrediction[i] += diff;
    if (m_data->_utilityVsLsPrediction[i] < 0)
      m_data->_utilityVsLsPrediction[i] = 0.0;
    if (m_data->_utilityVsLsPrediction[i] > 1)
      m_data->_utilityVsLsPrediction[i] = 1.0;
  }
  int random = rand() % 5;
  if (random > 3 && m_data->_utilityVsLsPrediction.back() > 0.1)
    m_data->_utilityVsLsPrediction.push_back(m_data->_utilityVsLsPrediction.back()-0.1);
    
  for (int i = 0; i < 6; i++) //put random numbers of tuples in arcs
    m_data->_numTuplesOnArcs[i] = rand() % 5;
  for (int i = 6; i < 11; i++) //put random numbers of tuples in arcs
    m_data->_numTuplesOnArcs[i] = rand() % 40;
  m_data->_numTuplesOnArcs[11] = 20 + rand() % 60;
}
