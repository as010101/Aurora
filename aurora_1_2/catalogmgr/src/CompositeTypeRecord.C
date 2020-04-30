#include <strstream.h>

#include "CompositeTypeRecord.H"
#include "DataOutputStream.H"
#include "DataInputStream.H"

const char* CompositeTypeRecord::databaseFileName = "CompositeType.db";

CompositeTypeRecord::CompositeTypeRecord()
{
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

CompositeTypeRecord::CompositeTypeRecord(int typeId, string name, int numberOfFields)
{
  set(typeId, name, numberOfFields);
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

void CompositeTypeRecord::set(int typeId, string name, int numberOfFields)
{
  strstreambuf buf;
  DataOutputStream o(&buf);
  m_typeId = typeId;
  m_name = name;
  m_numberOfFields = numberOfFields;
  o.writeInt(m_typeId);
  o.writeUTF(m_name);
  o.writeInt(m_numberOfFields);
  o.writeInt( 0 ); // non inferred type. Inferred types are hidden in GUI.
  char* buffer = new char[o.size()];
  memcpy(buffer, buf.str(), o.size());
  set_size(o.size());
  if (get_data() != NULL) free(get_data());  // set_data() doesn't seem to dispose pre-assigned memory space.
  set_data(buffer);
}

void CompositeTypeRecord::parse()
{
  strstreambuf buf((char*)get_data(), get_size());
  DataInputStream i(&buf);
  m_typeId = i.readInt();
  m_name = i.readUTF();
  m_numberOfFields = i.readInt();

  i.readInt(); // GUI specific parameter, ignored.
}

string CompositeTypeRecord::toString()
{
  strstream s;
  s << "CompositeType (" << m_typeId << ", " << m_name << ", " << m_numberOfFields << ")";
  return s.str();
}

int CompositeTypeRecord::getTypeId()
{
  return m_typeId;
}

string CompositeTypeRecord::getName()
{
  return m_name;
}

int CompositeTypeRecord::getNumberOfFields()
{
  return m_numberOfFields;
}
