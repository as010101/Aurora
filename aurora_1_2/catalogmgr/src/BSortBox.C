#include "BSortBox.H"
#include <strstream.h>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"
#include <vector>

// The catalog creates this box to represent every box in memory during the lifetime
// of a network. Any type of state must be stored here.
BSortBox::BSortBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height) {
  // We take from the modifer what we want, and voila it gets saved here
  // into its "state" which is how the schedulers BsortQBox can
  // get all that stuff back!
  m_modifier = modifier;
  
  // Stuff BSort wants as "state"
  _buffer_hash = new HashForBufferList();

}

BSortBox::~BSortBox() {}

HashForBufferList* BSortBox::getBufferHash() {
  return _buffer_hash;
}

int BSortBox::getBoxType() {
  return BSORT;
}

const char* BSortBox::getModifier() {
  return m_modifier.c_str();
}

string BSortBox::toString() {
  strstream s;
  s << "BSortBox (" << Box::toString() << ", [thats all!] )" << '\0'; // huh? I dont think u need the null
  return s.str();
}

