#include <buffercache/include/FramelessPagesMap.H>
#include <StorageMgr_Exceptions.H>
#include <algorithm>

//===============================================================================

FramelessPagesMap::FramelessPagesMap()
  throw (exception)
  : _prioritySets(10)
{
}

//===============================================================================

FramelessPagesMap::~FramelessPagesMap()
{
}

//===============================================================================

void FramelessPagesMap::setMapping(const PageAddr & pa, int priority)
  throw (exception)
{
  if ((priority < 1) || (priority > 10))
    {
      throw SmException(__FILE__, __LINE__, "priority is out of range.");
    }

  map<PageAddr, int>::iterator pos = _pageIndex.find(pa);
  if (pos != _pageIndex.end())
    {
      int oldPriority = pos->second;
      if (oldPriority == priority)
	{
	  return; // The user just asked us to repeat the existing mapping.
	}
      else
	{
	  _prioritySets[oldPriority - 1].erase(pa);
	}
    }

  _pageIndex[pa] = priority;
  _prioritySets[priority - 1].insert(pa);
}

//===============================================================================

void FramelessPagesMap::clearPage(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, int>::iterator pos = _pageIndex.find(pa);

  if (pos != _pageIndex.end())
    {
      int oldPriority = pos->second;
      _prioritySets[oldPriority - 1].erase(pa);
      _pageIndex.erase(pos);
    }
}

//===============================================================================

void FramelessPagesMap::clear()
  throw (exception)
{
  for (int i = 0; i < 10; i++)
    {
      _prioritySets[i].clear();
    }

  _pageIndex.clear();
}

//===============================================================================

bool FramelessPagesMap::findByPageAddr(const PageAddr & pa, int & priority)
  throw (exception)
{
  map<PageAddr, int>::iterator pos = _pageIndex.find(pa);

  if (pos == _pageIndex.end())
    {
      return false;
    }
  else
    {
      priority = pos->second;
      return true;
    }
}

//===============================================================================

const set<PageAddr> & FramelessPagesMap::getPagesForPriority(int priority)
  throw (exception)
{
  if ((priority < 1) || (priority > 10))
    {
      throw SmException(__FILE__, __LINE__, "priority is out of range.");
    }

  return _prioritySets[priority - 1];
}

//===============================================================================

int FramelessPagesMap::removePagesForPriority(vector<PageAddr> & pa, 
					       size_t maxPages, 
					       int priority)
  throw (exception)
{
  if ((priority < 1) || (priority > 10))
    {
      throw SmException(__FILE__, __LINE__, "priority is out of range.");
    }

  set<PageAddr> & srcPageSet = _prioritySets[priority-1];

  int numToRemove = min(maxPages, srcPageSet.size());

  pa.clear();
  pa.reserve(numToRemove);

  for (int i = 0; i < numToRemove; ++i)
    {
      set<PageAddr>::iterator pos = srcPageSet.begin();
      pa.push_back(*pos);
      ++pos;
    }

  return numToRemove;
}

//===============================================================================
