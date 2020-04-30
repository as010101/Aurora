#include <JoinBox.H>
#include <strstream.h>
#include <Parse.H>
#include <FunPred.H>
#include <BoxRecord.H>
#include <vector>

// The catalog creates this box to represent every box in memory during the lifetime
// of a network. Any type of state must be stored here.
JoinBox::JoinBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height) {
  // We take from the modifer what we want, and voila it gets saved here
  // into its "state" which is how the schedulers JoinQBox can
  // get all that stuff back!
  m_modifier = modifier;
  
  // Stuff Join wants as "state"
  _left_buffer = new BufferList('i'); // pass the type of the order-on attributes
  _right_buffer = new BufferList('i');

}

JoinBox::~JoinBox() {}


BufferList* JoinBox::getLeftBufferList() {
  return _left_buffer;
}

BufferList* JoinBox::getRightBufferList() {
  return _right_buffer;
}

int JoinBox::getBoxType() {
  return JOIN;
}

const char* JoinBox::getModifier() {
  return m_modifier.c_str();
}

string JoinBox::toString() {
  strstream s;
  s << "JoinBox (" << Box::toString() << ", [thats all!] )" << '\0'; // huh? I dont think u need the null
  return s.str();
}

