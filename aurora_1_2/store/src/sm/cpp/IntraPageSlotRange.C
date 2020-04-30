#include <sm/include/IntraPageSlotRange.H>
#include <StorageMgr_Exceptions.H>
#include <sstream>

#include <xercesDomUtil.H>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

PageAddr _pa;
size_t _lowSlot;
size_t _highSlot;

IntraPageSlotRange::IntraPageSlotRange(const PageAddr & pa, size_t lowSlot, 
				       size_t highSlot, 
				       unsigned long lowSlotSerialNum)
  : _pa(pa),
    _lowSlot(lowSlot),
    _highSlot(highSlot),
    _lowSlotSerialNum(lowSlotSerialNum)
{
}

//===============================================================================

IntraPageSlotRange::IntraPageSlotRange()
{
}

//===============================================================================
IntraPageSlotRange::IntraPageSlotRange(const IntraPageSlotRange & src)
  : _pa(src._pa),
    _lowSlot(src._lowSlot),
    _highSlot(src._highSlot),
    _lowSlotSerialNum(src._lowSlotSerialNum)
{
}

//===============================================================================
IntraPageSlotRange::~IntraPageSlotRange()
{
}

//===============================================================================

IntraPageSlotRange & IntraPageSlotRange::operator= (const IntraPageSlotRange & rhs)
{
  _pa = rhs._pa;
  _lowSlot = rhs._lowSlot;
  _highSlot = rhs._highSlot;
  _lowSlotSerialNum = rhs._lowSlotSerialNum;

  return *this;
}

//===============================================================================

bool IntraPageSlotRange::operator== (const IntraPageSlotRange & rhs) const
{
  return 
    (_pa == rhs._pa) &&
    (_lowSlot == rhs._lowSlot) &&
    (_highSlot == rhs._highSlot) &&
    (_lowSlotSerialNum == rhs._lowSlotSerialNum);
}

//===============================================================================

bool IntraPageSlotRange::operator != (const IntraPageSlotRange & rhs) const
{
  return ! ((*this) == rhs);
}

//===============================================================================

size_t IntraPageSlotRange::getNumSlots() const
  throw (exception)
{
  if (_highSlot < _lowSlot)
    {
      throw SmException(__FILE__, __LINE__, "_highSlot < _lowSlot");
    }

  return _highSlot - _lowSlot + 1;
}

//===============================================================================

string IntraPageSlotRange::getDebugInfo(int indentLevel) const
  throw (std::exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "IntraPageSlotRange(this = " << this << ")" << endl
     << indentStr << "   " << _pa.getDebugInfo(indentLevel) << endl
     << indentStr << "   _lowSlot  = " << _lowSlot << endl
     << indentStr << "   _highSlot = " << _highSlot << endl
     << indentStr << "   _lowSlotSerialNum = " << _lowSlotSerialNum;

  return os.str();
}

//===============================================================================

void IntraPageSlotRange::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  setDomAttribute(elem, "_lowSlot",          _lowSlot);
  setDomAttribute(elem, "_highSlot",         _highSlot);
  setDomAttribute(elem, "_lowSlotSerialNum", _lowSlotSerialNum);

  DOMElement * pPageAddrElem = createDomElement(doc, "PageAddr");
  elem.appendChild(pPageAddrElem);
  _pa.saveToXml(doc, *pPageAddrElem);
}

//===============================================================================

void IntraPageSlotRange::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_lowSlot",          _lowSlot);
  getDomAttribute(elem, "_highSlot",         _highSlot);
  getDomAttribute(elem, "_lowSlotSerialNum", _lowSlotSerialNum);

  DOMElement & paElem = *getDomChildByKeyTag(elem, "PageAddr");
  _pa.loadFromXml(paElem);
}

//===============================================================================
