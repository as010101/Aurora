#include <buffercache/include/PagesWithFramesMap.H>
#include <StorageMgr_Exceptions.H>
#include <algorithm>
#include <sstream>
#include <assert.h>
#include <sstream>

#include <iostream> // only for debugging

PagesWithFramesMap::PagesWithFramesMap()
  throw (exception)
  : _numPinnedPages(0),
    _userPriorities(10),
    _effectivePriorities(11)
{
}

//===============================================================================

PagesWithFramesMap::~PagesWithFramesMap()
{
}

//===============================================================================

void PagesWithFramesMap::insert(const PageAddr & pa, const FramedPageInfo & newPfi)
  throw (exception)
{
  if ((newPfi._userPriority < 0) || (newPfi._userPriority > 10))
    {
      throw SmException(__FILE__, __LINE__, "priority is out of range.");
    }

  _pageToInfoMap.insert(make_pair(pa, newPfi));
  _userPriorities.setPageRank(pa, newPfi._userPriority);
  _effectivePriorities.setPageRank(pa, newPfi.getEffectivePriority());
}

//===============================================================================

void PagesWithFramesMap::clearPage(const PageAddr & pa)
  throw (exception)
{
  _pageToInfoMap.erase(pa);
  _userPriorities.erasePageRank(pa);
  _effectivePriorities.erasePageRank(pa);
}

//===============================================================================

void PagesWithFramesMap::clearUserPriorities()
  throw (exception)
{
  // We can skip oldPriority = 0, because that necessarily has a _userPriority
  // of 0.
  //
  // You might fear that the fact that we're changing the effective priorities
  // as we do an effective-priority-based walk might trip us up. However, the
  // only effective priority we can change a page to is 0. (If it won't go to
  // 0, it must be pinned at 11.), and we're not walking over the 0-priority
  // pages.

  map<PageAddr, FramedPageInfo>::iterator pagePos = _pageToInfoMap.begin();
  while (pagePos != _pageToInfoMap.end())
    {
      int oldUserPriority = pagePos->second._userPriority;

      if (oldUserPriority != 0)
	{
	  int oldEffectivePriority = pagePos->second.getEffectivePriority();
	  pagePos->second._userPriority = 0;
	  int newEffectivePriority = pagePos->second.getEffectivePriority();

	  if (oldEffectivePriority != newEffectivePriority)
	    {
	      _effectivePriorities.setPageRank(pagePos->first, newEffectivePriority);
	    }

	  _userPriorities.setPageRank(pagePos->first, 0);
	}

      ++pagePos;
    }
}

//===============================================================================

void PagesWithFramesMap::clearUserPriority(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::iterator pos = _pageToInfoMap.find(pa);
  if (pos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pos->second;
  if (fpi._userPriority != 0)
    {
      int oldEffectivePriority = fpi.getEffectivePriority();
      fpi._userPriority = 0;
      int newEffectivePriority = fpi.getEffectivePriority();

      if (oldEffectivePriority != newEffectivePriority)
	{
	  _effectivePriorities.setPageRank(pa, newEffectivePriority);
	}

      _userPriorities.setPageRank(pa, 0);
    }
}

//===============================================================================

void PagesWithFramesMap::clear()
  throw (exception)
{
  _pageToInfoMap.clear();
  _userPriorities.clear();
  _effectivePriorities.clear();
}

//===============================================================================

bool PagesWithFramesMap::findByPageAddr(const PageAddr & pa, 
					FramedPageInfo & pfi) const
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::const_iterator pos = _pageToInfoMap.find(pa);
  if (pos != _pageToInfoMap.end())
    {
      pfi = pos->second;
      return true;
    }
  else
    {
      return false;
    }
}

//===============================================================================

FRAME_LOCK_TYPE PagesWithFramesMap::getHighestAllowableUserLock(const PageAddr & pa) const
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::const_iterator pos = _pageToInfoMap.find(pa);
  if (pos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  return pos->second.getHighestAllowableUserLock();
}

//===============================================================================

