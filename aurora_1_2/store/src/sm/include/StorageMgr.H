#ifndef STORAGEMGR_H
#define STORAGEMGR_H

#include <ILockable.H>
#include <StorageMgr_Exceptions.H>
#include <PropsFile.H>
#include <stats/include/SmRuntimeStats.H>
#include <buffercache/include/BufferCache.H>
#include <sm/include/SuperBoxPlanStep.H>
#include <sm/include/SuperBoxPacer.H>
#include <sm/include/QMetadata.H>
#include <sm/include/PagePriorityTracker.H>

using namespace std;

// ******************************************************************************
// THREADING ISSUES
// Multiple threads can invoke the methods on a StorageMgr object without 
// negative consequences. I.e., we built this class to be fully threadsafe.
// 
// This class implements internal locking to ensure linearizability. The only 
// method that get into trouble from concurrent invocations is the destructor, 
// but that would be a moronic thing to do anyway ;)
// ******************************************************************************

// ******************************************************************************
// LOCKING ISSUES
// The lock (lock()/unlock()) must be held whenever any of the public methods
// on this class are invoked (except for the lock() method, of course ;).
//
//                      ### EXCEPTION TO THE ABOVE RULE ###
// These methods: 
//    awaitEnqPossible(...) 
//    awaitDeqPossible(...)
//    awaitPageFraming(...)
//  must be called when the user does *not* hold this StorageMgr's lock.
//
// 
// When the lock is held by a thread, no other thread modifies the data 
// structure. There reasons the methods (other than 'unlock()') require 
// externally controlled locking rather than internally controlled locking is
// mutual consistency:
//
// getQviewInfo(...) returns, among other things, information about what pages 
// are presently framed. If the user plans to capitalize on that current framing, 
// he needs to schedule new train(s) before those pages are evicted for other 
// reasons. Thus, we ensure that getQviewInfo(...) and scheduleTrain(...) can 
// both be called within one locked period.
//
// Because so much lock contention might occur on the StorageMgr, and to avoid
// deadlock, the potentially blocking methods accept a BinarySem to post to,
// rather than having the methods themselves block the caller. This allows the
// caller to release the mutex on the StorageMgr while that method is doing its
// work.
// ******************************************************************************

class StorageMgr : public ILockable
{
public:
  StorageMgr(string dirPathname, 
	     size_t bytesPerPage,
	     size_t maxConcurrency,
	     size_t numPageFrames)
    throw (exception);

  // This is like the other c'tor, but all of the details are provided via the
  // PropsFile. The PropsFile must have the following properties defined:
  //    StorageMgr.dirPathname    (string)
  //    StorageMgr.bytesPerPage   (unsigned int)
  //    StorageMgr.maxConcurrency (unsigned int)
  //    StorageMgr.numPageFrames  (unsigned int)
  //
  // This StorageMgr does *not* retain a reference to the specified PropsFile 
  // after this c'tor returns.
  //
  // HACK NOTE: We assume that size_t = unsigned int. It we really want to do 
  // this cleanly, we can extend PropsFile to support "type=size_t". That will
  // just take a little more work, which doesn't seem justified right now. -cjc
  StorageMgr(const PropsFile & startupProps)
    throw (exception);

  virtual ~StorageMgr();

  void shutdown()
    throw (exception);

  virtual void lock()
    throw (exception);
  
  virtual void unlock()
    throw (exception);

  //=============================================================================
  // SuperBox-independent operations...
  //=============================================================================

  void createQ(int qbaseId,
	       const vector<int> & qviewIds,
	       size_t bytesPerRecord, 
	       bool cxnPoint = false)
    throw (exception);


  // Attaches to a cxn point.
  //
  // A cxn point may contain a longer history of tuples than is mandated by the
  // cxn point's history spec. If that's the case, then the cxn point qview
  // created by this method may (or may not) have access to that greater span of
  // history.
  //
  // Enforces the following preconditions, using exception throwing:
  // - a qbase with 'qbaseId' is already defined.
  // - a with the name {qbaseId, qviewId} isn't currently defined
  // - qviewId > 0
  // - This QTopology has a cxn point
  // - At least one of 'limitTuplesByNum' and 'limitTuplesByAge' is true.
  void createCxnPointQView(int qbaseId,
			   int qviewId,
			   bool limitTuplesByNum,
			   size_t maxNumTuples,
			   bool limitTuplesByAge,
			   unsigned int maxTupleAgeSeconds)
    throw (exception);

  // Adjusts the history spec of a cxn point. The parameters have the same 
  // meanings and limitations as in the method 'createCxnPointQView(...)'.
  void setCxnPointHistorySpec(int qbaseId,
			      bool limitTuplesByNum,
			      size_t maxNumTuples,
			      bool limitTuplesByAge,
			      unsigned int maxTupleAgeSeconds)
    throw (exception);

