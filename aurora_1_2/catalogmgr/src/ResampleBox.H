/*
 * ResampleBox.H : Author, Eddie Galvez (eddie@cs.brandeis.edu)
 *
 */

#ifndef RESAMPLE_BOX_H
#define RESAMPLE_BOX_H

#include "Box.H"
#include <strstream.h>
#include "Parse.H"
#include "BoxRecord.H"
#include <vector>
#include "BufferList.H"
#include "HashForNewState.H"

class ResampleBox : public Box {

protected:
  string m_modifier;
  BufferList *_left_buffer;
  BufferList *_right_buffer;
  HashForNewState *_hash;

  // Method(s)
public:
  ResampleBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);
  ~ResampleBox();
  const char* getModifier();
  BufferList* getLeftBufferList();
  BufferList* getRightBufferList();
  HashForNewState* getHash();
  int getBoxType();
  virtual string toString();
  
};

#endif
