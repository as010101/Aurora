#ifndef CXNPTINFO_H
#define CXNPTINFO_H

#include <StorageMgr_Exceptions.H>
#include <sys/time.h>
#include <xercesDomUtil.H>
#include <iolib/include/PageAddr.H>
#include <sm/include/PageSlotAddr.H>
#include <sm/include/TimevalAvgTracker.H>
#include <deque>
  
// All the cxn-point-specific bookkeeping for a single cxn point...
class CxnPtInfo
{
public:
  CxnPtInfo();
  CxnPtInfo(const CxnPtInfo & src);
  virtual ~CxnPtInfo();
  CxnPtInfo & operator = (CxnPtInfo & rhs);

  bool operator == (const CxnPtInfo & rhs) const
    throw (exception);

  bool operator != (const CxnPtInfo & rhs) const
    throw (exception);
  
  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  void loadFromXml(const DOMElement & elem)
    throw (exception);

  // 'maxNumTuples' is meaningful when and only when 'limitTuplesByNum' is 
  // true. It records that limit.
  // Same idea for 'limitTuplesByAge' / 'maxTupleAge'.
  // At least one of 'limitTuplesByNum' or 'limitTuplesByAge' must be true.
  void setRetentionBounds(bool limitTuplesByNum,
			  size_t maxNumTuples,
			  bool limitTuplesByAge,
			  unsigned int maxTupleAgeSeconds)
    throw (exception);


  // This must be called whenever an enq occurs. This method may or may not
  // do sanity checks to verify that the enq that's described retains the page
  // use patterns required by our architecture. A detected violation causes an
  // exception.
  //
  // slotsPerPage is the number of slots that the owning queue can pack onto a
  // page.
  void onEnqComplete(const PageAddr & pa,
		     size_t lowSlotNum,
		     size_t highSlotNum,
		     size_t slotsPerPage,
		     unsigned long long timevalSecTotal,
		     unsigned long long timevalMicroSecTotal)
    throw (exception);

  // This must be called whenever a page split occurs.
  // This method may throw an exception if any of the preconditions are violated.
  //
  // Preconditions: 
  //    (a) The head and tail page parts occupy the same page. 
  //    (b) 'newEnqPage' is not already used anywhere in the queue.
  //
  // Postcondition:
  //    (a) The page parts array is modified exactly so that the head page part
  //    (representing where the last enq occured) will be on 'newEnqPage'.
  void onPageSplit(const PageAddr & newEnqPage)
    throw (exception);

  // Given the current truncation criteria, this...
  // (a) consumes the tail portion of _pageParts that no longer belongs in the
  //     cxn point storage (only modifies _pageParts), and
  // (b) returns the number of slots that were consumed.
  // 
  // Presumably the caller will use this return value to advance the 
  // QviewTailPtr's cxn point by the specified number of tuples.
  //
  // slotsPerPage is the number of slots that the owning queue can pack onto a
  // page.
  size_t doBoundedTruncation(size_t slotsPerPage)
    throw (exception);

  // Tries to retrieve the PSA closest to the enq-point of the queue, that 
  // satisfies at least one of the specified criteria. 
  //
  // It's OK for neither 'limitTuplesByNum' and 'limitTuplesByAge' to be true. 
  // When they're both false, the entire history that's currently stored (plus
  // all future tuples, of course) is requested.
  //
  // If this returns 'true', then...
  // - 'psa' is set to the first tuple in the cxn's history, and
  // - 'timestampStats' describes all of the tuples currently in the cxn's
  //   history.
  //
  // If no tuple in the queue has a chance of meeting the specified 
  // criterion(ia), this returns 'false', and 'timestampStats' has a value
  // of undefined meaning.
  //
  // If this returns 'true', then at least one matching tuple was found. 
  // Regardless of which history spec was given, the tuple range associated
  // with the deq point may slightly exceed (by up to slotsPerPage - 1 tuples)
  // the set of tuples actually requested. This is a necessary concession to
  // let us accurately and efficiently calculate 'timestampStats'.
  bool getNextDeqPoint(size_t slotsPerPage,
		       PageSlotAddr & psa,
		       TimevalAvgTracker & timestampStats,
		       bool limitTuplesByNum,
		       size_t maxNumTuples,
		       bool limitTuplesByAge,
		       unsigned int maxTupleAgeSeconds) const
    throw (exception);

