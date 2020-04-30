#include <sm/include/CxnPtInfo.H>
#include <util.H>
#include <algorithm>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <sstream>
#include <iostream>
#include <xercesDomUtil.H>
#include <XmlTempString.H>

//===============================================================================

CxnPtInfo::CxnPtInfo()
  : _limitTuplesByNum(false),
    _limitTuplesByAge(false),
    _nextEnqSerialNum(0)
{
}

//===============================================================================

CxnPtInfo::CxnPtInfo(const CxnPtInfo & src)
  : _pageParts         (src._pageParts),
    _limitTuplesByNum  (src._limitTuplesByNum),
    _maxNumTuples      (src._maxNumTuples),
    _limitTuplesByAge  (src._limitTuplesByAge),
    _maxTupleAgeSeconds(src._maxTupleAgeSeconds),
    _nextEnqSerialNum  (src._nextEnqSerialNum)
{
}

//===============================================================================

CxnPtInfo::~CxnPtInfo()
{
}

//===============================================================================

CxnPtInfo & CxnPtInfo::operator = (CxnPtInfo & rhs)
{
  _pageParts          = rhs._pageParts;
  _limitTuplesByNum   = rhs._limitTuplesByNum;
  _maxNumTuples       = rhs._maxNumTuples;
  _limitTuplesByAge   = rhs._limitTuplesByAge;
  _maxTupleAgeSeconds = rhs._maxTupleAgeSeconds;
  _nextEnqSerialNum   = rhs._nextEnqSerialNum;
}

//===============================================================================

bool CxnPtInfo::operator == (const CxnPtInfo & rhs) const
  throw (exception)
{
  if ((_limitTuplesByNum != rhs._limitTuplesByNum) ||
      (_limitTuplesByAge != rhs._limitTuplesByAge) ||
      (_pageParts        != rhs._pageParts)        || 
      (_nextEnqSerialNum != rhs._nextEnqSerialNum))
    {
      return false;
    }

  if (_limitTuplesByNum && (_maxNumTuples != rhs._maxNumTuples))
    {
      return false;
    }

  if (_limitTuplesByAge && (_maxTupleAgeSeconds != rhs._maxTupleAgeSeconds))
    {
      return false;
    }

  return true;
}

//===============================================================================

bool CxnPtInfo::operator != (const CxnPtInfo & rhs) const
  throw (exception)
{
  return ! ((*this) == rhs);
}

//===============================================================================
  
void CxnPtInfo::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  // Save _pageParts...
  DOMElement * pPagePartsElem;
  pPagePartsElem = createDomElement(doc, "_pageParts");
  elem.appendChild(pPagePartsElem);

  size_t numParts = _pageParts.size();
  for (size_t i = 0; i < numParts; ++i)
    {
      DOMElement * pChildElem = createDomElement(doc, "PagePartInfo");
      pPagePartsElem->appendChild(pChildElem);
      _pageParts[i].saveToXml(doc, * pChildElem);
    }

  // Save _nextEnqSerialNum.
  setDomAttribute(elem, "_nextEnqSerialNum", _nextEnqSerialNum);

  // Save _limitTuplesByNum and _maxNumTuples...
  setDomAttribute(elem, "_limitTuplesByNum", _limitTuplesByNum);

  if (_limitTuplesByNum)
    {
      setDomAttribute(elem, "_maxNumTuples", _maxNumTuples);
    }

  // Save _limitTuplesByAge and _maxTupleAgeSeconds...
  setDomAttribute(elem, "_limitTuplesByAge", _limitTuplesByAge);

  if (_limitTuplesByAge)
    {
      setDomAttribute(elem, "_maxTupleAgeSeconds", _maxTupleAgeSeconds);
    }
}

//===============================================================================

