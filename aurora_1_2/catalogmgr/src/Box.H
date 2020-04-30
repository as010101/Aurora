/*
 * @(#)Box.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-Hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef BOX_H
#define BOX_H

#include <pthread.h>
#include <string>
#include "Predicate.H"
#include <vector>
#include <list>
#include "AggregateState.H"


using namespace std;

/**
 * The Box class is an abstract class that all Boxes are meant to subclass. 
 * A Box object contain box-fundamental information such as
 * the boxId, cost, selectivity of an Aurora Box.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class Box 
{
 private:
  list<int> appl_ids;


  // Attribute(s)
  protected:
    int m_id;
    int m_x;
    int m_y;
    int m_height;
    int m_width;
    float m_cost;
    float m_selectivity;
    pthread_mutex_t* m_mutex;
    //    AggregateState* m_state;
    
  // Method(s)
  public:
    /**
     * Constructs a new box having the specified id, cost, and selectivity.
     * @param id the id of the Box.
     * @param cost the cost of the Box.
     * @param sel the sel of the Box.
     */
    Box(int id, float cost, float sel, int x=0, int y=0, int width=50, int height=30);

  virtual ~Box();

    /**
     * Returns the id of this Box.
     * @return the id of this Box.
     */
    int getId();

    /**
     * Returns the leftmost point of this Box.
     */
    int getX();

    /**
     * Returns the topmost point of this Box.
     */
    int getY();

    /**
     * Returns the width of this Box.
     */
    int getWidth();

    /**
     * Returns the height of this Box.
     */
    int getHeight();

    /**
     * Returns the cost of this Box.
     * @return the cost of this Box.
     */
    float getCost();

    /**
     * Sets the cost of this Box.
     * @param cost the cost value.
     */
    void setCost(float cost);

    /**
     * Returns the selectivity of this Box.
     * @return the selectivity of this Box.
     */
    float getSelectivity();

    /**
     * Returns the predicate stored for this box (if applicable)
     */
    virtual Predicate* getPredicate();
    virtual vector<Expression*> *getExpression();
    virtual const char* getModifier();

    virtual AggregateState* getState();

    virtual float getDropRate();

    /**
     * Sets the selectivity of this Box.
     * @param selectivity the selectivity value.
     */
    void setSelectivity(float selectivity);

    /**
     * Returns the type of this Box.
     * @return the type of this Box.
     */
    virtual int getBoxType();

    /**
     * Locks this Box.
     */
    void lockBox();

    /**
     * Trylocks this Box.
     */
    int lockBoxTry();

    /**
     * Unlocks this Box.
     */
    void unlockBox();

    /**
     * Adds an ID for a parent application.
     */
    void addParentAppl( int appl );

    // this is really a temporary hack, since box is can
    // have multiple parent applications!
    int getParentAppl();

    list<int> getParentAppls();

    /**
     * Checks whether it has an ID for a parent application.
     */
    int isDescendantOf( int appl );

    /**
     * Returns the string representation of this Box.
     * @return the string representation of this Box.
     */
    virtual string toString();

};

#endif // BOX_H

