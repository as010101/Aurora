#ifndef QTOPOLOGY_H
#define QTOPOLOGY_H

#include <exception>
#include <iolib/include/PageAddr.H>
#include <sm/include/QviewTailPtrMap.H>
#include <sm/include/SimplePageRing.H>
#include <sm/include/PageSlotAddr.H>
#include <sm/include/IntraPageSlotRange.H>
#include <sm/include/CxnPtInfo.H>

using namespace std;

class DOMElement;
class DOMDocument;

// OVERVIEW:
// This has all of the persistent bookkeeping associated with a qbase/qview set.
// This class doesn't concern itself with framming / buffers / pinning / etc.
// It just worries about pages / slots.
//
// THREAD SAFETY:
// Only one thread at a time may use an instance of this class.

class QTopology
{
public:
  //-----------------------------------------------------------------------------
  // Setup/breakdown methods...
  //-----------------------------------------------------------------------------

  // Creates a new queue. The next record enqueued has a serial number of 0. The
  // qbase has no pages and no qviews.
  //
  // Iff 'isCxnPoint' is true, then this QTopology will maintain the internal 
  // data structures needed for a cxn point, and will support the 
  // "createCxnPointQview" method.
  QTopology(size_t slotsPerPage, bool cxnPoint = false)
    throw (exception);

  QTopology(const QTopology & src)
    throw (exception);

  virtual ~QTopology();
  
  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  void loadFromXml(const DOMElement & elem)
    throw (exception);

  //-----------------------------------------------------------------------------
  // Enqueue-related methods...
  //-----------------------------------------------------------------------------

  // Returns the number of additional records that can be enqueued before (a) a
  // page is added to the queue, and/or (b) the right dequeues occur. If this
  // returns 0, a call to getNextEnqRegion(...) will throw an exception.
  size_t getEnqCapacity() const
    throw (exception);

  // Returns true iff adding a page requires a page split.
  bool doesAddPageRequireSplit() const
    throw (exception);

  // Adds the specified page to the queue. Only valid to call this if a page 
  // split isn't needed.
  void addPageWithoutSplit(const PageAddr & pa)
    throw (exception);

  // Adds the specified page. Once this call returns, no other call is valid on
  // this QTopology object until the caller has made the following copy:
  // The contents of slots 'firstSlotToCopy' ... 'lastSlotToCopy' on page 
  // 'oldPage' must be copied to page 'newPage'.
  //
  // Remember that firstSlotToCopy and lastSlotToCopy are zero-based offsets, and 
  // describe record-offsets, *not* byte offsets.
  //
  // As long as no enqueues or dequeues occur, 'enqPage' is the page that will
  // appear in the return value from the next call to getNextEnqRegion().
  void addPageWithSplit(const PageAddr & newPage, PageAddr & enqPage, 
			size_t & firstSlotToCopy, size_t & lastSlotToCopy)
    throw (exception);

  // This method will throw an exception if the queue's enqueue capacity is 0.
  // Instructs the caller where the next enqueueings should occur. The range
  // is [firstSlot, lastSlot] on the specified page. 
  //
  // Remember that firstSlot and lastSlot are zero-based offsets, and describe 
  // record-offsets, *not* byte offsets.
  IntraPageSlotRange getNextEnqRegion()
    throw (exception);

  // Indicates what subset of slots described in the previous call to 
  // getNextEnqRegion(...) were actually populated.
  //
  //
  // 'deqTstampSecSum' and 'deqTstampMicroSecSum' contain the total, across all 
  // newly dequeued tuples, of the timestamp-seconds and timestamp-microseconds 
  // fields in those tuples (respectively).
  //
  // 'nonEmptyQviewsParameter' is the set of <qbaseId, qviewId> pairs for 
  // currently non-empty qviews. The qviews that becomes non-empty as a result 
  // of this method will be added to this set. It's an error for such a qview to 
  // appear in this set when the method is first invoked.
  //
  // If there's something obviously wrong about the number specified (ie., would
  // cause queue overflow or runs past a page boundary), this method may throw
  // an exception.
  void onEnqComplete(int qbaseId,
		     size_t numSlotsFilled,
		     unsigned long long enqTstampSecSum,
		     unsigned long long enqTstampMicroSecSum,
		     set<pair<int, int> > & nonEmptyQviewsParameter)
    throw (exception);