void CxnPtInfo::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  // Load _pageParts...
  DOMElement * pChildElem;
  pChildElem = getDomChildByKeyTag(elem, "_pageParts");

  DOMNodeList * pChildList = pChildElem->getChildNodes();
  XmlTempString xstrPagePartInfo("PagePartInfo");

  XMLSize_t listLen = pChildList->getLength();
  _pageParts.clear();

  for (XMLSize_t i = 0; i < listLen; ++i)
    {
      DOMElement & ppiElem = *(dynamic_cast<DOMElement *>(pChildList->item(i)));
      
      if (XMLString::compareString(ppiElem.getTagName(), 
				   xstrPagePartInfo.toXml()) != 0)
	{
	  throw SmException(__FILE__, __LINE__, "A child element wasn't tagged as PagePartInfo");
	}

      PagePartInfo ppi;
      ppi.loadFromXml(ppiElem);
      _pageParts.push_back(ppi);
    }

  // Load _nextEnqSerialNum.
  getDomAttribute(elem, "_nextEnqSerialNum", _nextEnqSerialNum);

  // Load _limitTuplesByNum and _maxNumTuples...
  getDomAttribute(elem, "_limitTuplesByNum", _limitTuplesByNum);

  if (_limitTuplesByNum)
    {
      getDomAttribute(elem, "_maxNumTuples", _maxNumTuples);
    }

  // Load _limitTuplesByAge and _maxTupleAgeSeconds...
  getDomAttribute(elem, "_limitTuplesByAge", _limitTuplesByAge);

  if (_limitTuplesByAge)
    {
      getDomAttribute(elem, "_maxTupleAgeSeconds", _maxTupleAgeSeconds);
    }
}

//===============================================================================

void CxnPtInfo::setRetentionBounds(bool limitTuplesByNum,
				   size_t maxNumTuples,
				   bool limitTuplesByAge,
				   unsigned int maxTupleAgeSeconds)
  throw (exception)
{
  if ((! limitTuplesByNum) && (! limitTuplesByAge))
    {
      throw SmException(__FILE__, __LINE__, 
			"At least one retention bound must be set");
    }

  _limitTuplesByNum   = limitTuplesByNum;
  _maxNumTuples       = maxNumTuples;
  _limitTuplesByAge   = limitTuplesByAge;
  _maxTupleAgeSeconds = maxTupleAgeSeconds;
}

//===============================================================================

