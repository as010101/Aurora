#include "UnionBox.H"
#include <sstream>
#include "BoxRecord.H"
#include <vector>


UnionBox::UnionBox(int id, float cost, float sel, int x, int y, int width, int height):
Box(id, cost, sel, x, y, width, height)
{
	assert( cost >= 0.0 );
}

UnionBox::~UnionBox()
{
}


vector<Arc*>& UnionBox::getInputArcs()
{
	return m_inputArcs;
}

void UnionBox::addInputArc(Arc* arc)
{
	m_inputArcs.push_back(arc);
}

int UnionBox::getBoxType()
{
  return UNION;
}

string UnionBox::toString()
{
  ostringstream s;
  s << "UnionBox (" << Box::toString() << ", input arcs: ";
  for (int i = 0; i < m_inputArcs.size(); i++)
  {
    if (i == 0)
      s << m_inputArcs[i]->toString();
    else
      s << ", " << m_inputArcs[i]->toString();
  }
  s << ")" << '\0';
  return s.str();
}

