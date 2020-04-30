#ifndef PAGESLOTADDR_H
#define PAGESLOTADDR_H

#include <iolib/include/PageAddr.H>

// Represents some slot in some page, as well as a progress serial number.
struct PageSlotAddr
{
  PageAddr _pa;
  int _slotNum;
  unsigned long _serialNum;

  PageSlotAddr();

  PageSlotAddr(const PageAddr & pa, int slotNum, unsigned long serialNum /*, bool addrValid*/);

  PageSlotAddr(const PageSlotAddr & src);

  PageSlotAddr(const DOMElement & elem)
    throw (exception);
  
  virtual ~PageSlotAddr();

  PageSlotAddr & operator= (const PageSlotAddr & rhs);

  bool operator== (const PageSlotAddr & rhs) const;

  bool operator!= (const PageSlotAddr & rhs) const;

  void loadFromXml(const DOMElement & elem)
    throw (exception);

  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  string getDebugInfo(int indentLevel = 0) const
    throw (std::exception);
};

#endif
