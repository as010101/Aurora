/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: QueryArc.C
 *    DATE: Sat Apr 19  1:06:22 2003
 *************************************************************************/
#include <QueryArc.H>
#include <Arc.H>

#include <qcanvas.h>

/*************************************************************************
 * Function Name: QueryArc::QueryArc
 * Parameters: int startX, int startY, int endX, int endY, int thickness
 * Effects: 
 *************************************************************************/

QueryArc::QueryArc(Arc * arc)
{
  m_arc = arc;
}


/*************************************************************************
 * Function Name: QueryArc::~QueryArc
 * Parameters: 
 * Effects: 
 *************************************************************************/

QueryArc::~QueryArc()
{

}


/*************************************************************************
 * Function Name: QueryArc::getSourceBox
 * Parameters: 
 * Returns: id of source box
 * Effects: 
 *************************************************************************/
int
QueryArc::getSourceBox()
{
  return m_arc->getSourceId();
}


/*************************************************************************
 * Function Name: QueryArc::getDestBox
 * Parameters: 
 * Returns: id of destination box
 * Effects: 
 *************************************************************************/
int
QueryArc::getDestBox()
{
  return m_arc->getDestinationId();
}

/*************************************************************************
 * Function Name: QueryArc::getSourceBox
 * Parameters: 
 * Returns: id of source box port
 * Effects: 
 *************************************************************************/
int
QueryArc::getSourcePort()
{
  return m_arc->getSourcePortId();
}


/*************************************************************************
 * Function Name: QueryArc::getDestBox
 * Parameters: 
 * Returns: id of destination box port
 * Effects: 
 *************************************************************************/
int
QueryArc::getDestPort()
{
  return m_arc->getDestinationPortId();
}

/*************************************************************************
 * Function Name: QueryArc::setNumTuples
 * Parameters: QCanvasText *
 * Returns: 
 * Effects: sets the text displaying how many tuples in arc
 *************************************************************************/
void
QueryArc::setNumTuples(QCanvasText * text)
{
  m_text = text;
} 

/*************************************************************************
 * Function Name: QueryArc::getNumTuples
 * Parameters:
 * Returns: 
 * Effects: gets the text displaying how many tuples in arc
 *************************************************************************/
QCanvasText *
QueryArc::getNumTuples()
{
  return m_text;
} 

/*************************************************************************
 * Function Name: QueryArc::getLine
 * Parameters: 
 * Returns: QCanvasLine *, line of arc
 * Effects: 
 *************************************************************************/
QCanvasLine * 
QueryArc::getLine()
{
  return m_line;
}

/*************************************************************************
 * Function Name: QueryArc::setLine
 * Parameters: 
 * Returns: 
 * Effects: sets the line representing this arc
 *************************************************************************/
void
QueryArc::setLine(QCanvasLine * line)
{
  m_line = line;
}


/*************************************************************************
 * Function Name: QueryArc::getID
 * Parameters: 
 * Returns: id of arc
 * Effects: 
 *************************************************************************/
int
QueryArc::getID()
{
  return m_arc->getId();
}

/*************************************************************************
 * Function Name: QueryArc::setDrop
 * Parameters: QCanvasText *
 * Returns: 
 * Effects: sets the text object displaying drop information
 *************************************************************************/
void
QueryArc::setDrop(QCanvasText * dropText)
{
  m_dropText = dropText;
} 

/*************************************************************************
 * Function Name: QueryArc::getDrop
 * Parameters:
 * Returns: 
 * Effects: gets the text displaying the drop predicate
 *************************************************************************/
QCanvasText *
QueryArc::getDrop()
{
  return m_dropText;
} 

/*************************************************************************
 * Function Name: QueryArc::setDropRectangle
 * Parameters: QCanvasText *
 * Returns: 
 * Effects: sets the rectangle representing a drop
 *************************************************************************/
void
QueryArc::setDropRectangle(QCanvasRectangle * dropRect)
{
  m_dropRect = dropRect;
} 

/*************************************************************************
 * Function Name: QueryArc::getDropRectangle
 * Parameters:
 * Returns: 
 * Effects: gets the rectangle illustrating a drop
 *************************************************************************/
QCanvasRectangle *
QueryArc::getDropRectangle()
{
  return m_dropRect;
} 
