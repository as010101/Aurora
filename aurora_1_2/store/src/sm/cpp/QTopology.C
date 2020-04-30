#include <sm/include/QTopology.H>
#include <StorageMgr_Exceptions.H>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <sstream>
#include <xercesDomUtil.H>
#include <XmlTempString.H>
#include <iostream>

//===============================================================================

QTopology::QTopology(size_t slotsPerPage, bool cxnPoint)
  throw (exception) : 
  _slotsPerPage(slotsPerPage),
  _pages(slotsPerPage),
  _nextDeqPoints(slotsPerPage, _pages),
  _firstEnqOccured(false),
  _pCxnPtInfo(NULL)
{
  if (cxnPoint)
    {
      _pCxnPtInfo = new CxnPtInfo();
      _nextDeqPoints.insertQview(0);
    }
}

//===============================================================================

QTopology::QTopology(const QTopology & src)
  throw (exception)
  : _slotsPerPage(src._slotsPerPage),
    _pages(src._pages),
    _nextDeqPoints(src._nextDeqPoints, _pages),
    _firstEnqOccured(src._firstEnqOccured),
    _prevEnqPoint(src._prevEnqPoint),
    _nextEnqBounds(src._nextEnqBounds)
{
  if (src._pCxnPtInfo != NULL)
    {
      _pCxnPtInfo = new CxnPtInfo(*(src._pCxnPtInfo));
    }
  else
    {
      _pCxnPtInfo = NULL;
    }
}

//===============================================================================

QTopology::~QTopology()
{
  if (_pCxnPtInfo != NULL)
    {
      delete _pCxnPtInfo;
    }
}

//===============================================================================

void QTopology::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_slotsPerPage",    _slotsPerPage);
  getDomAttribute(elem, "_firstEnqOccured", _firstEnqOccured);

  DOMElement * pChildElem; 

  pChildElem = getDomChildByKeyTag(elem, "_pages");
  _pages.loadFromXml(* pChildElem);

  pChildElem = getDomChildByKeyTag(elem, "_nextDeqPoints");
  _nextDeqPoints.loadFromXml(* pChildElem);

  pChildElem = getDomChildByKeyTag(elem, "_prevEnqPoint");
  _prevEnqPoint.loadFromXml(* pChildElem);

  // Load _pCxnPtInfo appropriately...
  bool isCxnPt;
  getDomAttribute(elem, "isCxnPt", isCxnPt);

  if (_pCxnPtInfo != NULL)
    {
      delete _pCxnPtInfo;
    }

  if (isCxnPt)
    {
      pChildElem = getDomChildByKeyTag(elem, "_pCxnPtInfo");
      _pCxnPtInfo = new CxnPtInfo();
      _pCxnPtInfo->loadFromXml(* pChildElem);
    }
  else
    {
      _pCxnPtInfo = NULL;
    }
}

//===============================================================================
  
void QTopology::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  setDomAttribute(elem, "_slotsPerPage",    _slotsPerPage);
  setDomAttribute(elem, "_firstEnqOccured", _firstEnqOccured);

  DOMElement * pChildElem;

  pChildElem = createDomElement(doc, "_pages");
  elem.appendChild(pChildElem);
  _pages.saveToXml(doc, * pChildElem);

  pChildElem = createDomElement(doc, "_nextDeqPoints");
  elem.appendChild(pChildElem);
  _nextDeqPoints.saveToXml(doc, * pChildElem);

  pChildElem = createDomElement(doc, "_prevEnqPoint");
  elem.appendChild(pChildElem);
  _prevEnqPoint.saveToXml(doc, * pChildElem);

  // Save _pCxnPtInfo appropriately...
  if (_pCxnPtInfo != NULL)
    {
      setDomAttribute(elem, "isCxnPt", true);

      pChildElem = createDomElement(doc, "_pCxnPtInfo");
      elem.appendChild(pChildElem);
      _pCxnPtInfo->saveToXml(doc, * pChildElem);
    }
  else
    {
      setDomAttribute(elem, "isCxnPt", false);
    }
}

//===============================================================================

size_t QTopology::getEnqCapacity() const
  throw (exception)
{
  if ((! _firstEnqOccured) || (! _nextDeqPoints.areAnyNonEmptyQviewsMapped()))
    {
      return _pages.getNumSlotsInRing();
    }
  else
    {
      size_t slotsInUse = _prevEnqPoint._serialNum 
	- _nextDeqPoints.getLowestSerialNumInfo()._serialNum + 1;

      return _pages.getNumSlotsInRing() - slotsInUse;
    }
}

