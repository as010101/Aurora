/*
 * @(#)IntegerArrayDbt.H	1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __IntegerArrayDbt_H
#define __IntegerArrayDbt_H

#include <db_cxx.h>
#include <string>

using namespace std;

/**
 * IntegerArrayDbt extends Dbt in a straightforward way to allow easy
 * stroage/retrieval of integer arrays in the Sleepycat DBMS.
 * An IntegerArrayDbt can be used as an integer key or a data item having an integer array.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class IntegerArrayDbt: public Dbt
{
  protected:
    int m_size;
    int* m_integers;

  // Methods
  public:

    /**
     * Constructs a newly allocated IntegerArrayDbt.
     */
    IntegerArrayDbt(int size);

    /**
     * Constructs a newly allocated IntegerArrayDbt object that represents
     * the primitive int argument.
     * @param values the values to be represented by the IntegerArrayDbt.
     */
    IntegerArrayDbt(int* values, int size);

    /**
     * Destructs this IntegerArrayDbt.
     */
    ~IntegerArrayDbt();

    /**
     * Sets the integer value of this IntegerArrayDbt object to the specfied integer.
     * @param values the values to be represented by the IntegerArrayDbt.
     */
    void setInteger(int* values, int size);

    /**
     * Returns the values represented by this IntegerArrayDbt.
     * @return the values represented by this IntegerArrayDbt.
     */
    int* getIntegers();

    /**
     * Returns the number of integers in this IntegerArrayDbt.
     * @return the number of integers in this IntegerArrayDbt.
     */
    int getSize();

    /**
     * Returns the string representation of this IntegerArrayDbt.
     * @return the string representation of this IntegerArrayDbt.
     */
    string toString();
};

#endif // __IntegerArrayDbt_H
