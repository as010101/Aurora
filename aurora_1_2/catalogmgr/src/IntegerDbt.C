#include <strstream.h>

#include "IntegerDbt.H"
#include "DataOutputStream.H"
#include "DataInputStream.H"

IntegerDbt::IntegerDbt()
{
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

IntegerDbt::IntegerDbt(int value)
{
  setInteger(value);
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

void IntegerDbt::setInteger(int value)
{
  strstreambuf buf;
  DataOutputStream o(&buf);
  o.writeInt(value);
  char* buffer = new char[o.size()];
  memcpy(buffer, buf.str(), o.size());
  set_size(o.size());
  if (get_data() != NULL) free(get_data());  // set_data() doesn't seem to dispose pre-assigned memory space.
  set_data(buffer);
}

int IntegerDbt::getInteger()
{
  strstreambuf buf((char*)get_data(), get_size());
  DataInputStream i(&buf);
  return i.readInt();
}

string IntegerDbt::toString()
{
  strstream s;
  s << "IntegerDbt (" << getInteger() << ")" << '\0'; 
  return s.str();
}