//===============================================================================

bool QTopology::doesAddPageRequireSplit() const
  throw (exception)
{
  if ((! _firstEnqOccured) || (! _nextDeqPoints.areAnyNonEmptyQviewsMapped()))
    {
      return false;
    }
  else
    {
      PageSlotAddr psa = _nextDeqPoints.getLowestSerialNumInfo();
      return ((_prevEnqPoint._pa == psa._pa) && 
	      (_prevEnqPoint._slotNum < psa._slotNum));
    }
}

//===============================================================================

void QTopology::addPageWithoutSplit(const PageAddr & pa)
  throw (exception)
{
  if (! _firstEnqOccured)
    {
      _pages.addPageAnywhere(pa);
    }
  else
    {
      _pages.addPageSuccessor(pa, _prevEnqPoint._pa);
    }
}

//===============================================================================

void QTopology::addPageWithSplit(const PageAddr & newPage, PageAddr & oldPage, 
				 size_t & firstSlotToCopy, size_t & lastSlotToCopy)
  throw (exception)
{
  PageSlotAddr psa = _nextDeqPoints.getLowestSerialNumInfo();
  oldPage = psa._pa;
  firstSlotToCopy = psa._slotNum;
  lastSlotToCopy = _slotsPerPage - 1;

  if ((psa._pa != _prevEnqPoint._pa) || (psa._slotNum <= _prevEnqPoint._slotNum))
    {
      throw SmException(__FILE__, __LINE__, "The page doen't need to be split");
    }
  
  _pages.addPageSuccessor(newPage, psa._pa);
  _nextDeqPoints.shiftQviewHeadsToNextPage(_prevEnqPoint);

  if (_pCxnPtInfo != NULL)
    {
      _pCxnPtInfo->onPageSplit(newPage);
    }
}

//===============================================================================

IntraPageSlotRange QTopology::getNextEnqRegion()
  throw (exception)
{  decideNextEnqBounds();

 return _nextEnqBounds;
}

//===============================================================================

void QTopology::onEnqComplete(int qbaseId,
			      size_t numSlotsFilled,
			      unsigned long long enqTstampSecSum,
			      unsigned long long enqTstampMicroSecSum,
			      set<pair<int, int> > & nonEmptyQviewsParameter)
  throw (exception)
{
  if (numSlotsFilled > _nextEnqBounds.getNumSlots())
    {
      throw SmException(__FILE__, __LINE__, "Enqueue buffer overflow");
    }

  // This is expensive. Once the software is trusted, we can remove it. It's
  // just a sanity check... (Alternatively, make getEnqCapacity() cheaper)
  if (numSlotsFilled > getEnqCapacity())
    {
      throw SmException(__FILE__, __LINE__, "Enqueue buffer overflow");
    }

  // The real work...
  if (numSlotsFilled > 0)
    {
      _nextDeqPoints.onEnqueue(qbaseId,
			       numSlotsFilled,
			       _nextEnqBounds._pa, 
			       _nextEnqBounds._lowSlot, 
			       _nextEnqBounds._lowSlotSerialNum,
			       enqTstampSecSum,
			       enqTstampMicroSecSum,
			       nonEmptyQviewsParameter);

      _firstEnqOccured = true;

      _prevEnqPoint._pa        = _nextEnqBounds._pa;
      _prevEnqPoint._slotNum   = _nextEnqBounds._lowSlot;
      _prevEnqPoint._serialNum = _nextEnqBounds._lowSlotSerialNum;

      // Maintain cxn point stuff...
      if (_pCxnPtInfo != NULL)
	{
	  _pCxnPtInfo->onEnqComplete(_prevEnqPoint._pa,
				     _prevEnqPoint._slotNum,
				     _prevEnqPoint._slotNum + numSlotsFilled - 1,
				     _slotsPerPage,
				     enqTstampSecSum,
				     enqTstampMicroSecSum);

	    // The keepCxnPointPruned(...) operation is logically unrelated to the method  
	    // it appears in, but we hijack the caller's thread to do this so that we don't 
	    // need to have our own thread just for this purpose.
	    keepCxnPointPruned(qbaseId, nonEmptyQviewsParameter);
	}

      _pages.advancePsaByOffset(_prevEnqPoint, numSlotsFilled - 1);
    }
}

