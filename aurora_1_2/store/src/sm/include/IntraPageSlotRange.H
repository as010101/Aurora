#ifndef INTRAPAGESLOTRANGE_H
#define INTRAPAGESLOTRANGE_H

#include <iolib/include/PageAddr.H>
#include <exception>

using namespace std;

class DOMElement;
class DOMDocument;

// Desribes a range of slots within some page. _lowSlot should be <= _highSlot.
struct IntraPageSlotRange
{
  PageAddr _pa;
  size_t _lowSlot;
  size_t _highSlot;
  unsigned long _lowSlotSerialNum;

  IntraPageSlotRange(const PageAddr & pa, size_t lowSlot, size_t highSlot, 
		     unsigned long lowSlotSerialNum);
  IntraPageSlotRange();
  IntraPageSlotRange(const IntraPageSlotRange & src);
  virtual ~IntraPageSlotRange();

  IntraPageSlotRange & operator= (const IntraPageSlotRange & rhs);
  bool operator== (const IntraPageSlotRange & rhs) const;
  bool operator!= (const IntraPageSlotRange & rhs) const;

  // Returns the total number of slots (inclusive) in the range 
  // [_lowSlot, _highSlot]. Throws an exception if _lowSlot > _highSlot.
  size_t getNumSlots() const
    throw (exception);

  void loadFromXml(const DOMElement & elem)
    throw (exception);

  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  string getDebugInfo(int indentLevel = 0) const
    throw (std::exception);
};

#endif
