/*
 * @(#)Arc.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Arc# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef Arc_H
#define Arc_H

#include <string>
#include "CompositeType.H"
#include "Stream.H"
#include <pthread.h>

/**
 * An Arc object represents an arc in the Aurora Network.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class Arc: public Stream
{
  // Attribute(s)
  protected:
    int m_sourceId;
    int m_sourcePortId;
    int m_destinationId;
    int m_destinationPortId;
    bool m_connectionPoint;
    bool m_inputArc, m_outputArc;
    pthread_mutex_t* m_mutex;

  // Method(s)
  public:

    /**
     * Constructs a new Arc having the specified id, rate, schema, periodicity, adjustability, and number of tuples.
     * @param id the id for the Arc.
     * @param rate the rate for the Arc.
     * @param schema the schema for the Arc.
     * @param sourceId the source node Id for the Arc.
     * @param sourcePortId the port index of the destination node.
     * @param destinationId the destination node id for the Arc.
     * @param destinationPortId the port index of the destination node.
     * @param cfFlag the connection point flag.
     */
    Arc(int id, float rate, CompositeType* schema, int sourceId, int sourcePortId, int destinationId, int destinationPortId, bool cfFlag);

    /**
     * Destroys this Arc.
     */
    ~Arc();

    /**
     * Returns the source node id of this Arc.
     * @return the source node id of this Arc.
     */
	  int getSourceId();

    /**
     * Returns the port index of the source node of this Arc.
     * @return the port index of the source node id of this Arc.
     */
	  int getSourcePortId();

    /**
     * Sets the source node id of this Arc.
     * @param sourceId the source node id.
     */
	  void setSourceId(int sourceId);

    /**
     * Sets the port index of the source node of this Arc.
     * @param the port index of the source node.
     */
	  void setSourcePortId(int portId);

    /**
     * Returns the target node id of this Arc.
     * @return the target node id of this Arc.
     */
	  int getDestinationId();

    /**
     * Returns the port index of the target node of this Arc.
     * @return the port index of the target node of this Arc.
     */
	  int getDestinationPortId();

    /**
     * Determines if this Arc is a connection point.
     * @return a boolean determining if this Arc is a connection point.
     */
	  bool isConnectionPoint();

    /**
     * Locks this Arc.
     */
	  void lockArc();

    /**
     * Unlocks this Arc.
     */
	  void unlockArc();

	  void makeInputArc(){ m_inputArc = true; }
	  void makeOutputArc(){ m_outputArc = true; }

	  bool isOutputArc() { return m_outputArc; }
	  bool isInputArc() { return m_inputArc; }
    /**
     * Returns the string representation of this Arc.
     * @return the string representation of this Arc.
     */
    virtual string toString();
};
#endif // Arc_H