  // It's undefined what this does if there's a train currently scheduled using
  // this queuebase or any of its qviews, so be careful...
  void deleteQ(int qbaseId)
    throw (exception);

  // Deletes the specified QView. This method deletes both normal and cxn-point
  // qviews.
  //
  // It's undefined what this does if there's a train currently scheduled using
  // this qview, so be careful...
  //
  // 'nonEmptyQviewsParameter' is the set of <qbaseId, qviewId> pairs for 
  // currently non-empty qviews. The qviews that becomes non-empty as a result 
  // of this method will be added to this set. It's an error for such a qview to 
  // appear in this set when the method is first invoked.
  //
  // qviewId must be > 0.
  void deleteQView(int qbaseId, int qviewId)
    throw (exception);

  // Returns various statistics about the specified qview.
  // If the qview is empty:
  //    'numRecsInQview' is set to 0.
  // If the qview is non-empty:
  //    - 'numRecsInQview' is set to the number of records in the qview.
  //    - 'numRecsFramed' is set to the number of records that can presently be
  //       dequeued from the qview before being stymied by a non-framed page.
  //       This number will always be <= 'numRecsInQview'.
  //    - 'avgTimestamp' is the average timestamp (rounded down) of all records
  //       currently in the qview.
  //
  // The only reason this isnt' a 'const' method is that it requires grabbing
  // a ILockable lock, which I had trouble making be a const operation.
  //
  // If qviewId = 0 and the specified qbase has a cxn point, then this reports
  // on that cxn point's historical data.
  //
  // LOCKING: The caller must already hold the StorageMgr's lock when  calling 
  // this method.
  void getQviewInfo(int qbaseId, int qviewId, 
		    size_t & numRecsInQview, 
		    size_t & numRecsFramed,
		    timeval & avgTimestamp)
    throw (exception);

  // Provides a live reference to the set of non-empty qviews in this storage
  // manager. This reference remains valid for the live of the StorageMgr object.
  //
  // The set will be updated (when appropriate) after calls to the methods:
  //    createQ(...)
  //    releaseEnqBuffer(...)
  //    releaseDeqBuffer(...)
  //
  // The 'first' value in the set's pairs is the qbaseId, and the 'second' value
  // is the qviewId.
  const set<pair<int, int> > & getNonEmptyQviews() const
    throw (exception);


  //=============================================================================
  // Runtime statistics methods...
  //=============================================================================

  // Takes a snapshot of the stats that have accumulated since the last snapshot.
  SmRuntimeStatsSnapshot getRuntimeStatsSnapshot()
    throw (exception);
 
  //=============================================================================
  // SuperBox-specific operations...
  //=============================================================================

  // It's illegal to schedule a superbox when there is already a superbox with 
  // the same superBoxId whose steps aren't completed yet.
  void scheduleSuperBox(int superBoxId, 
			const vector<SuperBoxPlanStep> & schedule)
    throw (exception);

  // Marks the current superbox step as complete (for the given superBoxId). Iff 
  // the superbox has another step:
  //   - The superbox schedule is advanced to the next step, and
  //   - this method returns true
  // Otherwise, this method returns false, and the specified superBoxId is 
  // considered  to no longer be scheduled.
  //
  // Throws an exception if the specifeid superBoxId doesn't refer to a currently 
  // scheduled superBoxId.
  bool onSuperBoxStepComplete(int superBoxId)
    throw (exception);

  // If the specified superbox step is immediately runnable, this method returns
  // 'true' and ignores the supplied BinarySem.
  //
  // Otherwise, this returns 'false' and will post to the BinarySem at some later
  // time, when the superbox is finally runnable.
  bool notifyWhenSuperBoxStepRunnable(int superBoxId, BinarySem & flag)
    throw (exception);

  // Returns the info for th current superbox step. The returned reference 
  // remains valid until the next call to onSuperBoxStepComplete(superBoxId).
  //
  // Throws an exception if the specified superBoxId isn't currently scheduled.
  const SuperBoxPlanStep & getCurrentSuperBoxStep(int superBoxId) const 
    throw (exception);

  //-----------------------------------------------------------------------------
  // For enqueueing data...
  //-----------------------------------------------------------------------------

  // Potentially blocking method. When the specified qbase can be enqueued into, 
  // this returns.
  //
  // *** DO NOT HOLD THIS StorageMgr OBJECT'S LOCK WHEN INVOKING THIS METHOD ***
  // This method needs the ability to repeatedy acquire and release the lock, 
  // which it can't do if the caller holds it.
  void awaitEnqPossible(int superBoxId, int qbaseId)
    throw (exception);

  // Gives the exact buffer address of where the next enqueued record is to go,
  // as well as the number of records that may be written to the memory starting
  // at that address.
  //
  // Calling thie method multiple times doesn't cause any problem.
  //
  // If no enqueues are presently possible (because of insufficient page
  // framing, etc.) this returns with 'numRecs' set to 0 and pBuffer set to
  // NULL.
  void getEnqBuffer(int superBoxId, int qbaseId, size_t & numRecs, char * & pBuffer)
    throw (exception);

