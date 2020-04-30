/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: QOSMonitor.H
 *	Asgn: auroraStuff
 *	Date: Tue Apr  8 23:18:39 EDT 2003
 **************************************************************/

#ifndef QOSMonitor_Header
#define QOSMonitor_Header

#include <qlayout.h>
#include <GUIConstants.H>

class QCanvasView;
class QCanvasRectangle;
class QCanvasLine;
class QCanvas;
class QLabel;

class QOSMonitor : public QVBoxLayout {
  Q_OBJECT

public:

  QOSMonitor(QWidget * parent = 0, QLayout * layout = 0, int num = 0, QOSTYPE type = UTILITY);

  virtual ~QOSMonitor();

public slots:
  void update(int newval);

  void setExpectedUtil(int util);

protected:

private:
  QCanvasView * m_view;
  QCanvas * m_canvas;
  QCanvasRectangle * m_border;
  QLabel * m_toplabel, * m_bottomlabel, * m_applabel;
  QCanvasLine ** m_lines;
  QCanvasLine * m_util;
  int m_position;
};

#endif