void CxnPtInfo::onEnqComplete(const PageAddr & pa,
			      size_t lowSlotNum,
			      size_t highSlotNum,
			      size_t slotsPerPage,
			      unsigned long long timevalSecTotal,
			      unsigned long long timevalMicroSecTotal)
  throw (exception)
{
  // Sanity checks...
  if (slotsPerPage == 0)
    {
      throw SmException(__FILE__, __LINE__, "slotsPerPage == 0");
    }

  if (highSlotNum < lowSlotNum)
    {
      throw SmException(__FILE__, __LINE__, "highSlotNum < lowSlotNum");
    }

  if (highSlotNum >= slotsPerPage)
    {
      throw SmException(__FILE__, __LINE__, "highSlotNum >= slotsPerPage");
    }

  // On to business...
  size_t numTuplesEnq = highSlotNum - lowSlotNum + 1;
  unsigned long lowestSerialNumJustEnq = _nextEnqSerialNum;
  _nextEnqSerialNum += numTuplesEnq;

  timeval now;
  gettimeofday(& now, NULL);

  if (_pageParts.empty())
    {
      _pageParts.push_back(PagePartInfo(pa, lowSlotNum, highSlotNum, now, 
					lowestSerialNumJustEnq,
					TimevalAvgTracker(numTuplesEnq,
							  timevalSecTotal,
							  timevalMicroSecTotal)));
      return;
    }
  else if (_pageParts.size() == 1)
    {
      // There's no wraparound currenly, although this enqueueing may have 
      // introduced it...
      PagePartInfo & oldPart = _pageParts[0];
      
      if (oldPart._pa == pa)
	{
	  // oldPart and the new part are on the same page. This permits only two
	  // legal possibilities:
	  // (a) The new part comes immediately after the old part, or
	  // (b) We've got wraparound, so the new part starts at the beginning of
	  //     the page and works towards the old part...

	  if (lowSlotNum > oldPart._highSlotNum)
	    {
	      // Handle case (a)...
	      if (lowSlotNum > (oldPart._highSlotNum + 1))
		{
		  throw SmException(__FILE__, __LINE__, "Illegal gap in page content"); 
		}

	      // Grow the existing page part...
	      oldPart._highSlotNum = highSlotNum;
	      oldPart._lastEnqTime = now;
	      oldPart._timestampStats.addTimevalSums(numTuplesEnq,
						     timevalSecTotal,
						     timevalMicroSecTotal);
	    }
	  else
	    {
	      // Handle case (b)...
	      if ((lowSlotNum != 0) || (highSlotNum >= oldPart._highSlotNum))
		{
		  throw SmException(__FILE__, __LINE__, 
				    "Illegal gap in page content, or slot collission");
		}

	      // We're introducing wraparound, so create a new page slot...

	      // *** DEBUGGING: I CHAGNED THIS FROM push_front TO push_back BECAUSE
	      // *** USING push_front LOOKED WRONG TO ME. IF THIS CAUSES PROBLEMS, WE
	      // *** SHOULD DOUBLE CHECK MY CHANGE'S CORRECTNESS. -CJC
	      _pageParts.push_back(PagePartInfo(pa, lowSlotNum, highSlotNum, now, 
						lowestSerialNumJustEnq,
						TimevalAvgTracker(numTuplesEnq,
								  timevalSecTotal,
								  timevalMicroSecTotal)));
	    }
	}
      else
	{
	  // We're tacking a new page into the queue...  
	  if (lowSlotNum != 0)
	    {
	      throw SmException(__FILE__, __LINE__, 
				"Illegal gap in page content");
	    }

	  _pageParts.push_back(PagePartInfo(pa, lowSlotNum, highSlotNum, now, 
					    lowestSerialNumJustEnq,
					    TimevalAvgTracker(numTuplesEnq,
							      timevalSecTotal,
							      timevalMicroSecTotal)));
	}
    }
  else
    {
      // We have two or more page parts already. All parts other than the 
      // first and must be full, because of our page fill scheme. Three cases 
      // are valid at this point:
      // (a) No existing wraparound, and we're extending the last page part
      //     to cover more of its page.
      // (b) No existing wraparound, and the last page part is full, and
      //     we're starting wraparound onto the tail page.
      // (c) No existing wraparound, and the last page part is full, and 
      //     we're introducing a brand new page.
      // (d) Existing wraparound, and we're growing towards the slots on the
      //     wraparound page that contain the tail of the queue.

      PagePartInfo & oldHeadPart = _pageParts[_pageParts.size() - 1];
      PagePartInfo & tailPart    = _pageParts[0];

      bool alreadyWrappedAround = (oldHeadPart._pa == tailPart._pa);

      if (! alreadyWrappedAround)
	{
	  // Handle cases (a), (b), and (c)...

	  if (oldHeadPart._pa == pa)
	    {
	      // Handle case (a)...
	      if (lowSlotNum > (oldHeadPart._highSlotNum + 1))
		{
		  throw SmException(__FILE__, __LINE__, "Illegal gap in page content"); 
		}

	      // Grow the existing page part...
	      oldHeadPart._highSlotNum = highSlotNum;
	      oldHeadPart._lastEnqTime = now;     
	      oldHeadPart._timestampStats.addTimevalSums(numTuplesEnq,
							 timevalSecTotal,
							 timevalMicroSecTotal);
	    }
	  else if (tailPart._pa == pa)
	    {
	      // Handle case (b)...
	      if ((lowSlotNum != 0) || (highSlotNum >= tailPart._highSlotNum))
		{
		  throw SmException(__FILE__, __LINE__, 
				    "Illegal gap in page content, or slot collission");
		}

	      // We're introducing wraparound, so create a new page slot...
	      //	      _pageParts.push_front(PagePartInfo(pa, lowSlotNum, highSlotNum, now));
	      _pageParts.push_back(PagePartInfo(pa, lowSlotNum, highSlotNum, now, 
						lowestSerialNumJustEnq,
						TimevalAvgTracker(numTuplesEnq,
								  timevalSecTotal,
								  timevalMicroSecTotal)));
	    }
	  else
	    {
	      // Handle case (c)...
	      if (lowSlotNum != 0)
		{
		  throw SmException(__FILE__, __LINE__, 
				    "Illegal gap in page content");
		}
	      
	      _pageParts.push_back(PagePartInfo(pa, lowSlotNum, highSlotNum, now, 
						lowestSerialNumJustEnq,
						TimevalAvgTracker(numTuplesEnq,
								  timevalSecTotal,
								  timevalMicroSecTotal)));
	    }
	}
      else
	{
	  // Handle case (d)...
	  if (lowSlotNum != (oldHeadPart._highSlotNum + 1))
	    {
	      throw SmException(__FILE__, __LINE__, 
				"Illegal gap in page content, or slot collission");
	    }

	  if (highSlotNum >= (tailPart._lowSlotNum))
	    {
	      throw SmException(__FILE__, __LINE__, "Slot collission");
	    }

	  // Grow the existing head page part...
	  oldHeadPart._highSlotNum = highSlotNum;
	  oldHeadPart._lastEnqTime = now;
	  oldHeadPart._timestampStats.addTimevalSums(numTuplesEnq,
						     timevalSecTotal,
						     timevalMicroSecTotal);
	}
    }
}

