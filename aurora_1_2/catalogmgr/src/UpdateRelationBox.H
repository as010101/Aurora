/*
 * @(#)UpdateRelationBox.H
 *
 */

#ifndef UPDATE_RELATION_BOX_H
#define UPDATE_RELATION_BOX_H

#include "Box.H"
#include "Predicate.H"
#include "Relation.H"
#include "RelationEnvironment.H"

/**
 * A UpdateRelationBox reps this box in the Aurora network. The box
 * takes in tuples and uses them to update a relation. It is
 * configured with the name of the relation, the keying information,
 * and probably other stuff that hasn't been added yet.
 *
 * The key is always the first N bytes of the tuples.
 *
 * TODO: add map-type functionality. More key flexibility. Updates
 * that lock a row and do something with it.
 *
 * @author Richard Tibbetts (tibbetts@mit.edu)
 */
class UpdateRelationBox : public Box
{
  // Attribute(s)
  protected:
  Db *db;
  int key_length;

  // Method(s)
  public:

  /**
   * Constructs a new UpdateRelationBox having the specified id and message.
   * @param id the id of the Box.
   * @param modifier configuration parameter to the box.
   *
   * The modifier is just a base 10 string rep of the integer
   * key_length.
   */
  UpdateRelationBox(int id, float cost, float sel, string modifier,
		int x=0, int y=0, int width=50, int height=30);

  /**
   * Destroys this UpdateRelationBox disposing its message.
   */
  ~UpdateRelationBox();

  /**
   * Return the key_length.
   * @return the key_length.
   */
  int UpdateRelationBox::getKeyLength() {return key_length;}
  
  /**
   * Return the Db handle.
   * @return the Db handle.
   */
  Db *UpdateRelationBox::getDb() {return db;}

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
#endif // UPDATE_RELATION_BOX_H
