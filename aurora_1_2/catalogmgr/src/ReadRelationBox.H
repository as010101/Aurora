/*
 * @(#)ReadRelationBox.H
 *
 */

#ifndef READ_RELATION_BOX_H
#define READ_RELATION_BOX_H

#include "Box.H"
#include "Predicate.H"
#include "Relation.H"
#include "RelationEnvironment.H"

/**
 * A ReadRelationBox reps this box in the Aurora network. The box
 * takes in tuples and uses them to read a relation. It is
 * configured with the name of the relation, the keying information,
 * 
 *
 * The key is always the first N bytes of the tuples.
 *
 * TODO: add map-type functionality. More key flexibility. Reads
 * that lock a row and do something with it.
 *
 * @author Richard Tibbetts (tibbetts@mit.edu)
 */
class ReadRelationBox : public Box
{
  // Attribute(s)
  protected:
  Db *db;
  int key_length;

  // Method(s)
  public:

  /**
   * Constructs a new ReadRelationBox having the specified id and message.
   * @param id the id of the Box.
   * @param modifier configuration parameter to the box.
   *
   * The modifier is just a base 10 string rep of the integer
   * key_length.
   */
  ReadRelationBox(int id, float cost, float sel, string modifier,
		  int x=0, int y=0, int width=50, int height=30);

  /**
   * Destroys this ReadRelationBox disposing its message.
   */
  ~ReadRelationBox();

  /**
   * Return the key_length.
   * @return the key_length.
   */
  int ReadRelationBox::getKeyLength() {return key_length;}
  
  /**
   * Return the Db handle.
   * @return the Db handle.
   */
  Db *ReadRelationBox::getDb() {return db;}

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
#endif // READ_RELATION_BOX_H
