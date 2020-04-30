/*
 * @(#)Stream.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef Stream_H
#define Stream_H

#include <string>
#include "CompositeType.H"

/**
 * A Stream object represents a stream in the Aurora Network.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class Stream
{
  // Attribute(s)
  protected:
    int m_id;
    float m_rate;
    CompositeType* m_schema;

  // Method(s)
  public:

    /**
     * Constructs a new Stream having the specified id, rate, and schema.
     * @param id the id for the Box.
     * @param rate the rate for the Box.
     * @param schema the schema for the Box.
     */
    Stream(int id, float rate, CompositeType* schema);

    /**
     * Destroys this Stream disposing its Predicate.
     */
    virtual ~Stream();

    /**
     * Returns the id of this Stream.
     * @return the id of this Stream.
     */
	  int getId();

    /**
     * Returns the rate of this Stream.
     * @return the rate of this Stream.
     */
	  float getRate();

    /**
     * Sets the rate of this Stream.
     * @param rate the rate for this Stream.
     */
	  void setRate(float rate);

    /**
     * Returns the schema of this Stream.
     * @return the schema of this Stream.
     */
	  CompositeType* getSchema();

    /**
     * Returns the string representation of this Box.
     * @return the string representation of this Box.
     */
    virtual string toString();
};
#endif // Stream_H
