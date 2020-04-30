#include <iolib/include/PageAddr.H>
#include <sstream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesDomUtil.H>

PageAddr::PageAddr()
  : _fileNum(-1),
    _pageNum(-1)
{
}

//===============================================================================

PageAddr::PageAddr(int fileNum, int pageNum)
  : _fileNum(fileNum),
    _pageNum(pageNum)
{
}

//===============================================================================

PageAddr::PageAddr(const PageAddr & src)
  : _fileNum(src._fileNum),
    _pageNum(src._pageNum)
{
}

//===============================================================================

PageAddr::PageAddr(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_pageNum", _pageNum);
  getDomAttribute(elem, "_fileNum", _fileNum);
}

//===============================================================================

PageAddr::~PageAddr()
{
}

//===============================================================================

string PageAddr::getDebugInfo(int indentLevel) const
  throw (std::exception)
{
  string indentStr(indentLevel, ' ');

  ostringstream os;
  os << indentStr << "PageAddr(this = " << this << ") ( _filenum = "
     << _fileNum << ", _pageNum = " << _pageNum << " )";
  return os.str();
}

//===============================================================================

bool PageAddr::operator < (const PageAddr & rhs) const
{
  return ((_fileNum < rhs._fileNum) || 
	  ((_fileNum == rhs._fileNum) && (_pageNum < rhs._pageNum)));
}

//===============================================================================

bool PageAddr::operator== (const PageAddr & rhs) const
{
  return ((_pageNum == rhs._pageNum) && (_fileNum == rhs._fileNum));
}

//===============================================================================

bool PageAddr::operator!= (const PageAddr & rhs) const
{
  return ((_pageNum != rhs._pageNum) || (_fileNum != rhs._fileNum));
}

//===============================================================================

void PageAddr::saveToXml(DOMDocument & doc, DOMElement & elem) const
  throw (exception)
{
  setDomAttribute(elem, "_pageNum", _pageNum);
  setDomAttribute(elem, "_fileNum", _fileNum);
}

//===============================================================================

void PageAddr::loadFromXml(const DOMElement & elem)
  throw (exception)
{
  getDomAttribute(elem, "_pageNum", _pageNum);
  getDomAttribute(elem, "_fileNum", _fileNum);
}

