/*
 * AggregateBox.H : Author, Eddie Galvez (eddie@cs.brandeis.edu)
 *
 */

#ifndef AGGREGATE_BOX_H
#define AGGREGATE_BOX_H

#include "Box.H"
#include <strstream.h>
#include "Parse.H"
#include "BoxRecord.H"
#include <vector>
#include "HashForAggregate.H"

class AggregateBox : public Box {
protected:
  string m_modifier;
  HashForAggregate* _hash_of_everything;
  vector<char*>  *_tuple_store;
  unsigned int* _tuple_counter; // used when ordering-on tuple #
  
public:
  AggregateBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);
  ~AggregateBox();
  const char* getModifier();
  HashForAggregate* getHash();
  vector<char*>* getTupleStore();
  int getBoxType();
  virtual string toString();
  unsigned int* AggregateBox::getTupleCounter();

};

#endif

