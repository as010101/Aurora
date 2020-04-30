/*************************************************************************
 *    NAME: Andrea Michelle Fein
 *    USER: afein
 *    FILE: FishWidget.C
 *    DATE: Mon Jun 16 14:28:13 2003
 *************************************************************************/
#include "FishWidget.H"
#include "AlertBox.H"
#include "FishTable.H"
#include "WaterTable.H"
#include "LabelBox.H"
#include "Model.H"

#include <cassert>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>

//const static int maxExpectedEnqTuples = 1908 - 1;
const static int maxExpectedEnqTuples = 377  - 1;

using namespace std;

/*************************************************************************
 * Function Name: FishWidget::FishWidget
 * Parameters: 
 * Effects: 
 *************************************************************************/

FishWidget::FishWidget() : QMainWindow()
{
  m_topLayout = NULL;
  m_mainLayout = NULL;
  m_filePopup = NULL;
  m_alertsLabel = NULL;
  m_alerts = NULL;
  m_sliderLabel = NULL;
  m_slider = NULL;
  m_sliderValue = NULL;
  m_startButton = NULL;
  m_progress = NULL;
  m_labelBox = NULL;
  m_waterLabel = NULL;
  m_waterTable = NULL;
  m_fishLabel = NULL;
  m_fishTable = NULL;
  m_pic = NULL;

  m_model = NULL;

  m_cellWidth = 200;
  m_cellHeight = 45;

  m_isPaused = true;

  m_topLayout = new QWidget(this, "widget");
  this->setCentralWidget(m_topLayout);
  
   //---------------- MENU BAR ------------------
  QMenuBar* menubar = this->menuBar();
  menubar->setSeparator(QMenuBar::InWindowsStyle);
  m_filePopup = new QPopupMenu(this);

  int quitID = m_filePopup->insertItem("&Quit");
  m_filePopup->connectItem(quitID, this, SLOT(quit()));

  menubar->insertItem("&File", m_filePopup);
  
  //---------------- LABELS ---------------
  QFont font;
  font.setStyleHint(QFont::SansSerif);
  font.setPointSize(20);
  //m_labelBox = new LabelBox(m_topLayout, m_cellWidth, m_cellHeight);
  
  m_alertsLabel = new QLabel(m_topLayout);
  m_alertsLabel->setText("Alerts");
  m_alertsLabel->setFont(font);
  m_alertsLabel->setFrameShape(QFrame::WinPanel);
  m_alertsLabel->setFrameShadow(QFrame::Raised);
  m_sliderLabel = new QLabel(m_topLayout);
  m_sliderLabel->setText("Input Rate");
  m_sliderLabel->setFont(font);
  m_sliderLabel->setFrameShape(QFrame::WinPanel);
  m_sliderLabel->setFrameShadow(QFrame::Raised);
  m_sliderValue = new QLabel(m_topLayout);
  m_sliderValue->setText("Time between inputs: ");
  font.setPointSize(16);
  m_sliderValue->setFont(font);
  font.setPointSize(20);
  m_sliderValue->setFrameShape(QFrame::WinPanel);
  m_sliderValue->setFrameShadow(QFrame::Sunken);
  QColor white("white");
  m_sliderValue->setPaletteBackgroundColor(white);
  m_waterLabel = new QLabel(m_topLayout);
  m_waterLabel->setText("Water Quality Data");
  m_waterLabel->setFont(font);
  m_waterLabel->setFrameShape(QFrame::WinPanel);
  m_waterLabel->setFrameShadow(QFrame::Raised);
  m_fishLabel = new QLabel(m_topLayout);
  m_fishLabel->setText("Out of Control Fish Numbers");
  m_fishLabel->setFont(font);
  m_fishLabel->setFrameShape(QFrame::WinPanel);
  m_fishLabel->setFrameShadow(QFrame::Raised);

  
  //---------------- TABLES ---------------
  m_waterTable = new WaterTable(m_topLayout, m_cellWidth, m_cellHeight);
  int headerHeight = (m_waterTable->horizontalHeader())->height();
  int rowHeight = m_waterTable->rowHeight(0);
  m_labelBox = new LabelBox(m_topLayout, rowHeight, headerHeight);

  m_fishTable = new FishTable(m_topLayout);

  //---------------- LIST BOX ---------------
  m_alerts = new AlertBox(m_topLayout, m_cellWidth, m_cellHeight);
  m_alerts->setFixedHeight(m_waterTable->height());
  
  //---------------- SLIDER ----------------
  m_slider = new QSlider(m_topLayout);
  m_slider->setOrientation(Qt::Horizontal);
  m_slider->setMinValue(0);
  m_slider->setMaxValue(log10(900000.0) * SLIDER_DOUBLE);
  
  //---------------- BUTTON -----------------
  m_startButton = new QPushButton("Start Data Flow", m_topLayout);
  m_startButton->setFont(font);

  //---------------- PROGRESS BAR -----------------
  m_progress = new QProgressBar(maxExpectedEnqTuples, m_topLayout);
  m_progress->setProgress(0);
  
  //---------------- CONNECTIONS -----------------
  this->connect(m_alerts, SIGNAL(clicked(QListBoxItem*)), this, 
		SLOT(updateDisplays(QListBoxItem*)));
  this->connect(m_slider, SIGNAL(valueChanged(int)), this, 
		SLOT(updateSliderValue(int)));
  this->connect(m_startButton, SIGNAL(clicked()), this,
		SLOT(buttonClicked()));

  //---------------- GRID LAYOUT -----------------  

  //m_pic = new QLabel(m_topLayout);
  int left = m_slider->height() + m_sliderValue->height() + m_startButton->height();
  int right = m_fishTable->height();
  int size = right - left;
  //m_pic->setFixedHeight(size + 5);
  //QPixmap pixmap("/u/afein/fishgui/fish.gif");
  //m_pic->setPixmap(pixmap);

  m_mainLayout = new QGridLayout(m_topLayout, 21, 6);
  m_mainLayout->addWidget(m_alertsLabel, 0, 0);
  m_mainLayout->addMultiCellWidget(m_alerts, 1, 14, 0, 0);
  m_mainLayout->addWidget(m_sliderLabel, 15, 0);
  m_mainLayout->addWidget(m_slider, 16, 0);
  m_mainLayout->addMultiCellWidget(m_sliderValue, 17, 18, 0, 0);
  m_mainLayout->addWidget(m_startButton, 19, 0);
  //m_mainLayout->addWidget(m_pic, 20, 0);
  m_mainLayout->addWidget(m_progress, 20, 0);
  m_mainLayout->addMultiCellWidget(m_labelBox, 1, 14, 1, 1);
  m_mainLayout->addMultiCellWidget(m_waterLabel, 0, 0, 1, 5);
  m_mainLayout->addMultiCellWidget(m_waterTable, 1, 14, 2, 5);
  m_mainLayout->addMultiCellWidget(m_fishLabel, 15, 15, 1, 5);
  m_mainLayout->addMultiCellWidget(m_fishTable, 16, 20, 1, 5);
 

  //REMOVE THIS LATER
  struct FishSlidingWindows fsw1 = {
    "3:00",
    10.0111111,
    12.0111111,
    14.0111111,
    .311111,
    .411111,
    .511111,
    3.99999,
    2.46666,
    2.16666,
    24.066666,
    24.26666,
    24.36666
  };
  struct FishSlidingWindows fsw2 = {
    "2:00",
    6.011111,
    8.01111,
    10.01111,
    0.011111,
    0.21111,
    0.311111,
    0.0111111,
    0.2111111,
    0.41111111,
    20.011111,
    20.2111,
    20.3111111
  };
  struct AlertEvent ae1 = {
    fsw1,
    fsw2,
    maxExpectedEnqTuples,
    maxExpectedEnqTuples + 2,
    3,
    4,
    2,
    4,
    1,
    0,
    0,
    0,
    0,
    0,
    54.0111111,
    6.81111111,
    34.01111111,
    45.0111111,
    5.011111,
    6.01111111,
    7.0111111,
    8.1111110
  };
  struct AlertEvent ae2 = {
    fsw2,
    fsw1,
    1201,
    1232,
    6,
    6,
    6,
    6,
    6,
    2,
    2,
    2,
    2,
    2,
    6.01111111,
    6.01111,
    6.011111,
    6.0111111111,
    7.01111111,
    7.01111111,
    7.01111,
    7.011111,
  };
  //addAlert(ae1);
  //addAlert(ae2);
}


