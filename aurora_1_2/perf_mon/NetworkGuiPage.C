/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: NetworkGuiPage.C
 *    DATE: Fri Apr 18 20:16:15 2003
 *************************************************************************/
#include <NetworkGuiPage.H> 

#include <CatalogManager.H>
#include <QueryNetwork.H>

#include <Box.H> 
#include <Arc.H>

#include <QueryBox.H>
#include <QueryArc.H>

#include <GUIConstants.H>

#include <qpainter.h>
#include <qcolor.h>

/*************************************************************************
 * Function Name: NetworkGuiPage::NetworkGuiPage
 * Parameters: QWidget * parent, const char *name
 * Effects: 
 *************************************************************************/
  
NetworkGuiPage::NetworkGuiPage(std::string catalog, QWidget * parent, const char *name):QCanvasView(parent, name)
{
  setMinimumSize(GUI_WIDTH, GUI_HEIGHT);
  setPaletteBackgroundColor(white);

  this->setFrameStyle(QFrame::NoFrame);
  m_canvas = new QCanvas(GUI_WIDTH, GUI_HEIGHT);
  this->setCanvas(m_canvas);
  
  m_catalogdir = catalog;
  m_manager = new CatalogManager(m_catalogdir);
  m_network = m_manager->load();
  
  //test code
  /*
    for (int i = 0; i < 7; i++) {
    AuroraBox * myBox = new AuroraBox(i, 0, 0, 50 + 120 * i, 100 + 15 * i, 50, 30);
    m_network->addBox(myBox);
    }
    for (int i = 0; i < 6; i++) {
    Arc * myArc = new Arc(i, 0, 0, i, 0, i+1, 0, false);
    m_network->addArc(myArc);
    }
    for (int i = 0; i < 6; i++) {
    Arc * myArc = new Arc(i+6, 0, 0, i, 1, i+1, 1, false);
    m_network->addArc(myArc);
    }
  */
  //end test code
  
  
  redrawNetwork();
}


/*************************************************************************
 * Function Name: NetworkGuiPage::~NetworkGuiPage
 * Parameters: 
 * Effects: 
 *************************************************************************/

NetworkGuiPage::~NetworkGuiPage()
{
  for (QueryBoxMapIter iter = m_boxes.begin(); iter!=m_boxes.end(); iter++) { 
    delete iter->second;
  }
  m_boxes.clear();
  for (QueryArcMapIter iter = m_arcs.begin(); iter!=m_arcs.end(); iter++) { 
    delete iter->second;
  }
  m_arcs.clear();
  m_inputs.clear();
  m_outputs.clear();
  delete m_manager;
}

/*************************************************************************
 * Function Name: NetworkGuiPage::updateNetwork
 * Parameters: 
 * Effects: 
 *************************************************************************/
void 
NetworkGuiPage::updateNetwork(std::map<int, int> arcTuples, std::map<int, std::string> arcDrops) 
{
  int tupleSum = 0;
  //first sum the tuples on the network in order to find the percentage of tuples on each arc
  for (QueryArcMapIter iter = m_arcs.begin(); iter!=m_arcs.end(); iter++) { 
    tupleSum+= arcTuples[iter->second->getID()];
    //get the text object in each arc and set it appropriately to the number of tuples
    QCanvasText * text = iter->second->getNumTuples();
    QString num;
    num.setNum(arcTuples[iter->second->getID()], 10);
    text->setText(num);
    QCanvasText * droptext = iter->second->getDrop();
    if (arcDrops.find(iter->second->getID())!= arcDrops.end()) {
      droptext->setText(arcDrops[iter->second->getID()]);
      droptext->show();
      iter->second->getDropRectangle()->show();
    }
    else {
      droptext->setText("");
      droptext->hide();
      iter->second->getDropRectangle()->hide();
    }
  }
  if (tupleSum <=0)
    tupleSum = 1;
  for (QueryArcMapIter iter = m_arcs.begin(); iter!=m_arcs.end(); iter++) {
    //calculate percentage of network tuples on this arc
    double percentage = ((double)arcTuples[iter->second->getID()]/(double)tupleSum);
    //five degrees of thickness, based on percentage of network tuples
    int thickness = (int)(percentage * 5.0);
    QColor col;
    //percentage of blue and red based on thickness
    col.setRgb( (int)(((double)thickness/5) * 255), 0, (int)((1.0 - (double)thickness/5) * 255));
    QCanvasLine * line = iter->second->getLine();
    line->setPen(QPen(col, thickness + 2));
  }
  m_canvas->update();
}

