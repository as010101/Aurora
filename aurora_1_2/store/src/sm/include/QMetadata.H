#ifndef QMETADATA_H
#define QMETADATA_H

#include <sm/include/QTopology.H>

// All runtime data associated with a qbase/qview set, some of which is 
// persistent. The qbaseid is implicit, as it's normally stored as the
// key to a map, and this QMetadata is the dependent data.
class QMetadata
{
public:
  // These parameter values can be bogus if you're going to call 
  // 'loadFromXml(...)' before using the object for real work. 
  // *** NOTE *** Don't let bytesPerRecord == 0, or else you'll get
  // a runtime divide-by-0 error.
  //
  // NOTE: If you're going to call 'loadFromXml' on this object, it doesn't
  // matter what value you suppled for 'cxnPoint', because it will be overridden
  // when loading the object from XML anyway.
  QMetadata(size_t bytesPerPage,
	    size_t bytesPerRecord,
	    bool cxnPoint)
    throw (exception);

  QMetadata(const QMetadata & src)
    throw (exception);

  virtual ~QMetadata();
  
  void saveToXml(DOMDocument & doc, DOMElement & elem) const
    throw (exception);

  void loadFromXml(const DOMElement & elem)
    throw (exception);

  QTopology _qt;

  size_t _bytesPerRecord;

  // Verifies internal integrity of this object.
  void debugCheck() const;
};

#endif
