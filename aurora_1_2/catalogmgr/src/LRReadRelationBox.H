/*
 * @(#)LRReadRelationBox.H
 *
 */

#ifndef LR_READ_RELATION_BOX_H
#define LR_READ_RELATION_BOX_H

#include "Box.H"
#include "Predicate.H"
#include "RelationEnvironment.H"

/**
 * A LRReadRelationBox reps this box in the Aurora network.
 *
 * This box implements sketchy hacks for linear road.
 *
 * @author Richard Tibbetts (tibbetts@mit.edu)
 */
class LRReadRelationBox : public Box
{
  // Attribute(s)
  protected:
  Db *accts_db;
  Db *accts_by_pos_db;
  Db *daily_db;
  Db *stats_db;
  Db *acc_db;
  int magic_number;

  // Method(s)
  public:

  /**
   * Constructs a new LRReadRelationBox having the specified id and message.
   * @param id the id of the Box.
   * @param modifier configuration parameter to the box.
   *
   * The modifier is just a base 10 string rep of the integer
   * magic_number.
   */
  LRReadRelationBox(int id, float cost, float sel, string modifier,
		  int x=0, int y=0, int width=50, int height=30);

  /**
   * Destroys this LRReadRelationBox disposing its message.
   */
  ~LRReadRelationBox();

  /**
   * Return the magic_number.
   * @return the magic_number.
   */
  int LRReadRelationBox::getMagicNumber() {return magic_number;}
  
  /**
   * Return the Accts Db handle.
   * @return the Accts Db handle.
   */
  Db *LRReadRelationBox::getAcctsDb() {return accts_db;}

  /**
   * Return the AcctsByPos Db handle.
   * @return the AcctsByPos Db handle.
   */
  Db *LRReadRelationBox::getAcctsByPosDb() {return accts_by_pos_db;}

  /**
   * Return the Daily Db handle.
   * @return the Daily Db handle.
   */
  Db *LRReadRelationBox::getDailyDb() {return daily_db;}

  /**
   * Return the Stats Db handle.
   * @return the Stats Db handle.
   */
  Db *LRReadRelationBox::getStatsDb() {return stats_db;}

  /**
   * Return the Acc Db handle.
   * @return the Acc Db handle.
   */
  Db *LRReadRelationBox::getAccDb() {return acc_db;}

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
#endif // LR_READ_RELATION_BOX_H
