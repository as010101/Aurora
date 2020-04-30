#include <buffercache/include/BufferCacheControlBlock.H>
#include <StorageMgr_Exceptions.H>
#include <algorithm>
#include <iostream>

//===============================================================================

BufferCacheControlBlock::BufferCacheControlBlock(size_t numFrames, 
						 int bytesPerFrame, 
						 BinarySem * pLazyWritesComplete)
  throw (exception)
  : _numFrames(numFrames),
    _pLazyWritesComplete(pLazyWritesComplete)
{
  for (size_t i = 0; i < _numFrames; i++)
    {
      char * pFrame = new char[bytesPerFrame];
      _pagelessFrames.insert(pFrame);
    }
}

//===============================================================================

BufferCacheControlBlock::~BufferCacheControlBlock()
{
  set<char *>::iterator pos1 = _pagelessFrames.begin();
  while (pos1 != _pagelessFrames.end())
    {
      delete[](*pos1);
      ++pos1;
    }

  _framedPages.deleteAllBuffers();
}

//===============================================================================

void BufferCacheControlBlock::lock()
  throw (exception)
{
  _mtx.lock();
}

//===============================================================================
  
void BufferCacheControlBlock::unlock()
  throw (exception)
{
  _mtx.unlock();
}

//===============================================================================

void BufferCacheControlBlock::clearUserPriorities()
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  _framelessPages.clear();
  _framedPages.clearUserPriorities();
}

//===============================================================================

void BufferCacheControlBlock::clearUserPrioritiesByPage(const vector<PageAddr> & pages)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  PagesWithFramesMap::FramedPageInfo fpi;
  size_t numPages = pages.size();

  for (size_t i = 0; i < numPages; ++i)
    {
      if (_framedPages.findByPageAddr(pages[i], fpi))
	{
	  _framedPages.clearUserPriority(pages[i]);
	}
      else
	{
	  _framelessPages.clearPage(pages[i]);
	}
    }
}

//===============================================================================

void BufferCacheControlBlock::setPriorities(const vector<PageAddr> & pages,
					    const vector<int> & priorities)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  // I have no question that this could be optimized significantly, especially
  // for the case where a page is being given the same priority now as it had
  // before. -cjc

  _framelessPages.clear();
  _framedPages.clearUserPriorities();

  size_t numPages = pages.size();

  if (numPages != priorities.size())
    {
      throw SmException(__FILE__, __LINE__, "pages.size() != priorities.size()");
    }

  vector<PageAddr> unmappedPages;
  unmappedPages.reserve(numPages);

  vector<int> unmappedPriorities;
  unmappedPriorities.reserve(numPages);

  _framedPages.updateMappedPriorities(pages, priorities, 
				      unmappedPages, unmappedPriorities);

  int numUnmappedPages = unmappedPages.size();
  for (int i = 0; i < numUnmappedPages; i++)
    {
      _framelessPages.setMapping(unmappedPages[i], unmappedPriorities[i]);
    }
}

//===============================================================================

void BufferCacheControlBlock::lookupPageAddr(const PageAddr & pa, bool & isMapped, bool & hasFrame)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  int dummy;
  if (_framelessPages.findByPageAddr(pa,dummy))
    {
      isMapped = true;
      hasFrame = false;
      return;
    }

  PagesWithFramesMap::FramedPageInfo dummy2;
  if (_framedPages.findByPageAddr(pa, dummy2))
    {
      isMapped = true;
      hasFrame = true;
      return;
    }

  isMapped = false;
  hasFrame = false;
  return;
}

//===============================================================================

bool BufferCacheControlBlock::setPageReadableOneShotAlert(const PageAddr & page,
							  BinarySem & alert)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  PagesWithFramesMap::FramedPageInfo fpi;

  if (_framedPages.findByPageAddr(page, fpi) &&
      (fpi.getHighestAllowableUserLock() >= FRAMELOCK_READONLY))
    {
      return true;
    }
  else
    {
      _pageReadableAlerts.insert(make_pair(page, & alert));
      return false;
    }
}

//===============================================================================

bool BufferCacheControlBlock::setPageWritableOneShotAlert(const PageAddr & page,
							  BinarySem & alert)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  PagesWithFramesMap::FramedPageInfo fpi;

  if (_framedPages.findByPageAddr(page, fpi) &&
      (fpi.getHighestAllowableUserLock() >= FRAMELOCK_READWRITE))
    {
      return true;
    }
  else
    {
      _pageWritableAlerts.insert(make_pair(page, & alert));
      return false;
    }
}

