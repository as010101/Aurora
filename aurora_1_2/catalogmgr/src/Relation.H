/*
 * @(#)Relation.H deal with relation environment and other sketchy
 * hacks that make linear road demo.
 *
 */

#ifndef RELATION_H
#define RELATION_H

#include <string>
#include "db_cxx.h"

using namespace std;

/*
 * Enumerations for LRUpdateRelation and LRReadRelation boxes.
 */

#define LR_UPDATE_ACCTS 1
#define LR_UPDATE_DAILY 2
#define LR_UPDATE_STATS 3
#define LR_UPDATE_ACC   4

#define LR_CALC_TOLL_NEW_READ 11
#define LR_SEG_STAT_READ_ACC 12
#define LR_READ_POS_READ_ACCTS 13
#define LR_ACC_ALERT_READ_ACCTS 14

/*
 * Some constants.
 */

// Null values are -1.
#define LR_NULL -1
// Status values.
#define LR_STATUS_NEW 0
#define LR_STATUS_EXIT 1
#define LR_STATUS_SAME 2

/**
 * An instance of Relation stores a single database handle, and
 * supports operations on it.
 *
 * @author Richard Tibbetts (tibbetts@mit.edu)
 */

class Relation
{
private:
  Db *dbp;
  // TODO: Secondary indexes.
  // TODO: More accessor functions.
  // TODO: Cleaner.
public:
  Relation::Relation(DbEnv *dbenv, string name,
		     bt_compare_fcn_type bt_func = NULL);
  Relation::Relation(Db *dbp);
  ~Relation();

  Db *getDb() { return dbp; };
};

#endif // RELATION_H

