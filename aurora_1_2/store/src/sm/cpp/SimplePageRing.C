#include <sm/include/SimplePageRing.H>
#include <XmlTempString.H>
#include <xercesDomUtil.H>
#include <StorageMgr_Exceptions.H>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#include <algorithm>
#include <sstream>

#include <iostream> // for debugging only.

SimplePageRing::SimplePageRing(size_t slotsPerPage)
  throw (exception)
  : _slotsPerPage(slotsPerPage)
{
}

//===============================================================================

SimplePageRing::SimplePageRing(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_slotsPerPage", _slotsPerPage);

  DOMNodeList * pChildList = elem.getChildNodes();

  XmlTempString xstrPageAddr("PageAddr");

  for (XMLSize_t i = 0; i < (pChildList->getLength()); ++i)
    {
      DOMElement & paElem = *(dynamic_cast<DOMElement *>(pChildList->item(i)));
      
      if (XMLString::compareString(paElem.getTagName(), 
				   xstrPageAddr.toXml()) != 0)
	{
	  throw SmException(__FILE__, __LINE__, "A child element wasn't tagged as PageAddr");
	}

      _pageRing.push_back(PageAddr(paElem));
    }
}
//===============================================================================

SimplePageRing::SimplePageRing(const SimplePageRing & rhs)
  : _slotsPerPage(rhs._slotsPerPage),
    _pageRing(rhs._pageRing)
{
}

//===============================================================================

SimplePageRing::~SimplePageRing()
{
}

//===============================================================================

void SimplePageRing::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  setDomAttribute(elem, "_slotsPerPage", _slotsPerPage);
  
  for (size_t i = 0; i < _pageRing.size(); ++i)
    {
      DOMElement * pChildPa = createDomElement(doc, "PageAddr");
      elem.appendChild(pChildPa);
      _pageRing[i].saveToXml(doc, * pChildPa);
    }
}

//===============================================================================

void SimplePageRing::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_slotsPerPage", _slotsPerPage);

  DOMNodeList * pChildList = elem.getChildNodes();

  XmlTempString xstrPageAddr("PageAddr");

  for (XMLSize_t i = 0; i < (pChildList->getLength()); ++i)
    {
      DOMElement & paElem = *(dynamic_cast<DOMElement *>(pChildList->item(i)));
      
      if (XMLString::compareString(paElem.getTagName(), 
				   xstrPageAddr.toXml()) != 0)
	{
	  throw SmException(__FILE__, __LINE__, "A child element wasn't tagged as PageAddr");
	}

      _pageRing.push_back(PageAddr(paElem));
    }
}

//===============================================================================

void SimplePageRing::addPageAnywhere(const PageAddr & pa)
  throw (exception)
{
  _pageRing.push_back(pa);
}

//===============================================================================

void SimplePageRing::addPageSuccessor(const PageAddr & newPage, 
				      const PageAddr & oldPage)
  throw (exception)
{
  vector<PageAddr>::iterator pos = find(_pageRing.begin(), _pageRing.end(), oldPage);
  if (pos == _pageRing.end())
    {
      throw SmException(__FILE__, __LINE__, "Can't find oldPage in the page ring");
    }

  // We want our insert to happen after the old page, not before it.
  ++pos;

  _pageRing.insert(pos, newPage); 
}

//===============================================================================

size_t SimplePageRing::getSlotDistance(const PageAddr & pa1, int slotNum1,
				       const PageAddr & pa2, int slotNum2) const
  throw (exception)
{
  size_t numPages = _pageRing.size();

  // Simple case: Both on the same page...
  if (pa1 == pa2)
    {
      if (slotNum1 <= slotNum2)
	{
	  return (slotNum2 - slotNum1);
	}
      else
	{
	  // Since the two slots in question are on the same page, we know if we
	  // got here that *all* of the other pages's slots will contribute to
	  // this tally...
	  return 
	    (_slotsPerPage - slotNum2) // the high-end of this page's slots
	    + slotNum1                 // the low-end of this page's slots
	    + (_slotsPerPage * (numPages - 1)); // All the other pages' slots
	}
    }

  // Harder case: Different pages...

  // Find vector index of each of the pages mentioned...
  size_t idxPa1, idxPa2;
  getPageVectorIndices(pa1, idxPa1, pa2, idxPa2);

  // Calculate the number of intermediate pages based on the vector indices.
  // These are the pages visited as you go from pa1 to pa2, but don't include
  // pa1 or pa2 themselves...
  size_t numIntermediatePages;
  if (idxPa1 < idxPa2)
    {
      numIntermediatePages = idxPa2 - idxPa1 - 1;
    }
  else
    {
      numIntermediatePages = idxPa2 - idxPa1 + 1;
    }

  return 
    (_slotsPerPage - slotNum2) // the high-end pa2's slots
    + slotNum1                 // the low-end of this pa1's slots
    + (_slotsPerPage * numIntermediatePages); // All the other pages' slots
}

