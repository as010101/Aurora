#ifndef BUFFERCACHECONTROLBLOCK_H
#define BUFFERCACHECONTROLBLOCK_H

#include <buffercache/include/AvailablePageSet.H>
#include <buffercache/include/BufferCache_defs.H>
#include <buffercache/include/FramelessPagesMap.H>
#include <buffercache/include/PagesWithFramesMap.H>
#include <PtMutex.H>
#include <BinarySem.H>
#include <vector>
#include <map>

class BufferCacheControlBlock : public ILockable
{
public:
  // If pLazyWritesComplete isn't NULL, then it points to a BinarySem that
  // must exist as long as this BufferCacheControlBlock exists. Any time a
  // worker thread discovers that a lazy write has completed, this sem will be
  // posted to sometime before that worker thread releases its lock on this
  // BufferCacheControlBlock.
  BufferCacheControlBlock(size_t numFrames, int bytesPerFrame, 
			  BinarySem * pLazyWritesComplete = NULL)
    throw (exception);

  // Deletes all of the frame buffers, regardless of state. Do your final
  // work on disk *before* calling this method. This method throws out data.
  virtual ~BufferCacheControlBlock();

  virtual void lock()
    throw (exception);
  
  virtual void unlock()
    throw (exception);

  //=============================================================================
  // You MUST hold the lock whenever accessing any of the following members...
  //=============================================================================


  // Contains all frames that are in no way currently associated with a page.
  set<char *> _pagelessFrames;

  // Contains all pages that the user has specified a priority for, but which
  // aren't currently assigned to a frame.
  FramelessPagesMap _framelessPages;

  // Contains all pages/frames that are currently assigned to eachother 
  // regardless of whether or not the user is currently asking for it.
  PagesWithFramesMap _framedPages;
  //-----------------------------------------------------------------------------
  

  // This is used by the user to specify how many new pages it expects to need, 
  // and to receive those new pages. 
  //
  // When the user needs a new page, it gets it from here. When the user wants
  // to discard a page it has been using, it returns it to here.
  //
  // It's used by the worker threads to discover and fulfill the user's desires
  // for new pages.
  //
  // During shutdown, any pages still in this object will be returned to the
  // backing store.
  AvailablePageSet _availPages;

  //*****************************************************************************
  // MAINLY FOR USERS...
  //*****************************************************************************

  // Eliminates all entries in _status that only represent user priorities. For
  // the entries describe page/frame pairs, this converts all entries that
  // aren't undergoing I/O operations into frame-only entries.
  void clearUserPriorities()
    throw (exception);

  // Like clearUserPriorities(...), but only affects the specified pages.
  void clearUserPrioritiesByPage(const vector<PageAddr> & pages)
    throw (exception);

  // Ensures that _status notes, for each specified page, the priority the user
  // has given. This will modify existing entries for pages that are already
  // mapped, and add entries for pages that are absent from the map.
  //
  // This method also essentially calls "clearUserPriorities()" at its beginning,
  // so each call to this method completely overrides the previous call.
  //
  // Each priority must be in the range [1...10].
  //
  // This will *not* assign pages to frames. While it will update the priority
  // for any page that is currently assigned to a frame, all other pagees 
  // mentioned here will get put into _framelessPages. All assignment of pages
  // to frames (and evictions) happens as part of the method 
  // beginFrameLoadOps(...).
  void setPriorities(const vector<PageAddr> & pages,
		     const vector<int> & priorities)
    throw (exception);

  // Iff the page is mapped at all in this container, then 'isMapped' is
  // set to true. Otherwise, 'isMapped' is set to false.
  //
  // Iff 'isMapped' is set to true, then 'hasFrame' will be set to indicate
  // whether the page is stored in '_framelessPages' or '_framedPages'.
  void lookupPageAddr(const PageAddr & pa, bool & isMapped, bool & hasFrame)
    throw (exception);


  // Give the caller a way of knowing when it should be ok to obtain a
  // FRAMELOCK_READONLY lock on the specified page. (I.e., when the page is
  // framed and isn't being loaded from disk.)
  //
  // If the page is marked for deletion prior to becoming readable, the 
  // associated alert will never fire.
  //
  // Iff the page is already readable, then this method will return true and the
  // specified BinarySem is ignored.
  //
  // Iff the page isn't currently readable, then this method returns false, and
  // the specified BinarySem will be posted to when the page becomes readable.
  //
  // When the BinarySem is posted to, it's forgotten about. In fact, if this
  // method returns 'false', then the specified BinarySem must remain 
  // instantiated until one of the following events occurs:
  // - It's posted to because of this method, or
  // - The specified page is marked for deletion, or
  // - This BufferCacheControlBlock is deleted.
  bool setPageReadableOneShotAlert(const PageAddr & page,
				   BinarySem & alert)
    throw (exception);

  // Like setPageReadableOneShotAlert, but the standard is that the page must be
  // valid for FRAMELOCK_READONLY *or* FRAMELOCK_READWRITE locking by the user.
  bool setPageWritableOneShotAlert(const PageAddr & page,
				   BinarySem & alert)
    throw (exception);

