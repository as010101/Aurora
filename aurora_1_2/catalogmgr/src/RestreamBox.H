/*
 * @(#)MapBox.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef RESTREAM_BOX_H
#define RESTREAM_BOX_H

#include "Box.H"
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"


class RestreamBox : public Box
{
  // Attribute(s)
 protected:
  string m_modifier;

  AggregateState *m_state;
  
  // Method(s)
  public:

    /**
     * Constructs a new RestreamBox having the specified id, cost, selectivity, and predicate.
     * @param id the id of the Box.
     * @param cost the cost of the Box.
     * @param sel the sel of the Box.
     * @param modifer the modifer representing the predicate of the Box.
     */
    RestreamBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);

    /**
     * Destroys this MapBox disposing its Expression.
     */
    ~RestreamBox();

    const char* getModifier();

    /**
     * Returns the type of this Box.
     * @return the type of this Box.
     */
    int getBoxType();

    /**
     * Returns the string representation of this Box.
     * @return the string representation of this Box.
     */
    virtual string toString();

    AggregateState* getState();


};
#endif // RESTREAM_BOX_H
