/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: FishWidget.H
 *	Asgn: fishgui
 *	Date: Mon Jun 16 14:22:50 EDT 2003
 **************************************************************/

#ifndef FishWidget_Header
#define FishWidget_Header

#include <qapplication.h>
#include <qmainwindow.h>
#include <qlayout.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qslider.h>
#include <qlistbox.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qfont.h>
#include <qpushbutton.h>
#include <map>
#include "constants.H"

class AlertBox;
class WaterTable;
class FishTable;
class LabelBox;
class Model;

typedef std::map<std::string, AlertEvent> AlertMap;
typedef AlertMap::iterator AlertMapIterator;

class FishWidget : public QMainWindow {

  Q_OBJECT

public:

  FishWidget();
  virtual ~FishWidget();

  void addAlert(const AlertEvent& e);
  int getSliderValue();
  void setModel(Model* model);
  void updateProgressDisplay(int progress);

public slots:
  void updateDisplays(QListBoxItem* item);
  void updateSliderValue(int time);
  void buttonClicked();
  void quit();

protected:

private:

  AlertMap m_alertMap;
  QWidget* m_topLayout;
  QGridLayout* m_mainLayout;
  QPopupMenu* m_filePopup;

  QLabel* m_alertsLabel;
  AlertBox* m_alerts;
  QLabel* m_sliderLabel;
  QSlider* m_slider;
  QLabel* m_sliderValue;
  QPushButton* m_startButton;
  QProgressBar* m_progress;
  LabelBox* m_labelBox;
  QLabel* m_waterLabel;
  WaterTable* m_waterTable;
  QLabel* m_fishLabel;
  FishTable* m_fishTable;
  QLabel* m_pic;

  Model* m_model;

  int m_cellWidth;
  int m_cellHeight;
  bool m_isPaused;

  const static double SLIDER_DOUBLE = 1000000.0;

};

#endif
