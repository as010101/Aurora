/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: LoadShedPage.H
 *	Asgn: auroraStuff
 *	Date: Fri Apr 11 23:04:51 EDT 2003
 **************************************************************/

#ifndef LoadShedPage_Header
#define LoadShedPage_Header

#include <qwidget.h>
#include <vector>

class QOSMonitor;
class QBoxLayout;
class QVBoxLayout;
class LoadShedWidget;

class LoadShedPage: public QWidget {
  Q_OBJECT

public:

  LoadShedPage(QWidget * parent, const char *name, int numApps = 1);

  virtual ~LoadShedPage();

public slots:

  void updateMonitor(int monitornum, int latencyvalue /*, int qosvalue*/);

  void updateGraph(std::vector<double> curve);

  void setMonitorsExpectedUtility(int val);

  //void setNumApplications(int val);

protected:

private:
  LoadShedWidget * m_lswidget;
  QBoxLayout * m_qhbox;
  QVBoxLayout ** m_qos;
  QOSMonitor ** m_qosmon; //utility monitors
  QOSMonitor ** m_qosmon2; //latency monitors
  int m_numapps;

};

#endif
