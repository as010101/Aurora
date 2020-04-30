#ifndef PAGEPRIORITYTRACKER_H
#define PAGEPRIORITYTRACKER_H

#include <iolib/include/PageAddr.H>
#include <buffercache/include/BufferCache_defs.H>
#include <set>
#include <map>
#include <vector>

// Tracks the stats necessary to keep page priorities optimal.
class PagePriorityTracker
{
public:
  PagePriorityTracker()
    throw (exception);

  //-----------------------------------------------------------------------------

  // Describes a particular level of caching need that the user needs to express 
  // for some page(s)...
  typedef enum UserPriority
    {
      USER_PRIORITY_PIN_READWRITE = 5,
      USER_PRIORITY_PIN_READONLY  = 4,
      USER_PRIORITY_TOP           = 3,
      USER_PRIORITY_HIGH          = 2,
      USER_PRIORITY_MEDIUM        = 1,
      USER_PRIORITY_LOW           = 0
    };

  //-----------------------------------------------------------------------------

  // Describes a particular level of caching that actually will be observed for
  // some page(s)...
  typedef enum EffectivePriority
    {
      // Those pages that have at least one count of the following user 
      // priorities will go into this group:
      //    USER_PRIORITY_PIN_READWRITE,
      //    USER_PRIORITY_PIN_READONLY, or
      //    USER_PRIORITY_TOP.
      EFFECTIVE_PRIORITY_TOP = 5,

      // Those pages whose highest level of user priority is USER_PRIORITY_HIGH
      // will go into this group.
      EFFECTIVE_PRIORITY_HIGH = 4,

      // Those pages whose highest level of user priority is USER_PRIORITY_MEDIUM
      // will go into this group.
      EFFECTIVE_PRIORITY_MEDIUM = 3,

      // Those pages whose highest level of user priority is USER_PRIORITY_LOW
      // will go into this group.
      EFFECTIVE_PRIORITY_LOW = 2,

      // Those pages that have no count for any user priority will go into this
      // group.
      EFFECTIVE_PRIORITY_NONE = 1
    };

  //-----------------------------------------------------------------------------

  // Increments (by 1) the specified priority counts for the specified pages.
  void increaseUseCounts(const vector<PageAddr> & topUserPriorityPages,
			 const vector<PageAddr> & highUserPriorityPages,
			 const vector<PageAddr> & mediumUserPriorityPages,
			 const vector<PageAddr> & lowUserPriorityPages)
    throw (exception);

  // Decrements (by 1) the specified priority counts for the specified pages.
  void decreaseUseCounts(const vector<PageAddr> & topPriorityPages,
			 const vector<PageAddr> & highPriorityPages,
			 const vector<PageAddr> & mediumPriorityPages,
			 const vector<PageAddr> & lowPriorityPages)
    throw (exception);

  // Returns true if at least one of the specified pages has a non-zero priority 
  // or is pinned in any way.
  bool areAnyPagesPrioritizedOrPinned(const vector<PageAddr> & pages)
    throw (exception);

  // Gives a live reference to the set of all PageAddr's that whose effective
  // priority is 'pc'. Note that EFFECTIVE_PRIORITY_NONE may not be passed to 
  // this method.
  const set<PageAddr> & getPageSetByEffectivePriority(EffectivePriority ep)
    throw (exception);

  // Increments the pinning count for the specified page. Iff this returns true, 
  // the caller is obligated to actually set the frame's lock level to the value
  // specified by myLockType. That implies the caller should only call this
  // method when he knows pinning is immanently possible.
  bool incrementPinCount(const PageAddr & pa,
			 FRAME_LOCK_TYPE myLockType)
    throw (exception);

  // Opposite of incrementPinCount(...). Iff this returns true, the caller is
  // obligated to set the frame's lock type to newLockType before unlocking the 
  // BufferCacheControlBlock.
  bool decrementPinCount(const PageAddr & pa, 
			 FRAME_LOCK_TYPE myLockType, 
			 FRAME_LOCK_TYPE & newLockType)
    throw (exception);

  string getDebugInfo() const
    throw (exception);

  // Does an internal consistency check. Calls abort() when a problem is found.
  void debugCheck() const;

private:
  // Adds 'modification' to each of the specified pages' count for the given 
  // priority, and makes any necessary adjustments to this object's data 
  // structures. Note that 'modification' can be negative to cause a count to 
  // decrement.
  void modifyPageUserPriorities(const vector<PageAddr> & pages, UserPriority up, 
				int modification)
    throw (exception);

  void moveToProperContainer(const PageAddr & pa, 
			     EffectivePriority oldEffectivePriority, 
			     EffectivePriority newEffectivePriority)
    throw (exception);

  //-----------------------------------------------------------------------------

  struct PagePriorityStats
  {
    PagePriorityStats();

    PagePriorityStats(size_t topUserPriority, 
		      size_t highUserPriority, 
		      size_t mediumUserPriority, 
		      size_t lowUserPriority,
		      size_t pinCountReadOnly,
		      size_t pinCountReadWrite);

    size_t _userPriorityCounts[USER_PRIORITY_PIN_READWRITE + 1];

    // Returns the highest priority that has at least one superbox needing it.
    EffectivePriority getEffectivePriority() const
      throw (exception);

    // Returns the lock level that this frame should have.
    FRAME_LOCK_TYPE getFrameLock() const;

    string getDebugInfo(int indentLevel = 0) const;
  };

  //-----------------------------------------------------------------------------

  // Has one entry for every page with a non-trivial priority or that's pinned.
  map<PageAddr, PagePriorityStats> _pageToPriorityStatsMap;

  // Has one entry for every page with a non-trivial priority. The dependent 
  // value is the set of all PageAddr's whose getPriority() method returns
  // that set's index in this array.
  set<PageAddr> _topEffectivePriorityPages;
  set<PageAddr> _highEffectivePriorityPages;
  set<PageAddr> _mediumEffectivePriorityPages;
  set<PageAddr> _lowEffectivePriorityPages;
};

#endif
