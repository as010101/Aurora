/*
 * @(#)typemgr.H 1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu) and Alexander
 * Rasin (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __TypeManager_H
#define __TypeManager_H

#include "CompositeType.H"

typedef vector<CompositeType*> CompositeTypeVector;

/**
 * A TypeManager object has a number of pre-defined primitive types and enables
 * creation, update and deletion of composite types.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class TypeManager
{
  // predefined static member variables
  public:
    static const int UNDEFINED_TYPE; // = 0;
    static const int INTEGER; // = -1;
    static const int FLOAT; // = -2;
    static const int DOUBLE ; //= -3;
    static const int STRING; // = -4;
    static const int BOOL; // = -5;
    static const int TIMESTAMP; // = -6;

    // note: all primitive types have values < 0,
    // all composite types have values > 0.

    static const char INTEGER_NAME[]; // = "integer";
    static const char FLOAT_NAME[]; // = "float";
    static const char DOUBLE_NAME[]; // = "double";
    static const char STRING_NAME[]; // = "string";
    static const char BOOL_NAME[]; // = "bool";
    static const char TIMESTAMP_NAME[]; // = "timestamp";

    static const int INTEGER_SIZE; // = 4;
    static const int FLOAT_SIZE; // = 4;
    static const int DOUBLE_SIZE; // = 8;
    static const int STRING_SIZE; // = -1;
    static const int BOOL_SIZE; // = 1;
    static const int TIMESTAMP_SIZE; // = 8;

  // Member variable(s)
  protected:
    CompositeTypeVector m_compositeTypes;

  // Methods
  public:
    /**
     * Constructs a new TypeManager object.
     */
    TypeManager();

    /**
     * Adds a CompositeType to this TypeManager.
     * @param compositeType the type to add.
     * @exception TypeException if the given name is already being used.
     */
    void addCompositeType(CompositeType* compositeType);

    /**
     * Returns the composite type at the specified index.
     * @param index index.
     */
    CompositeType* getCompositeType(int index);

    /**
     * Loads from tables in the specified directory.
     * @param directory the directory where the sleepycat tables are located.
     */
    void load(string& directory);

    /**
     * Generates types for the network
     * @param type size
     */
    void generate( int type_size );

    /**
     * Saves into tables in the specified directory
     * @param directory the directory where the sleepycat tables are located.
     */
    void save(string& directory);

    /**
     * Returns a String object representing this TypeManager.
     * @return a string representation of this TypeManager.
     */
    string toString();
};
#endif // __TypeManager_H

