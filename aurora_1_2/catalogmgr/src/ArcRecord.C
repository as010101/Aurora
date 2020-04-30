#include <strstream>

#include "ArcRecord.H"
#include "DataOutputStream.H"
#include "DataInputStream.H"

using namespace std;

const char* ArcRecord::databaseFileName = "ArcTable.db";

ArcRecord::ArcRecord()
{
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

ArcRecord::ArcRecord( int id, int sourceNodeId, int sourcePortIndex,
		      int targetNodeId, int targetPortIndex )
{
  set( id, sourceNodeId, sourcePortIndex, targetNodeId, targetPortIndex );

  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

void ArcRecord::set( int id, int sourceNodeId, int sourcePortIndex,
		     int targetNodeId, int targetPortIndex )
{
  strstreambuf buf;
  DataOutputStream o(&buf);

  m_id = id;
  m_rate = -2.0;   // This is meaningless, I've been told.
  m_typeId = 0;    // I believe this is composite type... this needs to change.
  m_sourceNodeId = sourceNodeId;
  m_targetNodeId = targetNodeId;
  m_sourcePortIndex = sourcePortIndex;
  m_targetPortIndex = targetPortIndex;
  m_cpFlag = -1;   //ditto.
  m_parentId = -1;

  o.writeInt(m_id);
  o.writeFloat(m_rate);
  o.writeInt(m_typeId);
  o.writeInt(m_sourceNodeId);
  o.writeInt(m_targetNodeId);
  o.writeInt(m_sourcePortIndex);
  o.writeInt(m_targetPortIndex);
  o.writeInt(m_cpFlag);
  o.writeInt(m_parentId);

  char* buffer = new char[ o.size() ];
  memcpy(buffer, buf.str(), o.size());
  set_size(o.size());
  if (get_data() != NULL) free(get_data());  
  // set_data() doesn't seem to dispose pre-assigned memory space.
  set_data(buffer);
}

void ArcRecord::parse()
{
  strstreambuf buf((char*)get_data(), get_size());
  DataInputStream i(&buf);
  readFields(i);
}

void ArcRecord::readFields(DataInputStream& inputStream)
{
  m_id = inputStream.readInt();
  m_rate = inputStream.readFloat();
  m_typeId = inputStream.readInt();
  m_sourceNodeId = inputStream.readInt();
  m_targetNodeId = inputStream.readInt();
  m_sourcePortIndex = inputStream.readInt();
  m_targetPortIndex = inputStream.readInt();
  m_cpFlag = inputStream.readInt();
  m_parentId = inputStream.readInt();
}

string ArcRecord::toString()
{
  strstream s;
  s << "Arc (" << m_typeId << ", " << m_sourceNodeId << ", " << m_sourcePortIndex << ", ";
  s << m_targetNodeId << ", " << m_targetPortIndex << ")" << '\0';
  return s.str();
}

int ArcRecord::getId()
{
  return m_id;
}

float ArcRecord::getRate()
{
  return m_rate;
}

int ArcRecord::getTypeId()
{
  return m_typeId;
}

int ArcRecord::getSourceNodeId()
{
  return m_sourceNodeId;
}

int ArcRecord::getTargetNodeId()
{
  return m_targetNodeId;
}

int ArcRecord::getSourcePortIndex()
{
  return m_sourcePortIndex;
}

int ArcRecord::getTargetPortIndex()
{
  return m_targetPortIndex;
}

int ArcRecord::getCpFlag()
{
  return m_cpFlag;
}

int ArcRecord::getParentId()
{
  return m_parentId;
}


