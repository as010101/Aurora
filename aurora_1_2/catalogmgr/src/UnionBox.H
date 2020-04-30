/*
 * @(#)UnionBox.H    1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef UNION_BOX_H
#define UNION_BOX_H

#include "Box.H"
#include "Arc.H"
#include <vector>

/**
 * A UnionBox class represents a union box in the Aurora Network.
 * A UnionBox has a predicate which was parsed from the modifier
 * specified by a Aurora programmer.
 *
 * @author  Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class UnionBox : public Box
{
  // Attribute(s)
  protected:
    vector<Arc*> m_inputArcs;

  // Method(s)
  public:

    /**
     * Constructs a new UnionBox having the specified id, cost, selectivity, and predicate.
     * @param id the id of the Box.
     * @param cost the cost of the Box.
     * @param sel the sel of the Box.
     */
    UnionBox(int id, float cost, float sel, int x=0, int y=0, int width=50, int height=30);

    /**
     * Destroys this UnionBox disposing its Expression.
     */
    ~UnionBox();

    /**
     * Returns the vector of input arcs of this UnionBox.
     * @return the vector of input arcs of this UnionBox.
     */
	  vector<Arc*>& getInputArcs();

    /**
     * Adds one arc to the vector of input arcs of this UnionBox object.
     * @param arc the arc to add.
     */
	  void addInputArc(Arc* arc);

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
};
#endif // UNION_BOX_H