//===============================================================================

void CxnPtInfo::onPageSplit(const PageAddr & newEnqPage)
  throw (exception)
{
  size_t numPageParts = _pageParts.size();

  // Verify the preconditions...
  if (numPageParts < 2)
    {
      throw SmException(__FILE__, __LINE__, "_pageParts.size() < 2");
    }

  if (_pageParts[0]._pa != _pageParts[numPageParts - 1]._pa)
    {
      ostringstream os;
      os << "Head and tail page parts don't share a page. " << endl
	 << "Tail page part's page = " << _pageParts[0]._pa.getDebugInfo() << endl
	 << "Head page part's page = " << _pageParts[numPageParts - 1]._pa.getDebugInfo();
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // On to real work...
  _pageParts[numPageParts - 1]._pa = newEnqPage;
}

//===============================================================================

size_t CxnPtInfo::doBoundedTruncation(size_t slotsPerPage)
  throw (exception)
{
  if (_pageParts.empty())
    {
      return 0;
    }

  size_t numTruncated = 0;

  // Step 1: Whack all of the tuples that we can based on keep-n logic, because
  // this is really easy and cheap to calculate...
  if (_limitTuplesByNum)
    {
      size_t numTuplesOriginally = getNumPagePartsTuples(slotsPerPage);
      size_t numToTruncateByCount;
      if (numTuplesOriginally > _maxNumTuples)
	{
	  numToTruncateByCount = numTuplesOriginally - _maxNumTuples;
	}
      else
	{
	  numToTruncateByCount = 0;
	}

      numTruncated += numToTruncateByCount;

      while (numToTruncateByCount > 0)
	{
	  // Note: We must always work from the low-indexed end of the vector, 
	  // because that describes the oldest page parts in the queue.
	  PagePartInfo & aPart = _pageParts.at(0); 

	  size_t tuplesInPart = aPart.getNumTuples();

	  if (numToTruncateByCount >= tuplesInPart)
	    {
	      // Eat the whole part...
	      numToTruncateByCount -= tuplesInPart;
	      _pageParts.pop_front();
	    }
	  else
	    {
	      // Consume a portion of the part...
	      aPart._lowSlotNum += numToTruncateByCount;
	      aPart._lowSlotSerialNum += numToTruncateByCount;
	      numToTruncateByCount = 0;
	    } 
	}
    }

  // Step 2: Whack all of the tuples that are too old, if such logic has been
  // specified by the user...
  if (_limitTuplesByAge)
    {
      timeval oldestValidEnqTime;
      gettimeofday(& oldestValidEnqTime, NULL);
      oldestValidEnqTime.tv_sec = 
	max(0UL, oldestValidEnqTime.tv_sec - _maxTupleAgeSeconds);
      
      bool done = false;
      while ((! done) && (! _pageParts.empty()))
	{
	  // Note: We must always work from the low-indexed end of the vector, 
	  // because that describes the oldest page parts in the queue.
	  PagePartInfo & aPart = _pageParts.at(0);

	  if (timevalsComp(aPart._lastEnqTime, oldestValidEnqTime) < 0)
	    {
	      numTruncated += aPart.getNumTuples();
	      _pageParts.pop_front();
	    }
	  else
	    {
	      done = true;
	    }
	}
    }

  return numTruncated;
}

//===============================================================================

bool CxnPtInfo::getNextDeqPoint(size_t slotsPerPage,
				PageSlotAddr & psa,
				TimevalAvgTracker & timestampStats,
				bool limitTuplesByNum,
				size_t maxNumTuples,
				bool limitTuplesByAge,
				unsigned int maxTupleAgeSeconds) const
  throw (exception)
{
  if (_pageParts.empty())
    {
      return false;
    }

  timestampStats.clear();

  if ((! limitTuplesByNum) && (! limitTuplesByAge))
    {
      // Give the caller the whole history...
      psa._pa        = _pageParts.at(0)._pa;
      psa._slotNum   = _pageParts.at(0)._lowSlotNum;
      psa._serialNum = _pageParts.at(0)._lowSlotSerialNum;

      size_t numPageParts = _pageParts.size();
      for (size_t i = 0; i < numPageParts; ++i)
	{
	  timestampStats.add(_pageParts[i]._timestampStats);
	}

      return true;
    }
  else if (limitTuplesByNum && (! limitTuplesByAge))
    {
      size_t pagePartsIdx;
      return findNthYoungestTuple(maxNumTuples,
				  slotsPerPage,
				  psa,
				  pagePartsIdx,
				  timestampStats);
    }
  else if ((! limitTuplesByNum) && limitTuplesByAge)
    {
      size_t pagePartsIdx;
      return findTupleByMaxAge(maxTupleAgeSeconds,
			       slotsPerPage,
			       psa,
			       pagePartsIdx,
			       timestampStats);
    }
  else // limit by both parameters...
    {
      TimevalAvgTracker tatTopN;

      PageSlotAddr psaNum;
      size_t firstPagePartsIdxByNum;
      bool someTuplesMatchedCount = findNthYoungestTuple(maxNumTuples,
							slotsPerPage,
							psaNum,
							firstPagePartsIdxByNum,
							 tatTopN);

      if (! someTuplesMatchedCount)
	{
	  // Either predicate failing to produce tuples stops our search.
	  return false;
	}

      PageSlotAddr psaAge;
      size_t firstPagePartsIdxByAge;
      TimevalAvgTracker tatByAge;
      bool someTuplesMatchedAge = findTupleByMaxAge(maxTupleAgeSeconds,
						    slotsPerPage,
						    psaAge,
						    firstPagePartsIdxByAge,
						    tatByAge);

      if (! someTuplesMatchedAge)
	{
	  // Either predicate failing to produce tuples stops our search.
	  return false;
	}

      // Find the smaller set of tuples...
      if (firstPagePartsIdxByNum > firstPagePartsIdxByAge)
	{
	  psa = psaNum;
	  timestampStats.add(tatTopN); // it was .clear()'ed above, so this is just assigment (semantically)
	}
      else if (firstPagePartsIdxByNum < firstPagePartsIdxByAge)
	{
	  psa = psaAge;
	  timestampStats.add(tatByAge); // it was .clear()'ed above, so this is just assigment (semantically)
	}
      else
	{
	  if (psaNum._slotNum > psaAge._slotNum)
	    {
	      psa = psaNum;
	      timestampStats.add(tatTopN); // it was .clear()'ed above, so this is just assigment (semantically)
	    }
	  else
	    {
	      psa = psaAge;
	      timestampStats.add(tatByAge); // it was .clear()'ed above, so this is just assigment (semantically)
	    }
	}

      return true;
    }
}

//===============================================================================

string CxnPtInfo::getDebugInfo() const
  throw (exception)
{
  ostringstream os;

  os << "CxnPtInfo (this = " << reinterpret_cast<const void *>(this) << ")" << endl
     << "   _limitTuplesByNum = " << _limitTuplesByNum << endl
     << "   _maxNumTuples = " << _maxNumTuples << endl
     << "   _limitTuplesByAge = " << _limitTuplesByAge << endl
     << "   _maxTupleAgeSeconds = " << _maxTupleAgeSeconds << endl
     << "   _pageParts (size = " << _pageParts.size() << ") = ";

  for (size_t i = 0; i < _pageParts.size(); ++i)
    {
      const PagePartInfo & aPart = _pageParts[i];

      os << endl
	 << "   #" << i << ": _pa = " << aPart._pa.getDebugInfo() << endl
	 << "      _lowSlotNum       = " << aPart._lowSlotNum << endl
	 << "      _highSlotNum      = " << aPart._highSlotNum << endl
	 << "      _lastEnqTime      = " << timevalToSimpleString(aPart._lastEnqTime);
    }

  return os.str();
}

//===============================================================================

CxnPtInfo::PagePartInfo::PagePartInfo()
{
}

//===============================================================================

CxnPtInfo::PagePartInfo::PagePartInfo(const PageAddr & pa,
				      size_t lowSlotNum,
				      size_t highSlotNum,
				      timeval lastEnqTime,
				      unsigned long lowSlotSerialNum,
				      TimevalAvgTracker timestampStats)
  : _pa(pa),
    _lowSlotNum(lowSlotNum),
    _highSlotNum(highSlotNum),
    _lastEnqTime(lastEnqTime),
    _lowSlotSerialNum(lowSlotSerialNum),
    _timestampStats(timestampStats)
{
}

//===============================================================================

bool CxnPtInfo::PagePartInfo::operator == (const PagePartInfo & rhs) const
{
  return 
    (_pa == rhs._pa)                               &&
    (_lowSlotNum == rhs._lowSlotNum)               &&
    (_highSlotNum == rhs._highSlotNum)             &&
    timevalsEqual(_lastEnqTime,  rhs._lastEnqTime) &&
    (_lowSlotSerialNum == rhs._lowSlotSerialNum);
}

//===============================================================================

void CxnPtInfo::PagePartInfo::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  // Save _pa...
  DOMElement * pChildElem;
  pChildElem = createDomElement(doc, "_pa");
  elem.appendChild(pChildElem);
  _pa.saveToXml(doc, * pChildElem);

  // Save _lowSlotNum and _highSlotNum...
  setDomAttribute(elem, "_lowSlotNum",       _lowSlotNum);
  setDomAttribute(elem, "_highSlotNum",      _highSlotNum);
  setDomAttribute(elem, "_lowSlotSerialNum", _lowSlotSerialNum);

  // Save _lastEnqTime...
  pChildElem = createDomElement(doc, "_lastEnqTime");
  elem.appendChild(pChildElem);
  saveTimevalToXml(_lastEnqTime, doc, * pChildElem);

  // Save _timestampStats...
  pChildElem = createDomElement(doc, "_timestampStats");
  elem.appendChild(pChildElem);
  _timestampStats.saveToXml(doc, * pChildElem);
}