//===============================================================================

void QTopology::createQView(int qviewId)
  throw (exception)
{
  if (qviewId < 1)
    {
      throw SmException(__FILE__, __LINE__, "qviewId must be > 0");
    }

  _nextDeqPoints.insertQview(qviewId);
}

//===============================================================================

// Throws an exception if 'qviewId' is already defined for this queue, or it 
// this QTopology doesn't have a cxn point. Also, at least one of 
// 'limitTuplesByNum' and 'limitTuplesByAge' must be true.
//
// qviewId must be > 0.
void QTopology::createCxnPointQView(int qviewId,
				    bool limitTuplesByNum,
				    size_t maxNumTuples,
				    bool limitTuplesByAge,
				    unsigned int maxTupleAgeSeconds)
  throw (exception)
{
  if (qviewId < 1)
    {
      throw SmException(__FILE__, __LINE__, "qviewId must be > 0");
    }

  if (_pCxnPtInfo == NULL)
    {
      throw SmException(__FILE__, __LINE__, "Not a cxn point");
    }

  if ((! limitTuplesByNum) && (! limitTuplesByAge))
    {
      throw SmException(__FILE__, __LINE__, "'limitTuplesByNum' and/or 'limitTuplesByAge' must be 'true': ");
    }

  PageSlotAddr psa;

  TimevalAvgTracker tat;
  bool hasHistory = _pCxnPtInfo->getNextDeqPoint(_slotsPerPage,
						 psa,
						 tat,
						 limitTuplesByNum,
						 maxNumTuples,
						 limitTuplesByAge,
						 maxTupleAgeSeconds);

  if (hasHistory)
    {
      // Since _pCxnPtInfo->getNextDeqPoint(...) returned 'true', we know for
      // sure that the queue isn't empty, which implies that a previous enq has
      // occured. Therefore, we know that '_prevEnqPoint' has a meaningful value.
      _nextDeqPoints.insertQviewAtLocation(qviewId, psa, tat,
					   _prevEnqPoint._serialNum);
    }
  else
    {
      _nextDeqPoints.insertQview(qviewId);
    }
}

//===============================================================================

void QTopology::setCxnPointHistorySpec(bool limitTuplesByNum,
				       size_t maxNumTuples,
				       bool limitTuplesByAge,
				       unsigned int maxTupleAgeSeconds)
  throw (exception)
{
  if (_pCxnPtInfo == NULL)
    {
      throw SmException(__FILE__, __LINE__, "Not a cxn point");
    }

  if ((! limitTuplesByNum) && (! limitTuplesByAge))
    {
      throw SmException(__FILE__, __LINE__, "'limitTuplesByNum' and/or 'limitTuplesByAge' must be 'true': ");
    }

  _pCxnPtInfo->setRetentionBounds(limitTuplesByNum,
				  maxNumTuples,
				  limitTuplesByAge,
				  maxTupleAgeSeconds);
}

//===============================================================================

void QTopology::deleteQView(int qbaseId, int qviewId/*,
						      set<pair<int, int> > & nonEmptyQviewsParameter*/)
    throw (exception)
{
  if (qviewId < 1)
    {
      throw SmException(__FILE__, __LINE__, "qviewId must be > 0");
    }

  _nextDeqPoints.deleteQview(qbaseId, qviewId);

  // Whenever we put in logic to free pages from a highly deflated queue, this
  // would probably be one dandy time to call that logic. -cjc
}

//===============================================================================

void QTopology::getQviewStats(int qviewId, size_t & numTuples, 
			      timeval & avgTimestamp) const
  throw (exception)
{ 
  if (_firstEnqOccured)
    {
      PageSlotAddr nextDeqPoint;
      _nextDeqPoints.getQviewStatus(qviewId, _prevEnqPoint._serialNum,
				    numTuples, nextDeqPoint, avgTimestamp);
    }
  else
    {
      numTuples = 0;
    }
}

//===============================================================================

