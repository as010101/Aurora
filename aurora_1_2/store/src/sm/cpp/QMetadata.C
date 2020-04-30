#include <sm/include/QMetadata.H>
#include <StorageMgr_Exceptions.H>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <sstream>
#include <xercesDomUtil.H>
#include <XmlTempString.H>

//===============================================================================

QMetadata::QMetadata(size_t bytesPerPage,
		     size_t bytesPerRecord,
		     bool cxnPoint)
  throw (exception)
  : _qt(bytesPerPage / bytesPerRecord, cxnPoint),
    _bytesPerRecord(bytesPerRecord)
{
}

//===============================================================================

QMetadata::QMetadata(const QMetadata & src)
  throw (exception)
  : _qt(src._qt),
    _bytesPerRecord(src._bytesPerRecord)
{
}

//===============================================================================

QMetadata::~QMetadata()
{
}

//===============================================================================
  
void QMetadata::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  setDomAttribute(elem, "_bytesPerRecord", _bytesPerRecord);

  DOMElement * pChildElem = createDomElement(doc, "_qt");
  elem.appendChild(pChildElem);
  _qt.saveToXml(doc, * pChildElem);  
}

//===============================================================================

void QMetadata::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_bytesPerRecord", _bytesPerRecord);

  DOMElement * pChildElem = getDomChildByKeyTag(elem, "_qt");
  _qt.loadFromXml(* pChildElem);
}

//===============================================================================

void QMetadata::debugCheck() const
{
  _qt.debugCheck();
}

//===============================================================================