  // Ensures that the page will be returned to _cb._availPages as soon as 
  // possible.
  void markPageForDeletion(const PageAddr & pa)
    throw (exception);


  //*****************************************************************************
  // MAINLY FOR WORKER THREADS...
  //*****************************************************************************

  // Picks up to the maxOps-best page/frame combinations for loading, marks them
  // as being in the "loading" state, and lists them in this method's output
  // parms. This clears "pages" and "frames" before populating them.
  void beginFrameLoadOps(size_t maxOps,
			 vector<PageAddr> & pages,
			 vector<char *> & frames)
    throw (exception);


  // Indicates that the specified frame buffer is now populated from disk. This
  // causes the appopriate entry in _status to be updated, and an alert to be
  // posted, if interest has been registered.
  // Throws an exception if pBuffer doesn't describe any entry in _status, or if
  // the state makes it hard to believe that a load operation had been underway.
  void onFrameLoadComplete(const PageAddr & pa)
    throw (exception);

  // Picks the maxOps-best frames for lazy writing, marks them as undergoing
  // writing, and lists them in this method's output parms. This clears 'pages'
  // and 'frames' before loading them.
  //
  // We call 'pages' and 'frames' .push_back(...) in a paired manner, s.t.
  // pages[i] and frames[i] together describe a single lazy write operation that
  // is to be peformed by the worker theads.
  void beginFrameLazyWriteOps(size_t maxOps,
			      vector<PageAddr> & pages,
			      vector<char *> & frames)
    throw (exception);

  // Indicates that the specified frame buffer is now consistent with disk. This
  // causes the appopriate entry in _status to be updated.
  // Throws an exception if pBuffer doesn't describe any entry in _status, or if
  // the state makes it hard to believe that a lazy write had been underway.
  void onFrameLazyWriteComplete(const PageAddr & pa)
    throw (exception);

  // Indicates that the specified pages have been allocated from the backing 
  // store. This method will add those pages to _availPages.
  void onNewPagesAvailable(const vector<PageAddr> & pages)
    throw (exception);

  // If a worker thread has made any call to onFrameLazyWriteComplete(...), then
  // it must call this method before releasing the lock on this 
  // BufferCacheControlBlock.
  void notifyLazyWritesComplete()
    throw (exception);

  //-----------------------------------------------------------------------------
  // Other...
  //-----------------------------------------------------------------------------

  string getDebugInfo() const
    throw (exception);

private:
  // Does any of the appopriate alert processing for the specified framed page, 
  // assuming that 'fpi' accurately reflects the current state of the page.
  void triggerAppropriateAlerts(const PageAddr & pa, 
				const PagesWithFramesMap::FramedPageInfo & fpi)
    throw (exception);

  // Takes frameless pages with the specified priority, and gives frames to as
  // many of them as possible. It does this by stealing frames from other pages 
  // that have an effective priority of 'framedPriority'.
  //
  // When this evicts a page, it will move that page to _framelessPages (if it
  // has a priority > 0) or the bit bucket (if it has a priority = 0).
  //
  // userLock, ioState, and bufferState give the values that will be specified in
  // the corresponding fields for the FramedPageInfo structure that's created for
  // each newly framed page
  //
  // This method won't perform more than maxOps evictions.
  //
  // We call .push_back(...) on 'pages' and 'frames', in a paired manner, to 
  // record the corresponding pages and frames that are paired together as a
  // result of this method call. 'page' and 'frames' are *not* cleared by this
  // method.
  //
  // This returns the number of evictions that were performed.
  int replaceFrameMappings(int framelessPriority, 
			   int framedPriority, 
			   FRAME_LOCK_TYPE userLock,
			   FRAME_IO_STATE ioState,
			   FRAME_BUFFER_STATE bufferState,
			   size_t maxOps,
			   vector<PageAddr> & pages,
			   vector<char *> & frames)
    throw (exception);

  // Evicts the specified number of frames with the specified effective priority,
  // and throws an exception if there aren't enouth to evict. Each evicted frame
  // is pushed to the back of 'evictedFrames'. This method does *not* clear
  // 'evictedFrames'.
  //
  // effectivePriority must be in the range [0, 10].
  //
  // Evicted pages with a non-zero userPriority are placed back into 
  // _framelessPages.
  void evictFrames(int effectivePriority, 
		   size_t numFrames, 
		   vector<char *> & evictedFrames)
    throw (exception);

  // Assigns up to the maxOps-most appropriate pages from _framelessPages to 
  // the frames found in _pagelessFrames.
  // Returns the number of such assignments that were made.
  // 'pages' and 'frames' are given pairwise push_back's of the paga/frame
  // assignments that are made.
  int assignPagelessFrames(size_t maxOps, 
			   vector<PageAddr> & pages,
			   vector<char *> & frames)
    throw (exception);


  PtMutex _mtx;
  size_t _numFrames;

  multimap<PageAddr, BinarySem *> _pageReadableAlerts;
  multimap<PageAddr, BinarySem *> _pageWritableAlerts;
  BinarySem * _pLazyWritesComplete;
};


#endif
