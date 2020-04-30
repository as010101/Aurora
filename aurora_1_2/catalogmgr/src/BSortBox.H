/*
 * BSortBox.H : Author, Eddie Galvez (eddie@cs.brandeis.edu)
 *
 */

#ifndef BSORT_BOX_H
#define BSORT_BOX_H

#include "Box.H"
#include <strstream.h>
#include "Parse.H"
#include "BoxRecord.H"
#include <vector>
#include "HashForBufferList.H"
class BSortBox : public Box {

protected:
  string m_modifier;
  HashForBufferList *_buffer_hash;

  // Method(s)
public:
  BSortBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);
  ~BSortBox();
  const char* getModifier();
  HashForBufferList* getBufferHash();
  int getBoxType();
  virtual string toString();
  
};

#endif