/*************************************************************************
 * Function Name: FishWidget::~FishWidget
 * Parameters: 
 * Effects: 
 *************************************************************************/

FishWidget::~FishWidget()
{   
  if (NULL != m_filePopup)
  {  
    delete m_filePopup;
    m_filePopup = NULL;
  }
  if (NULL != m_alertsLabel)
  {
    delete m_alertsLabel;
    m_alertsLabel = NULL;
  }
  if (NULL != m_alerts)
  {
    delete m_alerts;
    m_alerts = NULL;
  }
  if (NULL != m_sliderLabel)
  {
    delete m_sliderLabel;
    m_sliderLabel = NULL;
  }
  if (NULL != m_slider)
  {
    delete m_slider;
    m_slider = NULL;
  }
  if (NULL != m_sliderValue)
  {
    delete m_sliderValue;
    m_sliderValue = NULL;
  }
  if (NULL != m_startButton)
  {
    delete m_startButton;
    m_startButton = NULL;
  }
  if (NULL != m_progress)
  {
    delete m_progress;
    m_progress = NULL;
  }
  if (NULL != m_labelBox)
  {
    delete m_labelBox;
    m_labelBox = NULL;
  }
  if (NULL != m_waterLabel)
  {
    delete m_waterLabel;
    m_waterLabel = NULL;
  }
  if (NULL != m_waterTable)
  {
    delete m_waterTable;
    m_waterTable = NULL;
  }
  if (NULL != m_fishLabel)
  {
    delete m_fishLabel;
    m_fishLabel = NULL;
  }
  if (NULL != m_fishTable)
  {
    delete m_fishTable;
    m_fishTable = NULL;
  }
  if (NULL != m_pic)
  {
    delete m_pic;
    m_pic = NULL;
  }
  m_alertMap.clear();
  if (NULL != m_mainLayout)
  {
    delete m_mainLayout;
    m_mainLayout = NULL;
  }
  if (NULL != m_topLayout)
  {
    delete m_topLayout;
    m_topLayout = NULL;
  }
  m_alertMap.clear();
}