//===============================================================================

void SimplePageRing::advancePsaByOffset(PageSlotAddr & psa, size_t numSlotsToAdd) const
  throw (exception)
{
  size_t numPages = _pageRing.size();
  if (numSlotsToAdd >= (numPages * _slotsPerPage))
    {
      abort();
      throw SmException(__FILE__, __LINE__, "numSlotsToAdd >= (numPages * _slotsPerPage)");
    }

  // We know the method's going to success (assuming nothing goes really wrong.
  // Adjust psa._serialNum immediately, since we can do it now and get it out of 
  // the way...
  psa._serialNum += numSlotsToAdd;

  // 3 stages: (1) consume rest of current page, then (2) consume whole pages,
  // then (3) consume the appropriate fraction (if any) of a final page.


  // Stage (1): Adjusts _slotNum
  size_t remainingSlotsOnPage = _slotsPerPage - psa._slotNum - 1;
  size_t slotsJustConsumed = min(numSlotsToAdd, remainingSlotsOnPage);

  psa._slotNum   += slotsJustConsumed;
  numSlotsToAdd -= slotsJustConsumed;

  if (numSlotsToAdd == 0)
    return;

  // Stage (2): Adjusts _pa
  // Yes, I know there are much effecient ways to do this, i.e., O(1) vs. 0(# pages).
  // I just want something that's easy to verify right now. -cjc
  size_t wholePagesToConsume = numSlotsToAdd / _slotsPerPage;
  size_t vectorIdx = getPageVectorIdx(psa._pa);

  while (wholePagesToConsume > 0)
    {
      vectorIdx = (vectorIdx + 1) % numPages;
      --wholePagesToConsume;

      numSlotsToAdd -= _slotsPerPage;
    }

  psa._pa = _pageRing.at(vectorIdx);
  //  psa._pa = _pageRing[vectorIdx];

  if (numSlotsToAdd == 0)
    return;

  // We might have one final page traversal to do...
  if ((psa._slotNum + numSlotsToAdd) >= _slotsPerPage)
    {
      remainingSlotsOnPage = _slotsPerPage - psa._slotNum - 1;
      slotsJustConsumed = remainingSlotsOnPage + 1;

      vectorIdx = (vectorIdx + 1) % numPages;

      psa._slotNum = 0;
      psa._pa = _pageRing.at(vectorIdx);
      //      psa._pa = _pageRing[vectorIdx];

      numSlotsToAdd -= slotsJustConsumed;
    }

  // Stage (3): Adjusts _slotNum
  psa._slotNum += numSlotsToAdd;
}

//===============================================================================

size_t SimplePageRing::getNumSlotsInRing() const
  throw (exception)
{
  return _pageRing.size() * _slotsPerPage;
}

//===============================================================================

size_t SimplePageRing::getNumPagesInRing() const
  throw (exception)
{
  return _pageRing.size();
}

//===============================================================================

void SimplePageRing::getPageOccupancy(const PageAddr & pa1, size_t slotNum1,
				      const PageAddr & pa2, size_t slotNum2,
				      size_t & firstPageIdx,
				      vector<size_t> & numRecs) const
  throw (exception)
{
  size_t numPages = _pageRing.size();
  numRecs.clear();
  numRecs.reserve(numPages);  

  size_t secondPageIdx;
  getPageVectorIndices(pa1, firstPageIdx, pa2,  secondPageIdx);

  // Simple case: No page traversals needed...
  if (pa1 == pa2)
    {
      if (slotNum1 == slotNum2)
	{
	  throw SmException(__FILE__, __LINE__, "Identical slots specified");
	}
      if (slotNum1 < slotNum2)
	{
	  numRecs.push_back(slotNum2 - slotNum1 + 1);
	  return;
	}
    }

  // Harder case: Page traversals needed...

  // Since the two slots in question are on the same page, we know if we
  // got here that *all* of the other pages's slots will contribute to
  // this tally...

  // Cover the remainder of the first page...
  numRecs.push_back(_slotsPerPage - slotNum1);

  // Cover all the intermediate pages (but not the final one)...
  for (size_t idx = (firstPageIdx + 1) % numPages; 
       idx != secondPageIdx; 
       idx = ((idx + 1) % numPages))
    {
      numRecs.push_back(_slotsPerPage);
    }
	  
  // Cover the final page (which might also be the first page)...
  numRecs.push_back(slotNum2 + 1);
}

