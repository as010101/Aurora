#include "DataInputStream.H"

DataInputStream::DataInputStream(streambuf* psb):istream(psb)
{}

bool DataInputStream::readBoolean()
{
  bool v;

  // This won't compile at my home. Need to look into it. - cjc January 25
  //  read(&v, 1);
  return v;
}

char DataInputStream::readChar()
{
  short i;
  char* p = (char*) &i;
  read(p+1, 1);
  read(p, 1);
  return (char)i;
}

short DataInputStream::readShort()
{
  short v;
  char* p = (char*) &v;
  read(p+1, 1);
  read(p, 1);
  return v;
}

int DataInputStream::readInt()
{
  int v;
  char* p = (char*) &v;
  read(p+3, 1);
  read(p+2, 1);
  read(p+1, 1);
  read(p, 1);
  return v;
}

float DataInputStream::readFloat()
{
  float v;
  char* p = (char*) &v;
  read(p+3, 1);
  read(p+2, 1);
  read(p+1, 1);
  read(p, 1);
  return v;
}

double DataInputStream::readDouble()
{
  double v;
  char* p = (char*) &v;
  read(p+7, 1);
  read(p+6, 1);
  read(p+5, 1);
  read(p+4, 1);
  read(p+3, 1);
  read(p+2, 1);
  read(p+1, 1);
  read(p, 1);
  return v;
}

string DataInputStream::readUTF()
{
  int size = readShort();
  char* buffer = new char[size+1];
  read(buffer, size);
  string s(buffer, size);
  return s;
}
