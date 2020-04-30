/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: QueryBox.C
 *    DATE: Fri Apr 18 23:45:14 2003
 *************************************************************************/
#include <QueryBox.H>
#include <Box.H>
#include <qrect.h>


/*************************************************************************
 * Function Name: QueryBox::QueryBox
 * Parameters: QWidget * parent, const char *name
 * Effects: 
 *************************************************************************/

QueryBox::QueryBox(Box * box)
{
  m_box = box; 
  m_x = box->getX();
  m_y = box->getY();
  m_width = box->getWidth();
  m_height = box->getHeight();
  m_rect = new QRect(m_x, m_y, m_width, m_height);
}


/*************************************************************************
 * Function Name: QueryBox::~QueryBox
 * Parameters: 
 * Effects: 
 *************************************************************************/

QueryBox::~QueryBox()
{
  delete m_rect;
  for (PortMapIter iter = m_inports.begin(); iter!=m_inports.end(); iter++) { 
    delete iter->second;
  }
  m_inports.clear();
  for (PortMapIter iter = m_outports.begin(); iter!=m_outports.end(); iter++) { 
    delete iter->second;
  }
  m_outports.clear();
}

/*************************************************************************
 * Function Name: QueryBox::getBox
 * Parameters: none
 * Effects: returns rectangle of box
 *************************************************************************/
QRect* 
QueryBox::getBox()
{
  return m_rect;
}

/*************************************************************************
 * Function Name: QueryBox::getInputs
 * Parameters: none
 * Effects: returns rectangle array of inputs
 *************************************************************************/
PortMap&
QueryBox::getInputs()
{
  return m_inports;
}

/*************************************************************************
 * Function Name: QueryBox::getOutputs
 * Parameters: none
 * Effects: returns rectangle array of outputs
 *************************************************************************/
PortMap&
QueryBox::getOutputs()
{
  return m_outports;
}

/*************************************************************************
 * Function Name: QueryBox::getInput
 * Parameters: none
 * Effects: returns QRect of specified in port
 *************************************************************************/
QRect *
QueryBox::getInput(int id)
{
  return m_inports[id];
}

/*************************************************************************
 * Function Name: QueryBox::getOutput
 * Parameters: none
 * Effects: returns QRect of specified out port
 *************************************************************************/
QRect *
QueryBox::getOutput(int id)
{
  return m_outports[id];
}


/*************************************************************************
 * Function Name: QueryBox::addInput
 * Parameters: none
 * Effects: returns QRect of specified in port
 *************************************************************************/
void
QueryBox::addInput(int id)
{
  if (m_inports.find(id) == m_inports.end()) {
    int i = 0;
    for (PortMapIter iter = m_inports.begin(); iter!=m_inports.end(); iter++) { 
      iter->second->setY(m_y + (int)( (double)(i+0.5) / (double)(m_inports.size() + 1) * m_height ));
      iter->second->setHeight(3);
      i++;
    }
    QRect * rect = new QRect(m_x - 2, 
			     m_y + (int)( (double)(i+0.5) / (double)(m_inports.size() + 1) * m_height), 
			     5, 3);
    m_inports[id] = rect;
  }
}

/*************************************************************************
 * Function Name: QueryBox::addOutput
 * Parameters: none
 * Effects: returns QRect of specified out port
 *************************************************************************/
void
QueryBox::addOutput(int id)
{
  if (m_outports.find(id) == m_outports.end()) {
    int i = 0;
    for (PortMapIter iter = m_outports.begin(); iter!=m_outports.end(); iter++) { 
      iter->second->setY(m_y + (int)( (double)(i+0.5) / (double)(m_outports.size() + 1) * m_height ));
      iter->second->setHeight(3);
      i++;
    }
    QRect * rect = new QRect(m_x + m_width -2, 
			     m_y + (int)( (double)(i+0.5) / (double)(m_outports.size() + 1) * m_height), 
			     5, 3);
    m_outports[id] = rect;
  }
}

/*************************************************************************
 * Function Name: QueryBox::getBoxID
 * Parameters: none
 * Effects: returns id of box
 *************************************************************************/
int 
QueryBox::getBoxID()
{
  return m_box->getId();
}