//===============================================================================

void BufferCacheControlBlock::markPageForDeletion(const PageAddr & pa)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  int priority;
  if (_framelessPages.findByPageAddr(pa, priority))
    {
      // It's got a priority, but isn't framed, so we can return it to the free
      // pool right away...
      _framelessPages.clearPage(pa);
      _availPages.addPage(pa);
      return;
    }

  PagesWithFramesMap::FramedPageInfo fpi;
  if (_framedPages.findByPageAddr(pa, fpi))
    {
      bool deleted = _framedPages.markPageForDeletion(pa);
      if (deleted)
	{
	  _availPages.addPage(pa);
	}
    }  
}
//===============================================================================

void BufferCacheControlBlock::beginFrameLoadOps(size_t maxOps, 
						vector<PageAddr> & pages,
						vector<char *> & frames)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  pages.clear();
  frames.clear();

  // Don't start evicting pages before using all of our pageless frames...
  size_t numOps = assignPagelessFrames(maxOps, pages, frames);
  if (numOps == maxOps)
    {
      return;
    }

  // Step 2: Now to a priority-priority comparision between the pages that don't 
  // have frames and those that do.
  //
  // Note the way we do this: For unframed pages, we go from highest priority (10)
  // to lowest priority (1). For framed pages, we go from lowes priority (0) to 
  // highest (10). 
  //
  // If you think about this for a minute, you'll see that it give us the 
  // ultimate effect of having the proper pages assigned to frames.
  //
  // The reason we use simple user priority instead of effective priority:
  // For the unframed pages is that, their effective priority = user priority.
  // For framed pages, we're only considering the ones with effective priority
  // in [0...10] (since 11 == pinned), and in that range of effective priorities,
  // user priority == effective priority. User priority is faster to determine
  // than effective priority.

  // This looping might be optimizable, but we'll keep it simple initially...
  bool moreEvictionsPossible = true;

  for (int framelessPriority = 10; 
       (framelessPriority >= 1) && moreEvictionsPossible && (numOps < maxOps);
       --framelessPriority)
    {
      const set<PageAddr> & pageSet = 
	_framelessPages.getPagesForPriority(framelessPriority);

      for (int framedPriority = 0;
	   (framedPriority < framelessPriority) && 
	     (! pageSet.empty()) && 
	     (numOps < maxOps);
	   ++framedPriority)
	{
	  numOps += replaceFrameMappings(framelessPriority, 
					 framedPriority,
					 FRAMELOCK_NONE,
					 FRAMEIO_LOADING,
					 BUFFERSTATE_DIRTIED_BY_WORKERTHREADS,
					 (maxOps - numOps),
					 pages,
					 frames);
	}

      // If we evicted every frame we could and still have unframed pages left
      // over, then in future iterations of this loop where framelessPriority
      // is lower than it is right now, there's no hope of evicting a frame...
      if (! pageSet.empty())
	{
	  moreEvictionsPossible = false;
	}
    }
}

//===============================================================================

void BufferCacheControlBlock::onFrameLoadComplete(const PageAddr & pa)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);
  PagesWithFramesMap::FramedPageInfo fpi;
  if (! _framedPages.findByPageAddr(pa, fpi))
    {
      throw SmException(__FILE__, __LINE__, 
			"Internal error: Page not framed when it should be");
    }

  if (fpi._markedForDeletion)
    {
      _framedPages.clearPage(pa);
    }
  else
    {
      _framedPages.onFrameLoadComplete(pa);
    }

  // We need to get an updated fpi, because the _ioState was modified by our
  // call to _framedPages.onFrameLoadComplete, and our next call to 
  // triggeAppropriateAlerts(...) needs an up-to-date value for that field.
  //
  // We should probably get a modified version of findByPageAddr(...) that
  // returns a references to the actual FPI stored in the map, if that's a safe
  // thing to do. That would let us reduce the number of times we have to re-find
  // the same structure in that map.
  if (! _framedPages.findByPageAddr(pa, fpi))
    {
      throw SmException(__FILE__, __LINE__, 
			"Internal error: Page not framed when it should be");
    }

  triggerAppropriateAlerts(pa, fpi);
}

//===============================================================================