//===============================================================================

void CxnPtInfo::PagePartInfo::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  // Load _pa...
  DOMElement * pChildElem;
  pChildElem = getDomChildByKeyTag(elem, "_pa");
  _pa.loadFromXml(* pChildElem);

  // Load _lowSlotNum and _highSlotNum...
  getDomAttribute(elem, "_lowSlotNum",  _lowSlotNum);
  getDomAttribute(elem, "_highSlotNum", _highSlotNum);
  getDomAttribute(elem, "_lowSlotSerialNum", _lowSlotSerialNum);

  // Load _lastEnqTime
  pChildElem = getDomChildByKeyTag(elem, "_lastEnqTime");
  loadTimevalFromXml(_lastEnqTime, * pChildElem);

  // Load _timestampStats... 
  pChildElem = getDomChildByKeyTag(elem, "_timestampStats");
  _timestampStats.loadFromXml(* pChildElem);
}

//===============================================================================

size_t CxnPtInfo::PagePartInfo::getNumTuples() const
{
  assert(_highSlotNum >= _lowSlotNum);
  return _highSlotNum - _lowSlotNum + 1;
}

//===============================================================================

size_t CxnPtInfo::getNumPagePartsTuples(size_t slotsPerPage) const
  throw (exception)
{
  size_t numTuples = 0;
  size_t numPageParts = _pageParts.size();

  // Tackle the first and last page parts specially, because all the other 
  // page parts can be summed with a single operation.
  if (numPageParts > 0)
    {
      numTuples += _pageParts[0].getNumTuples();
    }

  if (numPageParts > 1)
    {
      numTuples += _pageParts[numPageParts - 1].getNumTuples();
    }

  // Tackle the interior page parts...
  if (numPageParts > 2)
    {
      numTuples += (slotsPerPage * (numPageParts - 2));
    }

  return numTuples;
}

