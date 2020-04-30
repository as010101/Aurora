/*
 * @(#)TypeFieldRecord.H	1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __TypeFieldRecord_H
#define __TypeFieldRecord_H

#include <db_cxx.h>
#include <string>

using namespace std;

/**
 * TypeFieldRecord extends Dbt in a straightforward way to allow easy
 * stroage/retrieval of rows in the TypeFieldRecord table.
 * A TypeFieldRecord consists of 5 attributes: [typeId(int), attributeIndex(int), attributeName(string), attributeTypeId(int), attributeSize(int)].
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class TypeFieldRecord: public Dbt
{
  // Memeber Variable(s)
  public:
    const static char* databaseFileName;  // the name is defined in TypeFieldRecord.cpp
  protected:
    int m_typeId;
    int m_attributeIndex;
    string m_attributeName; // variable-length
    int m_attributeTypeId;
    int m_attributeSize;
    int m_offset;

  // Method(s)
  public:

    /**
     * Constructs a newly allocated TypeFieldRecord.
     */
    TypeFieldRecord();

    /**
     * Constructs a newly allocated TypeFieldRecord.
     * @param typeId the id the composite type.
     * @param attributeIndex the index of the attribute.
     * @param attributeName the name of the attribute.
     * @param attributeTypeId the type id of the attribute.
     * @param attributeSize the size of the attribute.
     * @param offset the offset of the attribute.
     */
    TypeFieldRecord(int typeId, int attributeIndex, string attributeName, int attributeTypeId, int attributeSize, int offset);

    /**
     * Sets the memeber variables to the specfied values and streams out them to the Dbt buffer.
     * @param typeId the id the composite type.
     * @param attributeIndex the index of the attribute.
     * @param attributeName the name of the attribute.
     * @param attributeTypeId the type id of the attribute.
     * @param attributeSize the size of the attribute.
     * @param offset the offset of the attribute.
     */
    void set(int typeId, int attributeIndex, string attributeName, int attributeTypeId, int attributeSize, int offset);

    /**
     * Reads values from the Dbt buffer and sets the memeber variables to the corresponding values.
     */
    void parse();

    /**
     * Returns the string representation of this TypeFieldRecord.
     * @return the string representation of this TypeFieldRecord.
     */
    string toString();

    /**
     * Returns the type id of this TypeFieldRecord.
     * @return the type id of this TypeFieldRecord.
     */
    int getTypeId();

    /**
     * Returns the attribute index of this TypeFieldRecord.
     * @return the attribute index of this TypeFieldRecord.
     */
    int getAttributeIndex();

    /**
     * Returns the attribute name of this TypeFieldRecord.
     * @return the attribute name of this TypeFieldRecord.
     */
    string getAttributeName();

    /**
     * Returns the attribute type id of this TypeFieldRecord.
     * @return the attribute type id of this TypeFieldRecord.
     */
    int getAttributeTypeId();

    /**
     * Returns the attribute size of this TypeFieldRecord.
     * @return the attribute size of this TypeFieldRecord.
     */
    int getAttributeSize();

    /**
     * Returns the offset of this TypeFieldRecord.
     * @return the offset of this TypeFieldRecord.
     */
    int getOffset();
};

#endif // __TypeFieldRecord_H
