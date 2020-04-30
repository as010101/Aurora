#include <buffercache/include/SortedPageMap.H>
#include <StorageMgr_Exceptions.H>
#include <algorithm>

SortedPageMap::SortedPageMap(int maxRank)
  throw (exception)
{
  _maxRank = maxRank;

  _rankToPageSetVector.reserve(_maxRank);

  for (int i = 0; i <= _maxRank; i++)
    {
      _rankToPageSetVector.push_back(set<PageAddr>());
    }
}

//===============================================================================

SortedPageMap::~SortedPageMap()
{
}

//===============================================================================

void SortedPageMap::setPageRank(const PageAddr & pa, int newRank)
  throw (exception)
{
  if ((newRank < 0) || (newRank > _maxRank))
    {
      throw SmException(__FILE__, __LINE__, 
				  "(newRank < 0) || (newRank > _maxRank)");
    }

  map<PageAddr, int>::iterator indexPos = _pageToRankMap.find(pa);

  if (indexPos != _pageToRankMap.end())
    {
      int oldRank = indexPos->second;
      if (oldRank != newRank)
	{
	  _rankToPageSetVector.at(oldRank).erase(pa);
	  _rankToPageSetVector.at(newRank).insert(pa);
	  indexPos->second = newRank;
	}
    }
  else
    {
      _rankToPageSetVector.at(newRank).insert(pa);
      _pageToRankMap.insert(make_pair(pa, newRank));
    }
}

//===============================================================================

void SortedPageMap::erasePageRank(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, int>::iterator indexPos = _pageToRankMap.find(pa);

  if (indexPos == _pageToRankMap.end())
    {
      return;
    }

  _rankToPageSetVector[indexPos->second].erase(pa);
  _pageToRankMap.erase(indexPos);
}

//===============================================================================

void SortedPageMap::clear()
  throw (exception)
{
  _pageToRankMap.clear();
  
  for (int i = 0; i <= _maxRank; i++)
    {
      _rankToPageSetVector.clear();
    }
}

//===============================================================================

int SortedPageMap::getPageRank(const PageAddr & pa)
  throw (exception)
{
  map<PageAddr, int>::iterator indexPos = _pageToRankMap.find(pa);

  if (indexPos == _pageToRankMap.end())
    {
      throw SmException(__FILE__, __LINE__, 
				  "The page isn't mapped in this container");
    }

  return indexPos->second;
}

//===============================================================================

const set<PageAddr> & SortedPageMap::getPageSetForRank(int rank) const
  throw (exception)
{
  return _rankToPageSetVector[rank];
}

//===============================================================================
