#include <sm/include/QviewTailPtrMap.H>

#include <XmlTempString.H>
#include <xercesDomUtil.H>
#include <StorageMgr_Exceptions.H>
#include <util.H>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

#include <algorithm>
#include <sstream>
#include <iostream>

//===============================================================================

QviewTailPtrMap::QviewTailPtrMap(size_t slotsPerPage, 
				 const SimplePageRing & sprTopology)
  throw (exception)
  : _slotsPerPage(slotsPerPage),
    _sprTopology(sprTopology)
{
}

//===============================================================================

QviewTailPtrMap::QviewTailPtrMap(const QviewTailPtrMap & src, 
				 const SimplePageRing & sprTopology)
  throw (exception)
  : _slotsPerPage(src._slotsPerPage),
    _sprTopology(sprTopology),
    _emptyQviews(src._emptyQviews),
    _nonEmptyQviews(src._nonEmptyQviews)
{
}

//===============================================================================

QviewTailPtrMap::~QviewTailPtrMap()
{
}

//===============================================================================

void QviewTailPtrMap::insertQview(int qviewId)
  throw (exception)
{
  _emptyQviews.push_back(qviewId);
}

//===============================================================================

void QviewTailPtrMap::deleteQview(int qbaseId, int qviewId /*,
							     set<pair<int, int> > & nonEmptyQviewsParameter*/)
  throw (exception)
{
  if (_nonEmptyQviews.erase(qviewId) == 1)
    {
      /*
      if (_nonEmptyQviews.erase(make_pair(qbaseId, qviewId)) != 1)
	{
	  throw SmException(__FILE__, __LINE__, 
			    "nonEmptyQviewsParameter didn't have a qviewId it should have");
	}
      */

      return;
    }

  vector<int>::iterator pos = find(_emptyQviews.begin(), _emptyQviews.end(), 
				   qviewId);
  if (pos != _emptyQviews.end())
    {
      _emptyQviews.erase(pos);
      return;
    }

  throw SmException(__FILE__, __LINE__, "The specified qviewId isn't in use.");
}

//===============================================================================

void QviewTailPtrMap::insertQviewAtLocation(int qviewId, 
					    const PageSlotAddr & psa,
					    const TimevalAvgTracker & tat,
					    unsigned long lastEnqSerialNum)
  throw (exception)
{
  if (psa._serialNum >= lastEnqSerialNum)
    {
      throw SmException(__FILE__, __LINE__, "psa._serialNum >= lastEnqSerialNum");
    }

  unsigned long numTuplesInQview = lastEnqSerialNum - psa._serialNum + 1;

  if (! _nonEmptyQviews.insert(make_pair(qviewId, 
					 QviewMetadata(psa, 
						       numTuplesInQview, 
						       tat))).second)
    {
      ostringstream os;
      os << "The qviewId value " << qviewId << " is already in use.";
      throw SmException(__FILE__, __LINE__, os.str());
    }
}

//===============================================================================

int QviewTailPtrMap::shiftQviewHeadsToNextPage(const PageSlotAddr & unshiftedPart)
  throw (exception)
{
  PageAddr nextPage = _sprTopology.getSuccessorPage(unshiftedPart._pa);

  int minSlotNum = _slotsPerPage; // 1 greater than any valid value

  for (map<int, QviewMetadata>::iterator pos = _nonEmptyQviews.begin();
       pos != _nonEmptyQviews.end();
       ++pos)
    {
      if ((pos->second._nextDeqPoint._pa     == unshiftedPart._pa) &&
	  (pos->second._nextDeqPoint._slotNum > unshiftedPart._slotNum))
	{
	  pos->second._nextDeqPoint._pa = nextPage;
	  minSlotNum = min(minSlotNum, pos->second._nextDeqPoint._slotNum);
	}
    }

  return minSlotNum;
}

//===============================================================================

