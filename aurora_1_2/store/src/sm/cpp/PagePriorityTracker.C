#include <StorageMgr_Exceptions.H>
#include <sm/include/PagePriorityTracker.H>
#include <algorithm>
#include <sstream>

#include <iostream> // just for debugging

PagePriorityTracker::PagePriorityTracker()
  throw (exception)
{
}

//===============================================================================

void PagePriorityTracker::increaseUseCounts(const vector<PageAddr> & topUserPriorityPages,
					    const vector<PageAddr> & highUserPriorityPages,
					    const vector<PageAddr> & mediumUserPriorityPages,
					    const vector<PageAddr> & lowUserPriorityPages)
  throw (exception)
{
  modifyPageUserPriorities(topUserPriorityPages,    USER_PRIORITY_TOP,    1);
  modifyPageUserPriorities(highUserPriorityPages,   USER_PRIORITY_HIGH,   1);
  modifyPageUserPriorities(mediumUserPriorityPages, USER_PRIORITY_MEDIUM, 1);
  modifyPageUserPriorities(lowUserPriorityPages,    USER_PRIORITY_LOW,    1);
}

//===============================================================================

void PagePriorityTracker::decreaseUseCounts(const vector<PageAddr> & topUserPriorityPages,
					    const vector<PageAddr> & highUserPriorityPages,
					    const vector<PageAddr> & mediumUserPriorityPages,
					    const vector<PageAddr> & lowUserPriorityPages)
  throw (exception)
{
  modifyPageUserPriorities(topUserPriorityPages,    USER_PRIORITY_TOP,    -1);
  modifyPageUserPriorities(highUserPriorityPages,   USER_PRIORITY_HIGH,   -1);
  modifyPageUserPriorities(mediumUserPriorityPages, USER_PRIORITY_MEDIUM, -1);
  modifyPageUserPriorities(lowUserPriorityPages,    USER_PRIORITY_LOW,    -1);
}

//===============================================================================

bool PagePriorityTracker::areAnyPagesPrioritizedOrPinned(const vector<PageAddr> & pages)
  throw (exception)
{
  size_t numPages = pages.size();
  map<PageAddr, PagePriorityStats>::iterator endPos = _pageToPriorityStatsMap.end();

  for (size_t i = 0; i < numPages; ++i)
    {
      if (_pageToPriorityStatsMap.find(pages[i]) != endPos)
	{
	  return true;
	}
    }

  return false;
}

//===============================================================================

const set<PageAddr> & 
PagePriorityTracker::getPageSetByEffectivePriority(EffectivePriority ep)
  throw (exception)
{
  switch (ep)
    {
    case EFFECTIVE_PRIORITY_TOP:
      return _topEffectivePriorityPages;
      break;
    case EFFECTIVE_PRIORITY_HIGH:
      return _highEffectivePriorityPages;
      break;
    case EFFECTIVE_PRIORITY_MEDIUM:
      return _mediumEffectivePriorityPages;
      break;
    case EFFECTIVE_PRIORITY_LOW:
      return _lowEffectivePriorityPages;
      break;
    case EFFECTIVE_PRIORITY_NONE:
    default:
      throw SmException(__FILE__, __LINE__, "Invalid priority class specified");
    };
}

//===============================================================================

void PagePriorityTracker::debugCheck() const
{ 
}

//===============================================================================

string PagePriorityTracker::getDebugInfo() const
  throw (exception)
{
  ostringstream os;
  os << "PagePriorityTracker (this = " << this << ")" << endl;

  os << "   _topEffectivePriorityPages:" << endl;
  for (set<PageAddr>::const_iterator pos = _topEffectivePriorityPages.begin();
       pos != _topEffectivePriorityPages.end();
       ++pos)
    {
      os << pos->getDebugInfo(6) << endl;
    }

  os << "   _highEffectivePriorityPages:" << endl;
  for (set<PageAddr>::const_iterator pos = _highEffectivePriorityPages.begin();
       pos != _highEffectivePriorityPages.end();
       ++pos)
    {
      os << pos->getDebugInfo(6) << endl;
    }

  os << "   _mediumEffectivePriorityPages:" << endl;
  for (set<PageAddr>::const_iterator pos = _mediumEffectivePriorityPages.begin();
       pos != _mediumEffectivePriorityPages.end();
       ++pos)
    {
      os << pos->getDebugInfo(6) << endl;
    }

  os << "   _lowEffectivePriorityPages:" << endl;
  for (set<PageAddr>::const_iterator pos = _lowEffectivePriorityPages.begin();
       pos != _lowEffectivePriorityPages.end();
       ++pos)
    {
      os << pos->getDebugInfo(6) << endl;
    }

  os << endl;

  os << "   _pageToPriorityStatsMap" << endl;
  for (map<PageAddr, PagePriorityStats>::const_iterator pos = _pageToPriorityStatsMap.begin();
       pos != _pageToPriorityStatsMap.end();
       ++pos)
    {
      os << pos->first.getDebugInfo(6) << endl
	 << pos->second.getDebugInfo(9) << endl
	 << endl;
    }

  return os.str();
}

