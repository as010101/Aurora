//******* GIANT GLARING HACK ALERT *******
//******* GIANT GLARING HACK ALERT *******
//******* GIANT GLARING HACK ALERT *******
//******* GIANT GLARING HACK ALERT *******
// This class was designed to track avg. timestamp for every qview is tracked.
// However, that's unreasonable for qviewId = 0, which (in the way we use this
// class), represents a cxn point's tail.
//
// The reason it's unreasonable is that when we truncate a cxn point, we don't
// actually load in its pages into memory. Therefore, we have no good way of
// knowing what the cummulative enq timestamps would be for those tuple we're
// truncating.
//
// Unfortunately, several calls on this class require just such information.
// So, the hack is that for qviewId = 0, we're going to avoid all proper 
// maintainance of cummulative enq timestamps.
// -cjc



#ifndef QVIEWTAILPTRMAP_H
#define QVIEWTAILPTRMAP_H

#include <exception>
#include <sm/include/SimplePageRing.H>
#include <sm/include/TimevalAvgTracker.H>
#include <vector>
#include <map>
#include <set>

using namespace std;

class DOMDocument;
class DOMElement;

class QviewTailPtrMap
{
public:
  // If you're creating this from scratch (rather than from XML), then it
  // models a brand neq queue that has no data yet. That means that any
  // qview that's been created will be empty until the first enqueuing
  // occurs. Any qviews added will be marked as empty, until the first 
  // call of onEnqueue.
  QviewTailPtrMap(size_t bytesPerPage, 
		  const SimplePageRing & sprTopology)
    throw (exception);

  // Copies the basic content of the src tail ptr map, but will use 
  // 'sprTopology' for this instance's value of _sprTopology, *not*
  // the value supplied via src.
  QviewTailPtrMap(const QviewTailPtrMap & src, 
		  const SimplePageRing & sprTopology)
    throw (exception);
  
  virtual ~QviewTailPtrMap();

  // Introduces a new qview to the map. Since this is a new Qview (if it already
  // existed, we would have picked it up during XML loading), it's assumed 
  // that it will be empty until the next enqueueing occurs.
  void insertQview(int qviewId)
    throw (exception);

  // Removes a qview from the map. Throws an exception if qviewId isn't presently
  // in the map.
  //
  // If the qviewId curently is non-empty, then it must also be included in the
  // set 'nonEmptyQviewsParameter', and it will then also be removed from the
  // set.
  void deleteQview(int qbaseId, int qviewId/*,
		   set<pair<int, int> > & nonEmptyQviewsParameter*/)
    throw (exception);

  // Like 'insertQview(...)', except the caller supplies an exactly valid
  // (including _serialNum) PageSlotAddr of the next tuple that the qview is to
  // dequeue. 'psa' must be valid giving the current page topology.
  // 'lastEnqSerialNum' is the highest serial number issued to any tuple so far
  // (i.e., the serial number of the last-enqueued tuple).
  //
  // 'tat' is the value with which the qview will be seeded. Since this method
  // creates a qview ex nihilo and tries to make it look like a qview that's been
  // around for a while *and has a specific set of tuples already*, we also need 
  // to see the qview's _avgTstampTracker value. This value is to accurately 
  // describe the set of tuples that the new qview presently contains.
  void insertQviewAtLocation(int qviewId, const PageSlotAddr & psa,
			     const TimevalAvgTracker & tat,
			     unsigned long lastEnqSerialNum)
    throw (exception);

  // For all elements in this map who have the same PageAddr as the specified 
  // PSA, but have a higher slot #, this shifts each of those pages to the
  // successor page.
  //
  // If no page was shifted, this returns -1. Otherwise, this returns the lowest
  // slot number of any of the PSAs that got shifted.
  int shiftQviewHeadsToNextPage(const PageSlotAddr & unshiftedPart)
    throw (exception);

  // Advances the specified PSA by the specified number of slots. Crosses page
  // boundaries as needed. I.e., this is meant to be called when a dequeue 
  // occurs.
  //
  // If the advancement places the specified PSA one slot/serialNum beyond 
  // 'lastGoodSerialNum', then the PSA is marked as empty until a call to 
  // onEnqueue tells us that its serial number once again refers to an existing
  // record.
  //
  // If the advancement places the specified PSA even further beyond 
  // 'lastGoodValue', an exception is thrown.
  //
  // 'enqTstampMicrosec' and 'enqTstampSec' contain the total, across all newly
  // dequeued tuples, of the timestamp-seconds and timestamp-microseconds fields
  // in those tuples (respectively).
  //
  // 'nonEmptyQviewsParameter' is the set of <qbaseId, qviewId> pairs for 
  // currently non-empty qviews. Any qview that becomes empty as a result of 
  // this method will be removed from this set by this method. It's an error for 
  // such a qview to be absent from this set when the method is first invoked.
  void advanceQviewTail(int qbaseId, 
			int qviewId, 
			size_t numSlots,
			unsigned long lastGoodSerialNum,
			unsigned long long enqTstampSecSum,
			unsigned long long enqTstampMicroSecSum,
			set<pair<int, int> > & nonEmptyQviewsParameter)
    throw (exception);

