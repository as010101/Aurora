#ifndef PAGESWITHFRAMESMAP_H
#define PAGESWITHFRAMESMAP_H

#include <exception>
#include <map>
#include <vector>
#include <iolib/include/PageAddr.H>
#include <buffercache/include/SortedPageMap.H>
#include <buffercache/include/BufferCache_defs.H>

using namespace std;

// We introduce the idea of *effective* priority.
// Basically, user priority remains contstrained to the range [1...10].
// An effective priority has these meanings:
// 0 : The user has no standing prioritization for this page, and it's OK to 
//     evict the page from the frame if need be.
// 1...10: The user's stated priority for the page remains in effect.
// 11: For some reason, the frame's page priority is temporarily boosted so that
//     the page won't be evicted from the frame. This is how we show pinning.
//
// A PageAddr only appears in this container if it's currently assigned to a 
// frame. It might be assigned a frame because the user wants it to be, or 
// because it's not a good time to evict the page, or because the worker threads
// just having gotten around to changing the page/frame mapping yet.
class PagesWithFramesMap
{
public:
  PagesWithFramesMap()
    throw (exception);

  virtual ~PagesWithFramesMap();

  //-----------------------------------------------------------------------------

  struct FramedPageInfo
  {
    // This can be in the range 0...10. 0 means the user has no standing setting
    // for this page's priority.
    int _userPriority;

    FRAME_LOCK_TYPE _userLock;
    FRAME_IO_STATE _ioState;
    FRAME_BUFFER_STATE _bufferState;
    char * _pBuffer;

    bool _markedForDeletion;

    int getEffectivePriority() const;

    bool operator< (const FramedPageInfo & rhs) const;

    string getDebugInfo(int indentLevel = 0) const
      throw (exception);

    FRAME_LOCK_TYPE getHighestAllowableUserLock() const
      throw (exception);
  };

  //-----------------------------------------------------------------------------

  // Sets or overwrites the info for the specified page. Note that this pretty 
  // effecient in handling mere changes to a page's FramedPageInfo, also.
  void insert(const PageAddr & pa, const FramedPageInfo & newPfi)
    throw (exception);

  // Ensures that the specified page has no mapping in this container.
  void clearPage(const PageAddr & pa)
    throw (exception);

  // Sets the _userPriority of every page in this map to 0. If this causes a 
  // change in effective priority, the page is moved to the proper priority map,
  // resulting in updates to _pageIndex and _priorityMaps.
  void clearUserPriorities()
    throw (exception);

  // Like clearUserPriorities(), but it only works on the specified page. Throws
  // an exception if the page isn't framed.
  void clearUserPriority(const PageAddr & pa)
    throw (exception);

  // Clears all of the mappings in this container.
  void clear()
    throw (exception);

  // Lookups up the mapping for the specified page. If found, sets 'pfi'
  // accordingly and returns 'true'. Otherwise, returns 'false'.
  bool findByPageAddr(const PageAddr & pa, FramedPageInfo & pfi) const
    throw (exception);

  // Gives the highest locking level that the user can currently succeed in
  // acquiring for the specified page. Throws an exception if the page isn't
  // mapped.
  FRAME_LOCK_TYPE getHighestAllowableUserLock(const PageAddr & pa) const
    throw (exception);

  // Releases any lock the user has on the page. The page must be framed.
  void setPageLockLevelNone(const PageAddr & pa)
    throw (exception);

  // Adjusts the user's lock to read-only. The page must be framed, and the 
  // worker threads must not presently hold a lock on the page (that only
  // happens during page loading anyway).
  // The return value is a read-only pointer to the frame. It's illegal to
  // modify that buffer using this return value.
  const char * setPageLockLevelReadOnly(const PageAddr & pa)
    throw (exception);

  // Adjusts the user's lock to read-write. The page must be framed, and the 
  // worker threads must not presently hold a lock on the page (that only
  // happens during page loading anyway).
  // The return value is a read-write pointer to the frame. If you modify
  // the buffer, you're obligated to mark the page dirty before lowering
  // the lock level.
  char * setPageLockLevelReadWrite(const PageAddr & pa)
    throw (exception);

  // Indicates whether or not at least one page frame currently has a 
  // user-lock other than FRAMELOCK_NONE.
  bool PagesWithFramesMap::atLeastOnePageUserLocked() const
    throw (exception);
  
