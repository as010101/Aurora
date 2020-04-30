/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: MainWindow.H
 *	Asgn: auroraStuff
 *	Date: Tue Apr  8 00:47:10 EDT 2003
 **************************************************************/

#ifndef MainWindow_Header
#define MainWindow_Header

#include <qmainwindow.h>

#include <DataManager.H>

class QWidget;
class LoadShedPage;
class QPopupMenu;
class QTabWidget;
class NetworkGuiPage;
class QTimer;

class MainWindow: public QMainWindow {
  Q_OBJECT
public:

  MainWindow(DataManager * data, int updateSpeed);

  virtual ~MainWindow();

public slots:

  void updateGUI();

  void updateLoadShed(float val);

protected:

private:
  StatsImage m_image;
  QTimer * m_timer;
  QPopupMenu * m_filepopup;
  LoadShedPage * m_loadshedpage;
  NetworkGuiPage * m_netgui;
  QTabWidget * m_tabs;
  DataManager * m_data;
  int m_numapps;

};

#endif
