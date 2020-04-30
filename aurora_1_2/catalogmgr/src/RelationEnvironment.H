/*
 * @(#)RelationEnvironment.H class to hold context for all the
 * relations.
 */

#ifndef RELATION_ENVIRONMENT_H
#define RELATION_ENVIRONMENT_H

#include <db_cxx.h>
#include <map>
#include "Relation.H"

/**
 * Singleton class for storing all the relations. Points to the actual
 * location (directory) on disk, and contains the Berkeley DB env.
 * Supports looking up a relation by name.
 */
class RelationEnvironment
{
private:
  DbEnv *dbenv;
  map<string,Relation*> relations;
public:
  /**
   * Create a relation environment, open the DB environment.
   */
  RelationEnvironment(string directory);

  /**
   * Destroy the relation environment. Shouldn't be done until all
   * Relations are destroyed.
   */
  ~RelationEnvironment();

  /**
   * Get the relation with the specified name.
   * If create is true, then create the relation if it does not exist.
   */
  Relation *createRelation(string name, bool create,
			   bt_compare_fcn_type bt_func = NULL);

  /**
   * Destroy a relation.
   */
  void destroyRelation(string name);

  /**
   * Do nasty things to set up Linear Road.
   */
  void setupLinearRoadEnvironment();

};

extern RelationEnvironment *relation_environment;

#endif //RELATION_ENVIRONMENT_H
