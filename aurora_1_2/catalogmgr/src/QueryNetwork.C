#include "QueryNetwork.H"
#include <sstream>
#include <set>

QueryNetwork::QueryNetwork()
{
}

QueryNetwork::~QueryNetwork()
{
}

int QueryNetwork::getNumberOfBoxes()
{
  return m_boxes.size();
}

BoxMap& QueryNetwork::getBoxes()
{
  return m_boxes;
}

Box *QueryNetwork::getBox(int id)
{
  if (m_boxes.find(id) != m_boxes.end())
    return(m_boxes[id]);
  else
    return NULL;
}

void QueryNetwork::addBox(Box *box)
{
  //  cout << box->toString();

  if (m_boxes.find(box->getId()) == m_boxes.end())
    m_boxes[box->getId()] = box;
}

int QueryNetwork::getNumberOfInputStreams()
{
  return m_streams.size();
}

StreamMap& QueryNetwork::getInputStreams()
{
  return m_streams;
}

SourceStream *QueryNetwork::getInputStream(int id)
{
  if (m_streams.find(id) != m_streams.end())
    return m_streams[id];
  else
    return NULL;
}

void QueryNetwork::addInputStream(SourceStream *sc)
{
  if (m_streams.find(sc->getId()) == m_streams.end())
      m_streams[sc->getId()] = sc;
}

int QueryNetwork::getNumberOfApplications()
{
  return m_applications.size();
}

ApplicationMap& QueryNetwork::getApplications()
{
  return m_applications;
}

Application *QueryNetwork::getApplication(int id)
{
  if (m_applications.find(id) != m_applications.end())
    return m_applications[id];
  else
    return NULL;
}

void QueryNetwork::addApplication(Application *app)
{
  if (m_applications.find(app->getId()) == m_applications.end())
      m_applications[app->getId()] = app;
}

int QueryNetwork::getNumberOfArcs()
{
  return m_arcs.size();
}

ArcMap& QueryNetwork::getArcs()
{
  return m_arcs;
}

Arc *QueryNetwork::getArc(int id)
{
  if (m_arcs.find(id) != m_arcs.end())
    return m_arcs[id];
  else
    return NULL;
}

void QueryNetwork::addArc(Arc *a)
{
  if (m_arcs.find(a->getId()) == m_arcs.end())
      m_arcs[a->getId()] = a;
}

string QueryNetwork::toString()
{
  ostringstream s;
  s << "QueryNetwork:" << endl << endl;

  s << "Boxes:" << endl;
  for (BoxMapIter biter = m_boxes.begin(); biter != m_boxes.end(); biter++)
    s <<  ((*biter).second)->toString() << endl;
  s << endl;

  //  cout << "QueryNetwork: Boxes are done\n";
  //	s << "Source Streams:" << endl;
  for (StreamMapIter siter = m_streams.begin(); siter != m_streams.end(); siter++)
      s << ((*siter).second)->toString() << endl;
  s << endl;

  //  cout << "QueryNetwork: Streams are done\n";

  s << "Applications:" << endl;
  for (ApplicationMapIter piter = m_applications.begin(); piter != m_applications.end(); piter++)
    s << ((*piter).second)->toString() << endl;
  s << endl;

  //  cout << "QueryNetwork: Applications are done\n";

  s << "Arcs:" << endl;
  for (ArcMapIter aiter = m_arcs.begin(); aiter != m_arcs.end(); aiter++)
    s << ((*aiter).second)->toString() << endl;

  //  cout << "QueryNetwork: ALL (Arcs) are done\n";

  s << '\0';
  return s.str();
}

int QueryNetwork::getMaxBoxId() {

	BoxMapIter iter = m_boxes.end();
    return((*(--iter)).first);
}

int QueryNetwork::getMaxArcId() {

	ArcMapIter iter = m_arcs.end();
    return((*(--iter)).first);
}

int QueryNetwork::getMaxStreamId() {

	StreamMapIter iter = m_streams.end();
    return((*(--iter)).first);
}

int QueryNetwork::getMaxAppId() {

	ApplicationMapIter iter = m_applications.end();
    return((*(--iter)).first);
}

int QueryNetwork::getDestinationBoxId(int arcId)
{
  Arc* arc = getArc(arcId);
  if (arc != NULL)
  	return arc->getDestinationId();
  else
  	return -1;
}


int QueryNetwork::getBoxOutputPortCount(int boxId)
{
	set<int> outputPorts;

	for (ArcMapIter pos = m_arcs.begin();
		 pos != m_arcs.end();
		 ++ pos)
		{
			Arc * pArc = pos->second;
			
			if (pArc->getSourceId() == boxId)
				{
					outputPorts.insert(pArc->getSourcePortId());
				}
		}
	
	return outputPorts.size();
}
