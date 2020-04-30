#include <sm/include/StorageMgr.H>
#include <xercesDomUtil.H>
#include <XmlTempString.H>
#include <util.H>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sys/time.h>

// just for debugging...
#include <iostream>
#include <pthread.h>

//===============================================================================

StorageMgr::StorageMgr(string dirPathname, 
		       size_t bytesPerPage,
		       size_t maxConcurrency,
		       size_t numPageFrames)
  throw (exception)
  : _numPageFrames(numPageFrames),
    _bytesPerPage(bytesPerPage),
    _maxConcurrency(maxConcurrency),
    _dirPathname(ensureStringEndsWith(dirPathname, '/')),
    _xmlFilename(_dirPathname + "StorageManager.xml"),
    _admissionPacer(_numPageFrames),
    _bufferCache(dirPathname, _bytesPerPage, _maxConcurrency, _numPageFrames, 
		 _dummySem, _dummySem),
    _pRuntimeStats(SmRuntimeStats::getInstance())
{
  if (fileExists(_xmlFilename))
    {
      LockHolder lh(_mtx);
      loadFromFile(_xmlFilename);
    }
}

//===============================================================================

StorageMgr::StorageMgr(const PropsFile & startupProps)
  throw (exception)
  : _numPageFrames(  startupProps.getUInt("StorageMgr.numPageFrames")),
    _bytesPerPage(   startupProps.getUInt("StorageMgr.bytesPerPage")),
    _maxConcurrency( startupProps.getUInt("StorageMgr.maxConcurrency")),
    _dirPathname(    ensureStringEndsWith(startupProps.getString("StorageMgr.dirPathname"), '/')),
    _xmlFilename(    startupProps.getString("StorageMgr.dirPathname") + "StorageManager.xml"),
    _admissionPacer( startupProps.getUInt("StorageMgr.numPageFrames")),
    _bufferCache(    startupProps.getString("StorageMgr.dirPathname"), _bytesPerPage, _maxConcurrency, _numPageFrames, 
		 _dummySem, _dummySem),
    _pRuntimeStats(SmRuntimeStats::getInstance())
{
  if (fileExists(_xmlFilename))
    {
      LockHolder lh(_mtx);
      loadFromFile(_xmlFilename);
    }
}


//===============================================================================

StorageMgr::~StorageMgr()
{
  SmRuntimeStats::releaseInstance();
}

//===============================================================================

void StorageMgr::shutdown()
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);
  if (_admissionPacer.getNumScheduledSuperBoxes() > 0)
    {
      throw SmException(__FILE__, __LINE__, 
			"Called StorageMgr::shutdown() when superboxes "
			"are still scheduled.");
    }

  _bufferCache.beginShutdown();
  saveToFile(_xmlFilename);
  _bufferCache.awaitShutdown();
}

//===============================================================================

void StorageMgr::lock()
  throw (exception)
{
  _mtx.lock();
}

//===============================================================================
  
void StorageMgr::unlock()
  throw (exception)
{
  _mtx.unlock();
}

//===============================================================================

void StorageMgr::createQ(int qbaseId,
			 const vector<int> & qviewIds,
			 size_t bytesPerRecord, 
			 bool cxnPoint)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  if (bytesPerRecord < sizeof(timeval))
    {
      throw SmException(__FILE__, __LINE__, "record size too small to hold a timestamp");
    }

  size_t slotsPerPage = _bytesPerPage / bytesPerRecord;

  if (slotsPerPage == 0)
    {
      throw SmException(__FILE__, __LINE__, "record size exceeds page size");
    }

  pair<map<int, QMetadata>::iterator, bool> rc = 
    _queueMap.insert(make_pair(qbaseId, 
			       QMetadata(_bytesPerPage, bytesPerRecord, cxnPoint)));

  if (! rc.second)
    {
      throw SmException(__FILE__, __LINE__, "qbaseId is already defined");
    }

  QTopology & qt = rc.first->second._qt;

  size_t numQviews = qviewIds.size();
  for (size_t i = 0; i < numQviews; ++i)
    {
      qt.createQView(qviewIds[i]);
    }
}

//===============================================================================

void StorageMgr::createCxnPointQView(int qbaseId,
				     int qviewId,
				     bool limitTuplesByNum,
				     size_t maxNumTuples,
				     bool limitTuplesByAge,
				     unsigned int maxTupleAgeSeconds)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  map<int, QMetadata>::iterator pos = _queueMap.find(qbaseId);
  if (pos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "qbaseId isn't defined");
    }

  pos->second._qt.createCxnPointQView(qviewId,
				      limitTuplesByNum,
				      maxNumTuples,
				      limitTuplesByAge,
				      maxTupleAgeSeconds);
}

//===============================================================================

void StorageMgr::deleteQView(int qbaseId, int qviewId)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  map<int, QMetadata>::iterator pos = _queueMap.find(qbaseId);
  if (pos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "qbaseId isn't defined");
    }

  QTopology & qt = pos->second._qt;
  qt.deleteQView(qbaseId, qviewId);
}

//===============================================================================

void StorageMgr::setCxnPointHistorySpec(int qbaseId,
					bool limitTuplesByNum,
					size_t maxNumTuples,
					bool limitTuplesByAge,
					unsigned int maxTupleAgeSeconds)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  map<int, QMetadata>::iterator pos = _queueMap.find(qbaseId);
  if (pos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "qbaseId isn't defined");
    }

  QTopology & qt = pos->second._qt;
  qt.setCxnPointHistorySpec(limitTuplesByNum,
			    maxNumTuples,
			    limitTuplesByAge,
			    maxTupleAgeSeconds);
}

//===============================================================================

void StorageMgr::deleteQ(int qbaseId)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);
  LockHolder lh(_bufferCache._cb);

  map<int, QMetadata>::iterator pos = _queueMap.find(qbaseId);
  if (pos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "qbaseId isn't defined");
    }

  const vector<PageAddr> & pages = pos->second._qt.getPageRing();

  // Return the pages to the freepool...
  size_t numPages = pages.size();
  for (size_t i = 0; i < numPages; ++i)
    {
      _bufferCache._cb.markPageForDeletion(pages.at(i));
    }

  // And finally...
  _queueMap.erase(pos);
}

