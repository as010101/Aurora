// CompositeType.cpp

/////////////////////////////////////////////////////////////////////////////
// Author: Jeong-hyon Hwang(jhhwang@cs.brown.edu)
// Version 1.0 - 2002/07/15
/////////////////////////////////////////////////////////////////////////////

#include "CompositeType.H"
#include <strstream.h>

Attribute::Attribute(string fieldName, int fieldType, int size)
{
  m_fieldName = fieldName;
  m_fieldType = fieldType;
  m_size = size;
}

string Attribute::toString()
{
    strstream s;
    s << m_fieldName << ":" << m_fieldType << ":" << m_size << '\0';
    return s.str();
}

CompositeType::CompositeType(string name)
{
  m_name = name;
}

string CompositeType::getName()
{
  return m_name;
}

int CompositeType::getNumberOfAttributes()
{
  return m_attributes.size();
}

Attribute& CompositeType::getAttribute(int index)
{
  return *(m_attributes[index]);
}

void CompositeType::addAttribute(Attribute* attribute)
{
  m_attributes.push_back(attribute);
}

string CompositeType::toString()
{
    strstream s;
    s << m_name << " = {";
    bool first = true;
    for (register int i = 0; i < m_attributes.size(); i++)
    {
        if (first)
            first = false;
        else
            s << ", ";
        Attribute* a = m_attributes[i];
        s << a->toString();
    }
    s << "}" << '\0';
    return s.str();
}


