#ifndef SORTEDPAGEMAP_H
#define SORTEDPAGEMAP_H

#include <iolib/include/PageAddr.H>
#include <exception>
#include <map>
#include <set>
#include <vector>

using namespace std;

class SortedPageMap
{
public:
  SortedPageMap(int maxRank)
    throw (exception);

  virtual ~SortedPageMap();

  // Sets page's rank. Overrides any existing mapping for the same page.
  // Rank must be in the range [0, maxRank].
  void setPageRank(const PageAddr & pa, int newRank)
    throw (exception);

  // Ensure that the specified page isn't mapped by this container
  void erasePageRank(const PageAddr & pa)
    throw (exception);

  // Wipes clean the contents of this container.
  void clear()
    throw (exception);

  // Throws an exception if the page isn't in the map.
  // Rank must be in the range [0, maxRank].
  int getPageRank(const PageAddr & pa)
    throw (exception);

  // Returns a live reference to a set used by this container.
  // Rank must be in the range [0, maxRank].
  const set<PageAddr> & getPageSetForRank(int rank) const
    throw (exception);

private:
  int _maxRank;
  map<PageAddr, int>       _pageToRankMap;
  vector<set<PageAddr> >   _rankToPageSetVector;
};

#endif