/*************************************************************************
 * Function Name: NetworkGuiPage::redrawNetwork
 * Parameters: 
 * Effects: 
 *************************************************************************/
void 
NetworkGuiPage::redrawNetwork() 
{
  //first delete all old arcs and boxes
  for (QueryArcMapIter iter = m_arcs.begin(); iter!=m_arcs.end(); iter++) { 
    delete iter->second;
  }
  m_arcs.clear();
  for (QueryBoxMapIter iter = m_boxes.begin(); iter!=m_boxes.end(); iter++) { 
    delete iter->second;
  }
  m_boxes.clear();
  m_inputs.clear();
  m_outputs.clear();

  //now we start to set up new arcs/boxes from the network
  BoxMap boxes = m_network->getBoxes();
  ArcMap arcs = m_network->getArcs(); 
  StreamMap inputs = m_network->getInputStreams();
  ApplicationMap outputs = m_network->getApplications();
  int i;

  for (BoxMapIter iter = boxes.begin(); iter!=boxes.end(); iter++) { //set up boxes
    m_boxes[iter->second->getId()] = new QueryBox(iter->second);
    QCanvasRectangle * rect = new QCanvasRectangle(*(m_boxes[iter->second->getId()])->getBox(), m_canvas);
    rect->show(); //display boxes
  }

  i = 0;
  for (ApplicationMapIter iter = outputs.begin(); iter!=outputs.end(); iter++) { //set up outputs
    int height = (int)(((i+0.5)/outputs.size()) * GUI_HEIGHT);
    m_outputs[iter->second->getId()] = height+5;
    QCanvasRectangle * rect = new QCanvasRectangle(GUI_WIDTH-30, height, 10, 10, m_canvas);
    rect->show();
    i++;
  }

  i = 0;
  for (StreamMapIter iter = inputs.begin(); iter!=inputs.end(); iter++) { //set up outputs
    int height = (int)(((i+0.5)/inputs.size()) * GUI_HEIGHT);
    m_inputs[iter->second->getId()] = height+5;
    QCanvasRectangle * rect = new QCanvasRectangle(0, height, 10, 10, m_canvas);
    rect->show();
    i++;
  }

  
  for (ArcMapIter iter = arcs.begin(); iter!=arcs.end(); iter++) { //set up arcs and box ports
    Arc * curArc = iter->second;
    m_arcs[curArc->getId()] = new QueryArc(curArc);
    //add input and output ports to boxes
    int sourceID = curArc->getSourceId();
    int destID = curArc->getDestinationId();
    if (!curArc->isInputArc()) //make sure we only add to existing boxes
      m_boxes[sourceID]->addOutput(curArc->getSourcePortId());
    if (!curArc->isOutputArc())  //make sure we only add to existing boxes
      m_boxes[destID]->addInput(curArc->getDestinationPortId());
  }
  
  for (ArcMapIter iter = arcs.begin(); iter!=arcs.end(); iter++) { 
    //now that box ports have been set up, we can draw the actual lines
    Arc * curArc = iter->second;
    int sourceID = curArc->getSourceId();
    int destID = curArc->getDestinationId();
    QCanvasLine * line = new QCanvasLine(m_canvas); //add the line to the arc
    //use ports to draw lines
    if (!curArc->isInputArc() && !curArc->isOutputArc())
      line->setPoints(m_boxes[sourceID]->getOutput(curArc->getSourcePortId())->center().x(), 
		      m_boxes[sourceID]->getOutput(curArc->getSourcePortId())->center().y(), 
		      m_boxes[destID]->getInput(curArc->getDestinationPortId())->center().x(),
		      m_boxes[destID]->getInput(curArc->getDestinationPortId())->center().y());
    else if (curArc->isInputArc() && !curArc->isOutputArc())
      line->setPoints(10, m_inputs[sourceID],
		      m_boxes[destID]->getInput(curArc->getDestinationPortId())->center().x(),
		      m_boxes[destID]->getInput(curArc->getDestinationPortId())->center().y());
    else if (!curArc->isInputArc() && curArc->isOutputArc())
      line->setPoints(m_boxes[sourceID]->getOutput(curArc->getSourcePortId())->center().x(), 
		      m_boxes[sourceID]->getOutput(curArc->getSourcePortId())->center().y(), 
		      GUI_WIDTH-30, m_outputs[destID]);
    else
      line->setPoints(10, m_inputs[sourceID], GUI_WIDTH-30, m_outputs[destID]);
    line->setPen(QPen(blue, 2));
    m_arcs[curArc->getId()]->setLine(line);
    QCanvasText * text = new QCanvasText("0", m_canvas);
    QFont font;
    font.setPointSize(10);
    text->setFont(font);
    text->setColor(darkGreen);
    text->setX((line->startPoint().x() + line->endPoint().x())/2.0 - 2);
    text->setY((line->startPoint().y() + line->endPoint().y())/2.0 - 15);
    text->setZ(4);
    text->show();
    QCanvasText * dropText = new QCanvasText("", m_canvas);
    QFont font2;
    font2.setPointSize(8);
    dropText->setFont(font2);
    dropText->setColor(darkGray);
    dropText->setX((line->startPoint().x() + line->endPoint().x())/2.0 - 12);
    dropText->setY((line->startPoint().y() + line->endPoint().y())/2.0 + 15);
    dropText->setZ(5);
    dropText->hide();
    QCanvasRectangle * dropRect = new QCanvasRectangle(m_canvas);
    dropRect->setX((line->startPoint().x() + line->endPoint().x())/2.0 - 10);
    dropRect->setY((line->startPoint().y() + line->endPoint().y())/2.0 - 3);
    dropRect->setSize(19, 13);
    dropRect->setZ(4);
    dropRect->setPen(darkGray);
    dropRect->setBrush(darkGray);
    dropRect->hide();
    m_arcs[curArc->getId()]->setNumTuples(text);
    m_arcs[curArc->getId()]->setDrop(dropText);
    m_arcs[curArc->getId()]->setDropRectangle(dropRect);
    line->show(); //show the lines
  }
  
  
  for (QueryBoxMapIter iter = m_boxes.begin(); iter!=m_boxes.end(); iter++) { //finally draw all ports
    PortMap inports = iter->second->getInputs();
    PortMap outports = iter->second->getOutputs();
    for (PortMapIter iter = inports.begin(); iter!=inports.end(); iter++) {
      QCanvasRectangle * rect = new QCanvasRectangle(*(iter->second), m_canvas);
      rect->setBrush(black);
      rect->setZ(1);
      rect->show();
    }
    for (PortMapIter iter = outports.begin(); iter!=outports.end(); iter++) {
      QCanvasRectangle * rect = new QCanvasRectangle(*(iter->second), m_canvas);
      rect->setBrush(black);
      rect->setZ(1);
      rect->show();
    }
  }
  m_canvas->update();
}


/*************************************************************************
 * Function Name: NetworkGuiPage::redrawNetwork
 * Parameters: QResizeEvent* e
 * Effects: 
 *************************************************************************/
void
NetworkGuiPage::resizeEvent(QResizeEvent* e)
{
  //forces the canvas to also change size
  m_canvas->resize(width(), height());
}
