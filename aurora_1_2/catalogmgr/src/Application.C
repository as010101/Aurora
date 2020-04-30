#include "Application.H"
#include <sstream>
#include <stdio.h>
#include <string>

Application::Application(int id, string modifier)
{
  string st( modifier );
  m_modifier = st;
  
  m_id = id;
  m_incomingArcId = -1;
  int pos = modifier.find(':'); 
  if (pos >= 0)
  {
  string s = modifier.substr(0, pos);
  sscanf(s.c_str(), "%d", &m_incomingArcId);
  modifier = modifier.substr(pos+1, modifier.length()-pos);

  pos = modifier.find(':');
  s = modifier.substr(0, pos);
  modifier = modifier.substr(pos+1, modifier.length()-pos);
  int numberOfQoses = 0;
  sscanf(s.c_str(), "%d", &numberOfQoses);
  for (register int i = 0; i < numberOfQoses; i++)
  {
    pos = modifier.find(':');
    if (pos < 0) 
    {
      pos = modifier.length();
      s = modifier.substr(0, pos);
    }
    else
    {
      s = modifier.substr(0, pos);
      modifier = modifier.substr(pos+1, modifier.length()-pos);
     }
    m_qosVector.push_back(new QoS(s));
  }
  }
  sharing = 0.0;
}

Application::~Application() {}

int Application::getId()
{
	return m_id;
}

int Application::getIncomingArcId() 
{
	return m_incomingArcId;
}

void Application::setIncomingArcId(int id)
{
	m_incomingArcId = id;
}

QoS *Application::getQoS()
{
  return m_qosVector[ 0 ];
}

// This function is added by tatbul@cs.brown.edu
// type=0 => latency-based QoS
// type=1 => loss-tolerance QoS
// type=2 => value-based QoS
// Note: assumption here is that there can be at most one graph of a
// particular type. This assumption has to be removed when multiple
// type 2 graphs are used for multiple attributes.
QoS *Application::getQoS(int type)
{
	return m_qosVector[type];
}

char *Application::getModifier()
{
  char *mod = (char*)malloc( m_modifier.size() + 1);
  memcpy( mod, m_modifier.c_str(), m_modifier.size() );
  *(mod+m_modifier.size()) = '\0';
  //printf(" DEBUG: APpl: %s\n", mod );
  return mod;
}

string Application::toString()
{
  ostringstream s;
  s << "Application (id: " << m_id << " arc in " << getIncomingArcId(); 
  if (m_qosVector.size() > 0)
  {
    s << ", QOS's: ";
    for (register int i = 0; i < m_qosVector.size(); i++)
    {
      if (i != 0)
        s << ", ";
      s << m_qosVector[i]->toString();
    }

  }
  s << ")" << '\0';

  return s.str();
}
