/*
 * @(#)IntegerDbt.H	1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __IntegerDbt_H
#define __IntegerDbt_H

#include <db_cxx.h>
#include <string>

using namespace std;

/**
 * IntegerDbt extends Dbt in a straightforward way to allow easy
 * stroage/retrieval of integers in the Sleepycat DBMS.
 * An IntegerDbt can be used as an integer key or a data item having one integer type attribute.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class IntegerDbt: public Dbt
{
  // Methods
  public:

    /**
     * Constructs a newly allocated IntegerDbt.
     */
    IntegerDbt();

    /**
     * Constructs a newly allocated IntegerDbt object that represents
     * the primitive int argument.
     * @param value the value to be represented by the IntegerDbt.
     */
    IntegerDbt(int value);

    /**
     * Sets the integer value of this IntegerDbt object to the specfied integer.
     * @param value the value to be represented by the IntegerDbt.
     */
    void setInteger(int value);

    /**
     * Returns the value represented by this IntegerDbt.
     * @return the value represented by this IntegerDbt.
     */
    int getInteger();

    /**
     * Returns the string representation of this IntegerDbt.
     * @return the string representation of this IntegerDbt.
     */
    string toString();
};

#endif // __IntegerDbt_H
