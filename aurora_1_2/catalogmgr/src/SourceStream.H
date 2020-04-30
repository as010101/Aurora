/*
 * @(#)SourceStream.H    1.0 10/16/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang 
 * (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu). 
 * All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef SourceStream_H
#define SourceStream_H

#include <string>
#include "CompositeType.H"
#include "Stream.H"

/**
 * A SourceStream object represents a SourceStream in the Aurora Network.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class SourceStream: public Stream
{
  // Attribute(s)
  protected:
    bool m_periodicity;
    bool m_adjustability;
    int m_numberOfTuples;
    char *modifier;

  // Method(s)
  public:

    /**
     * Constructs a new SourceStream having the specified id, rate, schema, periodicity, adjustability, and number of tuples.
     * @param id the id for the Box.
     * @param rate the rate for the Box.
     * @param schema the schema for the Box.
     * @param periodicity the periodicity for the Box.
     * @param adjustability the adjustability for the Box.
     * @param numberOfTuples the numberOfTuples for the Box.
     */
    SourceStream(int id, float rate, CompositeType* schema, bool periodicity, bool adjustability, int numberOfTuples);

    /**
     * Destroys this SourceStream.
     */
    ~SourceStream();

    /**
     * Returns the modifier string for this stream. (for writing into DB)
     * @return a modifier string with stream paramenters.
     */
    char* getModifier();

    /**
     * Determines if this SourceStream is periodic.
     * @return a boolean determining if this SourceStream is periodic.
     */
      bool isPeriodic();

    /**
     * Determines if this SourceStream is adjustable.
     * @return a boolean determining if this SourceStream is adjustable.
     */
	  bool isAdjustable();

    /**
     * Returns the number tuples in this SourceStream.
     * @return the number tuples in this SourceStream.
     */
	  int getNumberOfTuples();

    /**
     * Sets the number tuples in this SourceStream.
     * @param numberOfTuples the number tuples for this SourceStream.
     */
	  void setNumberOfTuples(int numberOfTuples);

    /**
     * Returns the string representation of this SourceStream.
     * @return the string representation of this SourceStream.
     */
    virtual string toString();
};
#endif // SourceStream_H
