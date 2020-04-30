/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: LoadShedPage.C
 *    DATE: Fri Apr 11 23:08:29 2003
 *************************************************************************/
#include <LoadShedPage.H>

#include <qlayout.h>

#include <QOSMonitor.H>
#include <LoadShedWidget.H>
#include <GUIConstants.H>

/*************************************************************************
 * Function Name: LoadShedPage::LoadShedPage
 * Parameters: 
 * Effects: 
 *************************************************************************/

LoadShedPage::LoadShedPage(QWidget * parent, const char *name, int numApps):QWidget(parent, name) 
{
  setMinimumSize(GUI_WIDTH, GUI_HEIGHT);

  m_qhbox = new QHBoxLayout(this);

  m_qhbox->insertStretch(0, 5);

  m_lswidget = new LoadShedWidget(this, m_qhbox);

  m_qhbox->setStretchFactor(m_lswidget, 0);

  m_numapps = numApps;

  m_qos = new QVBoxLayout*[1];
  m_qos[0] = new QVBoxLayout(m_qhbox);
  //m_qos[1] = new QVBoxLayout(m_qhbox);
  m_qhbox->setStretchFactor(m_qos[0], 0);
  //m_qhbox->setStretchFactor(m_qos[1], 0);
  m_qosmon = new QOSMonitor*[m_numapps];
  //m_qosmon2 = new QOSMonitor*[m_numapps];
  
  //latency monitors
  for (int i = 0; i < m_numapps; i++) {
    m_qosmon[i] = new QOSMonitor(this, m_qos[0], i, LATENCY);
    m_qos[0]->setStretchFactor(m_qosmon[i], 0);
  }
  //utility monitors
  /* not currently implemented... waiting for _qosUtility to become available in StatsImage
  for (int i = 0; i < m_numapps; i++) {
    m_qosmon2[i] = new QOSMonitor(this, m_qos[1], i, UTILITY);
    m_qos[1]->setStretchFactor(m_qosmon2[i], 0);
  }
  */
  m_qos[0]->insertStretch(-1, 5);
  //m_qos[1]->insertStretch(-1, 5);
}

/*************************************************************************
 * Function Name: LoadShedPage::~LoadShedPage
 * Parameters: 
 * Effects: 
 *************************************************************************/

LoadShedPage::~LoadShedPage()
{
  delete [] m_qos;
  delete [] m_qosmon;
}

/*************************************************************************
 * Function Name: LoadShedPage::updateMonitor
 * Parameters: 
 * Effects: updates specified QOSMonitor with newest value
 *************************************************************************/

void 
LoadShedPage::updateMonitor(int monitornum, int latencyvalue /*, int qosvalue*/)
{
  m_qosmon[monitornum]->update(latencyvalue);
  //m_qosmon2[monitornum]->update(qosvalue);
}

/*************************************************************************
 * Function Name: LoadShedPage::updateGraph
 * Parameters: 
 * Effects: updates the graph with latest curve
 *************************************************************************/

void 
LoadShedPage::updateGraph(std::vector<double> curve) {
  m_lswidget->updateGraph(curve);
}

/*************************************************************************
 * Function Name: LoadShedPage::setMonitorsExpectedUtility
 * Parameters: int value
 * Effects: sets the expected utility on all qos monitors
 *************************************************************************/
void 
LoadShedPage::setMonitorsExpectedUtility(int val)
{
  /* for (int i = 0; i < m_numapps; i++) {
     m_qosmon[i]->setExpectedUtil(val);
     }
  */
}

/*************************************************************************
 * Function Name: LoadShedPage::setNumApplications
 * Parameters: int val
 * Effects: sets the number of QOS Monitors to display
 *************************************************************************/
/*
  void 
  LoadShedPage::setNumApplications(int val)
  {
  if (val == 0) val = 1;
  if (val > MAX_NUM_APPLICATIONS) val = MAX_NUM_APPLICATIONS;
  if (m_numapps != val) {
  int numqoscols = m_numapps/6;
  int newnumqoscols = val/6;
  if (newnumqoscols > numqoscols) {
  for (int i = numqoscols; i < newnumqoscols; i++) {
  m_qos[i] = new QVBoxLayout(m_qhbox);
  m_qhbox->setStretchFactor(m_qos[i], 0);
  }
  }
  if (val > m_numapps) {
  for (int i = m_numapps; i < val; i++) {
  m_qosmon[i] = new QOSMonitor(this, m_qos[i/6], i);
  m_qos[i/6]->setStretchFactor(m_qosmon[i], 0);
  }
  }
  else if (val < m_numapps) {
  for (int i = val; i < m_numapps; i++) {
  m_qos[i/6]->removeItem(m_qosmon[i]);
  delete m_qosmon[i];
  }
  }
  //for (int i = numqoscols; i < newnumqoscols; i++)
  //m_qos[i]->insertStretch(-1, 5);
  m_numapps = val;
  }
  }
*/
