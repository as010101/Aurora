/*
 * @(#)ArcRecord.H	1.1 10/16/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu) and Alexander
 * Rasin (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __ArcRecord_H
#define __ArcRecord_H

#include <db_cxx.h>
#include <string>
#include "DataInputStream.H"

using namespace std;

/**
 * ArcRecord extends Dbt in a straightforward way to allow easy
 * storage/retrieval of rows in the Box table.
 * A ArcRecord consists of 11 attributes: [arcId(int), rate(float), 
 * typeId(int), sourceNodeId(int), targetNodeId(int),
 * sourcePortIndex(int), targetPortIndex(int), cpFlag(int), parentId(int)].
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin
 * (alexr@cs.brown.edu)
 * @version 1.1 10/16/2002
 */
class ArcRecord: public Dbt
{
   //Member Variable(s)
  public:
   const static char* databaseFileName;

   protected:
    int m_id;
    float m_rate;
    int m_typeId;
    int m_sourceNodeId;
    int m_targetNodeId;
    int m_sourcePortIndex;
    int m_targetPortIndex;
    int m_cpFlag;
    int m_parentId;

  // Method(s)
  public:

    /**
     * Constructs a newly allocated ArcRecord.
     */
    ArcRecord();

    /**
     * Constructs a newly allocated ArcRecord with given parameters.
     */
    ArcRecord( int id, int sourceNodeId, int sourcePortIndex,
	       int targetNodeId, int targetPortIndex );

    /**
     * Sets the actual parameters for the ArcRecord.
     */
    void set( int id, int sourceNodeId, int sourcePortIndex,
	      int targetNodeId, int targetPortIndex );

    /**
     * Reads values from the Dbt buffer and sets the memeber variables to the corresponding values.
     */
    void parse();

    /**
     * Returns the string representation of this ArcRecord.
     * @return the string representation of this ArcRecord.
     */
    string toString();

    /**
     * Returns the arc id of this ArcRecord.
     * @return the arc id of this ArcRecord.
     */
    int getId();

    /**
     * Returns the rate of this ArcRecord.
     * @return the rate of this ArcRecord.
     */
    float getRate();

     /**
     * Returns the type id of this ArcRecord.
     * @return the type id of this ArcRecord.
     */
   int getTypeId();

    /**
     * Returns the source node id of this ArcRecord.
     * @return the source node id of this ArcRecord.
     */
    int getSourceNodeId();

    /**
     * Returns the target node id of this ArcRecord.
     * @return the target node id of this ArcRecord.
     */
    int getTargetNodeId();

    /**
     * Returns the port index of the source node.
     * @return the port index of the source node.
     */
    int getSourcePortIndex();

    /**
     * Returns the port index of the target node.
     * @return the port index of the target node.
     */
    int getTargetPortIndex();

     /**
     * Determines if this Arc is a connection point.
     * @return an integer determining if this Arc is a connection point.
     */
     int getCpFlag();

     /**
     * Returns the parent box id of this ArcRecord.
     * @return the parent box id of this ArcRecord.
     */
     int getParentId();

  protected:

    void readFields(DataInputStream& inputStream);

};

#endif // __ArcRecord_H
