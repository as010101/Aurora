#include "ResampleBox.H"
#include <strstream.h>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"
#include <vector>

// The catalog creates this box to represent every box in memory during the lifetime
// of a network. Any type of state must be stored here.
ResampleBox::ResampleBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height) {
  // We take from the modifer what we want, and voila it gets saved here
  // into its "state" which is how the schedulers ResampleQBox can
  // get all that stuff back!
  m_modifier = modifier;
  
  // Stuff Resample wants as "state"
  _left_buffer = new BufferList('i'); // pass the type of the order-on attributes
  _right_buffer = new BufferList('i');
  _hash = new HashForNewState('i');

}

ResampleBox::~ResampleBox() {}


BufferList* ResampleBox::getLeftBufferList() {
  return _left_buffer;
}

BufferList* ResampleBox::getRightBufferList() {
  return _right_buffer;
}

HashForNewState* ResampleBox::getHash() {
  return _hash;
}


int ResampleBox::getBoxType() {
  return RESAMPLE;
}

const char* ResampleBox::getModifier() {
  return m_modifier.c_str();
}

string ResampleBox::toString() {
  strstream s;
  s << "ResampleBox (" << Box::toString() << ", [thats all!] )" << '\0'; // huh? I dont think u need the null
  return s.str();
}

