#ifndef SIMPLEPAGERING_H
#define SIMPLEPAGERING_H

#include <iolib/include/PageAddr.H>
#include <sm/include/PageSlotAddr.H>
#include <exception>
#include <vector>

using namespace std;

class DOMElement;
class DOMDocument;

class SimplePageRing
{
public:
  SimplePageRing(size_t slotsPerPage)
    throw (exception);

  SimplePageRing(const DOMElement & elem)
    throw (exception);

  SimplePageRing(const SimplePageRing & rhs);

  virtual ~SimplePageRing();

  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  void loadFromXml(const DOMElement & elem)
    throw (exception);

  // Adds the specified page address to an arbitrary location in the ring.
  void addPageAnywhere(const PageAddr & pa)
    throw (exception);

  // Adds a page, to be the successor of the specified page. Throws an exception
  // on any problem.
  void addPageSuccessor(const PageAddr & newPage, const PageAddr & oldPage)
    throw (exception);

  // Give the distance between the two page slots, as the ring is traversed in
  // predecessor->successor order. If s2 is the next slot after s1, this 
  // returns 1. The distance between a slot and itself is 0.
  size_t getSlotDistance(const PageAddr & pa1, int slotNum1,
			 const PageAddr & pa2, int slotNum2) const
    throw (exception);

  // Advances the page address, slot number, and serial number of the specified
  // PageSlotAddr by the specified number of slots. (The page ring is traversed
  // in order to calculate the new page addr / slot num.)
  //
  // Throws an exception if numSlotsToAdd is >= the total number of slots in the
  // page ring.
  void advancePsaByOffset(PageSlotAddr & psa, size_t numSlotsToAdd) const
    throw (exception);

  // Just returns slotsPerPage * number of pages in the ring.
  size_t getNumSlotsInRing() const
    throw (exception);

  size_t getNumPagesInRing() const
    throw (exception);

  // Fairly complicated method. It's goal is to specify the sequence of pages
  // containing the specified range of slots (inclusive), as well as how many 
  // slots exist in that range on each page.
  //
  // firstPageIdx is an index into the vector returned by getPageRing(). It is
  // the index of the page in that contains the next record to be dequeued from
  // this qview.
  //
  // numRecs is populated, in order, with the pages that contain the data for
  // the specified range (from pa1 to pa2, not the other way around). The value
  // in the vector refers to the number of slots the range contains for the 
  // specified page. Note...
  // (A) The first and last elements in the vector may refer to the same page.
  //     In that scenario, numRecs[0] refers to the first part of the range's
  //     slots in the sequence, and numRecs[numRecs.size()-1] refers to the final
  //     set of slots in the range, which happen to reside on the same page as
  //     the first set of slots.
  // (B) Here's the formula to know what PageAddr a certain element of numRecs
  //     corresponds to:
  //     pa = getPageRing()[
  //        (firstPageIdx + (index into numRecs)) % getPageRing.size()]
  //
  // Notes:
  // - This method will throw an exception if the range is empty 
  //   (pa1 == pa2 and slotNum1 == slotNum2)
  // - *This method will call numRecs.clear() before adding elements to it*
  void getPageOccupancy(const PageAddr & pa1, size_t slotNum1,
			const PageAddr & pa2, size_t slotNum2,
			size_t & firstPageIdx,
			vector<size_t> & numRecs) const
    throw (exception);

  // Returns the next page in the ring following the specified page. If the ring
  // only has one page, then the parameter and return value will be identical.
  PageAddr getSuccessorPage(const PageAddr & pa) const
    throw (exception);

  const vector<PageAddr> & getPageRing() const
    throw (exception);

  string getDebugInfo(int indentLevel) const
    throw (exception);

  bool operator == (const SimplePageRing & rhs) const
    throw (exception);

  bool operator != (const SimplePageRing & rhs) const
    throw (exception);

  // Does a debugging check to make sure the object is internally consistent.
  void debugCheck() const;

private:
  size_t _slotsPerPage;

  SimplePageRing() { assert(false); }

  size_t getPageVectorIdx(const PageAddr & pa) const
    throw (exception);

  // Similar to getPageVectorIdx, but it's optimized to look up two pages at
  // the same time. Throws an exception if pa1 and/or pa2 wasn't in the ring.
  void getPageVectorIndices(const PageAddr & pa1, size_t & idxPa1, 
			    const PageAddr & pa2, size_t & idxPa2) const
    throw (exception);

  // _pageRing[(i+1) % _pageRing.size()] is the successor to _pageRing[i].
  vector<PageAddr> _pageRing;
};

#endif
