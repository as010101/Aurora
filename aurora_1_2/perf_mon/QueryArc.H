/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: QueryArc.H
 *	Asgn: perf_mon
 *	Date: Sat Apr 19 01:02:14 EDT 2003
 **************************************************************/

#ifndef QueryArc_Header
#define QueryArc_Header

class QPoint;
class Arc;
class QCanvasLine;
class QCanvasText;
class QCanvasRectangle;

class QueryArc {

public:

  QueryArc(Arc * arc);

  virtual ~QueryArc();

  int getID();

  int getSourceBox();

  int getSourcePort();

  int getDestBox();

  int getDestPort();

  void setNumTuples(QCanvasText * text);

  QCanvasText * getNumTuples();

  void setLine(QCanvasLine* line);

  QCanvasLine * getLine();

  void setDrop(QCanvasText * dropText);

  QCanvasText * getDrop();

  void setDropRectangle(QCanvasRectangle * dropRect);

  QCanvasRectangle * getDropRectangle();

protected:

private:
  QCanvasText * m_text;
  QCanvasText * m_dropText; //text for drop predicate
  QCanvasRectangle * m_dropRect; //rectangle for drop predicate
  QCanvasLine * m_line;
  Arc * m_arc;
  //int m_thickness;
};

#endif
