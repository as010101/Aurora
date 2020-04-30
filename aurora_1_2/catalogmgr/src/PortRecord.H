/*
 * @(#)PortRecord.H	1.1 10/16/2002
 *
 * Copyright 2002 Alexander Rasin (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __PortRecord_H
#define __PortRecord_H

#include <db_cxx.h>
#include <string>
#include "DataInputStream.H"

/**
 * PortRecord extends Dbt in a straightforward way to allow easy
 * storage/retrieval of rows in the Box table.
 * A PortRecord consists of 11 attributes: [arcId(int), rate(float), 
 * typeId(int), sourceNodeId(int), targetNodeId(int),
 * sourcePortIndex(int), targetPortIndex(int), cpFlag(int), parentId(int)].
 *
 * @author Alexander Rasin (alexr@cs.brown.edu)
 * @version 1.1 10/18/2002
 */
class PortRecord: public Dbt
{
   //Member Variable(s)
  public:
   const static char* databaseFileName;
   static int INPUTPORTTYPE;
   static int OUTPUTPORTTYPE;

    protected:
      int m_boxId;
      int m_portIndex;
      int m_typeId;
      int m_portType; // (0 == input, 1 == output)

  // Method(s)
  public:

    /**
     * Constructs a newly allocated PortRecord.
     */
    PortRecord();

    /**
     * Constructs a newly allocated PortRecord with given parameters.
     */
    PortRecord( int boxId, int portIndex, int typeId, int portType );

    /**
     * Sets the actual parameters for the PortRecord.
     */
    void set( int boxId, int portIndex, int typeId, int portType );

    /**
     * Reads values from the Dbt buffer and sets the memeber variables to the corresponding values.
     */
    void parse();

    /**
     * Returns the string representation of this PortRecord.
     * @return the string representation of this PortRecord.
     */
    string toString();

    /**
     * Returns the box id of this PortRecord.
     * @return the box id of this PortRecord.
     */
    int getBoxId();

    /**
     * Returns the port index of this PortRecord.
     * @return the port index of this PortRecord.
     */
    int getPortIndex();

     /**
     * Returns the type id of this PortRecord.
     * @return the type id of this PortRecord.
     */
   int getTypeId();

    /**
     * Returns the port type (input/output) this PortRecord.
     * @returns the port type (input/output) this PortRecord.
     */
   int getPortType();

  protected:

    void readFields(DataInputStream& inputStream);

};

#endif // __PortRecord_H