//===============================================================================

void PagePriorityTracker::modifyPageUserPriorities(const vector<PageAddr> & pages, 
						   UserPriority up, 
						   int modification)
  throw (exception)
{
  if (modification == 0)
    {
      return;
    }

  size_t numPages;

  PagePriorityStats newStatsPrototype;

  if ((up < USER_PRIORITY_LOW) || (up > USER_PRIORITY_PIN_READWRITE))
    {
      throw SmException(__FILE__, __LINE__, "Invalid UserPriority specified");
    }

  newStatsPrototype._userPriorityCounts[up] = modification;

  numPages = pages.size();
  map<PageAddr, PagePriorityStats>::iterator pos;

  for (size_t i = 0; i < numPages; ++i)
    {
      pos = _pageToPriorityStatsMap.find(pages[i]);

      if (pos == _pageToPriorityStatsMap.end())
	{
	  _pageToPriorityStatsMap.insert(make_pair(pages[i], newStatsPrototype));

	  switch (newStatsPrototype.getEffectivePriority())
	    {
	    case EFFECTIVE_PRIORITY_TOP:
	      _topEffectivePriorityPages.insert(pages[i]);
	      break;
	    case EFFECTIVE_PRIORITY_HIGH:
	      _highEffectivePriorityPages.insert(pages[i]);
	      break;
	    case EFFECTIVE_PRIORITY_MEDIUM:
	      _mediumEffectivePriorityPages.insert(pages[i]);
	      break;
	    case EFFECTIVE_PRIORITY_LOW:
	      _lowEffectivePriorityPages.insert(pages[i]);
	      break;
	    case EFFECTIVE_PRIORITY_NONE:
	    default:
	      throw SmException(__FILE__, __LINE__, "Invalid EffectivePriority");
	    };
	}
      else
	{
	  EffectivePriority oldEffectivePriority = pos->second.getEffectivePriority();
	  pos->second._userPriorityCounts[up] += modification;
	  EffectivePriority newEffectivePriority = pos->second.getEffectivePriority();

	  moveToProperContainer(pos->first, 
				oldEffectivePriority, 
				newEffectivePriority);
	}
    }
}

//===============================================================================

PagePriorityTracker::PagePriorityStats::PagePriorityStats()
{
  for (size_t i = USER_PRIORITY_LOW; i <= USER_PRIORITY_PIN_READWRITE; ++i)
    {
      _userPriorityCounts[i] = 0;
    }
}

//===============================================================================

PagePriorityTracker::PagePriorityStats::PagePriorityStats(size_t topUserPriority, 
							  size_t highUserPriority, 
							  size_t mediumUserPriority, 
							  size_t lowUserPriority,
							  size_t pinCountReadOnly,
							  size_t pinCountReadWrite)
{
  _userPriorityCounts[USER_PRIORITY_PIN_READWRITE] = pinCountReadWrite;
  _userPriorityCounts[USER_PRIORITY_PIN_READONLY]  = pinCountReadOnly;
  _userPriorityCounts[USER_PRIORITY_TOP]           = topUserPriority;
  _userPriorityCounts[USER_PRIORITY_HIGH]          = highUserPriority;
  _userPriorityCounts[USER_PRIORITY_MEDIUM]        = mediumUserPriority;
  _userPriorityCounts[USER_PRIORITY_LOW]           = lowUserPriority;
}

//===============================================================================

PagePriorityTracker::EffectivePriority 
PagePriorityTracker::PagePriorityStats::getEffectivePriority() const
  throw (exception)
{
  PagePriorityTracker::EffectivePriority returnVal;

  if ((_userPriorityCounts[USER_PRIORITY_PIN_READWRITE] > 0)  ||
      (_userPriorityCounts[USER_PRIORITY_PIN_READONLY]  > 0)  ||
      (_userPriorityCounts[USER_PRIORITY_TOP]           > 0))
    {
      returnVal = EFFECTIVE_PRIORITY_TOP;
    }
  else if (_userPriorityCounts[USER_PRIORITY_HIGH] > 0)
    {
      returnVal = EFFECTIVE_PRIORITY_HIGH;
    }
  else if (_userPriorityCounts[USER_PRIORITY_MEDIUM] > 0)
    {
      returnVal = EFFECTIVE_PRIORITY_MEDIUM;
    }
  else if (_userPriorityCounts[USER_PRIORITY_LOW] > 0)
    {
      returnVal = EFFECTIVE_PRIORITY_LOW;
    }
  else
    {
      returnVal = EFFECTIVE_PRIORITY_NONE;
    }

  return returnVal;
}

