#ifndef TIMEVALAVGTRACKER_H
#define TIMEVALAVGTRACKER_H

#include <gmpxx.h>
#include <exception>

using namespace std;

class DOMDocument;
class DOMElement;

class TimevalAvgTracker
{
public:
  // Starts out with an empty population...
  TimevalAvgTracker()
    throw (exception);

  // Starts out with the specified population...
  TimevalAvgTracker(unsigned long numTimevals,
		    unsigned long long timevalSecTotal,
		    unsigned long long timevalMicroSecTotal)
    throw (exception);

  virtual ~TimevalAvgTracker();

  // Sets this object to reflect an empty population of data.
  void clear()
    throw (exception);

  // Affect the stats as though 'numTimevals' timeval objects were being added
  // into the population, whose tv_sec fields had a total value of 
  // 'timevalSecTotal' and whose tv_usec fields had a total value of
  // 'timevalMicroSecTotal'...
  //
  // No normalization is assumed regarding the sec/usec parms.
  //
  // Throws an exception if this method causes the total number of timevals to
  // exceed the number that can be expressed in an unsigned long int.
  void addTimevalSums(unsigned long numTimevals,
		      unsigned long long timevalSecTotal,
		      unsigned long long timevalMicroSecTotal)
    throw (exception);

  // Adds in the population from another TimevalAvgTracker.
  void add(const TimevalAvgTracker & srcAvg)
    throw (exception);

  // Affect the stats as though 'numTimevals' timeval objects were being removed
  // from the population, whose tv_sec fields had a total value of 
  // 'timevalSecTotal' and whose tv_usec fields had a total value of
  // 'timevalMicroSecTotal'...
  //
  // No normalization is assumed regarding the sec/usec parms.
  //
  // Throws an exception on mathematical underflow of the population size or
  // of the total number of microseconds.
  void subtractTimevalSums(unsigned long numTimevals,
			   unsigned long long timevalSecTotal,
			   unsigned long long timevalMicroSecTotal)
    throw (exception);

  // Returns the number of timevals in the population.
  unsigned long getNumTimevals() const;

  // Populates 'tv' with the average timestamp value of the timeval population.
  // If the population is size 0, this will throw an exception.
  void getAvgTimeval(timeval & tv) const;


  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);
    
  void loadFromXml(DOMElement & elem)
    throw (exception);

  bool operator == (const TimevalAvgTracker & rhs) const
    throw (exception);

  bool operator != (const TimevalAvgTracker & rhs) const
    throw (exception);

private:
  // Returns the numeric value represented by 'ullNum', as an mpz_class object.
  mpz_class ullToMpz(unsigned long long ullNum)
    throw (exception);

  mpz_class _totalNumTimevals;
  mpz_class _totalMicroSecs;
};

#endif
