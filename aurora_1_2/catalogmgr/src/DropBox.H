/*
 * @(#)DropBox.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef DROP_BOX_H
#define DROP_BOX_H

#include "Box.H"

/**
 * A DropBox class represents a filter box in the Aurora Network.
 * A DropBox has a predicate which was parsed from the modifier
 * specified by a Aurora programmer.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class DropBox : public Box
{
  // Attribute(s)
  protected:
    float m_dropRate;

  // Method(s)
  public:

    /**
     * Constructs a new DropBox having the specified id, cost, selectivity, and predicate.
     * @param id the id of the Box.
     * @param cost the cost of the Box.
     * @param sel the sel of the Box.
     * @param dropRate the drop rate of this DropBox.
     */
    DropBox(int id, float cost, float sel, float dropRate, int x=0, int y=0, int width=50, int height=30);

    /**
     * Destroys this DropBox disposing its Predicate.
     */
    ~DropBox();

    /**
     * Returns the drop rate of this DropBox.
     * @return the drop rate of this DropBox.
     */
	  float getDropRate();

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
#endif // DROP_BOX_H