//===============================================================================

PageAddr SimplePageRing::getSuccessorPage(const PageAddr & pa) const
  throw (exception)
{
  int idx = getPageVectorIdx(pa);
  return _pageRing.at((idx + 1) % _pageRing.size());
  //  return _pageRing[(idx + 1) % _pageRing.size()];
}

//===============================================================================

void SimplePageRing::debugCheck() const
{
}

//===============================================================================

// Returns index into _pageRing. On failed lookup, throws exception.
size_t SimplePageRing::getPageVectorIdx(const PageAddr & pa) const
  throw (exception)
{
  int idxPage = -1;
  size_t numPages = _pageRing.size();

  for (size_t i = 0; (i < numPages) && (idxPage == -1); ++i)
    {
      if (_pageRing.at(i) == pa)
	//      if (_pageRing[i] == pa)
	  idxPage = i;
    }

  if (idxPage == -1)
    {
      throw SmException(__FILE__, __LINE__, "Page not found in _pageRing");
    }

  return size_t(idxPage);
}

//===============================================================================

const vector<PageAddr> & SimplePageRing::getPageRing() const
  throw (exception)
{
  return _pageRing;
}

//===============================================================================

string SimplePageRing::getDebugInfo(int indentLevel) const
  throw (std::exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "SimplePageRing(this = " << this << ")" << endl;
  return os.str();
}

//===============================================================================

bool SimplePageRing::operator == (const SimplePageRing & rhs) const
  throw (exception)
{
  size_t numPages = _pageRing.size();

  if ((_slotsPerPage != rhs._slotsPerPage) ||
      (numPages != rhs._pageRing.size()))
    return false;

  // The vectors just need the same sets of pages in the same order. We don't
  // demand equal indices for paired pages.
  if (numPages > 0)
    {
      size_t rhsIdx;
      try
	{
	  rhsIdx = rhs.getPageVectorIdx(_pageRing.at(0));
	  //	  rhsIdx = rhs.getPageVectorIdx(_pageRing[0]);
	}
      catch (const exception & e)
	{
	  return false;
	}

      for (size_t thisIdx = 1; thisIdx < numPages; ++thisIdx)
	{
	  rhsIdx = (rhsIdx + 1) % numPages;
	  if (_pageRing.at(thisIdx) != rhs._pageRing.at(rhsIdx))
	    //	  if (_pageRing[thisIdx] != rhs._pageRing[rhsIdx])
	    return false;
	}
    }

  return true;
}

//===============================================================================

bool SimplePageRing::operator != (const SimplePageRing & rhs) const
  throw (exception)
{
  return ! ((*this) == rhs);
}

//===============================================================================

void SimplePageRing::getPageVectorIndices(const PageAddr & pa1, size_t & idxPa1, 
					  const PageAddr & pa2, size_t & idxPa2) const
  throw (exception)
{
  bool foundPa1 = false;
  bool foundPa2 = false;

  size_t numPages = _pageRing.size();

  for (size_t i = 0; 
       ((i < numPages) && (! (foundPa1 && foundPa2)));
       ++i)
    {
      if ((! foundPa1) && (_pageRing.at(i) == pa1))
	//      if ((! foundPa1) && (_pageRing[i] == pa1))
	{
	  idxPa1 = i;
	  foundPa1 = true;
	}

      if ((! foundPa2) && (_pageRing.at(i) == pa2))
	//      if ((! foundPa2) && (_pageRing[i] == pa2))
	{
	  idxPa2 = i;
	  foundPa2 = true;
	}    
    }

  if ((! foundPa1) || (! foundPa2))
    {
      ostringstream os;
      os << "pa1 = " << pa1.getDebugInfo() << endl
	 << "pa2 = " << pa2.getDebugInfo() << endl
	 << (foundPa1 ? "found" : "couldn't find") << " pa1" << endl
	 << (foundPa2 ? "found" : "couldn't find") << " pa2" << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }
}

//===============================================================================