/*************************************************************************
 * Function Name: FishWidget::addAlert
 * Parameters: const AlertEvent& e
 * Returns: void
 * Effects: 
 *************************************************************************/
// Adds the alert to the map and the list box
void
FishWidget::addAlert(const AlertEvent& e)
{
  string time = e._alertWindows._timestamp;
  m_alertMap[time] = e;
  m_alerts->insertItem(time, 0);
  m_alerts->ensureVisible(0, 0);
  m_alerts->update();
}


/*************************************************************************
 * Function Name: FishWidget::getSliderValue
 * Parameters: 
 * Returns: int
 * Effects: 
 *************************************************************************/
int
FishWidget::getSliderValue()
{
  return m_slider->value();
}

/*************************************************************************
 * Function Name: FishWidget::setModel
 * Parameters: Model* model
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishWidget::setModel(Model* model)
{
  m_model = model;
  m_slider->setValue(m_slider->maxValue());
  updateSliderValue(m_slider->maxValue());
}


/*************************************************************************
 * Function Name: FishWidget::updateProgressDisplay
 * Parameters: int progress
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishWidget::updateProgressDisplay(int progress)
{
  m_progress->setProgress(progress);
}


/*************************************************************************
 * Function Name: FishWidget::updateDisplays
 * Parameters: int index
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishWidget::updateDisplays(QListBoxItem* item)
{
  if (item != NULL)
  {
    string t = item->text();
    AlertMapIterator it = m_alertMap.find(t);
    assert(it != m_alertMap.end());
    AlertEvent ae = it->second;
    m_waterTable->updateData(ae);
    m_fishTable->updateData(ae);
  }
}


/*************************************************************************
 * Function Name: FishWidget::updateSliderValue
 * Parameters: int time
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishWidget::updateSliderValue(int time)
{
  double t;
  if (time == 0)
  {
    m_sliderValue->setText("Time between inputs:\t\n  0min 0sec 0ms");
    if (m_model != NULL)
    {
      m_model->setEnqDelay(0);
    }
  }
  else if (time == m_slider->maxValue())
  {
    m_sliderValue->setText("Time between inputs:\t\n  15min 0sec 0ms");
    if (m_model != NULL)
    {
      m_model->setEnqDelay(900000);
    }
  }
  else
  {
    stringstream minConverter, secConverter, milliConverter;
    double t = (double)time;
    t = t/SLIDER_DOUBLE;
    t = pow(10.0, t);
    
    if (m_model != NULL)
    { 
      m_model->setEnqDelay(t);
    }

    int minutes = (int)t/60000;
    
    t = remainder(t, 60000.0);
    if (t < 0.0)
    {
      t += 60000.0;
    }
    int seconds = (int)t/1000;
    
    int milliseconds = (int)remainder(t, 1000.0);
    if (milliseconds < 0.0)
    {
      milliseconds += 1000;
    }
    
    string str, temp;
    minConverter << minutes;
    minConverter >> temp;
    str = temp + "min ";
    
    secConverter << seconds;
    secConverter >> temp;
    str += temp + "sec ";
    
    milliConverter << milliseconds;
    milliConverter >> temp;
    str += temp + "ms";
    m_sliderValue->setText("Time between inputs:\t\n  " + str);
  }
   
 
  
}


/*************************************************************************
 * Function Name: FishWidget::buttonClicked
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishWidget::buttonClicked()
{
  if (m_model != NULL)
  {
    if (m_isPaused)
    {
      m_isPaused = false;
      m_startButton->setText("Pause Data Flow");
      m_model->setFeederRunState(Model::FEEDER_RUNSTATE_ACTIVE);
    }
    else
    {
      m_isPaused = true;
      m_startButton->setText("Start Data Flow");
      m_model->setFeederRunState(Model::FEEDER_RUNSTATE_PAUSED);
    }
  }
}


/*************************************************************************
 * Function Name: FishWidget::quit
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishWidget::quit()
{  
  if (m_model != NULL) 
  {
    m_model->setFeederRunState(Model::FEEDER_RUNSTATE_FINISHED);
    m_model->setAppShutdown(true);
  }

  if (NULL != m_filePopup)
  {  
    delete m_filePopup;
    m_filePopup = NULL;
    //cout << "deleted filePopup" << endl;
  }
  if (NULL != m_alertsLabel)
  {
    delete m_alertsLabel;
    m_alertsLabel = NULL;
    //cout << "deleted alertsLabel" << endl;
  }
  if (NULL != m_alerts)
  {
    delete m_alerts;
    m_alerts = NULL;
    //cout << "deleted alerts" << endl;
  }
  if (NULL != m_sliderLabel)
  {
    delete m_sliderLabel;
    m_sliderLabel = NULL;
    //cout << "deleted sliderLabel" << endl;
  }
  if (NULL != m_slider)
  {
    delete m_slider;
    m_slider = NULL;
    //cout << "deleted slider" << endl;
  }
  if (NULL != m_sliderValue)
  {
    delete m_sliderValue;
    m_sliderValue = NULL;
    //cout << "deleted sliderValue" << endl;
  }
  if (NULL != m_startButton)
  {
    delete m_startButton;
    m_startButton = NULL;
    //cout << "deleted startButton" << endl;
  }
  if (NULL != m_progress)
  {
    delete m_progress;
    m_progress = NULL;
  }
  if (NULL != m_labelBox)
  {
    delete m_labelBox;
    m_labelBox = NULL;
    //cout << "deleted labelBox" << endl;
  }
  if (NULL != m_waterLabel)
  {
    delete m_waterLabel;
    m_waterLabel = NULL;
    //cout << "deleted waterLabel" << endl;
  }
  if (NULL != m_waterTable)
  {
    delete m_waterTable;
    m_waterTable = NULL;
    //cout << "deleted waterTable" << endl;
  }
  if (NULL != m_fishLabel)
  {
    delete m_fishLabel;
    m_fishLabel = NULL;
    //cout << "deleted alertFishLabel" << endl;
  }
  if (NULL != m_fishTable)
  {
    delete m_fishTable;
    m_fishTable = NULL;
    //cout << "deleted fishTable" << endl;
  }
  if (NULL != m_pic)
  {
    delete m_pic;
    m_pic = NULL;
  }
  if (NULL != m_mainLayout)
  {
    delete m_mainLayout;
    m_mainLayout = NULL;
    //cout << "deleted mainLayout" << endl;
  }
  if (NULL != m_topLayout)
  {
    delete m_topLayout;
    m_topLayout = NULL;
    //cout << "deleted topLayout" << endl;
  }
  qApp->quit();
}


