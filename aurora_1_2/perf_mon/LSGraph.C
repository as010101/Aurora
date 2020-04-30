/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: LSGraph.C
 *    DATE: Sat Apr 12 13:30:18 2003
 *************************************************************************/
#include <LSGraph.H>

#include <qpointarray.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <GUIConstants.H>

/*************************************************************************
 * Function Name: LSGraph::LSGraph
 * Parameters: QWidget* parent, const char * name
 * Effects: 
 *************************************************************************/

LSGraph::LSGraph(QWidget* parent, const char * name):QCanvasView(parent, name)
{ 
  this->setFrameStyle(QFrame::NoFrame);
  m_canvas = new QCanvas(LOADSHED_GRAPH_SIZE, LOADSHED_GRAPH_SIZE);
  this->setCanvas(m_canvas);
  m_canvas->update();

  m_graphpoints = new QPointArray(0);
 
  m_sliderpos = 0;
  m_selected = -1;
   
  connect(this, SIGNAL(changeLoadShed(float)),
	  parent->parent(), SLOT(updateLoadShed(float)) );
}


/*************************************************************************
 * Function Name: LSGraph::~LSGraph
 * Parameters: 
 * Effects: 
 *************************************************************************/

LSGraph::~LSGraph()
{
  delete m_canvas;
  delete m_graphpoints;
}


/*************************************************************************
 * Function Name: LSGraph::updateGraph
 * Parameters: int numpoints, double * values
 * Returns: void
 * Effects: Redraws the graph based on new set of values
 *************************************************************************/
void
LSGraph::updateGraph(std::vector<double> curve)
{
  //delete all old items
  QCanvasItemList list = m_canvas->allItems();        
  for ( QCanvasItemList::iterator it = list.begin(); it != list.end(); ++it )            
    delete *it;
  int numpoints = curve.size();
  m_graphpoints->resize(numpoints);
  for (int i = 0; i < numpoints; i++) {
    m_graphpoints->setPoint(i, (int)(((double)i/numpoints)*LOADSHED_GRAPH_SIZE), (int)(LOADSHED_GRAPH_SIZE*((double)(1-curve[i]))) );
  }
  int xdistance = (int)((1.0/m_graphpoints->size())*LOADSHED_GRAPH_SIZE);
  bool setselected = false;
  for (int i = 0; i < numpoints; i++) {
    if (!setselected && ((i == numpoints -1) || (m_graphpoints->point(i+1).y() > m_sliderpos))) {
      m_selected = i;
      setselected = true;
      i = numpoints;
    }
  }
  for (int i = 0; i < numpoints; i++) {
    QCanvasRectangle * rect = new QCanvasRectangle((m_graphpoints->point(i)).x(), (m_graphpoints->point(i)).y(), 
				     xdistance, LOADSHED_GRAPH_SIZE - (m_graphpoints->point(i)).y(),
				     m_canvas);
    rect->setPen(QPen(black, 1));
    if (i == m_selected) 
      rect->setBrush(blue);
    else 
      rect->setBrush(red);
    rect->setZ(0);
    rect->show();

    if (i == m_selected) {
      QFont font;
      font.setPointSize(8);
      QString barNumber;
      barNumber = barNumber.setNum(i, 10);
      QCanvasText * text = new QCanvasText(barNumber, m_canvas);
      text->setColor(black);
      text->setX((m_graphpoints->point(i)).x() + (int)xdistance/2 - 1);
      text->setY(((m_graphpoints->point(m_selected)).y() - 12));
      if (text->y() < 0)
	text->setY(0);
      text->setZ(1);
      text->setFont(font);
      text->show();
    }
  }
  QCanvasLine * line = new QCanvasLine(m_canvas);
  line->setPoints(0, m_sliderpos, LOADSHED_GRAPH_SIZE, m_sliderpos);
  line->setPen(QPen(black, 1));
  line->setZ(2);
  line->show();

  m_canvas->update();
}

/*************************************************************************
 * Function Name: LSGraph::moveSlider
 * Parameters: int newposition
 * Returns: void
 * Effects: moves the line specifying slider correlation to graph
 *************************************************************************/
void
LSGraph::moveSlider(int newposition)
{
  QCanvasItemList list = m_canvas->allItems();
  int old = m_sliderpos;
  int oldselected = m_selected;
  m_sliderpos = (int)(((double)newposition/1000)*LOADSHED_GRAPH_SIZE);

  bool setselected = false;
  for (int i = 0; i < m_graphpoints->size(); i++) {
    if (!setselected && ((i == m_graphpoints->size() -1) || (m_graphpoints->point(i+1).y() > m_sliderpos))) {
      m_selected = i;
      setselected = true;
      i = m_graphpoints->size();
    }
  }
  int xdistance = (int)((1.0/m_graphpoints->size())*LOADSHED_GRAPH_SIZE); //distance between each bar
  for ( QCanvasItemList::iterator it = list.begin(); it != list.end(); ++it ) {         
    if ((*it)->rtti() == QCanvasItem::Rtti_Line) {
      (*it)->moveBy(0, m_sliderpos - old); //find the line in the list and move it
    }
    if (oldselected != m_selected) { //if the selected rectangle has changed, set new rect to blue and old to red
      if ((*it)->rtti() == QCanvasItem::Rtti_Text) {
	(*it)->moveBy((m_selected - oldselected) * xdistance, 0);
	(*it)->setY((m_graphpoints->point(m_selected)).y() - 12);
	if ((*it)->y() < 0)
	  (*it)->setY(0);
	QString text;
	text = text.setNum(m_selected, 10);
	((QCanvasText*)(*it))->setText(text);
      }
      if (((*it)->rtti() == QCanvasItem::Rtti_Rectangle) && (*it)->y() == (m_graphpoints->point(m_selected)).y())
	((QCanvasRectangle*)(*it))->setBrush(blue);
      if (((*it)->rtti() == QCanvasItem::Rtti_Rectangle) && (*it)->y() == (m_graphpoints->point(oldselected)).y())
	((QCanvasRectangle*)(*it))->setBrush(red);
    }
  }  
  m_canvas->update();
  emit changeLoadShed((float)newposition/1000.0);
}