IntraPageSlotRange QTopology::getNextDeqRegion(int qviewId) const
  throw (exception)
{
  if (qviewId < 1)
    {
      throw SmException(__FILE__, __LINE__, "qviewId must be > 0");
    }

  if (! _firstEnqOccured)
    {
      throw SmException(__FILE__, __LINE__, "Qview is empty");
    }

  size_t numTuples;
  PageSlotAddr nextDeqPoint;
  timeval avgTimestamp;

  _nextDeqPoints.getQviewStatus(qviewId, _prevEnqPoint._serialNum,
				numTuples, nextDeqPoint, avgTimestamp);
  if (numTuples == 0)
    {
      throw SmException(__FILE__, __LINE__, "Qview is empty");
    }

  // Number of slots that are in this qview, other than the slot pointed to by
  // nextDeqPoint.
  size_t otherSlotsInQview = numTuples - 1; //_prevEnqPoint._serialNum - nextDeqPoint._serialNum;

  size_t slotsToEndOfPage = _slotsPerPage - nextDeqPoint._slotNum - 1;

  size_t lowSlot = nextDeqPoint._slotNum;
  size_t highSlot = lowSlot + min(slotsToEndOfPage, otherSlotsInQview);
  unsigned long lowSlotSerialNum = nextDeqPoint._serialNum;

  return IntraPageSlotRange(nextDeqPoint._pa, lowSlot, highSlot, 
			    lowSlotSerialNum);
}

//===============================================================================

void QTopology::onDeqComplete(int qbaseId, 
			      int qviewId, 
			      size_t numSlotsFilled,
			      unsigned long long deqTstampSecSum,
			      unsigned long long deqTstampMicroSecSum,
			      set<pair<int, int> > & nonEmptyQviewsParameter)
  throw (exception)
{
  if (qviewId < 1)
    {
      throw SmException(__FILE__, __LINE__, "qviewId must be > 0");
    }

  // The keepCxnPointPruned(...) operation is logically unrelated to the method  
  // it appears in, but we hijack the caller's thread to do this so that we don't 
  // need to have our own thread just for this purpose.
  keepCxnPointPruned(qbaseId, nonEmptyQviewsParameter);

  // This is expensive. Once the software is trusted, we can remove it. It's
  // just a sanity check...
  IntraPageSlotRange r = getNextDeqRegion(qviewId);
  if (r.getNumSlots() < numSlotsFilled)
    {
      throw SmException(__FILE__, __LINE__, "Dequeue buffer underflow");
    }

  // The real work...
  if (numSlotsFilled > 0)
    {
      _nextDeqPoints.advanceQviewTail(qbaseId, 
				      qviewId, 
				      numSlotsFilled, 
				      _prevEnqPoint._serialNum,
				      deqTstampSecSum,
				      deqTstampMicroSecSum,
				      nonEmptyQviewsParameter);
    }
}

//===============================================================================

void QTopology::decideNextEnqBounds()
  throw (exception)
{
  if (getEnqCapacity() == 0)
    {
      throw SmException(__FILE__, __LINE__, "Can't call this when the queue is full");
    }

  if ((! _firstEnqOccured) || (! _nextDeqPoints.areAnyNonEmptyQviewsMapped()))
    {
      // The queue has spare capacity, but no content. The enqueue can go 
      // wherever we want it to go...
      _nextEnqBounds._pa = (_pages.getPageRing()).at(0);
      //      _nextEnqBounds._pa = (_pages.getPageRing())[0];
      _nextEnqBounds._lowSlot = 0;
      _nextEnqBounds._highSlot = _slotsPerPage - 1;

      if (_firstEnqOccured)
	{
	  _nextEnqBounds._lowSlotSerialNum = _prevEnqPoint._serialNum + 1;
	}
      else
	{
	  _nextEnqBounds._lowSlotSerialNum = 0;
	}
    }
  else
    {
      // The queue isn't empty, so that means the enq point and 
      // oldest-record-location are presently fixed. There's no room for creativity
      // here...
      PageSlotAddr nextEnqPoint(_prevEnqPoint);
      _pages.advancePsaByOffset(nextEnqPoint, 1);

      _nextEnqBounds._pa = nextEnqPoint._pa;
      _nextEnqBounds._lowSlot = nextEnqPoint._slotNum;
      _nextEnqBounds._lowSlotSerialNum = nextEnqPoint._serialNum;

      PageSlotAddr oldestUsedSlot = _nextDeqPoints.getLowestSerialNumInfo();

      if ((oldestUsedSlot._pa != nextEnqPoint._pa) ||        // on different pages, or
	  (nextEnqPoint._slotNum > oldestUsedSlot._slotNum))  // occupies only earlier part of page
	{
	  _nextEnqBounds._highSlot = _slotsPerPage - 1;
	}
      else
	{
	  _nextEnqBounds._highSlot = oldestUsedSlot._slotNum - 1;
	}
    }
}