void BufferCacheControlBlock::beginFrameLazyWriteOps(size_t maxOps,
						     vector<PageAddr> & pages,
						     vector<char *> & buffers)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  _framedPages.beginFrameLazyWriteOps(maxOps, pages, buffers);
}

//===============================================================================

void BufferCacheControlBlock::onFrameLazyWriteComplete(const PageAddr & pa)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  PagesWithFramesMap::FramedPageInfo fpi;
  if (! _framedPages.findByPageAddr(pa, fpi))
    {
      throw SmException(__FILE__, __LINE__, 
			"Internal error: Page not framed when it should be");
    }

  if (fpi._markedForDeletion)
    {
      _framedPages.clearPage(pa);
    }
  else
    {
      _framedPages.onFrameLazyWriteComplete(pa);
    }

  // We need to get an updated fpi, because the _ioState was modified by our
  // call to _framedPages.onFrameLoadComplete, and our next call to 
  // triggeAppropriateAlerts(...) needs an up-to-date value for that field.
  //
  // We should probably get a modified version of findByPageAddr(...) that
  // returns a references to the actual FPI stored in the map, if that's a safe
  // thing to do. That would let us reduce the number of times we have to re-find
  // the same structure in that map.
  if (! _framedPages.findByPageAddr(pa, fpi))
    {
      throw SmException(__FILE__, __LINE__, 
			"Internal error: Page not framed when it should be");
    }

  triggerAppropriateAlerts(pa, fpi);
}

//===============================================================================

void BufferCacheControlBlock::onNewPagesAvailable(const vector<PageAddr> & pages)
    throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  int numPages = pages.size();
  for (int i = 0; i < numPages; i++)
    {
      _availPages.addPage(pages[i]);
    }
}

//===============================================================================

void BufferCacheControlBlock::notifyLazyWritesComplete()
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  if (_pLazyWritesComplete != NULL)
    _pLazyWritesComplete->post();
}

//===============================================================================

string BufferCacheControlBlock::getDebugInfo() const
  throw (exception)
{
  return "TBD";
}

//===============================================================================

void BufferCacheControlBlock::triggerAppropriateAlerts(const PageAddr & pa, 
						       const PagesWithFramesMap::FramedPageInfo & fpi)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  switch (fpi.getHighestAllowableUserLock())
    {
    case FRAMELOCK_READONLY:
      for (multimap<PageAddr, BinarySem *>::iterator pos = 
	     _pageReadableAlerts.lower_bound(pa);
	   pos != _pageReadableAlerts.upper_bound(pa);)
	{
	  pos->second->post();
	  _pageReadableAlerts.erase(pos++);
	}

      break;

    case FRAMELOCK_READWRITE:
      for (multimap<PageAddr, BinarySem *>::iterator pos = 
	     _pageWritableAlerts.lower_bound(pa);
	   pos != _pageWritableAlerts.upper_bound(pa);)
	{
	  pos->second->post();
	  _pageWritableAlerts.erase(pos++);
	}

      // Writable also implies readable...
      for (multimap<PageAddr, BinarySem *>::iterator pos = 
	     _pageReadableAlerts.lower_bound(pa);
	   pos != _pageReadableAlerts.upper_bound(pa);)
	{
	  pos->second->post();
	  _pageReadableAlerts.erase(pos++);
	}

      break;

    default:
      break;
    }
}

//===============================================================================