//===============================================================================

FRAME_LOCK_TYPE PagePriorityTracker::PagePriorityStats::getFrameLock() const
{
  if (_userPriorityCounts[USER_PRIORITY_PIN_READWRITE] > 0)
    return FRAMELOCK_READWRITE;
  else if (_userPriorityCounts[USER_PRIORITY_PIN_READONLY] > 0)
    return FRAMELOCK_READONLY;
  else
    return FRAMELOCK_NONE;
}

//===============================================================================

string PagePriorityTracker::PagePriorityStats::getDebugInfo(int indentLevel) const
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "PagePriorityStats(this = " << this << ")" << endl
     << indentStr << "   _userPriorityCounts[USER_PRIORITY_PIN_READWRITE]    = " 
     << _userPriorityCounts[USER_PRIORITY_PIN_READWRITE] << endl
     << indentStr << "   _userPriorityCounts[USER_PRIORITY_PIN_READONLY]     = " 
     << _userPriorityCounts[USER_PRIORITY_PIN_READONLY] << endl
     << indentStr << "   _userPriorityCounts[USER_PRIORITY_TOP]              = " 
     << _userPriorityCounts[USER_PRIORITY_TOP] << endl
     << indentStr << "   _userPriorityCounts[USER_PRIORITY_HIGH]             = " 
     << _userPriorityCounts[USER_PRIORITY_HIGH] << endl
     << indentStr << "   _userPriorityCounts[USER_PRIORITY_MEDIUM]           = " 
     << _userPriorityCounts[USER_PRIORITY_MEDIUM] << endl
     << indentStr << "   _userPriorityCounts[USER_PRIORITY_LOW]              = " 
     << _userPriorityCounts[USER_PRIORITY_LOW] << endl
     << indentStr << "   getEffectivePriority() = ";

  switch (getEffectivePriority())
    {
    case EFFECTIVE_PRIORITY_TOP:
      os << "EFFECTIVE_PRIORITY_TOP" << endl;
      break;
    case EFFECTIVE_PRIORITY_HIGH:
      os << "EFFECTIVE_PRIORITY_HIGH" << endl;
      break;
    case EFFECTIVE_PRIORITY_MEDIUM:
      os << "EFFECTIVE_PRIORITY_MEDIUM" << endl;
      break;
    case EFFECTIVE_PRIORITY_LOW:
      os << "EFFECTIVE_PRIORITY_LOW" << endl;
      break;
    case EFFECTIVE_PRIORITY_NONE:
      os << "EFFECTIVE_PRIORITY_NONE" << endl;
      break;
    default:
      os << "(illegal value)" << endl;
      break;
    }

  os << indentStr << "   getFrameLock()               = ";
  
  switch (getFrameLock())
    {
    case FRAMELOCK_READWRITE:
      os << "FRAMELOCK_READWRITE" << endl;
      break;
    case FRAMELOCK_READONLY:
      os << "FRAMELOCK_READONLY" << endl;
      break;
    case FRAMELOCK_NONE:
      os << "FRAMELOCK_NONE" << endl;
      break;
    default:
      os << "(illegal value)" << endl;
      break;
    }

  return os.str();
}

//===============================================================================

bool PagePriorityTracker::incrementPinCount(const PageAddr & pa, 
					    FRAME_LOCK_TYPE lockType)
  throw (exception)
{
  if ((lockType != FRAMELOCK_READONLY) && (lockType != FRAMELOCK_READWRITE))
    {
      throw SmException(__FILE__, __LINE__, "Invalid lockType");
    }

  map<PageAddr, PagePriorityStats>::iterator pos 
    = _pageToPriorityStatsMap.find(pa);

  if (pos == _pageToPriorityStatsMap.end())
    {
      if (lockType == FRAMELOCK_READONLY)
	{
 	_pageToPriorityStatsMap.insert(make_pair(pa, PagePriorityStats(0,0,0,0,1,0)));
	}
      else

	{
	_pageToPriorityStatsMap.insert(make_pair(pa, PagePriorityStats(0,0,0,0,0,1)));
	}

      _topEffectivePriorityPages.insert(pa);
      return true;
    }
  else
    {
      FRAME_LOCK_TYPE oldLock = pos->second.getFrameLock();
      EffectivePriority oldEffectivePriority = pos->second.getEffectivePriority();

      if (lockType == FRAMELOCK_READONLY)
	{
	  pos->second._userPriorityCounts[USER_PRIORITY_PIN_READONLY]++;
	}
      else
	{
	  pos->second._userPriorityCounts[USER_PRIORITY_PIN_READWRITE]++;
	}

      EffectivePriority newEffectivePriority = pos->second.getEffectivePriority();

      moveToProperContainer(pa, oldEffectivePriority, newEffectivePriority);
      return (oldLock != lockType);
    }
}