  // Marks the page dirty. It's only legal to call this method when the page
  // has a user-lock-level of read-write.
  void markPageDirty(const PageAddr & pa)
    throw (exception);

  // If the page is in the map, it will either be deleted from the map 
  // immediately (method returns true), or will be marked for deletion (method
  // returns false).
  //
  // If the page was only marked for deletion, a BufferCache worker thread will
  // be obligated to delete the page from this PagesWithFramesMap and return it
  // to the _availPages pool ASAP.
  bool markPageForDeletion(const PageAddr & pa)
    throw (exception);

  // Adjusts the page's FramePageInfo accordingly, and will handle the shuffling
  // around needed if this results in a change to the page's effective priority.
  //
  // Throws an exception if the page isn't in this container, or if its staring
  // state is obviously not that of a page that's being loaded.
  void onFrameLoadComplete(const PageAddr & pa)
    throw (std::exception);

  // Picks up to the maxOps-best frames for lazy writing, marks them as 
  // undergoing writing, and lists them in this method's output parms. This 
  // clears 'pages' and 'frames' before loading them.
  //
  // We call 'pages' and 'frames' .push_back(...) in a paired manner, s.t.
  // pages[i] and frames[i] together describe a single lazy write operation that
  // is to be peformed by the worker theads.
  void beginFrameLazyWriteOps(size_t maxOps,
			      vector<PageAddr> & pages,
			      vector<char *> & frames)
    throw (std::exception);
  
  // Adjusts the page's FramePageInfo accordingly, and will handle the shuffling
  // around needed if this results in a change to the page's effective priority.
  //
  // Throws an exception if the page isn't in this container, or if its staring
  // state is obviously not that of a page that's being lazy written.
  void onFrameLazyWriteComplete(const PageAddr & pa)
    throw (std::exception);

  // Clears 'unmappedPages' and 'unmappedPriorities'. Then...
  // For each page/priority pair given by the first two parameters:
  // (A) If the page is mapped in this container, this method ensure that the
  //     page's _userPriority is set to the supplied value. Otherwise...
  // (B) The page/priority pair is push_back(...)'d to 'unmappedPages' and
  //     'unmappedPriorities'.
  void updateMappedPriorities(const vector<PageAddr> & srcPages,
			      const vector<int>      & srcPriorities,
			      vector<PageAddr>       & unmappedPages,
			      vector<int>            & unmappedPriorities)
    throw (exception);


  // Returns a live reference to the set of all pages with the specified priority.
  // The priority must be in the range [0,11].
  const set<PageAddr> & getPagesByEffectivePriority(int effectivePriority) const
    throw (exception);

  // This will evict up to maxOps pages at the specified effective priority.
  //
  // This operates on up to maxOps pages that have an effective priority of
  // 'oldEffectivePriority'. oldEffectivePriority must be in [0...10].
  //
  // This method clears 'pagesGivenFrames' and 'stolenFrames'.
  //
  // For the ith eviction, this method will:
  //  - Assign the frame to newPages[i-1]
  //  - Give the page a userPriority of newPriorities[i-1]
  //  - Append to evictedPages and stolenFrames, respectively, the page
  //    and frame that had been previously tied.
  //  - Modify the frame to have the specified values for 'userLock', 'ioState',
  //    and 'bufferState'.
  //
  // This returns the number of evictions that were performed. 
  // newPages[0...(rc-1)] have been assigned to frames.
  int stealAndModifyFrames(int oldEffectivePriority,
			   const vector<PageAddr> & newPages,
			   int newUserPriority,
			   FRAME_LOCK_TYPE userLock,
			   FRAME_IO_STATE ioState,
			   FRAME_BUFFER_STATE bufferState,
			   size_t maxOps,
			   vector<PageAddr> & evictedPages,
			   vector<char *> & stolenFrames)
    throw (exception);

  // For every page in this container, the _pBuffer is delete[]'ed and set to NULL.
  void deleteAllBuffers()
    throw (exception);

  string getDebugInfo(int indentLevel = 0) const
    throw (exception);

private:
  int _numPinnedPages;
  map<PageAddr, FramedPageInfo> _pageToInfoMap;
  SortedPageMap _userPriorities;
  SortedPageMap _effectivePriorities;
};


#endif
