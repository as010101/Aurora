/*
 * @(#)CompositeTypeRecord.H	1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu) and Alexander
 * Rasin (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __CompositeTypeRecord_H
#define __CompositeTypeRecord_H

#include <db_cxx.h>
#include <string>

using namespace std;

/**
 * CompositeTypeRecord extends Dbt in a straightforward way to allow easy
 * stroage/retrieval of rows in the CompositeType table.
 * A CompositeTypeRecord consists of 3 attributes: [typeId(int), typeName(string), numberOfFields(int)].
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class CompositeTypeRecord: public Dbt
{
  // Memeber Variable(s)
  public:
    const static char* databaseFileName;  // the name is defined in CompsiteTypeRecord.cpp
  protected:
    int m_typeId;
    string m_name; // variable-length
    int m_numberOfFields;

  // Method(s)
  public:

    /**
     * Constructs a newly allocated CompositeTypeRecord.
     */
    CompositeTypeRecord();

    /**
     * Constructs a newly allocated CompositeTypeRecord.
     * @param typeId the type id.
     * @param name the name.
     * @param numberOfFields the number of attributes.
     */
    CompositeTypeRecord(int typeId, string name, int numberOfFields);

    /**
     * Sets the memeber variables to the specfied values and streams out them to the Dbt buffer.
     * @param typeId the type id.
     * @param name the name.
     * @param numberOfFields the number of attributes.
     */
    void set(int typeId, string name, int numberOfFields);

    /**
     * Reads values from the Dbt buffer and sets the memeber variables to the corresponding values.
     */
    void parse();

    /**
     * Returns the string representation of this CompositeTypeRecord.
     * @return the string representation of this CompositeTypeRecord.
     */
    string toString();

    /**
     * Returns the type id of this CompositeTypeRecord.
     * @return the type id of this CompositeTypeRecord.
     */
    int getTypeId();

    /**
     * Returns the name of this CompositeTypeRecord.
     * @return the name of this CompositeTypeRecord.
     */
    string getName();

    /**
     * Returns the number of fields in this CompositeTypeRecord.
     * @return the number of fields in this CompositeTypeRecord.
     */
    int getNumberOfFields();
};

#endif // __CompositeTypeRecord_H