//===============================================================================

void StorageMgr::getQviewInfo(int qbaseId, 
			      int qviewId, 
			      size_t & numRecsInQview, 
			      size_t & numRecsFramed,
			      timeval & avgTimestamp)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  map<int, QMetadata>::const_iterator pos = _queueMap.find(qbaseId);
  if (pos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "qbaseId isn't defined");
    }

  // Discover numRecsInQview...
  const QTopology & qt = pos->second._qt;
  qt.getQviewStats(qviewId, numRecsInQview, avgTimestamp);

  if (numRecsInQview == 0)
    {
      numRecsFramed = 0;
    }
  else
    {
      // Discover numRecsFramed... 
      size_t firstPageIdx;
      vector<size_t> numRecs;
      size_t numPageParts = numRecs.size();

      qt.getPageOccupancy(qviewId, firstPageIdx, numRecs);
      const vector<PageAddr> & pageVector =  qt.getPageRing();
      size_t numPages = pageVector.size();

      bool done = false;
      size_t i = 0;
      numRecsFramed = 0;

      {
	LockHolder lh(_bufferCache._cb);

	const PagesWithFramesMap & pfm = _bufferCache._cb._framedPages;
	PagesWithFramesMap::FramedPageInfo fpi;

	// We know it's ok to go through the loop at least once, because the qview
	// isn't empty...
	while (! done)
	  {
	    if (i == numPageParts)
	      {
		done = true;
	      }

	    const PageAddr & pa = pageVector.at((firstPageIdx + i) % numPages);
	    //	    const PageAddr & pa = pageVector[(firstPageIdx + i) % numPages];
	    if (pfm.findByPageAddr(pa, fpi))
	      {
		// The page is framed...
		numRecsFramed += numRecs.at(i);	    
	      }
	    else
	      {
		// The page wasn't framed. That's the end of our range of contiguous
		// pages.
		done = true;
	      }

	    ++i;
	  }
      }
    }
}

//===============================================================================

const set<pair<int, int> > & StorageMgr::getNonEmptyQviews() const
  throw (exception)
{
  return _nonEmptyQviews;
}

//===============================================================================

SmRuntimeStatsSnapshot StorageMgr::getRuntimeStatsSnapshot()
  throw (exception)
{
  return _pRuntimeStats->getSnapshot();
}
 
//===============================================================================

void StorageMgr::scheduleSuperBox(int superBoxId, 
				  const vector<SuperBoxPlanStep> & schedule)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  // The following maps should be empty, if the specified superbox id isn't
  // currently in use...
  if (! _prevSuperBoxPriorities.insert(make_pair(superBoxId, PriorityPages())).second)
    {
      throw SmException(__FILE__, __LINE__, 
			"Map had an entry when it shouldn't. superBoxId already scheduled?");
    }

  _admissionPacer.submitSuperBoxPlan(superBoxId, schedule);

  discardPagePriorities(superBoxId);
  establishNeededPagePriorities(superBoxId, schedule, 0);

  {
    LockHolder lh(_bufferCache._cb);
    updateBcPagePriorities();
  }
}

//===============================================================================

bool StorageMgr::notifyWhenSuperBoxStepRunnable(int superBoxId, BinarySem & flag)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  return _admissionPacer.awaitNextSuperBoxStep(superBoxId, flag);
}

//===============================================================================

bool StorageMgr::onSuperBoxStepComplete(int superBoxId)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  // Get a *live* reference to this superbox's info. When we call 
  // onSuperBoxStepComplete, the information pointed to by this sbi will be
  // changed as well...
  const SuperBoxPacer::SuperBoxPlanInfo & sbi = 
    _admissionPacer.getSuperBoxInfo(superBoxId);

  // Release the old priorities associated with this step.
  bool hasMoreSteps = _admissionPacer.onSuperBoxStepComplete(superBoxId);
  if (hasMoreSteps)
    {
      discardPagePriorities(superBoxId);
      establishNeededPagePriorities(superBoxId, sbi._plan, sbi._currentStep);
    }
  else
    {
      discardPagePriorities(superBoxId);

      // Later on, we might consider simply clearing the vectors rather than
      // deleting them. However, at this point it's better to avoid the risk of
      // accidentally picking up priorities from previous schedules of a given
      // supertrain, because it's less error prone.
      _prevSuperBoxPriorities.erase(superBoxId);
    }

  return hasMoreSteps;
}

//===============================================================================

const SuperBoxPlanStep & StorageMgr::getCurrentSuperBoxStep(int superBoxId) const 
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  return _admissionPacer.getCurrentSuperBoxStep(superBoxId);
}

//===============================================================================

