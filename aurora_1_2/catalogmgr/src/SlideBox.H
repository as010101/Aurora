/*
 * @(#)SlideBox.H    1.0 07/15/2002
 *
 */

#ifndef SLIDE_BOX_H
#define SLIDE_BOX_H

#include "Box.H"
#include <strstream.h>
#include "Parse.H"
#include "BoxRecord.H"
#include <vector>

/**
 * A SlideBox class represents a Slide  box in the Aurora Network.
 * A SlideBox has a predicate which was parsed from the modifier
 * specified by a Aurora programmer.
 *
 * @author Daniel Abadi (dna@cs.brown.edu) and Anurag Maskey (anurag@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class SlideBox : public Box
{
  // Attribute(s)
  protected:
    AggregateState* m_state;
    string m_modifier;

  // Method(s)
  public:

    /**
     * Constructs a new SlideBox having the specified id, cost, selectivity, and aggregate state.
     * @param id the id of the Box.
     * @param cost the cost of the Box.
     * @param sel the sel of the Box.
     * @param modifer the modifer representing the predicate of the Box.
     */
    SlideBox(int id, float cost, float sel, string modifier, int x=0, int y=0, int width=50, int height=30);

    /**
     * Destroys this SlideBox disposing its Aggregate State.
     */
    ~SlideBox();

    /**
     * Returns the aggregate state of this SlideBox.
     * @return the aggregate state of this SlideBox.
     */
    AggregateState *getState();

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
#endif // SLIDE_BOX_H
