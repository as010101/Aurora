#include "SourceStream.H"
#include "BoxRecord.H"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <stringutil.H>

SourceStream::SourceStream(int id, 
			   float rate, 
			   CompositeType* schema, 
			   bool periodicity, 
			   bool adjustability, 
			   int numberOfTuples) :
  Stream(id, rate, schema)
{
  m_periodicity = periodicity;
  m_adjustability = adjustability;
  m_numberOfTuples = numberOfTuples;

  /*
  const char *format = "%d:%f:%d:%d:%d:%d:";
  char *mod = ( char* )malloc( 200 );
  int n = sprintf( mod, format, 0, rate, periodicity, adjustability, 1, 
	   numberOfTuples );
  int count = 6;
  for ( int i = 0; i < 200 && count != 0; i++ )
    {
      if ( *(mod+i) == ':' ) count--;
      if ( count == 0 )
	count = i;
    }

  modifier = (char*)malloc( count+1 );
  memcpy( modifier, mod, count );
  *(modifier + count) = '\0';
  */

  ostringstream os;
  os << 0              << ":" 
     << rate           << ":" 
     << periodicity    << ":" 
     << adjustability  << ":" 
     << 1              << ":"
     << numberOfTuples << ":";

  modifier = newCstrCopy(os.str());
}

SourceStream::~SourceStream()
{
  // Question: Should we delete 'modifier' here, or is it possible that a caller
  // of 'getModifier()' is still holding a copy even when this d'tor is caller?
} 

char *SourceStream::getModifier()
{
  return modifier;
}

bool SourceStream::isPeriodic()
{
	return m_periodicity;
}

bool SourceStream::isAdjustable()
{
	return m_adjustability;
}

int SourceStream::getNumberOfTuples()
{
	return m_numberOfTuples;
}

void SourceStream::setNumberOfTuples(int numberOfTuples)
{
	m_numberOfTuples = numberOfTuples;
}

string SourceStream::toString()
{
  ostringstream s;
  s << "SourceStream (" << Stream::toString() << ", periodicity: " << m_periodicity;
  s << ", adjustability: " << m_adjustability << ", number of tuples: " << m_numberOfTuples;
  s << ")" << '\0';
  return s.str();
}