void StorageMgr::awaitEnqPossible(int superBoxId, int qbaseId)
  throw (exception)
{
  LockHolder lhSm(_mtx);

  //-----------------------------------------------------------------------------
  // Step 1: Discover if we need to do page addition, and if so, do it...
  //-----------------------------------------------------------------------------
  map<int, QMetadata>::iterator queuePos =  _queueMap.find(qbaseId);
  if (queuePos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "Specified qbase isn't defined");
    }

  QTopology & qt = queuePos->second._qt;

  BinarySem aFlag;

  // If this is true, then later in our code we'll need to pin *both* enqPage
  // and newPage and do the split memcpy.
  bool mustDoPageSplit = false;

  // These are only meaningful if mustDoPageSplit is true...
  PageAddr newPage, enqPage;
  size_t firstSlotToCopy, lastSlotToCopy;

  if (qt.getEnqCapacity() == 0)
    {
      // We need to get the bc page priorities to account for the newly added page.
      // We'll undo our reference counts now, and then redo them when the new page
      // is part of the queue. This is an unbelievably ineffecient approach, btw.
      const SuperBoxPacer::SuperBoxPlanInfo & sbi = 
	_admissionPacer.getSuperBoxInfo(superBoxId);

      discardPagePriorities(superBoxId);

      establishNeededPagePriorities(superBoxId, sbi._plan, sbi._currentStep);

      bool atLeastOnePageReady = 
	_bufferCache._cb._availPages.requestPages(superBoxId, 1, aFlag);

      if (atLeastOnePageReady)
	{
	  _pRuntimeStats->onNewPageGrantImmediate();
	}
      else
	{
	  timeval t1, t2;
	  gettimeofday(& t1, NULL);

	  {
	    LockHolder lhBccb(_bufferCache._cb);
	    _bufferCache.awakenWorkerThreads();
	  }

	  lhSm.release();
	  aFlag.awaitPostThenClear();

	  gettimeofday(& t2, NULL);
	  _pRuntimeStats->onNewPageGrantDelayed(t1, t2);

	  lhSm.reacquire();
	}

      vector<PageAddr> newPageVector;
      _bufferCache._cb._availPages.getPages(superBoxId, 
					    newPageVector, 1);

      newPage = newPageVector.at(0);
      //      newPage = newPageVector[0];

      if (qt.doesAddPageRequireSplit())
	{
	  mustDoPageSplit = true;
	  qt.addPageWithSplit(newPage, enqPage, firstSlotToCopy, lastSlotToCopy);
	  
	  // All we did was add one page to one queue. We've got to find a 
	  // lighter-weight way of causing it to be given a high priority...
	  updateBcPagePriorities();
	}
      else
	{
	  qt.addPageWithoutSplit(newPage);
	}

      discardPagePriorities(superBoxId);
      establishNeededPagePriorities(superBoxId, sbi._plan, sbi._currentStep);
    }


  //-----------------------------------------------------------------------------
  // Pin the enq page...
  //-----------------------------------------------------------------------------

  IntraPageSlotRange enqRegion = qt.getNextEnqRegion();

  bool mustPinEnqPage = 
    _neededPagePriorities.incrementPinCount(enqRegion._pa, FRAMELOCK_READWRITE);

  char * pEnqPageFrame;

  if (mustPinEnqPage)
    {
      // It's not framed and/or we have lock contention...

      // If we can pin it now, great. Otherwise, we're going to have to let the
      // BufferCache know how important it is to us and then wait...
      BinarySem alert;

      bool canWriteLockNow;
      {
	LockHolder lhBccb(_bufferCache._cb);
	canWriteLockNow = _bufferCache._cb.setPageWritableOneShotAlert(enqRegion._pa, alert);
      }

      if (canWriteLockNow)
	{
	  _pRuntimeStats->onFramingAlreadyFramed();

	  // Later on we might look into making a single call to the following
	  // method, at the end of this method, s.t.:
	  // - we defer the call if we don't need its results to be immediate
	  //   (i.e., we don't need to start waiting on a page that might not
	  //   be framed unless we call the method immdiately,), and
	  // - we don't call it at all if we don't have any change to actually
	  //   push out.
	  LockHolder lhBccb(_bufferCache._cb);
	  updateBcPagePriorities();
	}
      else
	{
	  // Just for stats collection...
	  PagesWithFramesMap::FramedPageInfo pfi;
	  bool wasFramed = 
	    _bufferCache._cb._framedPages.findByPageAddr(enqRegion._pa, pfi);
	  
	  timeval t1;
	  if (wasFramed)
	    {
	      _pRuntimeStats->onFramingAlreadyFramed();
	    }
	  else
	    {
	      gettimeofday(& t1, NULL);
	    }

	  {
	    LockHolder lhBccb(_bufferCache._cb);
	    updateBcPagePriorities();
	    _bufferCache.awakenWorkerThreads();
	  }

	  lhSm.release();
	  alert.awaitPostThenClear();

	  if (! wasFramed)
	    {
	      timeval t2;
	      gettimeofday(& t2, NULL);
	      _pRuntimeStats->onFramingDelayed(t1, t2);
	    }

	  lhSm.reacquire();
	}

      pEnqPageFrame = 
	_bufferCache._cb._framedPages.setPageLockLevelReadWrite(enqRegion._pa);
    }
  else
    {
      _pRuntimeStats->onFramingAlreadyFramed();

      PagesWithFramesMap::FramedPageInfo fpi;
      {
	LockHolder lhBccb(_bufferCache._cb);
	_bufferCache._cb._framedPages.findByPageAddr(enqRegion._pa, fpi);
      }

      if (fpi._userLock != FRAMELOCK_READWRITE)
	{
	  throw SmException(__FILE__, __LINE__, "Frame wasn't already locked");
	}

      pEnqPageFrame = fpi._pBuffer;
    }

  //-----------------------------------------------------------------------------
  // If we did a page split, do the pinning/copying/unpinning for that...
  //-----------------------------------------------------------------------------

  if (mustDoPageSplit)
    {    
      //  Pin the new page's frame...
      if (enqPage != enqRegion._pa)
	{
	  throw SmException(__FILE__, __LINE__, "Unexpected page mismatch");
	}

      bool mustPinNewPage = 
	_neededPagePriorities.incrementPinCount(newPage, FRAMELOCK_READWRITE);

      char * pNewPageFrame;

      if (mustPinNewPage)
	{
	  // It's not framed and/or we have lock contention...
	  LockHolder lhBccb(_bufferCache._cb);

	  // If we can pin it now, great. Otherwise, we're going to have to let the
	  // BufferCache know how important it is to us and then wait...
	  BinarySem alert;

	  bool canWriteLockNow = 
	    _bufferCache._cb.setPageWritableOneShotAlert(newPage, alert);

	  if (canWriteLockNow)
	    {
	      _pRuntimeStats->onFramingAlreadyFramed();

	      // Later on we might look into making a single call to the following
	      // method, at the end of this method, s.t.:
	      // - we defer the call if we don't need its results to be immediate
	      //   (i.e., we don't need to start waiting on a page that might not
	      //   be framed unless we call the method immdiately,), and
	      // - we don't call it at all if we don't have any change to actually
	      //   push out.
	      updateBcPagePriorities();
	    }
	  else
	    {
	      timeval t1, t2;
	      gettimeofday(& t1, NULL);

	      updateBcPagePriorities();
	      _bufferCache.awakenWorkerThreads();

	      lhBccb.release();
	      lhSm.release();
	      alert.awaitPostThenClear();

	      gettimeofday(& t2, NULL);
	      _pRuntimeStats->onFramingDelayed(t1, t2);

	      lhSm.reacquire();
	      lhBccb.reacquire();
	    }

	  pNewPageFrame = 
	    _bufferCache._cb._framedPages.setPageLockLevelReadWrite(newPage);
	}
      else
	{
	  _pRuntimeStats->onFramingAlreadyFramed();

	  PagesWithFramesMap::FramedPageInfo fpi;
	  LockHolder lhBccb(_bufferCache._cb);
	  _bufferCache._cb._framedPages.findByPageAddr(newPage, fpi);

	  if (fpi._userLock != FRAMELOCK_READWRITE)
	    {
	      throw SmException(__FILE__, __LINE__, "Frame wasn't already locked");
	    }

	  pNewPageFrame = fpi._pBuffer;
	}

      // Do the actual copying...
      size_t bufferOffset = _bytesPerPage * firstSlotToCopy;
      size_t bytesToCopy = (lastSlotToCopy - firstSlotToCopy + 1) * _bytesPerPage;

      memcpy(pNewPageFrame + bufferOffset, 
	     pEnqPageFrame + bufferOffset, 
	     bytesToCopy);

      // Unpin / mark dirty the new page...
      {
	LockHolder lhBccb(_bufferCache._cb);
	_bufferCache._cb._framedPages.markPageDirty(newPage);
      }

      FRAME_LOCK_TYPE newLockType;
      bool mustUnpinNewPage = 
	_neededPagePriorities.decrementPinCount(newPage, FRAMELOCK_READWRITE, 
						newLockType);
      if (mustUnpinNewPage)
	{
	  LockHolder lhBccb(_bufferCache._cb);
	  switch (newLockType)
	    {
	    case FRAMELOCK_READWRITE:
	      throw SmException(__FILE__, __LINE__, "Unexpected framelock level");
	      break;
	    case FRAMELOCK_READONLY:
	      _bufferCache._cb._framedPages.setPageLockLevelReadOnly(newPage);
	      break;
	    case FRAMELOCK_NONE:
	      updateBcPagePriorities();
	      _bufferCache._cb._framedPages.setPageLockLevelNone(newPage);
	      break;
	    }

	  _bufferCache.awakenWorkerThreads();
	}
    }
}