void PagesWithFramesMap::setPageLockLevelNone(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::iterator pos = _pageToInfoMap.find(pa);
  if (pos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pos->second;
 
  int oldEffectivePriority = fpi.getEffectivePriority();
  fpi._userLock = FRAMELOCK_NONE;
  int newEffectivePriority = fpi.getEffectivePriority();

  if (oldEffectivePriority != newEffectivePriority)
    {
      _effectivePriorities.setPageRank(pa, newEffectivePriority);
    }
}

//===============================================================================

const char * PagesWithFramesMap::setPageLockLevelReadOnly(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::iterator pos = _pageToInfoMap.find(pa);
  if (pos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pos->second;

  if (fpi.getHighestAllowableUserLock() < FRAMELOCK_READONLY)
    {
      throw SmException(__FILE__, __LINE__, "Can't grant this level of lock right now");
    }
  
  int oldEffectivePriority = fpi.getEffectivePriority();
  fpi._userLock = FRAMELOCK_READONLY;
  int newEffectivePriority = fpi.getEffectivePriority();

  if (oldEffectivePriority != newEffectivePriority)
    {
      _effectivePriorities.setPageRank(pa, newEffectivePriority);
    }

  return fpi._pBuffer;
}

//===============================================================================

char * PagesWithFramesMap::setPageLockLevelReadWrite(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::iterator pos = _pageToInfoMap.find(pa);
  if (pos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pos->second;

  if (fpi.getHighestAllowableUserLock() < FRAMELOCK_READWRITE)
    {
      ostringstream os;
      os << "For " << pa.getDebugInfo() << endl
	 << "   Can't grant FRAMELOCK_READWRITE lock right now." << endl
	 << "   State = " << pos->second.getDebugInfo();
      throw SmException(__FILE__, __LINE__, os.str());
    }
  
  int oldEffectivePriority = fpi.getEffectivePriority();
  fpi._userLock = FRAMELOCK_READWRITE;
  int newEffectivePriority = fpi.getEffectivePriority();

  if (oldEffectivePriority != newEffectivePriority)
    {
      _effectivePriorities.setPageRank(pa, newEffectivePriority);
    }

  return fpi._pBuffer;
}

//===============================================================================

bool PagesWithFramesMap::atLeastOnePageUserLocked() const
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::const_iterator posInfo = _pageToInfoMap.begin();
  while (posInfo != _pageToInfoMap.end())
    {
      if (posInfo->second._userLock != FRAMELOCK_NONE)
	{
	  return true;
	}

      ++posInfo;
    }

  return false;
}

//===============================================================================
  
void PagesWithFramesMap::markPageDirty(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::iterator pos = _pageToInfoMap.find(pa);
  if (pos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pos->second;

  if (fpi._userLock != FRAMELOCK_READWRITE)
    {
      throw SmException(__FILE__, __LINE__, "Only r/w-locked pages can be marked dirty");
    }

  if (fpi._bufferState == BUFFERSTATE_DIRTIED_BY_WORKERTHREADS)
    {
      // Something's not right here. The worker threads are still bringing this
      // page's content into the frame from disk...
      throw SmException(__FILE__, __LINE__, "The page is still being loaded from disk.");
    }
  
  int oldEffectivePriority = fpi.getEffectivePriority();
  fpi._bufferState = BUFFERSTATE_DIRTIED_BY_USER;
  int newEffectivePriority = fpi.getEffectivePriority();

  if (oldEffectivePriority != newEffectivePriority)
    {
      _effectivePriorities.setPageRank(pa, newEffectivePriority);
    }
}

//===============================================================================

bool PagesWithFramesMap::markPageForDeletion(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::iterator pos = _pageToInfoMap.find(pa);
  if (pos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pos->second;

  if (fpi._ioState == FRAMEIO_IDLE)
    {
      clearPage(pa);
      return true;
    }
  else
    {
      fpi._markedForDeletion = true;
      return false;
    }
}

//===============================================================================

void PagesWithFramesMap::onFrameLoadComplete(const PageAddr & pa)
  throw (std::exception)
{
  //-----------------------------------------------------------------------------
  // Initial lookups...
  //-----------------------------------------------------------------------------
  map<PageAddr, FramedPageInfo>::iterator pagePos = _pageToInfoMap.find(pa);
  if (pagePos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pagePos->second;
  int oldEffectivePriority = fpi.getEffectivePriority();


  //-----------------------------------------------------------------------------
  // Verify that it looks like a frame that is being loaded...
  //-----------------------------------------------------------------------------

  if ((fpi._userLock != FRAMELOCK_NONE) ||
      (fpi._ioState != FRAMEIO_LOADING) ||
      (fpi._bufferState != BUFFERSTATE_DIRTIED_BY_WORKERTHREADS))
    {
      ostringstream os;
      os << "PagesWithFramesMap::onFrameLoadComplete(...) was handed a page "
	 << "that doesn't look like it was being loaded: " << endl
	 << pa.getDebugInfo() << endl
	 << fpi.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  //-----------------------------------------------------------------------------
  // Install the changes...
  //-----------------------------------------------------------------------------
  fpi._ioState     = FRAMEIO_IDLE;
  fpi._bufferState = BUFFERSTATE_CLEAN;

  int newEffectivePriority = fpi.getEffectivePriority();

  if (oldEffectivePriority != newEffectivePriority)
    {
      _effectivePriorities.setPageRank(pa, newEffectivePriority);
    }
}

//===============================================================================

void PagesWithFramesMap::beginFrameLazyWriteOps(size_t maxOps,
						vector<PageAddr> & pages,
						vector<char *> & buffers)
  throw (std::exception)
{
  pages.clear();
  buffers.clear();

  size_t numOps = 0;

  // We prefer to lazy-write lower-user-priority frames first, because those are 
  // the ones that we most want to be able to evict..
  for (int priority = 0; 
       (priority < 11) && (numOps < maxOps);
       ++priority)
    {
      const set<PageAddr> & pageSet = _userPriorities.getPageSetForRank(priority);
      set<PageAddr>::const_iterator priorityPagePos = pageSet.begin();

      while ((priorityPagePos != pageSet.end()) && (numOps < maxOps))
	{
	  const PageAddr & pa = *priorityPagePos;

	  map<PageAddr, FramedPageInfo>::iterator infoPos =  _pageToInfoMap.find(pa);
	  FramedPageInfo & fpi = infoPos->second;

	  if ((fpi._userLock    != FRAMELOCK_READWRITE) &&
	      (fpi._ioState     == FRAMEIO_IDLE)   &&
	      (fpi._bufferState == BUFFERSTATE_DIRTIED_BY_USER))
	    {
	      int oldEffectivePriority = fpi.getEffectivePriority();

	      fpi._ioState = FRAMEIO_SAVING;
	      pages.push_back(pa);
	      buffers.push_back(fpi._pBuffer);
	      ++numOps;

	      int newEffectivePriority = fpi.getEffectivePriority();

	      if (oldEffectivePriority != newEffectivePriority)
		{
		  _effectivePriorities.setPageRank(pa, newEffectivePriority);
		}
	    }

	  ++priorityPagePos;
	}
    }
}

//===============================================================================

void PagesWithFramesMap::onFrameLazyWriteComplete(const PageAddr & pa)
    throw (std::exception)
{
  //-----------------------------------------------------------------------------
  // Initial lookups...
  //-----------------------------------------------------------------------------
  map<PageAddr, FramedPageInfo>::iterator pagePos = _pageToInfoMap.find(pa);
  if (pagePos == _pageToInfoMap.end())
    {
      throw SmException(__FILE__, __LINE__, "The page isn't mapped.");
    }

  FramedPageInfo & fpi = pagePos->second;

  //-----------------------------------------------------------------------------
  // Verify that it looks like a frame that is being lazy written...
  //-----------------------------------------------------------------------------
  if (fpi._userLock == FRAMELOCK_READWRITE)
    {
      throw SmException(__FILE__, __LINE__, "fpi._userLock == FRAMELOCK_READWRITE");
    }

  if (fpi._ioState != FRAMEIO_SAVING)
    {
      throw SmException(__FILE__, __LINE__, "fpi._ioState != FRAMEIO_SAVING");
    }

  if (fpi._bufferState != BUFFERSTATE_DIRTIED_BY_USER)
    {
      throw SmException(__FILE__, __LINE__, "fpi._bufferState != BUFFERSTATE_DIRTIED_BY_USER");
    }

  //-----------------------------------------------------------------------------
  // Install the changes...
  //-----------------------------------------------------------------------------

  int oldEffectivePriority = fpi.getEffectivePriority();

  fpi._ioState = FRAMEIO_IDLE;
  fpi._bufferState = BUFFERSTATE_CLEAN;

  int newEffectivePriority = fpi.getEffectivePriority();

  if (oldEffectivePriority != newEffectivePriority)
    {
      _effectivePriorities.setPageRank(pa, newEffectivePriority);
    }
}

//===============================================================================

bool PagesWithFramesMap::FramedPageInfo::operator< (const FramedPageInfo & rhs) const
{
  // "NO SORT FOR YOU!!!" -- The Sort Nazi.
  return false;
}

//===============================================================================

int PagesWithFramesMap::FramedPageInfo::getEffectivePriority() const
{
  if ((_userLock != FRAMELOCK_NONE)                 ||
      (_ioState  != FRAMEIO_IDLE)                   ||
      (_bufferState == BUFFERSTATE_DIRTIED_BY_USER) ||
      (_bufferState == BUFFERSTATE_DIRTIED_BY_WORKERTHREADS))
    {
    return 11;
    }
  else
    {
    return _userPriority;
    }
}

//===============================================================================

string PagesWithFramesMap::FramedPageInfo::getDebugInfo(int indentLevel) const
  throw (exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "PagesWithFramesMap::FramedPageInfo (this = " << reinterpret_cast<const void *>(this) << ")" << endl;

  os << indentStr << "   _userPriority = " << _userPriority << endl;

  os << indentStr << "   _userLock = ";
  switch (_userLock)
    {
    case FRAMELOCK_NONE:
      os << "FRAMELOCK_NONE" << endl;
      break;
    case FRAMELOCK_READONLY:
      os << "FRAMELOCK_READONLY" << endl;
      break;
    case FRAMELOCK_READWRITE:
      os << "FRAMELOCK_READWRITE" << endl;
      break;
    default:
      os << "<<< ILLEGAL VALUE: " << _userLock << ">>>" << endl;
    }

  os << indentStr << "   _ioState = ";
  switch (_ioState)
    {
    case FRAMEIO_IDLE:
      os << "FRAMEIO_IDLE" << endl;
      break;
    case FRAMEIO_LOADING:
      os << "FRAMEIO_LOADING" << endl;
      break;
    case FRAMEIO_SAVING:
      os << "FRAMEIO_SAVING" << endl;
      break;
    default:
      os << "<<< ILLEGAL VALUE: " << _ioState << ">>>" << endl;
    }

  os << indentStr << "   _bufferState = ";
  switch (_bufferState)
    {
    case BUFFERSTATE_INIT:
      os << "BUFFERSTATE_INIT" << endl;
      break;
    case BUFFERSTATE_CLEAN:
      os << "BUFFERSTATE_CLEAN" << endl;
      break;
    case BUFFERSTATE_DIRTIED_BY_USER:
      os << "BUFFERSTATE_DIRTIED_BY_USER" << endl;
      break;
    case BUFFERSTATE_DIRTIED_BY_WORKERTHREADS:
      os << "BUFFERSTATE_DIRTIED_BY_WORKERTHREADS" << endl;
      break;
    default:
      os << "<<< ILLEGAL VALUE: " << _bufferState << ">>>" << endl;
    }

  os << indentStr << "   _pBuffer = " << reinterpret_cast<const void*>(_pBuffer) << endl
     << indentStr << "   _markedForDeletion = " << _markedForDeletion;

  return os.str();
}

//===============================================================================

void PagesWithFramesMap::updateMappedPriorities(const vector<PageAddr> & srcPages,
						const vector<int>      & srcPriorities,
						vector<PageAddr>       & unmappedPages,
						vector<int>            & unmappedPriorities)
  throw (exception)
{
  unmappedPages.clear();
  unmappedPriorities.clear();


  int numSrcPages = srcPages.size();
  map<PageAddr, FramedPageInfo>::const_iterator pageEndPos = _pageToInfoMap.end();

  for (int i = 0; i < numSrcPages; i++)
    {
      const PageAddr & pa = srcPages[i];
      int newUserPriority = srcPriorities[i];

      map<PageAddr, FramedPageInfo>::iterator pagePos = _pageToInfoMap.find(pa);

      if (pagePos != pageEndPos)
	{
	  FramedPageInfo & fpi = pagePos->second;

	  if (fpi._userPriority != newUserPriority)
	    {
	      _userPriorities.setPageRank(pa, newUserPriority);

	      int oldEffectivePriority = fpi.getEffectivePriority();
	      fpi._userPriority = newUserPriority;
	      int newEffectivePriority = fpi.getEffectivePriority();

	      if (oldEffectivePriority != newEffectivePriority)
		{
		  _effectivePriorities.setPageRank(pa, newEffectivePriority);
		}
	    }
	}
      else
	{
	  unmappedPages.push_back(pa);
	  unmappedPriorities.push_back(newUserPriority);
	}
    }
}

//===============================================================================

const set<PageAddr> & 
PagesWithFramesMap::getPagesByEffectivePriority(int effectivePriority) const
  throw (exception)
{
  return _effectivePriorities.getPageSetForRank(effectivePriority);
}

//===============================================================================

int PagesWithFramesMap::stealAndModifyFrames(int oldEffectivePriority,
					     const vector<PageAddr> & newPages,
					     int newUserPriority,
					     FRAME_LOCK_TYPE userLock,
					     FRAME_IO_STATE ioState,
					     FRAME_BUFFER_STATE bufferState,
					     size_t maxOps,
					     vector<PageAddr> & evictedPages,
					     vector<char *> & stolenFrames)
  throw (exception)
{
  //-----------------------------------------------------------------------------
  // Parameter checking....
  //-----------------------------------------------------------------------------
  if ((oldEffectivePriority < 0) || (oldEffectivePriority > 10))
    {
      throw SmException(__FILE__, __LINE__, 
				  "(oldEffectivePriority < 0) || (oldEffectivePriority > 10)");
    }
  
  if ((newUserPriority < 0) || (newUserPriority > 10))
    {
      throw SmException(__FILE__, __LINE__, 
				  "(newUserPriority < 0) || (newUserPriority > 10)");
    }

  evictedPages.clear();
  stolenFrames.clear();

  //-----------------------------------------------------------------------------
  // Decide on the victim pages...
  //-----------------------------------------------------------------------------
  const set<PageAddr> & victimPageLiveSet = 
    _effectivePriorities.getPageSetForRank(oldEffectivePriority);

  size_t numPagesToEvict = min(maxOps, victimPageLiveSet.size());

  evictedPages.reserve(numPagesToEvict);

  set<PageAddr>::const_iterator pos = victimPageLiveSet.begin();
  for (size_t i = 0; i < numPagesToEvict; ++i)
    {
      evictedPages.push_back(*pos);
      ++pos;
    }

  //-----------------------------------------------------------------------------
  // Perform the evictions...
  //-----------------------------------------------------------------------------
  for (size_t i = 0; i < numPagesToEvict; ++i)
    {
      // Recover the frame...
      map<PageAddr, FramedPageInfo>::iterator pos2 = 
	_pageToInfoMap.find(evictedPages[i]);

      stolenFrames.push_back(pos2->second._pBuffer);

      // Remove the old owning page from this structure...
      _pageToInfoMap.erase(evictedPages[i]);
      _userPriorities.erasePageRank(evictedPages[i]);
      _effectivePriorities.erasePageRank(evictedPages[i]);

      // Establish the replacement page as the owner of the frame...
      FramedPageInfo fpi;
      fpi._userLock     = userLock;
      fpi._ioState      = ioState;
      fpi._bufferState  = bufferState;
      fpi._userPriority = newUserPriority;
      fpi._pBuffer      = stolenFrames[i];

      _pageToInfoMap.insert(make_pair(newPages[i], fpi));
      _userPriorities.setPageRank(newPages[i], fpi._userPriority);
      _effectivePriorities.setPageRank(newPages[i], fpi.getEffectivePriority());
    }

  assert(stolenFrames.size() == evictedPages.size());
  assert(stolenFrames.size() == numPagesToEvict);
  return numPagesToEvict;
}

//===============================================================================

void PagesWithFramesMap::deleteAllBuffers()
  throw (exception)
{
  map<PageAddr, FramedPageInfo>::iterator pos =  _pageToInfoMap.begin();
  while (pos != _pageToInfoMap.end())
    {
      delete[](pos->second._pBuffer);
      pos->second._pBuffer = NULL;
      ++pos;
    }
}

//===============================================================================

string PagesWithFramesMap::getDebugInfo(int indentLevel) const
  throw (exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "PagesWithFramesMap (this = " << reinterpret_cast<const void *>(this) << ")" << endl;

  for (int i = 0; i <= 11; ++i)
    {
      const set<PageAddr> & pages = _effectivePriorities.getPageSetForRank(i);
      os << indentStr << "   Effective priority = " << i << ": # pages = " << pages.size() << endl;
    }

  return os.str();
}

//===============================================================================

FRAME_LOCK_TYPE PagesWithFramesMap::FramedPageInfo::getHighestAllowableUserLock() const
  throw (exception)
{
  switch (_ioState)
    {
    case FRAMEIO_IDLE:
      return FRAMELOCK_READWRITE;
    case FRAMEIO_SAVING:
      return FRAMELOCK_READONLY;
    case FRAMEIO_LOADING:
      return FRAMELOCK_NONE;
    default:
      throw SmException(__FILE__, __LINE__, "Illegal _ioState value.");
    }
}

//===============================================================================
