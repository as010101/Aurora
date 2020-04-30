#include <strstream.h>

#include "TypeFieldRecord.H"
#include "DataOutputStream.H"
#include "DataInputStream.H"

const char* TypeFieldRecord::databaseFileName = "TypeField.db";

TypeFieldRecord::TypeFieldRecord()
{
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

TypeFieldRecord::TypeFieldRecord(int typeId, int attributeIndex, string attributeName, int attributeTypeId, int attributeSize, int offset)
{
  set(typeId, attributeIndex, attributeName, attributeTypeId, attributeSize, offset);
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

void TypeFieldRecord::set(int typeId, int attributeIndex, string attributeName, int attributeTypeId, int attributeSize, int offset)
{
  strstreambuf buf;
  DataOutputStream o(&buf);
  m_typeId = typeId;
  m_attributeIndex = attributeIndex;
  m_attributeName = attributeName;
  m_attributeTypeId = attributeTypeId;
  m_attributeSize = attributeSize;
  m_offset = offset;
  o.writeInt(m_typeId);
  o.writeInt(m_attributeIndex);
  o.writeUTF(m_attributeName);
  o.writeInt(m_attributeTypeId);
  o.writeInt(m_attributeSize);
  o.writeInt(m_offset);
  char* buffer = new char[o.size()];
  memcpy(buffer, buf.str(), o.size());
  set_size(o.size());
  if (get_data() != NULL) free(get_data());  // set_data() doesn't seem to dispose pre-assigned memory space.
  set_data(buffer);
}

void TypeFieldRecord::parse()
{
  strstreambuf buf((char*)get_data(), get_size());
  DataInputStream i(&buf);
  m_typeId = i.readInt();
  m_attributeIndex = i.readInt();
  m_attributeName = i.readUTF();
  m_attributeTypeId = i.readInt();
  m_attributeSize = i.readInt();
  m_offset = i.readInt();
}

string TypeFieldRecord::toString()
{
  strstream s;
  s << "Type Field (" << m_typeId << ", " << m_attributeIndex << ", ";
  s << m_attributeName << ", " << m_attributeTypeId << ", " << m_attributeSize << ", " << m_offset << ")" << '\0';
  return s.str();
}

int TypeFieldRecord::getTypeId()
{
  return m_typeId;
}

int TypeFieldRecord::getAttributeIndex()
{
  return m_attributeIndex;
}

string TypeFieldRecord::getAttributeName()
{
  return m_attributeName;
}

int TypeFieldRecord::getAttributeTypeId()
{
  return m_attributeTypeId;
}

int TypeFieldRecord::getAttributeSize()
{
  return m_attributeSize;
}
int TypeFieldRecord::getOffset()
{
  return m_offset;
}
