#ifndef AVAILABLEPAGESET_H
#define AVAILABLEPAGESET_H

#include <BinarySem.H>
#include <iolib/include/PageAddr.H>
#include <set>
#include <vector>
#include <map>
#include <queue>

using namespace std;

/**
 This is a pool of PageAddr objects that are available for immediate
 use. They've already been allocated from the AsynchPageDir, and
 are awaiting being put to actual use.

 Note that this object doesn't do the actual work of acquiring the
 PageAddrs or of returning extra ones. It lets some other, external worker 
 thread keep it populated.

 NOTE: We offer two intended interfaces onto this class. Interface #1 was done
 initially, and interface #2 was done after knowing our actual needs better.
 I kept iface #1 because we had unit tests written against it. It's a good 
 candidate for omission in a rewrite (if we don't entirely change the way we
 get new pages anyway).

 */
class AvailablePageSet 
{
public:
  AvailablePageSet()
    throw (std::exception);
  
  virtual ~AvailablePageSet();

  //=============================================================================  
  // INTERFACE VARIETY #1
  //=============================================================================  

  // Sets the number of pages that this object should have read for immediate
  // allocation.
  //
  // Note that setting this to a value less than the current page count for this
  // object will might cause the expulsion of PageAddrs from this object.
  void setDesiredPageCount(size_t numPages)
    throw (std::exception);

  void incrementDesiredPageCount(size_t numPages)
    throw (std::exception);
  
  // If there's at least one PageAddr in this object, then a PageAddr
  // is returned via the 'pa' parameter and this method returns 'true'.
  // Otherwise, the method returns 'false'.
  // 
  // When a PageAddr is returned, the 'decrementDesiredCount' parameter becomes
  // relevant:
  // Iff 'decrementDesiredCount', then the number of PageAddrs that this object
  // would like to keep on hand is reduced by one. (The reduction will be skipped
  // if it would cause a negative value for the 'desired page count').
  bool getPageNonBlocking(PageAddr & pa, bool decrementDesiredCount)
    throw (std::exception);

  //=============================================================================  
  // INTERFACE VARIETY #2
  //=============================================================================  

  // Specifies how many new pages should be allocated for the specified superbox.
  // This adds to any number previously requested. 
  // If 'readyFlag' differs from a previously supplied value for this 
  // superBoxId, the old readyFlag will be posted to when requests issued under a
  // the earlier readyFlag become  available.
  //
  // 'numPagesWanted' must be > 0.
  //
  // Iff at least one page is immediately available, this returns 'true' and 
  // 'readyFlag' will not be recorded by this object.
  //
  // Otherwise, this will return 'false', and each time more pages are placed
  // into the superBoxId's mailbox, 'readyFlag' will be posted.
  bool requestPages(int superBoxId, size_t numPagesWanted, BinarySem & readyFlag)
    throw (exception);

  // For the given superBoxId, this tells the number of pages that are currently
  // ready and awaiting pickup. 'superBoxId' must have been an argument to an
  // earlier call to requestPages(...).
  size_t getNumPagesAvailable(int superBoxId)
    throw (exception);

  // Removes 'numWantedNow' pages from this superBoxId's mailbox, and puts them
  // into 'pages'. ('pages' is cleared by this method before being added to.)
  // The number of pages available must be >= 'numWantedNow', or else an 
  // exception is thrown.
  void getPages(int superBoxId, vector<PageAddr> & pages, size_t numWantedNow)
    throw (exception);

  //=============================================================================  
  
  // Adds the specified PageAddr to this object's pool. If the PageAddr is
  // already in the pool, and exception will be thrown.
  void addPage(PageAddr pa)
    throw (std::exception);

  // Returns the number of PageAddr's that this object would currently
  // like to have on hand.
  int getDesiredPageCount() const
      throw (std::exception);
  
  // Returns the number of PageAddr's that this object currently has 
  // in stock (and not assigned to any particular mailbox).
  int getCurrentPageCount() const
    throw (std::exception);

private:
  long _desiredPageCount;
  set<PageAddr> _pagePool;

  struct PageRequest
  {
    PageRequest(int superBoxId, size_t numPagesStillNeeded, BinarySem & readyFlag)
      : _superBoxId(superBoxId),
	_numPagesStillNeeded(numPagesStillNeeded),
	_readyFlag(readyFlag)
    {
    }
    
    PageRequest() : _readyFlag(* (new BinarySem())) {}

    PageRequest(const PageRequest & rhs);

    PageRequest & operator= (const PageRequest & rhs) {assert(false); return *this;}

    int _superBoxId;
    size_t _numPagesStillNeeded;
    BinarySem & _readyFlag;
  };

  queue<PageRequest> _pendingRequests;

  map<int, vector<PageAddr> > _mailboxes;
};

#endif
