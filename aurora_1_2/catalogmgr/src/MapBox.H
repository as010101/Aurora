/*
 * @(#)MapBox.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef MAP_BOX_H
#define MAP_BOX_H

#include "Box.H"
#include "Expression.H"
#include <vector>

/**
 * A MapBox class represents a filter box in the Aurora Network.
 * A MapBox has a predicate which was parsed from the modifier
 * specified by a Aurora programmer.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class MapBox : public Box
{
  // Attribute(s)
  protected:
    vector<Expression*> m_expressions;
	string m_modifier;

  // Method(s)
  public:

    /**
     * Constructs a new MapBox having the specified id, cost, selectivity, and predicate.
     * @param id the id of the Box.
     * @param cost the cost of the Box.
     * @param sel the sel of the Box.
     * @param modifer the modifer representing the predicate of the Box.
     */
    MapBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);

    /**
     * Destroys this MapBox disposing its Expression.
     */
    ~MapBox();

    /**
     * Returns the Expression of this MapBox.
     * @return the Expression of this MapBox.
     */
	  vector<Expression*> *getExpression();

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
#endif // MAP_BOX_H
