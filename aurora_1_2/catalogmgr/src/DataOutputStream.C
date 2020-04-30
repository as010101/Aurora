#include "DataOutputStream.H"

DataOutputStream::DataOutputStream(streambuf* psb):ostream(psb)
{}

void DataOutputStream::writeBoolean(bool v)
{
  // This won't compile at my home. Need to look into it. - cjc January 25
  //  write(&v, 1);
}

void DataOutputStream::writeChar(char v)
{
  short i = v;
  char* p = (char*) &i;
  write(p+1, 1);
  write(p, 1);
}

void DataOutputStream::writeShort(short v)
{
  char* p = (char*) &v;
  write(p+1, 1);
  write(p, 1);
}

void DataOutputStream::writeInt(int v)
{
  char* p = (char*) &v;
  write(p+3, 1);
  write(p+2, 1);
  write(p+1, 1);
  write(p, 1);
}

void DataOutputStream::writeFloat(float v)
{
  char* p = (char*) &v;
  write(p+3, 1);
  write(p+2, 1);
  write(p+1, 1);
  write(p, 1);
}

void DataOutputStream::writeDouble(double v)
{
  char* p = (char*) &v;
  write(p+7, 1);
  write(p+6, 1);
  write(p+5, 1);
  write(p+4, 1);
  write(p+3, 1);
  write(p+2, 1);
  write(p+1, 1);
  write(p, 1);
}

void DataOutputStream::writeUTF(string v)
{
  short int size = v.size();
  writeShort(size);
  write(v.c_str(), size);
}

long DataOutputStream::size()
{
  return tellp();
}