//===============================================================================

bool CxnPtInfo::findTupleByMaxAge(unsigned int maxTupleAgeSeconds,
				  size_t slotsPerPage,
				  PageSlotAddr & psa,
				  size_t & pagePartsIdx,
				  TimevalAvgTracker & timestampStats) const
  throw (exception)
{
  if ((_pageParts.size() == 0) ||
      (maxTupleAgeSeconds == 0))
    {
      return false;
    }

  timeval now;
  gettimeofday(& now, NULL);

  size_t numPageParts = _pageParts.size();

  // NOTE: Later on we can change this to a binary search. But for now, we'll
  // just code a linear scan to keep development time down. -cjc

  bool foundFirst = false;
  bool foundLast  = false;
  timestampStats.clear();

  for (size_t idx = 0; (! foundLast) && (idx < numPageParts); ++idx)
    {
      // Traverse backwards...
      const PagePartInfo & ppi = _pageParts[numPageParts - idx - 1];
      
      timestampStats.add(ppi._timestampStats);
      
      timeval youngestTupleAge = getTimevalDiff(ppi._lastEnqTime, now);

      if (youngestTupleAge.tv_sec <= maxTupleAgeSeconds)
	{
	  foundFirst = true;
	}

      if (youngestTupleAge.tv_sec > maxTupleAgeSeconds)
	{
	  // At least one tuple on this page-part isn't too old...
	  psa._pa        = ppi._pa;
	  psa._slotNum   = ppi._lowSlotNum;
	  psa._serialNum = ppi._lowSlotSerialNum;
	  pagePartsIdx   = idx;
	  foundLast = true;
	}
    }
  return foundFirst;
}