  string getDebugInfo() const
    throw (exception);

private:
  // What the cxn point must track for each contiguous range of page slots.
  // A page of the form:
  //
  //   +--------------------------+------------------+
  //   + Recently Enq Data (head) |  Old data (tail) |
  //   +--------------------------+------------------+
  //    ^                                           ^
  //    |                                           |
  //  slot 0                                      slot (n-1)
  //
  // 
  // will be modelled with two CxnPtPagePartInfo objects: one for the head, and
  // one for the tail.
  //
  // Note that such a page could instead have the following structure:
  //
  //   +--------------------------+----------------+------------------+
  //   + Recently Enq Data (head) |  Unused slots  | Old data (tail)  |
  //   +--------------------------+----------------+------------------+
  //    ^                                                            ^
  //    |                                                            |
  //  slot 0                                                     slot (n-1)
  // 
  struct PagePartInfo
  {
    PagePartInfo();

    PagePartInfo(const PageAddr & pa,
		 size_t lowSlotNum,
		 size_t highSlotNum,
		 timeval lastEnqTime,
		 unsigned long lowSlotSerialNum,
		 TimevalAvgTracker timestampStats);

    bool operator == (const PagePartInfo & rhs) const;

    void saveToXml(DOMDocument & doc, DOMElement & elem) const
      throw (exception);

    void loadFromXml(const DOMElement & elem)
      throw (exception);

    size_t getNumTuples() const;

    // Which Page this CxnPtPagePartInfo pertains to.
    PageAddr _pa;

    // The slot numbers of _pa that this CxnPtPagePartInfo pertains to.
    size_t _lowSlotNum, _highSlotNum;

    // Latest wallclock time that enqueues were done to this range.
    timeval _lastEnqTime;

    // Serial number of tue tuple stored in _lowSlotNum...
    unsigned long _lowSlotSerialNum;

    // Stats for all the tuples occupying this PPI.
    TimevalAvgTracker _timestampStats;
  };
    
  // Counts the number of tuples that are represented by the contents of 
  // '_pageParts'.
  size_t getNumPagePartsTuples(size_t slotsPerPage) const
    throw (exception);

  // Searches _pageParts for the oldest tuple that's known to be no older than
  // 'maxTupleAgeSeconds' seconds.
  //
  // If such a tuple is found, then this...
  // - sets 'psa' to that slot's address,
  // - sets 'pagePartsIdx' to the corresponding index into _pageParts, 
  // - sets 'timestampStats' to describe the population of timestamps in the
  //   tuples that make up the returned history, and
  // - returns 'true'.
  //
  // If no such tuple is found, then this returns 'false', and 'timestampStats'
  // has a value of undefined meaning.
  //
  // NOTE: 
  // *** THIS METHOD RETURNS THE FIRST SLOT OF THE Page Part CONTAINING THE 
  // *** DESIRED TUPLE, SO THAT WE CAN ACCURATELY DETERMINE THE TIMESTAMP AVG. OF
  // *** THE SPECIFIED SET OF TUPLES.
  bool findTupleByMaxAge(unsigned int maxTupleAgeSeconds,
			 size_t slotsPerPage,
			 PageSlotAddr & psa,
			 size_t & pagePartsIdx,
			 TimevalAvgTracker & timestampStats) const
    throw (exception);

  // Like 'findTupleByMaxAge', except the constraint is on how many tuples, from
  // the last-enqueued tuples, to include.
  //
  // NOTE: 
  // *** THIS METHOD RETURNS THE FIRST SLOT OF THE Page Part CONTAINING THE 
  // *** DESIRED TUPLE, SO THAT WE CAN ACCURATELY DETERMINE THE TIMESTAMP AVG. OF
  // *** THE SPECIFIED SET OF TUPLES.
  bool findNthYoungestTuple(unsigned int maxNumTuples,
			    size_t slotsPerPage,
			    PageSlotAddr & psa,
			    size_t & pagePartsIdx,
			    TimevalAvgTracker & timestampStats) const
    throw (exception);

  // Index 0 = the region whose low slot number is the cxn point's oldest
  // tuple. Index 1 = next region, etc. 
  // Index = (_pageParts.size() - 1) is the head of the queue. The only time a
  // certain PageAddr will appear twice in this vector's data is when the head
  // and tail page parts lie on the same page because of wrap-around use of the
  // page.
  deque<PagePartInfo> _pageParts;

  // True when and only when this cxn point caps the # of tuples stored
  bool _limitTuplesByNum;

  // Meaningful when and only when '_limitTuplesByNum' is true. Gives the 
  // limit.
  size_t _maxNumTuples;

  // True when and only when this cxn point caps the age of tuples that are
  // retained.
  bool _limitTuplesByAge;

  // Meaningful when and only when '_limitTuplesByAge' is true. Gives the
  // limit.
  unsigned int _maxTupleAgeSeconds;

  // The sequence number of the last tuple to be enqueued. This is set to 0
  // initially, so that the first tuple in the queue has a serial num of 0.
  unsigned long _nextEnqSerialNum;
};

#endif