//===============================================================================

void StorageMgr::getEnqBuffer(int superBoxId, int qbaseId, size_t & numRecs, 
			      char * & pBuffer)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  LockHolder lh(_bufferCache._cb);

  map<int, QMetadata>::iterator queuePos = _queueMap.find(qbaseId);
  if (queuePos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "Specified qbase isn't defined");
    }

  QMetadata & qmd =queuePos->second; 
  QTopology & qt = qmd._qt;
  IntraPageSlotRange enqRegion = qt.getNextEnqRegion();

  PagesWithFramesMap::FramedPageInfo fpi;
  _bufferCache._cb._framedPages.findByPageAddr(enqRegion._pa, fpi);

  if (fpi._userLock != FRAMELOCK_READWRITE)
    {
      throw SmException(__FILE__, __LINE__, "Frame wasn't already locked");
    }

  pBuffer = fpi._pBuffer + (enqRegion._lowSlot * qmd._bytesPerRecord);
  numRecs = enqRegion._highSlot - enqRegion._lowSlot + 1;
}

//===============================================================================

void StorageMgr::releaseEnqBuffer(int superBoxId, int qbaseId, 
				  size_t numRecsEnqueued)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  if (numRecsEnqueued > 0)
    {
      // Tally the timestamps of the records that were just enqueued.
      //
      // (Later on, we may want to require a 1:1 corrspondance betwwen calls to
      // getEnqBuffer(...) and releaseEnqBuffer(...). That would let use cache
      // this information (enqRegion and the buffer address) when 
      // getEnqBuffer(...) was called, and uncache it when releaseEnqBuffer was
      // called. -cjc
      //
      // Later on, we may want to make the *caller* supply the tallied seconds
      // and microseconds. They had to iterate over the records anyway, probably.
      // - cjc
      map<int, QMetadata>::iterator queuePos = _queueMap.find(qbaseId);
      if (queuePos == _queueMap.end())
	{
	  throw SmException(__FILE__, __LINE__, "Specified qbase isn't defined");
	}

      QMetadata & qmd = queuePos->second;
      QTopology & qt = qmd._qt;
      IntraPageSlotRange enqRegion = qt.getNextEnqRegion();

      PagesWithFramesMap::FramedPageInfo fpi;
      _bufferCache._cb._framedPages.findByPageAddr(enqRegion._pa, fpi);
      
      if (fpi._userLock != FRAMELOCK_READWRITE)
	{
	  throw SmException(__FILE__, __LINE__, "Frame wasn't already locked");
	}
      
      const char * pBuffer = fpi._pBuffer + (enqRegion._lowSlot * qmd._bytesPerRecord);

      unsigned long long enqTstampSecSum = 0;
      unsigned long long enqTstampMicroSecSum = 0;
      timeval tv;

      for (size_t i = 0; i < numRecsEnqueued; ++i)
	{
	  memcpy(& tv, pBuffer, sizeof(tv));
	  enqTstampSecSum += tv.tv_sec;
	  enqTstampMicroSecSum += tv.tv_usec;

	  pBuffer += qmd._bytesPerRecord;
	}


      // Mark the page dirty, and perhaps unpin it...
      LockHolder lh(_bufferCache._cb);
      _bufferCache._cb._framedPages.markPageDirty(enqRegion._pa);

      FRAME_LOCK_TYPE newLockType;
      bool mustUnpinEnqPage = 
	_neededPagePriorities.decrementPinCount(enqRegion._pa, FRAMELOCK_READWRITE, 
						newLockType);
      if (mustUnpinEnqPage)
	{
	  switch (newLockType)
	    {
	    case FRAMELOCK_READWRITE:
	      throw SmException(__FILE__, __LINE__, "Unexpected framelock level");
	      break;
	    case FRAMELOCK_READONLY:
	      _bufferCache._cb._framedPages.setPageLockLevelReadOnly(enqRegion._pa);
	      break;
	    case FRAMELOCK_NONE:
	      updateBcPagePriorities();
	      _bufferCache._cb._framedPages.setPageLockLevelNone(enqRegion._pa);
	      break;
	    }

	  _bufferCache.awakenWorkerThreads();
	}

      // Update our other data structures regarding the queue adjustment...
      qt.onEnqComplete(qbaseId, numRecsEnqueued, enqTstampSecSum, 
		       enqTstampMicroSecSum, _nonEmptyQviews);
    }
}

