#ifndef PAGEADDR_H
#define PAGEADDR_H

#include <string>
#include <exception>

using namespace std;

class DOMDocument;
class DOMElement;

class PageAddr
{
public:
  PageAddr();
  PageAddr(int fileNum, int pageNum);
  PageAddr(const PageAddr & src);

  // Like calling loadFromXml(...)...
  PageAddr(const DOMElement & elem)
    throw (exception);

  virtual ~PageAddr();

  string getDebugInfo(int indentLevel = 0) const
    throw (std::exception);
  
  int _fileNum;
  int _pageNum;

  bool operator<  (const PageAddr & rhs) const;
  bool operator== (const PageAddr & rhs) const;
  bool operator!= (const PageAddr & rhs) const;

  // Given an already-created DOMElement, this QInfo is make that element be the
  // root of a subtree that describes the state of this QInfo object. 'elem'
  // itself may be modified (i.e., elem's attributes may be set, children may
  // be added, etc.). When this method is called, elem is as though freshly
  // created from the owning DOMDocument object (doc).
  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  // Converse operation of saveToXml(...).
  void loadFromXml(const DOMElement & elem)
    throw (exception);
};

#endif
