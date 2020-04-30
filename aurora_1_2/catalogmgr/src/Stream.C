#include "Stream.H"
#include <sstream>
#include <stdio.h>

Stream::Stream(int id, float rate, CompositeType* schema) 
{
  m_id = id;
  m_rate = rate;
  m_schema = schema;
}

Stream::~Stream()
{
}

int Stream::getId()
{
	return m_id;
}

float Stream::getRate()
{
	return m_rate;
}

void Stream::setRate(float rate)
{
	m_rate = rate;
}

CompositeType* Stream::getSchema()
{
	return m_schema;
}

string Stream::toString()
{
  ostringstream s;
  s << "id: " << m_id << ", rate: " << m_rate;
  s << ", schema: ";
  if (m_schema != NULL)
    s << m_schema->toString();
  else
    s << "null";
  s << '\0';
  return s.str();
}