//===============================================================================

void StorageMgr::awaitDeqPossible(int superBoxId, int qbaseId, int qviewId)
  throw (exception)
{
  LockHolder lhSm(_mtx);

  //-----------------------------------------------------------------------------
  // Test for qview emptiness...
  //-----------------------------------------------------------------------------
  map<int, QMetadata>::iterator queuePos =  _queueMap.find(qbaseId);
  if (queuePos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "Specified qbase isn't defined");
    }

  QTopology & qt = queuePos->second._qt;
  size_t numRecsInQview;
  timeval avgTimestamp;
  qt.getQviewStats(qviewId, numRecsInQview, avgTimestamp);

  if (numRecsInQview == 0)
    {
      throw SmException(__FILE__, __LINE__, "The specified qview is empty");
    }

  //-----------------------------------------------------------------------------
  // Pin the deq page...
  //-----------------------------------------------------------------------------
  IntraPageSlotRange deqRegion = qt.getNextDeqRegion(qviewId);

  bool mustPinDeqPage = 
    _neededPagePriorities.incrementPinCount(deqRegion._pa, FRAMELOCK_READONLY);

  const char * pDeqPageFrame;
  if (mustPinDeqPage)
    {
      LockHolder lhBccb(_bufferCache._cb);
      BinarySem alert;

      bool canReadLockNow = 
	_bufferCache._cb.setPageReadableOneShotAlert(deqRegion._pa, alert);

      if (canReadLockNow)
	{
	  _pRuntimeStats->onFramingAlreadyFramed();

	  // Later on we might look into making a single call to the following
	  // method, at the end of this method, s.t.:
	  // - we defer the call if we don't need its results to be immediate
	  //   (i.e., we don't need to start waiting on a page that might not
	  //   be framed unless we call the method immdiately), and
	  // - we don't call it at all if we don't have any change to actually
	  //   push out.
	  updateBcPagePriorities();
	}
      else
	{
	  // Just for stats collection...
	  PagesWithFramesMap::FramedPageInfo pfi;
	  bool wasFramed = 
	    _bufferCache._cb._framedPages.findByPageAddr(deqRegion._pa, pfi);
	  
	  timeval t1;
	  if (wasFramed)
	    {
	      _pRuntimeStats->onFramingAlreadyFramed();
	    }
	  else
	    {
	      gettimeofday(& t1, NULL);
	    }

	  updateBcPagePriorities();
	  _bufferCache.awakenWorkerThreads();

	  lhBccb.release();
	  lhSm.release();
	  alert.awaitPostThenClear();

	  if (! wasFramed)
	    {
	      timeval t2;
	      gettimeofday(& t2, NULL);
	      _pRuntimeStats->onFramingDelayed(t1, t2);
	    }

	  lhSm.reacquire();
	  lhBccb.reacquire();
	}

      pDeqPageFrame = 
	_bufferCache._cb._framedPages.setPageLockLevelReadOnly(deqRegion._pa);
    }
  else
    {
      LockHolder lhBccb(_bufferCache._cb);

      PagesWithFramesMap::FramedPageInfo fpi;
      _bufferCache._cb._framedPages.findByPageAddr(deqRegion._pa, fpi);

      if ((fpi._userLock != FRAMELOCK_READONLY) && 
	  (fpi._userLock != FRAMELOCK_READWRITE))
	{
	  throw SmException(__FILE__, __LINE__, "Frame wasn't already locked");
	}

      pDeqPageFrame = fpi._pBuffer;
    }
}

//===============================================================================

void StorageMgr::getDeqBuffer(int superBoxId, 
			      int qbaseId, 
			      int qviewId, 
			      size_t & numRecs, 
			      const char * & pBuffer, 
			      unsigned long & firstSeqNum)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  LockHolder lh(_bufferCache._cb);

  map<int, QMetadata>::iterator queuePos =  _queueMap.find(qbaseId);
  if (queuePos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "Specified qbase isn't defined");
    }

  QMetadata & qmd = queuePos->second;
  QTopology & qt = qmd._qt;
  IntraPageSlotRange deqRegion = qt.getNextDeqRegion(qviewId);

  PagesWithFramesMap::FramedPageInfo fpi;
  _bufferCache._cb._framedPages.findByPageAddr(deqRegion._pa, fpi);

  if ((fpi._userLock != FRAMELOCK_READONLY) &&
      (fpi._userLock != FRAMELOCK_READWRITE))
    {
      throw SmException(__FILE__, __LINE__, "Frame wasn't already properly locked");
    }

  pBuffer = fpi._pBuffer + (deqRegion._lowSlot * qmd._bytesPerRecord);
  numRecs = deqRegion._highSlot - deqRegion._lowSlot + 1;
  firstSeqNum = deqRegion._lowSlotSerialNum;
}

//===============================================================================

