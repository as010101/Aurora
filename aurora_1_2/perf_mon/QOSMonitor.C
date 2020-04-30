/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: QOSMonitor.C
 *    DATE: Tue Apr  8 23:21:15 2003
 *************************************************************************/
#include <QOSMonitor.H>
#include <qcolor.h>
#include <qpen.h>
#include <qcanvas.h>
#include <qlabel.h>


/*************************************************************************
 * Function Name: QOSMonitor::QOSMonitor
 * Parameters: 
 * Effects: 
 *************************************************************************/

QOSMonitor::QOSMonitor(QWidget* parent, QLayout* layout, int num, QOSTYPE type) : QVBoxLayout(layout)
{
  setAlignment(Qt::AlignTop);
  m_view = new QCanvasView(parent, "QOSCanvasView");
  m_view->setFrameStyle(QFrame::NoFrame);
  m_view->setFixedHeight(QOS_MONITOR_HEIGHT);
  m_toplabel = new QLabel("100", parent);
  m_toplabel->setPaletteForegroundColor(blue);
  m_bottomlabel = new QLabel("0", parent);
  m_bottomlabel->setPaletteForegroundColor(blue);
  QString mynum;
  mynum.setNum(num+1, 10);
  QString mystring;
  if (type == UTILITY)
    mystring = "Utility: App# ";
  else if (type == LATENCY) //type is LATENCY
    mystring = "Latency-Based
Utility: App# ";
  mystring+=mynum;
  m_applabel = new QLabel(mystring, parent);
  m_toplabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_bottomlabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_view->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  m_applabel->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));;

  addWidget(m_toplabel);
  addWidget(m_view);
  addWidget(m_bottomlabel);
  addWidget(m_applabel);

  m_canvas = new QCanvas(QOS_MONITOR_WIDTH, QOS_MONITOR_HEIGHT);
  m_view->setCanvas(m_canvas);
  m_border = new QCanvasRectangle(0, 0, QOS_MONITOR_WIDTH, QOS_MONITOR_HEIGHT, m_canvas);
  m_border->show();
  m_view->show();
  m_toplabel->show();
  m_bottomlabel->show();
  m_applabel->show();
  m_lines = new QCanvasLine*[QOS_MONITOR_LINES];
  for (int i = 0; i < QOS_MONITOR_LINES; i++) {
    m_lines[i] = new QCanvasLine(m_canvas);
    m_lines[i]->setPen(QPen(red));
    m_lines[i]->setZ(10);
  }
  m_lines[QOS_MONITOR_LINES - 1]->setPen(QPen(magenta)); //last line is magenta so new line is visibly apparent..
  m_position = 0;
  m_canvas->update();
  m_util = new QCanvasLine(m_canvas);
  m_util->setZ(0);
  setExpectedUtil(0);
}


/*************************************************************************
 * Function Name: QOSMonitor::~QOSMonitor
 * Parameters: 
 * Effects: 
 *************************************************************************/

QOSMonitor::~QOSMonitor()
{
  delete m_canvas;
  delete [] m_lines;
}

/*************************************************************************
 * Function Name: QOSMonitor::update
 * Parameters: int newval
 * Effects: updates the monitor with value from 0-100, 0 is bottom and 100 is top
 *************************************************************************/
void 
QOSMonitor::update(int newval) {
  //the position can't go past 49...
  if (m_position > QOS_MONITOR_LINES - 1) { //move all previous lines back
    for (int i = 0; i < QOS_MONITOR_LINES - 1; i++) //move in array
      m_lines[i]->setPoints(m_lines[i+1]->startPoint().x() - 2, 
			    m_lines[i+1]->startPoint().y(),
			    m_lines[i+1]->endPoint().x() -2,
			    m_lines[i+1]->endPoint().y());
    m_position = QOS_MONITOR_LINES - 1; 
  }

  //adjust value to fit in window, and with 0 at the bottom
  newval = 100 - newval;
  newval = (int)(newval/((double)100/QOS_MONITOR_HEIGHT));

  //set the line
  m_lines[m_position]->setPoints((int)(m_position*((double)QOS_MONITOR_WIDTH/QOS_MONITOR_LINES)), newval, (int)((m_position+1)*((double)QOS_MONITOR_WIDTH/QOS_MONITOR_LINES)-1), newval);
  m_lines[m_position]->show();

  //increment the current monitor position
  m_position++;	
  m_canvas->update();
}

/*************************************************************************
 * Function Name: QOSMonitor::setExpectedUtil
 * Parameters: int util (0-100), 0 is top, 100 is bottom
 * Effects: sets the predicted utility level into the QOSMonitor
 *************************************************************************/
void 
QOSMonitor::setExpectedUtil(int util)
{
  int height = (int) (util/((double)1000.0/QOS_MONITOR_HEIGHT));
  m_util->setPoints(0, height, QOS_MONITOR_WIDTH, height);
  m_util->setPen(QPen(gray, 1));
  m_util->show();
  m_canvas->update();
}
