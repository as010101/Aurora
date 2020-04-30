/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: MainWindow.C
 *    DATE: Tue Apr  8  0:51:03 2003
 *************************************************************************/
#include <MainWindow.H>

#include <qapplication.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtabwidget.h>
#include <qtimer.h>

#include <GUIConstants.H>
#include <LoadShedPage.H>
#include <NetworkGuiPage.H>
#include <iostream>

/*************************************************************************
 * Function Name: MainWindow::MainWindow
 * Parameters: QWidget * parent = 0, const char *name=0
 * Effects: 
 *************************************************************************/

MainWindow::MainWindow(DataManager* data, int updateSpeed):QMainWindow(0, 0)
{
  m_data = data;
  this->setGeometry(25, 25, 0, 0);
  setMinimumSize(GUI_WIDTH, GUI_HEIGHT);

  //make tab bar central widget, use it to select pages
  m_tabs = new QTabWidget(this, "pagetab");
  m_tabs->setMinimumSize(GUI_WIDTH, GUI_HEIGHT);
  this->setCentralWidget(m_tabs); 
  
  QMenuBar* menubar = this->menuBar();
  menubar->setSeparator(QMenuBar::InWindowsStyle);

  //make menu with quit option
  m_filepopup = new QPopupMenu(this);
  int quitID = m_filepopup->insertItem("&Quit");
  m_filepopup->connectItem(quitID, qApp, SLOT(quit()));

  menubar->insertItem("&Performance Monitor", m_filepopup);

  //wait to instantiate tabs until there is data in the data manager
  m_data->blockingHasData();
  m_image = m_data->readStatsImage(); //get the initial data
  m_data->releaseStatsImage(); //release mutex
  m_numapps = m_image._appArcsStats.size();

  //add pages to tab bar
  m_loadshedpage = new LoadShedPage(this, "Load Shed Tab", m_numapps);
  m_tabs->addTab(m_loadshedpage, "&LoadShedder");
  m_netgui = new NetworkGuiPage(m_data->getQueryNetwork(), this, "Query Network Tab"); 
  //use passed in string to access query network
  m_tabs->addTab(m_netgui, "&Query Network");

  updateGUI(); //start showing data

  //set up timer to show data periodically
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(updateGUI()) );
  m_timer->start(updateSpeed, false);
}


/*************************************************************************
 * Function Name: MainWindow::~MainWindow
 * Parameters: 
 * Effects: 
 *************************************************************************/

MainWindow::~MainWindow()
{
  delete m_loadshedpage;
  delete m_filepopup;
  delete m_timer;
}

/*************************************************************************
 * Function Name: MainWindow::updateGUI
 * Parameters: 
 * Effects: Queries the data manager for new data
 *************************************************************************/

void 
MainWindow::updateGUI()
{
  m_image = m_data->readStatsImage();
  m_data->releaseStatsImage();
  std::map<int, AppArcStats>::iterator iter = m_image._appArcsStats.begin();
  for (int i = 0; i < m_numapps; i++) {
    //std::cout<<iter->second._avgLatencyUtility<<std::endl;
    m_loadshedpage->updateMonitor(i, (int)(100.0 * iter->second._avgLatencyUtility));
    iter++;
  }
  m_loadshedpage->updateGraph(m_image._utilityVsLsPrediction);
  m_netgui->updateNetwork(m_image._numTuplesOnArcs, m_image._dropsOnArcs);
}

/*************************************************************************
 * Function Name: MainWindow::updateLoadShed
 * Parameters: 
 * Effects: sets loadshed val in Datamanager
 *************************************************************************/
void 
MainWindow::updateLoadShed(float val)
{
  m_data->setLoadShedderValue(1.0 - val);
}
