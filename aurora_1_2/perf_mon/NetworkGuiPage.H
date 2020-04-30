/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: NetworkGuiPage.H
 *	Asgn: perf_mon
 *	Date: Fri Apr 18 20:14:22 EDT 2003
 **************************************************************/

#ifndef NetworkGuiPage_Header
#define NetworkGuiPage_Header

#include <qcanvas.h>
#include <qevent.h>

#include <map>

class QueryBox;
class QueryArc;

typedef std::map<int, QueryBox*> QueryBoxMap;
typedef QueryBoxMap::iterator QueryBoxMapIter;

typedef std::map<int, QueryArc*> QueryArcMap;
typedef QueryArcMap::iterator QueryArcMapIter;

//didn't see a good reason to make a wrapper class around Applications and Sourcestream as I did for Boxes and Arcs
//so this map type is used to store the positions of Inputs and Outputs on the query network.
typedef std::map<int, int> QueryIOMap;
typedef QueryIOMap::iterator QueryIOMapIter;

class Box;

typedef Box AuroraBox;
//not sure exactly why I needed this typedef, I think
//there's a namespace bug with qt which wouldn't let
//me use Box so all Boxes in this class are now called
//AuroraBoxes

class CatalogManager;
class QueryNetwork;

class NetworkGuiPage: public QCanvasView {

public:

  NetworkGuiPage(std::string catalog, QWidget * parent, const char *name);

  virtual ~NetworkGuiPage();

  void updateNetwork(std::map<int, int> arcTuples, std::map<int, std::string> arcDrops);

  void redrawNetwork();

  void resizeEvent(QResizeEvent* e);

  //void paintEvent(QPaintEvent* event);

protected:

private:
  QueryIOMap m_inputs;
  QueryIOMap m_outputs;
  QueryBoxMap m_boxes;
  QueryArcMap m_arcs;
  QCanvas * m_canvas;
  CatalogManager * m_manager;
  QueryNetwork * m_network;
  std::string m_catalogdir;
};

#endif
