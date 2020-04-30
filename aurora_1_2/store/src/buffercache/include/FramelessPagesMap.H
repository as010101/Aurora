#ifndef FRAMELESSPAGESMAP_H
#define FRAMELESSPAGESMAP_H

#include <exception>
#include <set>
#include <map>
#include <vector>
#include <iolib/include/PageAddr.H>

using namespace std;

class FramelessPagesMap
{
public:
  FramelessPagesMap()
    throw (exception);

  virtual ~FramelessPagesMap();

  // Sets or overwrites the priority specification for the specified page.
  // The priority must be in the range 1...10.
  void setMapping(const PageAddr & pa, int priority)
    throw (exception);

  // Ensures that the specified page has no mapping in this container.
  void clearPage(const PageAddr & pa)
    throw (exception);

  // Clears all of the mappings in this container.
  void clear()
    throw (exception);

  // Lookups up the mapping for the specified page. If found, sets 'priority'
  // accordingly and returns 'true'. Otherwise, returns 'false'.
  bool findByPageAddr(const PageAddr & pa, int & priority)
    throw (exception);

  // Returns a live reference to the set of PageAddr objects with the specified
  // priority.
  const set<PageAddr> & getPagesForPriority(int priority)
    throw (exception);

  // Removes up to 'maxPages' pages of the specified priority from the map, and
  // records them into 'pa'. This calls 'pa.clear()' when invoked.
  // Returns the number of pages that have been plaecs in to 'pa'.
  int removePagesForPriority(vector<PageAddr> & pa, size_t maxPages, int priority)
    throw (exception);
  
private:
  // Note - we use 1-based indexing for this, so always subtract 1 to get
  // the vector index.
  vector< set<PageAddr> > _prioritySets;

  map<PageAddr, int> _pageIndex;
};

#endif