//===============================================================================

bool PagePriorityTracker::decrementPinCount(const PageAddr & pa, 
					    FRAME_LOCK_TYPE myLockType, 
					    FRAME_LOCK_TYPE & newLockType)
  throw (exception)
{
  if ((myLockType != FRAMELOCK_READONLY) && (myLockType != FRAMELOCK_READWRITE))
    {
      throw SmException(__FILE__, __LINE__, "Invalid lockType");
    }

  map<PageAddr, PagePriorityStats>::iterator pos 
    = _pageToPriorityStatsMap.find(pa);

  if (pos == _pageToPriorityStatsMap.end())
    {
      throw SmException(__FILE__, __LINE__, "Page isn't mapped, so it wasn't already pinned.");
    }

  EffectivePriority oldEffectivePriority = pos->second.getEffectivePriority();
  FRAME_LOCK_TYPE oldLock = pos->second.getFrameLock();

  if (myLockType == FRAMELOCK_READONLY)
    {
      if (pos->second._userPriorityCounts[USER_PRIORITY_PIN_READONLY] == 0)
	{
	  throw SmException(__FILE__, __LINE__, "read-only pincount underflow");
	}

      -- (pos->second._userPriorityCounts[USER_PRIORITY_PIN_READONLY]);
    }
  else
    {
      if (pos->second._userPriorityCounts[USER_PRIORITY_PIN_READWRITE] == 0)
	{
	  throw SmException(__FILE__, __LINE__, "read-write pincount underflow");
	}

      -- (pos->second._userPriorityCounts[USER_PRIORITY_PIN_READWRITE]);
    }

  EffectivePriority newEffectivePriority = pos->second.getEffectivePriority();
  moveToProperContainer(pos->first, oldEffectivePriority, newEffectivePriority);

  FRAME_LOCK_TYPE newLock = pos->second.getFrameLock();
  if (oldLock == newLock)
    {
      return false;
    }
  else
    {
      newLockType = newLock;
      return true;
    }
}

//===============================================================================

void PagePriorityTracker::moveToProperContainer(const PageAddr & pa, 
						EffectivePriority oldEffectivePriority, 
						EffectivePriority newEffectivePriority)
  throw (exception)
{
  if (oldEffectivePriority == newEffectivePriority)
    return;

  // Out from the old location...
  switch (oldEffectivePriority)
    {
    case EFFECTIVE_PRIORITY_TOP:
      _topEffectivePriorityPages.erase(pa);
      break;
    case EFFECTIVE_PRIORITY_HIGH:
      _highEffectivePriorityPages.erase(pa);
      break;
    case EFFECTIVE_PRIORITY_MEDIUM:
      _mediumEffectivePriorityPages.erase(pa);
      break;
    case EFFECTIVE_PRIORITY_LOW:
      _lowEffectivePriorityPages.erase(pa);
      break;
    case EFFECTIVE_PRIORITY_NONE:
      break;
    default:
      throw SmException(__FILE__, __LINE__, "Invalid EffectivePriority");
    }


  // Into the new location...
  switch (newEffectivePriority)
    {
    case EFFECTIVE_PRIORITY_TOP:
      _topEffectivePriorityPages.insert(pa);
      break;
    case EFFECTIVE_PRIORITY_HIGH:
      _highEffectivePriorityPages.insert(pa);
      break;
    case EFFECTIVE_PRIORITY_MEDIUM:
      _mediumEffectivePriorityPages.insert(pa);
      break;
    case EFFECTIVE_PRIORITY_LOW:
      _lowEffectivePriorityPages.insert(pa);
      break;
    case EFFECTIVE_PRIORITY_NONE:
      break;
    default:
      throw SmException(__FILE__, __LINE__, "Invalid EffectivePriority");
    }
}

//===============================================================================
