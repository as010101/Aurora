#include <strstream.h>

#include "IntegerArrayDbt.H"
#include "DataOutputStream.H"
#include "DataInputStream.H"

IntegerArrayDbt::IntegerArrayDbt(int size)
{
  m_size = size;
  m_integers = NULL;
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

IntegerArrayDbt::IntegerArrayDbt(int* values, int size)
{
  setInteger(values, size);
  m_integers = NULL;
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

IntegerArrayDbt::~IntegerArrayDbt()
{
  delete[] m_integers;
}

void IntegerArrayDbt::setInteger(int* values, int size)
{
  strstreambuf buf;
  DataOutputStream o(&buf);
  m_size = size;
  for (register int i = 0; i < m_size; i++)
    o.writeInt(values[i]);
  char* buffer = new char[o.size()];
  memcpy(buffer, buf.str(), o.size());
  set_size(o.size());

  if (get_data() != NULL) 
    delete[] reinterpret_cast<char*>(get_data());  // set_data() doesn't seem to dispose pre-assigned memory space.

  set_data(buffer);
}

int* IntegerArrayDbt::getIntegers()
{
  delete[] m_integers;

  if (m_size > 0)
  {
    m_integers = new int[m_size];
    strstreambuf buf((char*)get_data(), get_size());
    DataInputStream i(&buf);
    for (int ii = 0; ii < m_size; ii++)
      m_integers[ii] = i.readInt();
  }
  else
    m_integers = NULL;
  return m_integers;
}

int IntegerArrayDbt::getSize()
{
  return m_size;
}

string IntegerArrayDbt::toString()
{
  strstream s;
  int* integers = getIntegers();
  s << "IntegerArrayDbt (";
  bool first = true;
  for (register int i = 0; i < m_size; i++)
  {
    if (first)
        first = false;
    else
        s << ", ";
    s << integers[i];
  }
  s << ")" << '\0';
  return s.str();
}
