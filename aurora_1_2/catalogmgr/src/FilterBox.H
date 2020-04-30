/*
 * @(#)FilterBox.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef FILTER_BOX_H
#define FILTER_BOX_H

#include "Box.H"
#include "Predicate.H"

/**
 * A FilterBox class represents a filter box in the Aurora Network.
 * A FilterBox has predicates parsed from the modifier
 * specified by a Aurora programmer.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * Now Anurag and Eddie (anurag|eddie@cs.brandeis.edu)
 */
class FilterBox : public Box
{
  // Attribute(s)
  protected:
  //list<Predicate*> *_predicates;
  //bool _useFalsePort;
  string m_modifier;

  // Method(s)
  public:

    /**
     * Constructs a new FilterBox having the specified id, cost, selectivity, and predicate.
     * @param id the id of the Box.
     * @param cost the cost of the Box.
     * @param sel the sel of the Box.
     * @param modifer the modifer representing the predicate of the Box.
     */
    FilterBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);

    /**
     * Destroys this FilterBox disposing its Predicate.
     */
    ~FilterBox();

    /**
     * Returns the Predicates of this FilterBox as list of Predicate objects
     * Why list? we access them in sequential fashion, not random anyways
     */
  //list<Predicate*> *getPredicates();
  //Predicate*[] getPredicates();

  /**
   *
   * Return the modifier
   */
  const char *FilterBox::getModifier();
  

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
#endif // FILTER_BOX_H
