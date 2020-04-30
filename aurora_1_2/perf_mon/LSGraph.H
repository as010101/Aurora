/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: LSGraph.H
 *	Asgn: auroraStuff
 *	Date: Sat Apr 12 13:16:49 EDT 2003
 **************************************************************/

#ifndef LSGraph_Header
#define LSGraph_Header

#include <qcanvas.h>
#include <vector>
class QPointArray;

class LSGraph: public QCanvasView {
  Q_OBJECT
public:
  
  LSGraph(QWidget* parent, const char * name);
  
  virtual ~LSGraph();
  
public slots:

  void updateGraph(std::vector<double> curve);

  void moveSlider(int newposition);

  signals:
  void changeLoadShed(float);
  
protected:

private:
  QCanvas * m_canvas;
  QPointArray * m_graphpoints;
  int m_sliderpos; //slider's position on graph
  int m_selected; //currently selected region on graph

};

#endif