  //-----------------------------------------------------------------------------
  // Dequeue-related methods...
  //-----------------------------------------------------------------------------

  // Throws an exception if 'qviewId' is already defined for this queue. The 
  // qview will only receive subsequently enqueued records.
  //
  // qviewId must be > 0.
  void createQView(int qviewId)
    throw (exception);

  // Attaches to a cxn point.
  //
  // Enforces the following preconditions, using exception throwing:
  // - qviewId isn't yet defined in this QTopology.
  // - qviewId > 0
  // - This QTopology has a cxn point
  // - At least one of 'limitTuplesByNum' and 'limitTuplesByAge' is true.
  void createCxnPointQView(int qviewId,
			   bool limitTuplesByNum,
			   size_t maxNumTuples,
			   bool limitTuplesByAge,
			   unsigned int maxTupleAgeSeconds)
    throw (exception);

  // Adjusts the history spec. The parameters have the same meanings and 
  // limitations as in the method 'createCxnPointQView(...)'.
  void setCxnPointHistorySpec(bool limitTuplesByNum,
			      size_t maxNumTuples,
			      bool limitTuplesByAge,
			      unsigned int maxTupleAgeSeconds)
    throw (exception);


  // Deletes the specified QView. This method deletes both normal and cxn-point
  // qviews.
  //
  // 'nonEmptyQviewsParameter' is the set of <qbaseId, qviewId> pairs for 
  // currently non-empty qviews. The qviews that becomes non-empty as a result 
  // of this method will be added to this set. It's an error for such a qview to 
  // appear in this set when the method is first invoked.
  //
  // qviewId must be > 0.
  void deleteQView(int qbaseId, int qviewId)
    throw (exception);

  // Returns the number of records that the specified qview contains. E.g., the 
  // number of records that can be dequeued before more enqueues must occur.
  // If 'numTuples' is set to a value > 0, then 'avgTimestamp' gives the (rounded
  // down) average timestamp of all tuples in the qview.
  //
  // If qviewId = 0 and this QTopology has a cxn point, this reports on that
  // cxn point.
  void getQviewStats(int qviewId, size_t & numTuples, timeval & avgTimestamp) const
    throw (exception);

  // This method will throw an exception if the qview contains no records.
  // It instructs the caller where the next records to deqeu are stored. The 
  // range is [firstSlot, lastSlot] on the specified page. 
  //
  // Remember that firstSlot and lastSlot are zero-based offsets, and describe 
  // record-offsets, *not* byte offsets.
  //
  // qviewId must be > 0.
  IntraPageSlotRange getNextDeqRegion(int qviewId) const
    throw (exception);

  // Indicates what subset of slots described in the previous call to 
  // getNextDeqRegion(...) were actually dequeued.
  //
  // 'deqTstampSecSum' and 'deqTstampMicroSecSum' contain the total, across all 
  // newly dequeued tuples, of the timestamp-seconds and timestamp-microseconds 
  // fields in those tuples (respectively).
  //
  // 'nonEmptyQviews' is the set of <qbaseId, qviewId> pairs for currently non-empty
  // qviews. Any qview that becomes empty as a result of this method call must be
  // removed from this set. It's an error for such a qview to be absent from this set 
  // when the method is first invoked.
  //
  // If there's something obviously wrong about the number specified (ie., would
  // cause qview underflow or runs past a page boundary), this method may throw
  // an exception.
  //
  // qviewId must be > 0.
  void onDeqComplete(int qbaseId, 
		     int qviewId, 
		     size_t numSlotsFilled,
		     unsigned long long deqTstampSecSum,
		     unsigned long long deqTstampMicroSecSum,
		     set<pair<int, int> > & nonEmptyQviewsParameter)
    throw (exception);

  //-----------------------------------------------------------------------------
  // Miscellaneous...
  //-----------------------------------------------------------------------------

  // Returns 'true' iff this QTopology supports cxn-point operations.
  bool isCxnPoint() const;

  // Gives access to the set of pages that store this queue's data.
  const vector<PageAddr> & getPageRing() const
    throw (exception);