  // Causes 'numRecsDequeued' records to actually be enqueued. They're presumed
  // to be sitting in the buffer whose address is given by the most recent call
  // to getEnqBuffer.
  //
  // Throws an exception if 'numRecsDequeued' is < 0 or > the 'numRecs' 
  // parameter would be set to by a call to getEnqBuffer(...).
  void releaseEnqBuffer(int superBoxId, int qbaseId, size_t numRecsEnqueued)
    throw (exception);

  //-----------------------------------------------------------------------------
  // For dequeueing data...
  //-----------------------------------------------------------------------------

  // Potentially blocking method because of awaiting pages to be framed.
  //
  // If the qview is empty when this is called, this will throw an exception.
  // Avoid that problem by using the getQviewInfo(...) method and remembering
  // how many records you've dequeued since calling that method.
  //
  // *** DO NOT HOLD THIS StorageMgr OBJECT'S LOCK WHEN INVOKING THIS METHOD ***
  // This method needs the ability to repeatedy acquire and release the lock, 
  // which it can't do if the caller holds it.
  void awaitDeqPossible(int superBoxId, int qbaseId, int qviewId)
    throw (exception);

  void getDeqBuffer(int superBoxId, 
		    int qbaseId, 
		    int qviewId, 
		    size_t & numRecs, 
		    const char * & pBuffer, 
		    unsigned long & firstSeqNum)
    throw (exception);

  void releaseDeqBuffer(int superBoxId, int qbaseId, int qviewId, 
			size_t numRecsDequeued)
    throw (exception);

  //=============================================================================

  // Does an internal consistency check on this object.
  void debugCheck() const;

  // It's only valid to call this on a QBase with a cxn point. It causes all
  // potential pruning of overly old tuples to happen right away.
  // 
  // NOTE: I'm only including this method to assist in unit testing. Application
  // code shouldn't become reliant on this method without its programmer(s) first
  // getting approval of the person maintaining this code. -cjc
  void forceCxnPointPruning(int qbaseId);

private:
  // Destructively overwrites the specified file (it it's there), storing this
  // StorageMgr's durable state. The mutex must be held by wheter thread calls
  // this method.
  void saveToFile(string filepath)
    throw (exception);

  // Initialiazes this StorageMgr, then populates a subset of its members from
  // the specified serialization file. The mutex must be held by wheter thread 
  // calls this method.
  void loadFromFile(string filepath)
    throw (exception);

  // If the specified superbox has any page priorities that have been set using
  // adjustNeededPagePriorities(...), this undoes them.
  void discardPagePriorities(int superBoxId)
    throw (exception);

  // Adds new page priorities for the specified superbox, assuming firstIdx
  // to be the index into 'schedule' of the first step in still considered to
  // be in the schedule.
  void establishNeededPagePriorities(int superBoxId, 
				  const vector<SuperBoxPlanStep> & schedule, 
				  size_t firstIdx)
    throw (exception);

  // Sets the page priorities in the buffer cache to properly correspond to the
  // state of the current supertrain schedules (as given by 
  // _neededPagePriorities).
  void updateBcPagePriorities()
    throw (exception);

  // For the lock()/unlock() methods.
  PtMutex _mtx; 

  size_t _numPageFrames;
  size_t _bytesPerPage;
  size_t _maxConcurrency;
  string _dirPathname;

  // The name of the XML file used to load/save this StorageMgr's metadata.
  string _xmlFilename;

  // Just a placeholder when we don't really care about the signals that a
  // method will post. A method that receives this as a parameter is probably
  // a candidate for simplification during the next rewrite of the Storage 
  // Manager.
  BinarySem _dummySem;

  // We might want to later turn this into a QMetadata*, not a QMetadata. Then
  // we can avoid calling QMetadata's copy c'tor, which is rather expensive.
  // We'd just have to ensure that when we erased elements from the map, we also
  // deleted the QMetadata pointer.
  map<int, QMetadata> _queueMap;

  SuperBoxPacer _admissionPacer;
  BufferCache _bufferCache;
  PagePriorityTracker _neededPagePriorities;

  // For each scheduled superbox, we remember the set of pages it decided should
  // be at each priority. We retain this so that when it's time to remove those
  // prioritizations from _neededPagePriorities, we don't need to recalculate 
  // the old priorities, which is time consuming and error prone.
  struct PriorityPages
  {
    vector<PageAddr> _topPriorityPages;
    vector<PageAddr> _highPriorityPages;
    vector<PageAddr> _mediumPriorityPages;
    vector<PageAddr> _lowPriorityPages;
  };

  map<int, PriorityPages> _prevSuperBoxPriorities;

  set<pair<int, int> > _nonEmptyQviews;

  SmRuntimeStats * _pRuntimeStats;
};

#endif