int BufferCacheControlBlock::replaceFrameMappings(int framelessPriority, 
						  int framedPriority,
						  FRAME_LOCK_TYPE userLock,
						  FRAME_IO_STATE ioState,
						  FRAME_BUFFER_STATE bufferState,
						  size_t maxOps,
						  vector<PageAddr> & pages,
						  vector<char *> & frames)
    throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  //-----------------------------------------------------------------------------
  // Check parameters...
  //-----------------------------------------------------------------------------

  // Don't let the user try to evict pages that are pinned...
  if ((framedPriority < 0) || (framedPriority > 10))
    {
      throw SmException(__FILE__, __LINE__, 
				  "(framedPriority < 0) || (framedPriority > 10)");
    }
  
  //-----------------------------------------------------------------------------
  // Get the sets we're working with...
  //-----------------------------------------------------------------------------
  const set<PageAddr> & liveFramelessPageSet 
    = _framelessPages.getPagesForPriority(framelessPriority);

  const set<PageAddr> & liveFramedPageSet = 
    _framedPages.getPagesByEffectivePriority(framedPriority);

  int numFramesToEvict = 
    min(min(liveFramelessPageSet.size(), liveFramedPageSet.size()), maxOps);

  //-----------------------------------------------------------------------------
  // Start moving data...
  //-----------------------------------------------------------------------------

  // Evict the victim pages, and remember their frame buffers. We know that all
  // frames pages at this priority level are evictable, since we guaranteed above
  // that they don't have a priority of 11...
  size_t idxFirstEvictedFrame = frames.size();
  evictFrames(framedPriority, numFramesToEvict, frames);

  // Assign our new, better pages into the evicted frames...
  for (int i = 0; i < numFramesToEvict; ++i)
    {
      PagesWithFramesMap::FramedPageInfo fpi;
      fpi._userPriority      = framelessPriority;
      fpi._userLock          = userLock;
      fpi._ioState           = ioState;
      fpi._bufferState       = bufferState;
      fpi._pBuffer           = frames[i + idxFirstEvictedFrame];
      fpi._markedForDeletion = false;

      PageAddr pa = *(liveFramelessPageSet.begin());
      _framelessPages.clearPage(pa);

      // Careful: We have to push_back these pages onto 'pages' in the exact same
      // order as we push_back'ed frames onto 'frames', because the two vectors
      // are paired.
      pages.push_back(pa);

      _framedPages.insert(pa, fpi);
    }

  return numFramesToEvict;
}

//===============================================================================

void BufferCacheControlBlock::evictFrames(int effectivePriority, 
					  size_t numFrames, 
					  vector<char *> & evictedFrames)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  if ((effectivePriority < 0) || (effectivePriority > 10))
    {
      throw SmException(__FILE__, __LINE__, 
				  "(effectivePriority < 0) || (effectivePriority > 10)");
    }

  const set<PageAddr> & liveFramedPageSet = 
    _framedPages.getPagesByEffectivePriority(effectivePriority);

  if (liveFramedPageSet.size() < numFrames)
    {
      throw SmException(__FILE__, __LINE__, 
				  "Not enough frames for eviction");
    }

  evictedFrames.reserve(evictedFrames.size() + numFrames);

  for (size_t i = 0; i < numFrames; i++)
    {
      const PageAddr & pa = *(liveFramedPageSet.begin());

      PagesWithFramesMap::FramedPageInfo fpi;

      bool success = _framedPages.findByPageAddr(pa, fpi);
      if (! success)
	{
	  throw SmException(__FILE__, __LINE__, 
				      "Internal mapping inconsistency");
	}

      evictedFrames.push_back(fpi._pBuffer);

      if (fpi._userPriority > 0)
	{
	  _framelessPages.setMapping(pa, fpi._userPriority);
	}

      _framedPages.clearPage(pa);
    }
}

//===============================================================================

int BufferCacheControlBlock::assignPagelessFrames(size_t maxOps, 
						  vector<PageAddr> & pages,
						  vector<char *> & frames)
    throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  size_t numOps = 0;

  for (int pagePriority = 10; 
       ((pagePriority > 0) && (! _pagelessFrames.empty()) && (numOps < maxOps)); 
       --pagePriority)
    {
      const set<PageAddr> & pageSet = _framelessPages.getPagesForPriority(pagePriority);

      while ((! pageSet.empty()) && 
	     (! _pagelessFrames.empty()) && 
	     (numOps < maxOps))
	{
	  // Grab some unused frame buffer...
	  set<char *>::iterator posBuffer = _pagelessFrames.begin();

	  PagesWithFramesMap::FramedPageInfo fpi;
	  fpi._userPriority      = pagePriority;
	  fpi._userLock          = FRAMELOCK_NONE;
	  fpi._ioState           = FRAMEIO_LOADING;
	  fpi._bufferState       = BUFFERSTATE_DIRTIED_BY_WORKERTHREADS;
	  fpi._pBuffer           = *posBuffer;
	  fpi._markedForDeletion = false;

	  _pagelessFrames.erase(posBuffer);

	  // Grab some random PageAddr that we'd like to give a frame to...
	  PageAddr pa = *(pageSet.begin());
	  _framelessPages.clearPage(pa);

	  // Record the marriage...
	  _framedPages.insert(pa, fpi);

	  // Tell the caller about the marriage...
	  pages.push_back(pa);
	  frames.push_back(fpi._pBuffer);

	  ++numOps;
	}
    }

  return numOps;
}