//===============================================================================

string QTopology::getDebugInfo(int indentLevel) const
  throw (std::exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "QTopology(this = " << this << ")" << endl
     << indentStr << "   _slotsPerPage = "      << _slotsPerPage << endl
     << indentStr << "   _firstEnqOccured = "   << (_firstEnqOccured ? "true" : "false") << endl
     << indentStr << "   _nextEnqBounds = "      << _nextEnqBounds.getDebugInfo(indentLevel + 3) << endl
     << indentStr << "   _prevEnqPoint = "      << _prevEnqPoint.getDebugInfo(indentLevel + 3) << endl
     << indentStr << "   _pages = "             << _pages.getDebugInfo(indentLevel + 3) << endl
     << indentStr << "   _nextDeqPoints = "     << _nextDeqPoints.getDebugInfo(indentLevel + 3) << endl
     << indentStr << "   getEnqCapacity() = "   << getEnqCapacity() << endl;

  return os.str();
}

//===============================================================================

bool QTopology::isCxnPoint() const
{
  return (_pCxnPtInfo != NULL);
}

//===============================================================================

const vector<PageAddr> & QTopology::getPageRing() const
  throw (exception)
{
  return _pages.getPageRing();
}

//===============================================================================

void QTopology::getPageOccupancy(int qviewId, 
				 size_t & firstPageIdx, 
				 vector<size_t> & numRecs) const
  throw (exception)
{
  PageSlotAddr psaNextDeqPoint;
  size_t numTuples;
  timeval avgTimestamp;

  _nextDeqPoints.getQviewStatus(qviewId, 
				_prevEnqPoint._serialNum,
				numTuples, 
				psaNextDeqPoint, 
				avgTimestamp);

  if (numTuples == 0)
    {
      throw SmException(__FILE__, __LINE__, "The specified qview is empty");
    }

  _pages.getPageOccupancy(psaNextDeqPoint._pa, psaNextDeqPoint._slotNum,
			  _prevEnqPoint._pa,   _prevEnqPoint._slotNum,
			  firstPageIdx, numRecs);
}

//===============================================================================

void QTopology::getNonEmptyQviews(int qbaseId, 
				  set<pair<int, int> > & nonEmptyQviews) const
  throw (exception)
{
  _nextDeqPoints.getNonEmptyQviews(qbaseId, nonEmptyQviews);
}

//===============================================================================

bool QTopology::operator == (const QTopology & rhs) const
  throw (exception)
{
  return
    (_slotsPerPage == rhs._slotsPerPage) &&
    (_pages == rhs._pages) &&
    (_nextDeqPoints == rhs._nextDeqPoints) &&
    (_firstEnqOccured == rhs._firstEnqOccured) &&
    (_prevEnqPoint == rhs._prevEnqPoint);
}

//===============================================================================

bool QTopology::operator != (const QTopology & rhs) const
  throw (exception)
{
  return ! ((*this) == rhs);
}

//===============================================================================

void QTopology::debugCheck() const
{
  _pages.debugCheck();
  _nextDeqPoints.debugCheck();
}

//===============================================================================

void QTopology::forceCxnPointPruning(int thisQbaseId,
			      set<pair<int, int> > & nonEmptyQviewsParameter)
{
  if (_pCxnPtInfo == NULL)
    {
      throw SmException(__FILE__, __LINE__, "Not a cxn point");
    }

  keepCxnPointPruned(thisQbaseId, nonEmptyQviewsParameter);
}

//===============================================================================

void QTopology::keepCxnPointPruned(int thisQbaseId, 
				   set<pair<int, int> > & nonEmptyQviewsParameter)
  throw (exception)
{
  if (_pCxnPtInfo != NULL)
    {
      size_t slotsEaten = _pCxnPtInfo->doBoundedTruncation(_slotsPerPage);
      if (slotsEaten > 0)
	{
	  _nextDeqPoints.advanceQviewTail(thisQbaseId, 0, slotsEaten,
					  _prevEnqPoint._serialNum,
					  0, 0, nonEmptyQviewsParameter);

	}
    }
}
