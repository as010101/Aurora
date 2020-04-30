#include <sm/include/PageSlotAddr.H>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <sstream>
#include <xercesDomUtil.H>
#include <XmlTempString.H>
#include <StorageMgr_Exceptions.H>


//===============================================================================

PageSlotAddr::PageSlotAddr()
  : _slotNum(-1),
    _serialNum(0)
{
}

//===============================================================================

PageSlotAddr::PageSlotAddr(const PageAddr & pa, int slotNum, 
			   unsigned long serialNum)
  : _pa(pa),
    _slotNum(slotNum),
    _serialNum(serialNum)
{
}

//===============================================================================

PageSlotAddr::PageSlotAddr(const PageSlotAddr & src)
  : _pa(src._pa),
    _slotNum(src._slotNum),
    _serialNum(src._serialNum)
{
}

//===============================================================================

PageSlotAddr::PageSlotAddr(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_slotNum",   _slotNum);
  getDomAttribute(elem, "_serialNum", _serialNum);

  DOMElement & paElem = *getDomChildByKeyTag(elem, "PageAddr");
  _pa.loadFromXml(paElem);
}

//===============================================================================

PageSlotAddr::~PageSlotAddr()
{
}

//===============================================================================

PageSlotAddr & PageSlotAddr::operator= (const PageSlotAddr & rhs)
{
  _pa        = rhs._pa;
  _slotNum   = rhs._slotNum;
  _serialNum = rhs._serialNum;

  return *this;
}

//===============================================================================

bool PageSlotAddr::operator== (const PageSlotAddr & rhs) const
{
  return 
    (_pa == rhs._pa) &&
    (_slotNum == rhs._slotNum) &&
    (_serialNum == rhs._serialNum);
}

//===============================================================================

bool PageSlotAddr::operator != (const PageSlotAddr & rhs) const
{
  return ! ((*this) == rhs);
}

//===============================================================================

void PageSlotAddr::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  setDomAttribute(elem, "_slotNum",   _slotNum);
  setDomAttribute(elem, "_serialNum", _serialNum);

  DOMElement * pPageAddrElem = createDomElement(doc, "PageAddr");
  elem.appendChild(pPageAddrElem);
  _pa.saveToXml(doc, *pPageAddrElem);
}

//===============================================================================

void PageSlotAddr::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_slotNum",   _slotNum);
  getDomAttribute(elem, "_serialNum", _serialNum);

  DOMElement & paElem = *getDomChildByKeyTag(elem, "PageAddr");
  _pa.loadFromXml(paElem);
}

//===============================================================================

string PageSlotAddr::getDebugInfo(int indentLevel) const
  throw (std::exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "PageSlotAddr(this = " << this << ")" << endl
     << _pa.getDebugInfo(indentLevel + 3) << endl
     << indentStr << "   _slotNum = " << _slotNum << endl
     << indentStr << "   _serialNum = " << _serialNum << endl;
  return os.str();
}

//===============================================================================