  // Fairly complicated method...
  //
  // firstPageIdx is an index into the vector returned by getPageRing(). It is
  // the index of the page in that contains the next record to be dequeued from
  // this qview.
  //
  // If qviewId = 0 and this QTopology has a cxn point, this reports on that
  // cxn point.
  //
  // numRecs is populated, in order, with the pages that contain the data for
  // the specified qview. The value in the vector refers to the number of records
  // the qview has for the specified page. Note...
  // (A) The first and last elements in the vector may refer to the same page.
  //     In that scenario, numRecs[0] refers to the first part of the qview's 
  //     records that would be dequeued from that page, and 
  //     numRecs[numRecs.size()-1] refers to the final set of records in the 
  //     qview, which happen to reside on the same page as the first set of 
  //     records.
  // (B) Here's the formula to know what PageAddr a certain element of numRecs
  //     corresponds to:
  //     pa = getPageRing()[
  //        (firstPageIdx + (index into numRecs)) % getPageRing.size()]
  //
  // Notes:
  // - This method will throw an exception if the qview is empty.
  // - *This method will call numRecs.clear() before adding elements to it*
  void getPageOccupancy(int qviewId,
			size_t & firstPageIdx,
			vector<size_t> & numRecs) const
    throw (exception);

  // Adds to the specified set the qbaseid/qviewId of all qviews tracked by this
  // object that are currently non-empty. The first value in all of the pairs 
  // this method inserts will be 'qbaseId'.
  // This will throw an exception if it can't insert a pair into the set
  // (implying that the set already contains that element, I think).
  void getNonEmptyQviews(int qbaseId, 
			 set<pair<int, int> > & nonEmptyQviews) const
    throw (exception);

  bool operator == (const QTopology & rhs) const
    throw (exception);

  bool operator != (const QTopology & rhs) const
    throw (exception);

  QTopology & operator= (const QTopology & rhs) {assert(false); return *this;}

  string getDebugInfo(int indentLevel = 0) const
    throw (std::exception);

  // Does an internal consistency check on this object.
  void debugCheck() const;

  // It's only valid to call this it the QTopology has a cxn point. It causes all
  // potential pruning of overly old tuples to happen right away.
  // 
  // NOTE: I'm only including this method to assist in unit testing. Application
  // code shouldn't become reliant on this method without its programmer(s) first
  // getting approval of the person maintaining this code. -cjc
  void forceCxnPointPruning(int thisQbaseId,
			      set<pair<int, int> > & nonEmptyQviewsParameter);
private:
  QTopology();

  // Only call this when the queue has capacity for additional enqueues. 
  // Otherwise and exception will be thrown. This method updates _nextEnqPoint
  // and _nextEnqMaxSlotNum;
  void decideNextEnqBounds()
    throw (exception);

  // If there' cxn point for this QTopology, then this is a no-op. Otherwise, 
  // this will perform the proper maintenance on the set of tuples that comprise
  // the cxn point's history.
  void keepCxnPointPruned(int thisQbaseId, 
			  set<pair<int, int> > & nonEmptyQviewsParameter)
    throw (exception);

  size_t _slotsPerPage;
  SimplePageRing _pages;

  // If the map contains an entry for qviewid = 0, then that qview is for the
  // implementation of the cxn point.
  QviewTailPtrMap _nextDeqPoints;

  // _prevEnqPoint is meaningful exactly after the first record has been enqueued
  // into the queue (a once-in-a-queue's-lifetime event), which is designated by
  // _firstEnqOccured. _prevEnqPoint describes the last slot to receive a new
  // record. I.e., it's the location of the newest record in the queue.
  bool _firstEnqOccured;
  PageSlotAddr _prevEnqPoint;

  // This is only valid when the queue is non-empty, and decideNextEnqBounds() has
  // been called recently enough. It's not valid to call this when the queue lack
  // free slots for enqueueing. (I.e., has no pages, or all slots are used.)
  IntraPageSlotRange _nextEnqBounds;

  // This will be non-NULL for, and only for, queues that are cxn points. This is
  // maintained as we do enqueus and dequeues. It provides a quick index into 
  // our page ring, so that when (truncating the cxn point data) or (an app 
  // connects to a cxn point) we can quickly move the specified tail ptr in the
  // appropriate PSA.
  CxnPtInfo * _pCxnPtInfo;
};

#endif
