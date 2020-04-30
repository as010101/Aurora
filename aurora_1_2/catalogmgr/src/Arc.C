#include "Arc.H"
#include <sstream>
#include <stdio.h>

Arc::Arc(int id, float rate, CompositeType* schema, int sourceId, int sourcePortId, int destinationId, int destinationPortId, bool cfFlag):Stream(id, rate, schema)
{
  m_sourceId = sourceId;
  m_sourcePortId = sourcePortId;
  m_destinationId = destinationId;
  m_destinationPortId = destinationPortId;
  m_connectionPoint = cfFlag;
  m_mutex = new pthread_mutex_t;
  m_inputArc = false;
  m_outputArc = false;
}

Arc::~Arc()
{
}

int Arc::getSourceId()
{
	return m_sourceId;
}

int Arc::getSourcePortId()
{
	return m_sourcePortId;
}

void Arc::setSourceId(int sourceId)
{
	m_sourceId = sourceId;
}

void Arc::setSourcePortId(int sourcePortId)
{
	m_sourcePortId = sourcePortId;
}

int Arc::getDestinationId()
{
	return m_destinationId;
}

int Arc::getDestinationPortId()
{
	return m_destinationPortId;
}

bool Arc::isConnectionPoint()
{
	return m_connectionPoint;
}

void Arc::lockArc()
{
  pthread_mutex_lock(m_mutex);
}

void Arc::unlockArc()
{
  pthread_mutex_unlock(m_mutex);
}

string Arc::toString()
{
  ostringstream s;
  s << "Arc (" << Stream::toString() << ", source node id: " << m_sourceId<< ", source node port id: " << m_sourcePortId;
  s << ", target node id: " << m_destinationId << ", target node port index: " << m_destinationPortId;
  s << ", connection point: " << m_connectionPoint ;
  s << ")" << '\0';
  return s.str();
}

