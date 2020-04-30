/*
 * @(#)QueryNetwork.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef QUERYNETWORK_H
#define QUERYNETWORK_H

#include <map>
#include "Box.H"
#include "Application.H"
#include "SourceStream.H"
#include "Arc.H"

typedef map<int, Box*, less<int> > BoxMap;
typedef BoxMap::iterator BoxMapIter;

typedef map<int, SourceStream*, less<int> > StreamMap;
typedef StreamMap::iterator                 StreamMapIter;

typedef map<int, Application*, less<int> >  ApplicationMap;
typedef ApplicationMap::iterator            ApplicationMapIter;

typedef map<int, Arc*, less<int> >          ArcMap;
typedef ArcMap::iterator                    ArcMapIter;

/**
 * A QueryNetwork represents an Aurora Network consisting of a number of boxes, source streams, and applications connected by arcs.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class QueryNetwork
{
  // Attribute(s)
  protected:
    BoxMap m_boxes;
    StreamMap m_streams;
    ApplicationMap m_applications;
    ArcMap m_arcs;

  // Method(s)
  public:

    /**
     * Constructs a QueryNetwork.
     */
    QueryNetwork();

    /**
     * Destroys a QueryNetwork.
     */
    ~QueryNetwork();

    /**
     * Returns the number of boxes in this QueryNetwork.
     * @return the number of boxes in this QueryNetwork.
     */
    int getNumberOfBoxes();

    /**
     * Returns the box map of this QueryNetwork.
     * @return the box map of this QueryNetwork.
     */
    BoxMap& getBoxes();

    /**
     * Returns the box having the specified id.
     * @param id the id of the box want to retrieve.
     * @return the box having the specified id.
     */
    Box *getBox(int id);

    /**
     * Adds the specified box to this QueryNetwork.
     * @param box the box to add to this QueryNetwork.
     */
    void addBox(Box *box);

    /**
     * Returns the number of input streams in this QueryNetwork.
     * @return the number of input streams in this QueryNetwork.
     */
    int getNumberOfInputStreams();

    /**
     * Returns the input stream map of this QueryNetwork.
     * @return the input stream map of this QueryNetwork.
     */
    StreamMap& getInputStreams();

    /**
     * Returns the input stream having the specified id.
     * @param id the id of input stream want to retrieve.
     * @return the input stream having the specified id.
     */
    SourceStream *getInputStream(int id);

    /**
     * Adds the specified input stream to this QueryNetwork.
     * @param sc the input stream to add to this QueryNetwork.
     */
    void addInputStream(SourceStream *sc);

    /**
     * Returns the number of applications in this QueryNetwork.
     * @return the number of applications in this QueryNetwork.
     */
    int getNumberOfApplications();

    /**
     * Returns the application map of this QueryNetwork.
     * @return the application map of this QueryNetwork.
     */
    ApplicationMap& getApplications();

    /**
     * Returns the application having the specified id.
     * @param id the id of application box want to retrieve.
     * @return the application having the specified id.
     */
    Application *getApplication(int id);

    /**
     * Adds the specified application to this QueryNetwork.
     * @param app the application to add to this QueryNetwork.
     */
    void addApplication(Application *app);

    /**
     * Returns the number of arcs in this QueryNetwork.
     * @return the number of arcs in this QueryNetwork.
     */
    int getNumberOfArcs();

    /**
     * Returns the arc map of this QueryNetwork.
     * @return the arc map of this QueryNetwork.
     */
    ArcMap& getArcs();

    /**
     * Returns the arc having the specified id.
     * @param id the id of the arc that want to retrieve.
     * @return the arc having the specified id.
     */
    Arc *getArc(int id);

    /**
     * Adds the specified arc to this QueryNetwork.
     * @param a the arc to add to this QueryNetwork.
     */
    void addArc(Arc *a);

    /**
     * Returns the string representation of this QueryNetwork.
     * @return the string representation of this QueryNetwork.
     */
    string toString();

    /**
     * Returns the id of the destination box of the arc specified by the arcId.
     * @return the id of the destination box of the arc specified by the arcId.
     */
    int getDestinationBoxId(int arcId);

    int     getMaxBoxId();
    int     getMaxArcId();
    int     getMaxStreamId();
    int     getMaxAppId();


	/**
	 * Returns the number of output ports the specified box has.
	 * @return the number of output ports the specified box has.
	 */
	int getBoxOutputPortCount(int boxId);

/*
	ApplicationMap	getApplications();
*/
};

#endif

