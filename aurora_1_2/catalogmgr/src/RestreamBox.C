#include "RestreamBox.H"
#include <sstream>

RestreamBox::RestreamBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
Box(id, cost, sel, x, y, width, height)
{
  m_modifier = modifier;

  m_state = new AggregateState();
  m_state->group_hash = new GroupByHash();
}

RestreamBox::~RestreamBox()
{
}

const char* RestreamBox::getModifier()
{
  return m_modifier.c_str();
}

int RestreamBox::getBoxType()
{
  return RESTREAM;
}

string RestreamBox::toString()
{
  ostringstream s;
  s << "RestreamBox (" << Box::toString() << ", expression: " << m_modifier << ")" << '\0';
  return s.str();
}

AggregateState* RestreamBox::getState()
{
  return m_state;
}

