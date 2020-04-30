#include <sm/include/TimevalAvgTracker.H>
#include <limits>
#include <gmp.h>

#include <XmlTempString.H>
#include <xercesDomUtil.H>
#include <StorageMgr_Exceptions.H>
#include <util.H>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#include <algorithm>
#include <sstream>
#include <iostream>

//===============================================================================

TimevalAvgTracker::TimevalAvgTracker()
  throw (exception)
  : _totalNumTimevals(0),
    _totalMicroSecs(0)
{
}

//===============================================================================

TimevalAvgTracker::TimevalAvgTracker(unsigned long numTimevals,
				     unsigned long long timevalSecTotal,
				     unsigned long long timevalMicroSecTotal)
  throw (exception)
{
  // Maybe we can optimize this later to avoid the initialization of our two
  // mpz_class objects with values that are just going to be replaced right away.
  // -cjc

  (*this).addTimevalSums(numTimevals, timevalSecTotal, timevalMicroSecTotal);
}

//===============================================================================

TimevalAvgTracker::~TimevalAvgTracker()
{
}
//===============================================================================

void TimevalAvgTracker::clear()
  throw (exception)
{
  _totalNumTimevals = 0;
  _totalMicroSecs = 0;
}

//===============================================================================

void TimevalAvgTracker::addTimevalSums(unsigned long numTimevals,
				       unsigned long long timevalSecTotal,
				       unsigned long long timevalMicroSecTotal)
  throw (exception)
{
  _totalNumTimevals += numTimevals;
  if (! _totalNumTimevals.fits_ulong_p())
    {
      _totalNumTimevals -= numTimevals;
      throw SmException(__FILE__, __LINE__, "Exceeded max # of timevals");
    }

  _totalMicroSecs += ullToMpz(timevalSecTotal) * 1000000;
  _totalMicroSecs += ullToMpz(timevalMicroSecTotal);
}

//===============================================================================

void TimevalAvgTracker::add(const TimevalAvgTracker & srcAvg)
  throw (exception)
{
  _totalMicroSecs += srcAvg._totalMicroSecs;
  _totalNumTimevals += srcAvg._totalNumTimevals;

  if (! _totalNumTimevals.fits_ulong_p())
    {
      _totalNumTimevals -= srcAvg._totalMicroSecs;
      throw SmException(__FILE__, __LINE__, "Exceeded max # of timevals");
    }
}

//===============================================================================

void TimevalAvgTracker::subtractTimevalSums(unsigned long numTimevals,
					    unsigned long long timevalSecTotal,
					    unsigned long long timevalMicroSecTotal)
  throw (exception)
{
  _totalNumTimevals -= numTimevals;
  if (_totalNumTimevals < 0)
    {
      throw SmException(__FILE__, __LINE__, "Underflow in number of timevals");
    }

  _totalMicroSecs -= ((ullToMpz(timevalSecTotal) * 1000000) + 
		      (ullToMpz(timevalMicroSecTotal)));
  if (_totalMicroSecs < 0)
    {
      throw SmException(__FILE__, __LINE__, "Underflow in number of micro-seconds");
    }
}

//===============================================================================

unsigned long TimevalAvgTracker::getNumTimevals() const
{
  return _totalNumTimevals.get_ui(); // not a typo - it really is get_ui, not get_ul
}

//===============================================================================

void TimevalAvgTracker::getAvgTimeval(timeval & tv) const
{
  mpz_class avgTotalUsec = (_totalMicroSecs / _totalNumTimevals);
  tv.tv_usec = mpz_class(avgTotalUsec % 1000000).get_si();

  mpz_class avgWholeSec = avgTotalUsec / 1000000;
  if (! avgWholeSec.fits_slong_p())
    {
      throw SmException(__FILE__, __LINE__, 
			"Overflow: Can't fit avg. # whole seconds into a signed long int");
    }
  tv.tv_sec = avgWholeSec.get_si();
}

//===============================================================================

void TimevalAvgTracker::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  ostringstream os1;
  os1 << _totalNumTimevals;
  setDomAttribute(elem, "_totalNumTimevals", os1.str());

  ostringstream os2;
  os2 << _totalMicroSecs;
  setDomAttribute(elem, "_totalMicroSecs", os2.str());
}

//===============================================================================
    
void TimevalAvgTracker::loadFromXml(DOMElement & elem)
  throw (exception)
{
  string s;

  {
    getDomAttribute(elem, "_totalNumTimevals", s);

    // GMP isn't good at telling us if there was extra junk in the string, so
    // we'll do it ourselves so we can be confident the produced by parsing is
    // exactly what the string had in it...
    if (s.find_first_not_of("0123456789") != string::npos)
      {
	ostringstream os;
	os << "Tried to parse an XML string as an mpz_class, but we couldn't: \"" 
	   << s << "\"";
	throw SmException(__FILE__, __LINE__, os.str());     
      }

    istringstream iss(s);
    iss >> _totalNumTimevals;

    if (iss.fail() || iss.bad())
      {
	ostringstream os;
	os << "Tried to parse an XML string as an mpz_class, but we couldn't: \"" 
	   << s << "\"";
	throw SmException(__FILE__, __LINE__, os.str());     
      }
  }

  {
    getDomAttribute(elem, "_totalMicroSecs", s);

    // GMP isn't good at telling us if there was extra junk in the string, so
    // we'll do it ourselves so we can be confident the produced by parsing is
    // exactly what the string had in it...
    if (s.find_first_not_of("0123456789") != string::npos)
      {
	ostringstream os;
	os << "Tried to parse an XML string as an mpz_class, but we couldn't: \"" 
	   << s << "\"";
	throw SmException(__FILE__, __LINE__, os.str());     
      }

    istringstream iss(s);
    iss >> _totalMicroSecs;

    if (iss.fail() || iss.bad())
      {
	ostringstream os;
	os << "Tried to parse an XML string as an mpz_class, but we couldn't: \"" 
	   << s << "\"";
	throw SmException(__FILE__, __LINE__, os.str());     
      }
  }
}

//===============================================================================

mpz_class TimevalAvgTracker::ullToMpz(unsigned long long ullNum)
  throw (exception)
{
  mpz_class returnVal;

  typedef unsigned long ulong;
  static const ulong ulongMax = numeric_limits<unsigned long>::max();

  if (ullNum <= ulongMax)
    {
      returnVal = ulong(ullNum);
    }
  else
    {
      static const mpz_class mpzUlongMax = ulongMax;

      returnVal = mpzUlongMax * ulong(ullNum / ulongMax);
      returnVal += ulong(ullNum % ulongMax);
    }

  return returnVal;
}

//===============================================================================

bool TimevalAvgTracker::operator == (const TimevalAvgTracker & rhs) const
  throw (exception)
{
  return 
    (_totalNumTimevals == rhs._totalNumTimevals) &&
    (_totalMicroSecs == rhs._totalMicroSecs);
}

//===============================================================================

bool TimevalAvgTracker::operator != (const TimevalAvgTracker & rhs) const
  throw (exception)
{
  return ! ((*this) == rhs);
}

//===============================================================================