//===============================================================================

bool CxnPtInfo::findNthYoungestTuple(unsigned int maxNumTuples,
				     size_t slotsPerPage,
				     PageSlotAddr & psa,
				     size_t & pagePartsIdx,
				     TimevalAvgTracker & timestampStats) const
  throw (exception)
{
  if ((_pageParts.size() == 0) ||
      (maxNumTuples == 0))
    {
      return false;
    }

  // Keep this simple for now, even though we can do much better for performance
  // later. -cjc...

  size_t numPageParts = _pageParts.size();
  timestampStats.clear();
  unsigned int tuplesEaten = 0;

  for (size_t i = 1; (tuplesEaten < maxNumTuples) && (i <= numPageParts); ++i)
    {
      size_t idx = numPageParts - i;
      const PagePartInfo & ppi = _pageParts[idx];
      timestampStats.add(ppi._timestampStats);

      if ((tuplesEaten + ppi.getNumTuples()) < maxNumTuples)
	{
	  // We need to eat this whole page part, and then some...
	  tuplesEaten += ppi.getNumTuples();
	}
      /* Use alternative logic since we're now returning PPI-aligned results,
	 to facilitate timestamp stats accuracy...

      else if ((tuplesEaten + ppi.getNumTuples()) == maxNumTuples)
	{
	  tuplesEaten += ppi.getNumTuples();

	  // We need to eat exactly this whole page part, then we're done...
	  psa._pa        = ppi._pa;
	  psa._slotNum   = ppi._lowSlotNum;
	  psa._serialNum = ppi._lowSlotSerialNum;
	  pagePartsIdx   = idx;
	}
      else
	{
	  // We need to east just some of this page part, then we're done...
	  unsigned int tuplesStillNeeded = maxNumTuples - tuplesEaten;
	  tuplesEaten += tuplesStillNeeded;
	  assert(tuplesEaten == maxNumTuples);

	  psa._pa        = ppi._pa;
	  psa._slotNum   = ppi._highSlotNum - tuplesStillNeeded + 1;
	  psa._serialNum = _nextEnqSerialNum - maxNumTuples;
	  pagePartsIdx   = idx;
	}
      */
      else // we probably want to remove this block if we restore the commented-out code above...
	{
	  tuplesEaten += ppi.getNumTuples();

	  // We need to eat exactly this whole page part, then we're done...
	  psa._pa        = ppi._pa;
	  psa._slotNum   = ppi._lowSlotNum;
	  psa._serialNum = ppi._lowSlotSerialNum;
	  pagePartsIdx   = idx;
	}
    }

  if (tuplesEaten < maxNumTuples)
    {
      // There weren't enough tuples in the whole queue to meet our request...
      const PagePartInfo & ppi = _pageParts[0];

      psa._pa        = ppi._pa;
      psa._slotNum   = ppi._lowSlotNum;
      psa._serialNum = ppi._lowSlotSerialNum;
      pagePartsIdx   = 0;
    }

  return true;
}

//===============================================================================

