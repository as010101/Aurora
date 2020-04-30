// Alexander Rasin and Jeong-Hyon Hwang

#include <strstream.h>

#include "BoxRecord.H"
#include "DataOutputStream.H"
#include "DataInputStream.H"

const char* BoxRecord::databaseFileName = "BoxTable.db";

BoxRecord::BoxRecord()
{
  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

BoxRecord::BoxRecord( int id, float cost, float select, const char *mod, int type )
{
  //  printf(" constructor box record called\n");
  set( id, cost, select, mod, type );

  set_flags(DB_DBT_MALLOC); // tell Db to allocate on retrieval
}

void BoxRecord::set( int id, float cost, float select, const char *mod, int type )
{
  strstreambuf buf;
  DataOutputStream o(&buf);

  m_boxId = id;
  m_boxType = type;

  // that's probably not the most elegant way to do this. variable size ints?
  const char *format = "Box #%d";
  char *res = (char *) malloc( 10 );
  sprintf( res, format, id );
  m_label = res;

  m_description = "desc";
  string s( mod );
  m_modifier = s;
  m_parentId = -1;
  m_cost = cost;
  m_selectivity = select;
  m_x = 0;
  m_y = 0;
  m_width = 90;
  m_height = 30;

  o.writeInt( m_boxId );
  o.writeInt( m_boxType );
  o.writeUTF( m_label );
  o.writeUTF( m_description );
  o.writeUTF( m_modifier );
  o.writeInt( m_parentId );
  o.writeFloat( m_cost );
  o.writeFloat( m_selectivity );
  o.writeInt( 1 );  // some strange parameters *always* set to 1 in GUI?
  o.writeInt( 1 );
  o.writeInt( m_x );
  o.writeInt( m_y );
  o.writeInt( m_width );
  o.writeInt( m_height );

  char* buffer = new char[ o.size() ];
  memcpy(buffer, buf.str(), o.size());
  set_size(o.size());
  if (get_data() != NULL) free(get_data());  
  // set_data() doesn't seem to dispose pre-assigned memory space.
  set_data(buffer);
}

void BoxRecord::parse()
{
  strstreambuf buf((char*)get_data(), get_size());
  DataInputStream i(&buf);
  readFields(i);
}

void BoxRecord::readFields(DataInputStream& inputStream)
{
  m_boxId = inputStream.readInt();
  m_boxType = inputStream.readInt();
  m_label = inputStream.readUTF();
  m_description = inputStream.readUTF();
  m_modifier = inputStream.readUTF();
  m_parentId = inputStream.readInt();
  m_cost = inputStream.readFloat();
  m_selectivity = inputStream.readFloat();
  inputStream.readInt(); //some strange GUI parameters
  inputStream.readInt();
  m_x = inputStream.readInt();
  m_y = inputStream.readInt();
  m_width = inputStream.readInt();
  m_height = inputStream.readInt();
}


string BoxRecord::toString()
{
  strstream s;
  s << "Box (label: " <<  m_label << ", id: " << m_boxId;
  s << ", type: " << m_boxType << ", parent:" << m_parentId << ", modifier: " << m_modifier << ")" << '\0';
return s.str();
}

int BoxRecord::getBoxId()
{
  return m_boxId;
}

int BoxRecord::getBoxType()
{
  return m_boxType;
}

string BoxRecord::getLabel()
{
  return m_label;
}

string BoxRecord::getDescription()
{
  return m_modifier;
}

string BoxRecord::getModifier()
{
  return m_modifier;
}

int BoxRecord::getParentId()
{
  return m_parentId;
}

float BoxRecord::getCost()
{
  return m_cost;
}

float BoxRecord::getSelectivity()
{
  return m_selectivity;
}

int BoxRecord::getX()
{
  return m_x;
}

int BoxRecord::getY()
{
  return m_y;
}

int BoxRecord::getWidth()
{
  return m_width;
}

int BoxRecord::getHeight()
{
  return m_height;
}
