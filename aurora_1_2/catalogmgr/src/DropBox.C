#include "DropBox.H"
#include <sstream>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"


DropBox::DropBox(int id, float cost, float sel, float dropRate, int x, int y, int width, int height):
Box(id, cost, sel, x, y, width, height)
{
	m_dropRate = dropRate;
}

DropBox::~DropBox()
{
}

float DropBox::getDropRate()
{
	return m_dropRate;
 }

int DropBox::getBoxType()
{
  return DROP;
}

string DropBox::toString()
{
  ostringstream s;
  s << "DropBox (" << Box::toString() << ", drop rate: " << m_dropRate << ")" << '\0';
  return s.str();
}

