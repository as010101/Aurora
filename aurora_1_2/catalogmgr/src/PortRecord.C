#include <strstream.h>

#include "PortRecord.H"
#include "DataOutputStream.H"
#include "DataInputStream.H"

const char* PortRecord::databaseFileName = "PortTable.db";
int PortRecord::INPUTPORTTYPE = 0;
int PortRecord::OUTPUTPORTTYPE = 1;

PortRecord::PortRecord()
{
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

PortRecord::PortRecord( int boxId, int portIndex, int typeId, int portType )
{
  set( boxId, portIndex, typeId, portType );

  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

void PortRecord::set( int boxId, int portIndex, int typeId, int portType )
{
  strstreambuf buf;
  DataOutputStream o(&buf);

  m_boxId = boxId;
  m_portIndex = portIndex;
  m_typeId = typeId;
  m_portType = portType;

  o.writeInt( m_boxId );
  o.writeInt( m_portIndex );
  o.writeInt( m_typeId );
  o.writeInt( m_portType );

  char* buffer = new char[ o.size() ];
  memcpy(buffer, buf.str(), o.size());
  set_size(o.size());
  if (get_data() != NULL) free(get_data());  
  // set_data() doesn't seem to dispose pre-assigned memory space.
  set_data(buffer);
}

void PortRecord::parse()
{
  strstreambuf buf((char*)get_data(), get_size());
  DataInputStream i(&buf);
  readFields(i);
}

void PortRecord::readFields(DataInputStream& inputStream)
{
  m_boxId = inputStream.readInt();
  m_portIndex = inputStream.readInt();
  m_typeId  = inputStream.readInt();
  m_portType = inputStream.readInt();
}

string PortRecord::toString()
{
  strstream s;
  s << "Port (at box: " << m_boxId << ", index: " << m_portIndex << ", type " << m_typeId << ", type: " << m_portType << '\0';
  return s.str();
}

int PortRecord::getBoxId()
{
  return m_boxId;
}

int PortRecord::getTypeId()
{
  return m_typeId;
}

int PortRecord::getPortIndex()
{
  return m_portIndex;
}
int PortRecord::getPortType()
{
  return m_portType;
}