void StorageMgr::releaseDeqBuffer(int superBoxId, int qbaseId, int qviewId, 
				  size_t numRecsDequeued)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  LockHolder lh(_bufferCache._cb);

  if (numRecsDequeued > 0)
    {
      // Tally the timestamps of the records that were just dequeued.
      //
      // (Later on, we may want to require a 1:1 corrspondance betwwen calls to
      // getEnqBuffer(...) and releaseEnqBuffer(...). That would let use cache
      // this information (enqRegion and the buffer address) when 
      // getEnqBuffer(...) was called, and uncache it when releaseEnqBuffer was
      // called. -cjc
      //
      // Later on, we may want to make the *caller* supply the tallied seconds
      // and microseconds. They had to iterate over the records anyway, probably.
      // - cjc
      map<int, QMetadata>::iterator queuePos = _queueMap.find(qbaseId);
      if (queuePos == _queueMap.end())
	{
	  throw SmException(__FILE__, __LINE__, "Specified qbase isn't defined");
	}

      QMetadata & qmd = queuePos->second;
      QTopology & qt = qmd._qt;
      IntraPageSlotRange deqRegion = qt.getNextDeqRegion(qviewId);

      PagesWithFramesMap::FramedPageInfo fpi;
      _bufferCache._cb._framedPages.findByPageAddr(deqRegion._pa, fpi);

      if ((fpi._userLock != FRAMELOCK_READONLY) &&
	  (fpi._userLock != FRAMELOCK_READWRITE))
	{
	  throw SmException(__FILE__, __LINE__, "Frame wasn't already properly locked");
	}

      const char * pBuffer = fpi._pBuffer + (deqRegion._lowSlot * qmd._bytesPerRecord);
      size_t maxNumDeqRecs = deqRegion._highSlot - deqRegion._lowSlot + 1;

      if (numRecsDequeued > maxNumDeqRecs)
	{
	  ostringstream os;
	  os << "Caller claims to have dequeued " << numRecsDequeued 
	     << " records, but that's greater than the number allowed: " 
	     << maxNumDeqRecs << endl;
	  throw SmException(__FILE__, __LINE__, os.str());
	}

      unsigned long long deqTstampSecSum = 0;
      unsigned long long deqTstampMicroSecSum = 0;
      timeval tv;

      for (size_t i = 0; i < numRecsDequeued; ++i)
	{
	  memcpy(& tv, pBuffer, sizeof(tv));
	  deqTstampSecSum += tv.tv_sec;
	  deqTstampMicroSecSum += tv.tv_usec;

	  pBuffer += qmd._bytesPerRecord;
	}


      // Perhaps unpin the page...
      FRAME_LOCK_TYPE newLockType;
      bool mustUnpinDeqPage = 
	_neededPagePriorities.decrementPinCount(deqRegion._pa, FRAMELOCK_READONLY, 
						newLockType);
      if (mustUnpinDeqPage)
	{
	  switch (newLockType)
	    {
	    case FRAMELOCK_READWRITE:
	    case FRAMELOCK_READONLY:
	      throw SmException(__FILE__, __LINE__, "Unexpected framelock level");
	      break;
	    case FRAMELOCK_NONE:
	      updateBcPagePriorities();
	      _bufferCache._cb._framedPages.setPageLockLevelNone(deqRegion._pa);
	      break;
	    }

	  _bufferCache.awakenWorkerThreads();
	}

      // Update our other data structures regarding the queue adjustment...
      qt.onDeqComplete(qbaseId, qviewId, numRecsDequeued,
		       deqTstampSecSum, deqTstampMicroSecSum, _nonEmptyQviews);
    }
}

//===============================================================================

void StorageMgr::debugCheck() const
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  for (map<int, QMetadata>::const_iterator pos =  _queueMap.begin();
       pos != _queueMap.end(); 
       ++pos)
    {
      pos->second.debugCheck();
    }
}

//===============================================================================

void StorageMgr::forceCxnPointPruning(int qbaseId)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  map<int, QMetadata>::iterator queuePos = _queueMap.find(qbaseId);
  if (queuePos == _queueMap.end())
    {
      throw SmException(__FILE__, __LINE__, "Specified qbase isn't defined");
    }

  QTopology & qt = queuePos->second._qt;

  qt.forceCxnPointPruning(qbaseId, _nonEmptyQviews);
}

//===============================================================================

void StorageMgr::saveToFile(string filepath)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  XMLPlatformUtils::Initialize();
  XmlTempString xmlStorageMgrString("StorageMgr");
  DOMDocument * pDoc = getDomImplementation()->createDocument(NULL, xmlStorageMgrString.toXml(), NULL);
  DOMElement * pDocRootElem = pDoc->getDocumentElement();

  setDomAttribute(* pDocRootElem, "_bytesPerPage", _bytesPerPage);

  DOMElement * pQueueMapElem = createDomElement(* pDoc, "_queueMap");
  pDocRootElem->appendChild(pQueueMapElem);

  for (map<int, QMetadata>::iterator pos = _queueMap.begin();
       pos != _queueMap.end();
       ++pos)
    {
      DOMElement * pChildElem = createDomElement(* pDoc, "QMetadata");
      pQueueMapElem->appendChild(pChildElem);

      setDomAttribute(* pChildElem, "qbaseId", pos->first);
      //      setDomAttribute(* pQueueMapElem, "qbaseId", pos->first);
      pos->second.saveToXml(* pDoc, * pChildElem);

      // When the system runs stably, try uncommenting this to improve memory
      // use:
      // pQueueMapElem->release();
    }
  
  writeDomDocToFile(*pDoc, filepath);
  pDoc->release();
  XMLPlatformUtils::Terminate();
}

//===============================================================================

void StorageMgr::loadFromFile(string filepath)
  throw (exception)
{  
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  XMLPlatformUtils::Initialize();
  DOMDocument * pDoc = readDomDocFromFile(*getDomImplementationLs(), filepath);
  DOMElement * pDocRootElem = pDoc->getDocumentElement();

  size_t bytesPerPage;
  getDomAttribute(* pDocRootElem, "_bytesPerPage", bytesPerPage);
  if (bytesPerPage != _bytesPerPage)
    {
      throw SmException(__FILE__, __LINE__, "Page size mismatch");
    }

  DOMElement * pChildElem = getDomChildByKeyTag(* pDocRootElem, "_queueMap");
  DOMNodeList * pChildList = pChildElem->getChildNodes();
  for (XMLSize_t i = 0; i < (pChildList->getLength()); ++i)
    {
      DOMElement & qmdMapElem = *(dynamic_cast<DOMElement *>(pChildList->item(i)));

      // All the parameters are bogus - they're overwritten when loading from xml.
      QMetadata qmd(1,1, false);

      qmd.loadFromXml(qmdMapElem);

      int qbaseId;
      getDomAttribute(qmdMapElem, "qbaseId", qbaseId);
      _queueMap.insert(make_pair(qbaseId, qmd));

      // Later on, we can optimize the discovery of non-empty qviews by having
      // QviewTailPtrMap.loadFromXml(...) populate the non-empty qviews set.
      // For now, just do it here...
      qmd._qt.getNonEmptyQviews(qbaseId, _nonEmptyQviews);
    }
  
  XMLPlatformUtils::Terminate();
}

