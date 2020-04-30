/*
 * @(#)HelloWorldBox.H
 *
 */

#ifndef HELLO_WORLD_BOX_H
#define HELLO_WORLD_BOX_H

#include "Box.H"
#include "Predicate.H"

/**
 * A HelloWorldBox reps this box in the Aurora network. takes in tuples, prints a message, outputs
 * the same tuples.
 *
 * @author Richard Tibbetts (tibbetts@mit.edu)
 */
class HelloWorldBox : public Box
{
  // Attribute(s)
  protected:
  string message;

  // Method(s)
  public:

  /**
   * Constructs a new HelloWorldBox having the specified id and message.
   * @param id the id of the Box.
   * @param message the message to print out.
   */
  HelloWorldBox(int id, float cost, float sel, string message,
		int x=0, int y=0, int width=50, int height=30);

  /**
   * Destroys this HelloWorldBox disposing its message.
   */
  ~HelloWorldBox();

  /**
   *
   * Return the message
   */
  const char *HelloWorldBox::getModifier();
  

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
#endif // HELLO_WORLD_BOX_H
