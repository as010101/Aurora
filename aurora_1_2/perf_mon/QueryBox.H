/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: QueryBox.H
 *	Asgn: perf_mon
 *	Date: Fri Apr 18 23:34:22 EDT 2003
 **************************************************************/

#ifndef QueryBox_Header
#define QueryBox_Header

#include <map>

class QRect;
class Box; 

typedef std::map<int, QRect*> PortMap;
typedef PortMap::iterator PortMapIter;

class QueryBox{

public:

  QueryBox(Box * box);

  virtual ~QueryBox();

  QRect* getBox();

  PortMap& getInputs();
  
  PortMap& getOutputs();

  QRect* getInput(int id);

  QRect* getOutput(int id);

  void addInput(int id);

  void addOutput(int id);

  int getBoxID();

protected:

private:
  PortMap m_inports, m_outports;
  QRect * m_rect;
  Box * m_box;
  int m_x, m_y, m_width, m_height;
};

#endif