  // Returns the PSA of a qview with the lowest serial # of any mapped, non-empty 
  // qview. Throws an exception if there's no such entry.
  PageSlotAddr getLowestSerialNumInfo() const
    throw (exception);

  // Gives the status of the specified qview. 
  //
  // 'lastGoodSerialNum' is the serial number of the tuple that was most recently
  // recently enqueued into this qview's qbase.
  //
  // If the qview is empty, then only one output parameter is set: 
  //    numTuplesInQview is set to 0.
  // 
  // If the qview is not empty, then...
  //   - 'numTuplesInQview' is set to # of tuples currently in the qview
  //   - 'nextDeqPoint' is set to the next page slot where a dequeue will come 
  //     from, and the serial number that the next dequeued record will have.
  //   - 'avgTimestamp' is the average timestamp (rounded down) of all records
  //     currently in the qview.
  void getQviewStatus(int qviewId, 
		      unsigned long lastGoodSerialNum,
		      size_t & numTuplesInQview, 
		      PageSlotAddr & nextDeqPoint, 
		      timeval & avgTimestamp) const
    throw (exception);

  bool areAnyNonEmptyQviewsMapped() const;

  // Marks all PSAs as non-empty, and sets their PageAddr/slotNum fields to the 
  // specified values.
  // This would normally be called whenever an enqueue occurs. Since any 
  // 'invalid' page is invalid only for want of just one more record entering the
  // queue, we know that *all* currently invalid pages are validated by any 
  // enqueue.
  //
  // The parameters describe the slot of the first record that was enqueued
  // as part of the enqueue operations. I.e., if an enqueue populated slots 
  // 2...11 on page {13,20}, then these parameters should be ({13,20}, 2).
  //
  // 'enqTstampMicrosec' and 'enqTstampSec' contain the total, across all newly
  // enqueued tuples, of the timestamp-seconds and timestamp-microseconds fields
  // in those tuples (respectively).
  //
  // 'nonEmptyQviews' is the set of <qbaseId, qviewId> pairs for currently non-empty
  // qviews. Any qview that becomes non-empty as a result of this method call must be
  // added from this set. It's an error for such a qview to appear in this set 
  // when the method is first invoked.
  void onEnqueue(int qbaseId,
		 unsigned long numTuplesEnqueued,
		 const PageAddr & firstEnqPage, 
		 int firstEnqSlot,
		 unsigned long firstEnqSerialNum,
		 unsigned long long enqTstampSecSum,
		 unsigned long long enqTstampMicroSecSum,
		 set<pair<int, int> > & nonEmptyQviews)
    throw (exception);

  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  void loadFromXml(DOMElement & elem)
    throw (exception);

  string getDebugInfo(int indentLevel) const
    throw (exception);

  // Adds to the specified set the qbaseid/qviewId of all qviews tracked by this
  // object that are currently non-empty. The first value in all of the pairs 
  // this method inserts will be 'qbaseId'.
  // This will throw an exception if it can't insert a pair into the set
  // (implying that the set already contains that element, I think).
  void getNonEmptyQviews(int qbaseId, 
			 set<pair<int, int> > & nonEmptyQviews) const
    throw (exception);

  // Doesn't consider the equivalence ot the SimplePageRings that are involved.
  bool operator == (const QviewTailPtrMap & rhs) const
    throw (exception);

  bool operator != (const QviewTailPtrMap & rhs) const
    throw (exception);

  // Does an internal consistency check on this object.
  void debugCheck() const;

private:
  const size_t _slotsPerPage;
  const SimplePageRing & _sprTopology;

  QviewTailPtrMap & operator = (const QviewTailPtrMap & rhs) {assert(false); return *this;}

  vector<int> _emptyQviews;

  //-----------------------------------------------------------------------------

  // Now that we've finally introduced this class, it might makes sense to  move
  // the field _serialNum out from PageSlotAddr and into this class. It never fit
  // really well (logically) into PageSlotAddr. -cjc
  struct QviewMetadata
  {
    QviewMetadata(const PageSlotAddr & nextDeqPoint,
		  unsigned long numTuplesInQview,
		  const TimevalAvgTracker & avgTracker)
      throw (exception);

    QviewMetadata(DOMElement & elem)
      throw (exception);

    QviewMetadata(const QviewMetadata & src)
      throw (exception);

    void saveToXml(DOMDocument & doc, DOMElement & elem) const
      throw (exception);
    
    void loadFromXml(DOMElement & elem)
      throw (exception);

    bool operator == (const QviewMetadata & rhs) const;
    bool operator != (const QviewMetadata & rhs) const;

    PageSlotAddr _nextDeqPoint;
    unsigned long _numTuplesInQview;
    TimevalAvgTracker _avgTstampTracker;
  };

  //-----------------------------------------------------------------------------

  map<int, QviewMetadata> _nonEmptyQviews;
};

#endif
