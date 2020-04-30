/*
 * JoinBox.H : Author, Eddie Galvez (eddie@cs.brandeis.edu)
 *
 */

#ifndef JOIN_BOX_H
#define JOIN_BOX_H

#include <Box.H>
#include <strstream.h>
#include <Parse.H>
#include <BoxRecord.H>
#include <vector>
#include <BufferList.H>
class JoinBox : public Box {

protected:
  string m_modifier;
  BufferList *_left_buffer;
  BufferList *_right_buffer;

  // Method(s)
public:
  JoinBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);
  ~JoinBox();
  const char* getModifier();
  BufferList* getLeftBufferList();
  BufferList* getRightBufferList();
  int getBoxType();
  virtual string toString();
  
};

#endif
