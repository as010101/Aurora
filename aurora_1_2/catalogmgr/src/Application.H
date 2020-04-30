/*
 * @(#)Application.h    1.1 08/19/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef Application_H
#define Application_H

#include <string>
#include <vector>
#include "QoS.H"

using namespace std;

/**
 * An Application object represents an application in the Aurora Network.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class Application
{
  // Attribute(s)
  protected:
    int m_id;
    int m_incomingArcId;
    string m_modifier;
    float sharing;
    //char *modifier;
    vector<QoS*> m_qosVector;

  // Method(s)
  public:

    /**
     * Constructs a new Application having the specified id, qos1Id, qos2Id, and qos3Id.
     * @param id the id of the Box.
     * @param modifer the modifer representing the qos1Id, qos2Id, and qos3Id.
     */
    Application(int id, string modifier);

    /**
     * Destroys this Application disposing its Predicate.
     */
    virtual ~Application();

    /**
     * Returns the id of this Application.
     * @return the id of this Application.
     */
	  int getId();

    /**
     * Returns the incoming arc id of this Application.
     * @return the incoming arc id of this Application.
     */
	  int getIncomingArcId();

    /**
     * Sets the incoming arc id of this Application.
     * @param id the incoming arc id for this Application.
     */
	  void setIncomingArcId(int id);

	  QoS *getQoS();
	  QoS *getQoS(int type); 	// added by tatbul@cs.brown.edu

	  void setSharing( float s ) { sharing = s; }
	  float getSharing( ) { return sharing; }

    /**
     * Returns the modifier of the application;
     * @param no
     */
	  char *getModifier();


    /**
     * Returns the string representation of this Box.
     * @return the string representation of this Box.
     */
    virtual string toString();
};
#endif // Application_H