void QviewTailPtrMap::advanceQviewTail(int qbaseId, 
				       int qviewId, 
				       size_t numSlots,
				       unsigned long lastGoodSerialNum,
				       unsigned long long enqTstampSecSum,
				       unsigned long long enqTstampMicroSecSum,
				       set<pair<int, int> > & nonEmptyQviews)
  throw (exception)
{
  if (numSlots < 1)
    {
      throw SmException(__FILE__, __LINE__, "numSlots < 1");
    }

  map<int, QviewMetadata>::iterator pos = _nonEmptyQviews.find(qviewId);
  if (pos == _nonEmptyQviews.end())
    {
      throw SmException(__FILE__, __LINE__, "Unmapped qviewId");
    }

  QviewMetadata & qvm = pos->second;
  PageSlotAddr & psa  = qvm._nextDeqPoint;

  if ((psa._serialNum + numSlots) > (lastGoodSerialNum + 1))
    {
      throw SmException(__FILE__, __LINE__, "Qview underflow");
    }

  if ((psa._serialNum + numSlots) == (lastGoodSerialNum + 1))
    {
      _nonEmptyQviews.erase(pos);
      _emptyQviews.push_back(qviewId);

      if (nonEmptyQviews.erase(make_pair(qbaseId, qviewId)) != 1)
	{
	  ostringstream os;
	  os << "The nonEmptyQviews set didn't contain the element to be removed" << endl
	     << "   (qbaseId = " << qbaseId << ", qviewId = " << qviewId << ")";
	  throw SmException(__FILE__, __LINE__, os.str());
	}
    }
  else
    {
      qvm._numTuplesInQview -= numSlots;

      if (qviewId != 0)
	{
	  qvm._avgTstampTracker.subtractTimevalSums(numSlots, enqTstampSecSum, 
						    enqTstampMicroSecSum);
	}

      _sprTopology.advancePsaByOffset(psa, numSlots);
    }
}

//===============================================================================

PageSlotAddr QviewTailPtrMap::getLowestSerialNumInfo() const
  throw (exception)
{
  map<int, QviewMetadata>::const_iterator pos = _nonEmptyQviews.begin();
  if (pos == _nonEmptyQviews.end())
    {
      throw SmException(__FILE__, __LINE__, "No non-empty qviews");
    }

  unsigned long lowestSerialNum = pos->second._nextDeqPoint._serialNum;
  map<int, QviewMetadata>::const_iterator posFinal = pos;
  ++pos;

  while (pos != _nonEmptyQviews.end())
    {
      if (pos->second._nextDeqPoint._serialNum < lowestSerialNum)
	{
	  posFinal = pos;
	  lowestSerialNum = pos->second._nextDeqPoint._serialNum;
	}

      ++pos;
    }

  return posFinal->second._nextDeqPoint;
}

//===============================================================================

void QviewTailPtrMap::getQviewStatus(int qviewId, 
				     unsigned long lastGoodSerialNum,
				     size_t & numTuplesInQview,
				     PageSlotAddr & nextDeqPoint,
				     timeval & avgTimestamp) const
  throw (exception)
{
  // Perhaps it's non-empty...
  map<int, QviewMetadata>::const_iterator pos = _nonEmptyQviews.find(qviewId);
  if (pos != _nonEmptyQviews.end())
    {
      nextDeqPoint = pos->second._nextDeqPoint;
 
      numTuplesInQview = pos->second._numTuplesInQview;
      assert(numTuplesInQview > 0);

      if (pos->first == 0)
	{
	  avgTimestamp.tv_sec = 0;
	  avgTimestamp.tv_usec = 0;
	}
      else
	{
	  if (pos->second._avgTstampTracker.getNumTimevals() > 0)
	    {
	      pos->second._avgTstampTracker.getAvgTimeval(avgTimestamp);
	    }
	  else
	    {
	      avgTimestamp.tv_sec = 0;
	      avgTimestamp.tv_usec = 0;
	    }
	}

      return;
    }

  // Perhaps it's empty but still mapped...
  size_t numEmpty = _emptyQviews.size();
  for (size_t i = 0; i < numEmpty; ++i)
    {
      if (_emptyQviews.at(i) == qviewId)
	{
	  numTuplesInQview = 0;
	  return;
	}
    }

  throw SmException(__FILE__, __LINE__, "Unrecognized qviewId");
}

//===============================================================================

bool QviewTailPtrMap::areAnyNonEmptyQviewsMapped() const
{
  return (! _nonEmptyQviews.empty());
}

//===============================================================================