//===============================================================================

void StorageMgr::discardPagePriorities(int superBoxId)
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  // First, undo the prioritization previously due to this superbox...
  map<int, PriorityPages>::iterator pos =
    _prevSuperBoxPriorities.find(superBoxId);

  if (pos == _prevSuperBoxPriorities.end())
    {
      ostringstream os;
      os << "Couldn't find an expected superBoxId: " << superBoxId;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  vector<PageAddr> & topPriorityPages    = pos->second._topPriorityPages;
  vector<PageAddr> & highPriorityPages   = pos->second._highPriorityPages;
  vector<PageAddr> & mediumPriorityPages = pos->second._mediumPriorityPages;
  vector<PageAddr> & lowPriorityPages    = pos->second._lowPriorityPages;

  _neededPagePriorities.decreaseUseCounts(topPriorityPages,
					  highPriorityPages,
					  mediumPriorityPages,
					  lowPriorityPages);

  topPriorityPages.clear();
  highPriorityPages.clear();
  mediumPriorityPages.clear();
  lowPriorityPages.clear();
}

//===============================================================================

void StorageMgr::establishNeededPagePriorities(int superBoxId, 
					       const vector<SuperBoxPlanStep> & schedule, 
					       size_t firstIdx)
  throw (exception)
{
  // First, undo the prioritization previously due to this superbox...
  map<int, PriorityPages>::iterator pos =
    _prevSuperBoxPriorities.find(superBoxId);

  if (pos == _prevSuperBoxPriorities.end())
    {
      throw SmException(__FILE__, __LINE__, 
			"Couldn't find an expected superBoxId");
    }

  vector<PageAddr> & topPriorityPages    = pos->second._topPriorityPages;
  vector<PageAddr> & highPriorityPages   = pos->second._highPriorityPages;
  vector<PageAddr> & mediumPriorityPages = pos->second._mediumPriorityPages;
  vector<PageAddr> & lowPriorityPages    = pos->second._lowPriorityPages;

  // Give top priority to any pages slated for immediate enqueueing or 
  // dequeuing, and high priorities to all the other pages...
  const SuperBoxPlanStep & firstStep = schedule.at(firstIdx);
  //  const SuperBoxPlanStep & firstStep = schedule[firstIdx];

  // Handle the first step's qbases...
  const vector<int> & firstOutputQbaseIds = firstStep.getOutputQbaseIds();
  for (size_t i = 0; i < firstOutputQbaseIds.size(); ++i)
    {
      int qbaseId = firstOutputQbaseIds.at(i);
      //      int qbaseId = firstOutputQbaseIds[i];
      map<int, QMetadata>::iterator qmdPos = _queueMap.find(qbaseId);
      if (qmdPos == _queueMap.end())
	{
	  ostringstream os;
	  os << "Error: unmapped qbaseId (" << qbaseId 
	     << ") in schedule for superbox " << superBoxId << endl;
	  throw SmException(__FILE__, __LINE__, os.str());
	}

      QTopology & qt = qmdPos->second._qt;

      // It would be good if right away we could get going on bringing in any 
      // new page needed to enqueue records. Unfortunately, under the present
      // scheme that would be really hard, so we're just going to have to 
      // such up the fact that we lose an opportunity here...
      if (qt.getEnqCapacity() > 0)
	{
	  IntraPageSlotRange enqRange = qt.getNextEnqRegion();
	  const vector<PageAddr> & pages = qt.getPageRing();

	  size_t numPages = pages.size();
	  for (size_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
	    {
	      if (pages.at(pageIdx) == enqRange._pa)
		//	      if (pages[pageIdx] == enqRange._pa)
		{
		  topPriorityPages.push_back(pages.at(pageIdx));
		  //		  topPriorityPages.push_back(pages[pageIdx]);
		}
	      else
		{
		  highPriorityPages.push_back(pages.at(pageIdx));
		  //		  highPriorityPages.push_back(pages[pageIdx]);
		}
	    } 
	}
      else
	{
	  const vector<PageAddr> & pages = qt.getPageRing();

	  size_t numPages = pages.size();
	  for (size_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
	    {
	      highPriorityPages.push_back(pages.at(pageIdx));
	      //	      highPriorityPages.push_back(pages[pageIdx]);
	    }
	}

    }

  // Handle the first step's qviews...
  const vector<pair<int, int> > & firstOutputQviewIds = firstStep.getInputQviewIds();
  for (size_t i = 0; i < firstOutputQviewIds.size(); ++i)
    {
      int qbaseId = firstOutputQviewIds.at(i).first;
      int qviewId = firstOutputQviewIds.at(i).second;
      //      int qbaseId = firstOutputQviewIds[i].first;
      //      int qviewId = firstOutputQviewIds[i].second;

      map<int, QMetadata>::const_iterator qmdPos = _queueMap.find(qbaseId);
      if (qmdPos == _queueMap.end())
	{
	  throw SmException(__FILE__, __LINE__, 
			    "Error: unmapped qbaseId in schedule");
	}

      const QTopology & qt = qmdPos->second._qt;
      size_t numRecsInQview;
      timeval avgTimestamp;
      qt.getQviewStats(qviewId, numRecsInQview, avgTimestamp);

      if (numRecsInQview > 0)
	{
	  IntraPageSlotRange deqRange = qt.getNextDeqRegion(qviewId);
	  const vector<PageAddr> & pages = qt.getPageRing();

	  size_t numPages = pages.size();
	  for (size_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
	    {
	      if (pages.at(pageIdx) == deqRange._pa)
		//	      if (pages[pageIdx] == deqRange._pa)
		{
		  topPriorityPages.push_back(pages.at(pageIdx));
		  //		  topPriorityPages.push_back(pages[pageIdx]);
		}
	      else
		{
		  highPriorityPages.push_back(pages.at(pageIdx));
		  //		  highPriorityPages.push_back(pages[pageIdx]);
		}
	    }
	}
      else
	{
	  const vector<PageAddr> & pages = qt.getPageRing();

	  size_t numPages = pages.size();
	  for (size_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
	    {
	      highPriorityPages.push_back(pages.at(pageIdx));
	      //	      highPriorityPages.push_back(pages[pageIdx]);
	    }
	}
    }

  // Give medium priority to the pages in the following 3 steps (untuned number)
  // ...
  size_t firstMedPriorityIdx = firstIdx + 1;
  size_t lastMedPriorityIdx = min(schedule.size() - 1, firstMedPriorityIdx + 2);

  for (size_t i = firstMedPriorityIdx; i <= lastMedPriorityIdx; ++i)
    {
      const SuperBoxPlanStep & currentStep = schedule.at(i);
      //      const SuperBoxPlanStep & currentStep = schedule[i];

      // Handle the qbase pages...
      const vector<int> & qbaseIds = currentStep.getOutputQbaseIds();
      size_t numQbases = qbaseIds.size();

      for (size_t qbaseIdx = 0; qbaseIdx < numQbases; ++qbaseIdx)
	{
	  int qbaseId = qbaseIds.at(qbaseIdx);
	  //	  int qbaseId = qbaseIds[qbaseIdx];
	  map<int, QMetadata>::const_iterator qtPos = _queueMap.find(qbaseId);
	  if (qtPos == _queueMap.end())
	    {
	      throw SmException(__FILE__, __LINE__, 
				"Error: unmapped qbaseId in schedule");
	    }
	  
	  const QTopology & qt = qtPos->second._qt;
	  const vector<PageAddr> & pages = qt.getPageRing();

	  size_t numPages = pages.size();
	  for (size_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
	    {
	      mediumPriorityPages.push_back(pages.at(pageIdx));
	      //	      mediumPriorityPages.push_back(pages[pageIdx]);
	    }
	}
    }
  

  // Give low priority to all the remaining pages in the schedule...
  size_t firstLowPriorityIdx = lastMedPriorityIdx + 1;
  size_t lastLowPriorityIdx = schedule.size() - 1;

  // *** THIS IS A REPLICA OF THE CODE FOR SETTING MEDIUM-PRIORITY PAGES. WHEN
  // *** MODIFYING ONE OF THESE BLOCKS OF CODE, YOU PROBABLY WANT TO MODIFY THE
  // *** OTHER IN A SIMILAR FASHION.

  for (size_t i = firstLowPriorityIdx; i <= lastLowPriorityIdx; ++i)
    {
      const SuperBoxPlanStep & currentStep = schedule.at(i);
      //      const SuperBoxPlanStep & currentStep = schedule[i];

      // Handle the qbase pages...
      const vector<int> & qbaseIds = currentStep.getOutputQbaseIds();
      size_t numQbases = qbaseIds.size();

      for (size_t qbaseIdx = 0; qbaseIdx < numQbases; ++qbaseIdx)
	{
	  int qbaseId = qbaseIds.at(qbaseIdx);
	  //	  int qbaseId = qbaseIds[qbaseIdx];
	  map<int, QMetadata>::const_iterator qmdPos = _queueMap.find(qbaseId);
	  if (qmdPos == _queueMap.end())
	    {
	      throw SmException(__FILE__, __LINE__, 
				"Error: unmapped qbaseId in schedule");
	    }
	  
	  const QTopology & qt = qmdPos->second._qt;
	  const vector<PageAddr> & pages = qt.getPageRing();

	  size_t numPages = pages.size();
	  for (size_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
	    {
	      lowPriorityPages.push_back(pages.at(pageIdx));
	      //	      mediumPriorityPages.push_back(pages[pageIdx]);
	    }
	}
    }
  
  // Push out the changes...
  _neededPagePriorities.increaseUseCounts(topPriorityPages,
					  highPriorityPages,
					  mediumPriorityPages,
					  lowPriorityPages);
}

//===============================================================================

void StorageMgr::updateBcPagePriorities()
  throw (exception)
{
  VERIFY_CALLER_HOLDS_LOCK(_mtx);

  const set<PageAddr> & topEffectivePriorityPages = 
    _neededPagePriorities.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_TOP);

  const set<PageAddr> & highEffectivePriorityPages = 
    _neededPagePriorities.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_HIGH);

  const set<PageAddr> & mediumEffectivePriorityPages = 
    _neededPagePriorities.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_MEDIUM);

  const set<PageAddr> & lowEffectivePriorityPages = 
    _neededPagePriorities.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_LOW);

  vector<PageAddr> pages;
  vector<int> priorities;

  size_t numTotalPages = 
    topEffectivePriorityPages.size() + highEffectivePriorityPages.size() + 
    mediumEffectivePriorityPages.size() + lowEffectivePriorityPages.size();

  pages.reserve(numTotalPages);
  priorities.reserve(numTotalPages);

  set<PageAddr>::const_iterator aPos;
  set<PageAddr>::const_iterator endPos;

  for (aPos = topEffectivePriorityPages.begin(), endPos = topEffectivePriorityPages.end();
       aPos != endPos;
       ++aPos)
    {
      pages.push_back(*aPos);
      priorities.push_back(10);
    }

  for (aPos = highEffectivePriorityPages.begin(), endPos = highEffectivePriorityPages.end();
       aPos != endPos;
       ++aPos)
    {
      pages.push_back(*aPos);
      priorities.push_back(9);
    }

  for (aPos = mediumEffectivePriorityPages.begin(), endPos = mediumEffectivePriorityPages.end();
       aPos != endPos;
       ++aPos)
    {
      pages.push_back(*aPos);
      priorities.push_back(8);
    }

  for (aPos = lowEffectivePriorityPages.begin(), endPos = lowEffectivePriorityPages.end();
       aPos != endPos;
       ++aPos)
    {
      pages.push_back(*aPos);
      priorities.push_back(7);
    }

  _bufferCache._cb.clearUserPriorities();
  _bufferCache._cb.setPriorities(pages, priorities);
}

//===============================================================================
