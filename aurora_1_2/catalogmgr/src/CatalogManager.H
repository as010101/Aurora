/*
 * @(#)CatalogManager.H	1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu) and Alexander
 * Rasin (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __CatalogManager_H
#define __CatalogManager_H

#include <string>
#include "TypeManager.H"
#include "QueryNetwork.H"
#include "RandomPoint.H"

typedef map<int, Box*, less<int> >          SelectBoxMap;
typedef SelectBoxMap::iterator              SelectBoxMapIter;

/**
 * A CatalogManager has a type manager and an Aurora network
 * specification.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class CatalogManager
{
  // Attributes
  protected:
    TypeManager m_tm, m_tm2;
    string m_directory;

    int arc_id, box_id, /*in_id,*/ out_id;

  // Methods
  public:

    /**
     * Constructs a new CatalogManager object.
     * @param directory the directory where the sleepycat tables are located.
     */
    CatalogManager(const string & directory);

    /**
     * Returns the work directory.
     * @return the work directory.
     */
    string& getDirectory();

    /**
     * Generates a QueryNetwork from Sleepycat tables in the work directory.
     * @return a QueryNetwork generated from Sleepycat tables in the work directory.
     */
    QueryNetwork* load();

    /**
     * Writes a QueryNetwork to Sleepycat tables in the work directory.
     * @return nothing.
     */
    QueryNetwork* write( QueryNetwork *q );

    /**
     * Generates a custom Application (tree form)
     * @return nothing.
     */
    void generate_application( QueryNetwork* q, int ID, float branch, 
			       int depth, SelectBoxMap *sbm, int appl_id,
			       RandomPoint *cost, RandomPoint *select );
    

    /**
     * Generates a custom QueryNetwork.
     * @return a QueryNetwork generated based on parameters
     */
    QueryNetwork* generate( int count, int *depth, float *branches,
			    float *shared_boxes, RandomPoint ***qos,
			    RandomPoint **costs, RandomPoint **selects, int tuple_size );

    /**
     * Walk an application and share it randomly based on probability.
     * @return nada
     */
    void CatalogManager::walkAndShare( Box *ub, float probability, QueryNetwork *q, int level );

    void CatalogManager::addParentAppl( Box *ub, int appl, QueryNetwork *q );
    void CatalogManager::addSuperAppl( Box *ub, int appl, QueryNetwork *q );

    Box* CatalogManager::getBoxAtLevel( int level, Box *ub, QueryNetwork *q );
    double CatalogManager::getQoSBound() { return qosBound; }

    /**
     * Returns a reference to the type manager.
     * @return a reference to the type manager.
     */
    TypeManager& getTypeManager();

  protected:
    string getDirectory(const string & directory);

 private:
    double qosBound;
};
#endif // __CatalogManager_H