void QviewTailPtrMap::onEnqueue(int qbaseId, 
				unsigned long numTuplesEnqueued,
				const PageAddr & firstEnqPage, int firstEnqSlot, 
				unsigned long firstEnqSerialNum,
				unsigned long long enqTstampSecSum,
				unsigned long long enqTstampMicroSecSum,
				set<pair<int, int> > & nonEmptyQviewsParameter)
  throw (exception)
{
  // Update stats of existing non-empty qviews...
  for (map<int, QviewMetadata>::iterator pos =  _nonEmptyQviews.begin();
       pos != _nonEmptyQviews.end();
       ++pos)
    {
      pos->second._numTuplesInQview += numTuplesEnqueued;
      pos->second._avgTstampTracker.addTimevalSums(numTuplesEnqueued, 
						   enqTstampSecSum, 
						   enqTstampMicroSecSum);
    }

  // Any previously empty qview is definitely non-empty now...
  size_t numEmpty = _emptyQviews.size();
  if (numEmpty > 0)
    {
      QviewMetadata newQvm(PageSlotAddr(firstEnqPage, firstEnqSlot, firstEnqSerialNum),
			   numTuplesEnqueued,
			   TimevalAvgTracker(numTuplesEnqueued, enqTstampSecSum, enqTstampMicroSecSum));

      for (size_t i = 0; i < numEmpty; ++i)
	{
	  int qviewId = _emptyQviews[i];

	  if (qviewId == 0)
	    {
	      QviewMetadata cxnPtQvm(PageSlotAddr(firstEnqPage, firstEnqSlot, firstEnqSerialNum),
				     numTuplesEnqueued,
				     TimevalAvgTracker(numTuplesEnqueued, 0, 0));

	      _nonEmptyQviews.insert(make_pair(qviewId, cxnPtQvm));
	    }
	  else
	    {
	      _nonEmptyQviews.insert(make_pair(qviewId, newQvm));
	    }

	  if (! nonEmptyQviewsParameter.insert(make_pair(qbaseId, qviewId)).second)
	    {
	      ostringstream os;
	      os << "The nonEmptyQviews set already contained the element" << endl
		 << "   (qbaseId = " << qbaseId << ", qviewId = " << qviewId << ")";
	      throw SmException(__FILE__, __LINE__, os.str());
	    }
	}
    }

  _emptyQviews.clear();
}

//===============================================================================

void QviewTailPtrMap::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  DOMElement * pEmptyQviewsElem = createDomElement(doc, "_emptyQviews");
  elem.appendChild(pEmptyQviewsElem);
  saveIntVectorToXml(_emptyQviews, doc, * pEmptyQviewsElem);


  DOMElement * pNonEmptyQviewsElem = createDomElement(doc, "_nonEmptyQviews");
  elem.appendChild(pNonEmptyQviewsElem);

  for (map<int, QviewMetadata>::const_iterator pos = _nonEmptyQviews.begin();
       pos != _nonEmptyQviews.end(); 
       ++pos)
    {
      DOMElement * pChildQvm = createDomElement(doc, "QviewMetadata");
      pNonEmptyQviewsElem->appendChild(pChildQvm);

      setDomAttribute(* pChildQvm, "qviewId", pos->first);
      pos->second.saveToXml(doc, * pChildQvm);
    }
}

//===============================================================================

void QviewTailPtrMap::loadFromXml(DOMElement & elem)
  throw (exception)
{
  DOMElement & emptyQviewsElem = *getDomChildByKeyTag(elem, "_emptyQviews");
  loadIntVectorFromXml(_emptyQviews, emptyQviewsElem);

  DOMElement & pageRingElem = *getDomChildByKeyTag(elem, "_nonEmptyQviews");
  DOMNodeList * pChildList = pageRingElem.getChildNodes();

  XmlTempString xstrQvm("QviewMetadata");

  for (XMLSize_t i = 0; i < (pChildList->getLength()); ++i)
    {
      DOMElement & qvmElem = *(dynamic_cast<DOMElement *>(pChildList->item(i)));
      
      if (XMLString::compareString(qvmElem.getTagName(), 
				   xstrQvm.toXml()) != 0)
	{
	  throw SmException(__FILE__, __LINE__, "A child element wasn't tagged as QviewMetadata");
	}

      int qviewId;
      getDomAttribute(qvmElem, "qviewId", qviewId);

      _nonEmptyQviews.insert(make_pair(qviewId, QviewMetadata(qvmElem)));
    }
}

//===============================================================================

string QviewTailPtrMap::getDebugInfo(int indentLevel) const
  throw (std::exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "QviewTailPtrMap(this = " << this << ")" << endl
     << indentStr << "   _slotsPerPage = " << _slotsPerPage << endl;

  os << indentStr << "   _emptyQviews: { ";
  for (size_t i = 0; i < _emptyQviews.size(); ++i)
    {
      os << _emptyQviews.at(i);

      if (i < (_emptyQviews.size() - 1))
	{
	  os << ", ";
	}
    }
  os << " }" << endl;


  os << indentStr << "   _nonEmptyQviews:" << endl;
  if (_nonEmptyQviews.size() == 0)
    {
      os << indentStr << "      (empty)" << endl;
    }
  else
    {
      for (map<int, QviewMetadata>::const_iterator pos = _nonEmptyQviews.begin();
	   pos != _nonEmptyQviews.end(); 
	   ++pos)
	{
	  os << indentStr << "      qviewId = " << pos->first << ", PSA = " << endl
	     << pos->second._nextDeqPoint.getDebugInfo(indentLevel + 6) << endl;
	}
    }

  return os.str();
}

//===============================================================================

void QviewTailPtrMap::getNonEmptyQviews(int qbaseId, 
					set<pair<int, int> > & nonEmptyQviews) const
  throw (exception)
{
  for (map<int, QviewMetadata>::const_iterator pos = _nonEmptyQviews.begin();
       pos != _nonEmptyQviews.end();
       ++pos)
    {
      int qviewId = pos->first;

      if (! nonEmptyQviews.insert(make_pair(qbaseId, qviewId)).second)
	{
	  ostringstream os;
	  os << "Couldn't insert into nonEmptyQviews the element" << endl
	     << "   (qbaseId = " << qbaseId << ", qviewId = " << qviewId << ")";
	  throw SmException(__FILE__, __LINE__, os.str());
	}
    }
}

//===============================================================================

bool QviewTailPtrMap::operator == (const QviewTailPtrMap & rhs) const
  throw (exception)
{
  if ((_slotsPerPage != rhs._slotsPerPage) ||
      (_emptyQviews.size() != rhs._emptyQviews.size()) ||
      (_nonEmptyQviews.size() != rhs._nonEmptyQviews.size()))
    return false;

  // Gotta' hate O(n^2) searches. Still, we expect to use this operator == only
  // rarely (during debugging), so we're still better off with vectors rather than
  // sets.
  size_t numEmptyQviews = _emptyQviews.size();

  for (size_t i = 0; i < numEmptyQviews; ++i)
    {
      bool found = false;
      for (size_t j = 0; (j < numEmptyQviews) && (! found); ++j)
	{
	  if (_emptyQviews.at(i) == rhs._emptyQviews.at(j))
	    //	  if (_emptyQviews[i] == rhs._emptyQviews[j])
	    found = true;
	}

      if (! found)
	return false;
    }

  return _nonEmptyQviews == rhs._nonEmptyQviews;
}

//===============================================================================

bool QviewTailPtrMap::operator != (const QviewTailPtrMap & rhs) const
  throw (exception)
{
  return ! ((*this) == rhs);
}

//===============================================================================

void QviewTailPtrMap::debugCheck() const
{
  _sprTopology.debugCheck();
}

//===============================================================================

QviewTailPtrMap::QviewMetadata::QviewMetadata(const PageSlotAddr & nextDeqPoint,
					      unsigned long numTuplesInQview,
					      const TimevalAvgTracker & avgTstampTracker)

  throw (exception)
  : _nextDeqPoint(nextDeqPoint),
    _numTuplesInQview(numTuplesInQview),
    _avgTstampTracker(avgTstampTracker)
{
}

//===============================================================================

QviewTailPtrMap::QviewMetadata::QviewMetadata(DOMElement & elem)
  throw (exception)
{
  loadFromXml(elem);
}

//===============================================================================

QviewTailPtrMap::QviewMetadata::QviewMetadata(const QviewMetadata & src)
  throw (exception)
  : _nextDeqPoint(src._nextDeqPoint),
    _numTuplesInQview(src._numTuplesInQview),
    _avgTstampTracker(src._avgTstampTracker)
{
}

//===============================================================================

void QviewTailPtrMap::QviewMetadata::saveToXml(DOMDocument & doc, 
					       DOMElement & elem) const
  throw (exception)
{
  setDomAttribute(elem, "_numTuplesInQview",    _numTuplesInQview);

  DOMElement * pChildElem;

  pChildElem = createDomElement(doc, "_nextDeqPoint");
  elem.appendChild(pChildElem);
  _nextDeqPoint.saveToXml(doc, * pChildElem);

  pChildElem = createDomElement(doc, "_avgTstampTracker");
  elem.appendChild(pChildElem);
  _avgTstampTracker.saveToXml(doc, * pChildElem);
}
  
//===============================================================================
  
void QviewTailPtrMap::QviewMetadata::loadFromXml(DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_numTuplesInQview",    _numTuplesInQview);

  DOMElement & nextDeqPointElem = *getDomChildByKeyTag(elem, "_nextDeqPoint");
  _nextDeqPoint.loadFromXml(nextDeqPointElem);

  DOMElement & avgTstampTrackerElem = *getDomChildByKeyTag(elem, "_avgTstampTracker");
  _avgTstampTracker.loadFromXml(avgTstampTrackerElem);
}

//===============================================================================

bool QviewTailPtrMap::QviewMetadata::operator == (const QviewMetadata & rhs) const
{
  return 
    (_numTuplesInQview    == rhs._numTuplesInQview)    &&
    (_avgTstampTracker    == rhs._avgTstampTracker)    &&
    (_nextDeqPoint        == rhs._nextDeqPoint);
}

//===============================================================================

bool QviewTailPtrMap::QviewMetadata::operator != (const QviewMetadata & rhs) const
{
  return ! ((*this) == rhs);
}

//===============================================================================



