/*
 * @(#)BoxRecord.H	1.0 10/16/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu) and Alexander Rasin
 * (alexr@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __BoxRecord_H
#define __BoxRecord_H

#include <db_cxx.h>
#include <string>
#include "DataInputStream.H"

/**
 * BoxRecord extends Dbt in a straightforward way to allow easy
 * stroage/retrieval of rows in the Box table.
 * A BoxRecord consists of 11 attributes: [boxId(int), boxType(int), label(string), modifier(string), parentId(int), cost(float), selectivity(int),
 * x(int), y(int), width(int), height(int)].
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * 
 */

#define FILTER 0 // this is the REAL filter, FILTERBOX not SELECTBOX
#define MAP 1
//#define TUMBLE 2 replaced by AGGREGATE
#define AGGREGATE 2
#define UPDATE_RELATION 3
#define READ_RELATION 4
#define RESTREAM 6
#define UNION 7
#define RESAMPLE 8
#define JOIN 9
#define DROP 10
#define SUPERBOX 11				// relevant only to gui
#define INPUTPORT 12      // relevant only to gui/scheduler
#define OUTPUTPORT 13
// New operators - eddie
#define BSORT 5
// New operators - tibbetts
#define HELLO_WORLD 101
#define LR_UPDATE_RELATION 104
#define LR_READ_RELATION 105

class BoxRecord: public Dbt
{
  //Member Variable(s)
  public:
    const static char* databaseFileName;

  protected:
    int m_boxId;
    int m_boxType;
    string m_label;
    string m_description;
    string m_modifier;
    int m_parentId;

    float m_cost;
    float m_selectivity;

    int m_x;
    int m_y;
    int m_width;
    int m_height;

  // Method(s)
  public:

    /**
     * Constructs a newly allocated BoxRecord.
     */
    BoxRecord();

    /**
     * Constructs a BoxRecord to write into database.
     */
    BoxRecord( int id, float cost, float select, const char *mod, int type );

    /**
     * Takes values from the parameters and generates the full box record
     * to write into the database.
     */
    void set( int id, float cost, float select, const char *mod, int type );

    /**
     * Reads values from the Dbt buffer and sets the memeber variables to the corresponding values.
     */
    void parse();

    /**
     * Returns the string representation of this BoxRecord.
     * @return the string representation of this BoxRecord.
     */
    string toString();

    /**
     * Returns the box id of this BoxRecord.
     * @return the box id of this BoxRecord.
     */
    int getBoxId();

    /**
     * Returns the box type of this BoxRecord.
     * @return the box type of this BoxRecord.
     */
    int getBoxType();

    /**
     * Returns the label of this BoxRecord.
     * @return the label of this BoxRecord.
     */
    string getLabel();

    /**
     * Returns the description of this BoxRecord.
     * @return the description of this BoxRecord.
     */
    string getDescription();

    /**
     * Returns the number of fields in this BoxRecord.
     * @return the number of fields in this BoxRecord.
     */
    string getModifier();

    /**
     * Returns the parent id of this BoxRecord.
     * @return the parent id of this BoxRecord.
     */
    int getParentId();

    /**
     * Returns the cost of this BoxRecord.
     * @return the cost of this BoxRecord.
     */
    float getCost();

    /**
     * Returns the selectivity of this BoxRecord.
     * @return the selectivity of this BoxRecord.
     */
    float getSelectivity();

  /**
   * Returns the leftmost point of this BoxRecord.
   */
  int getX();

  /**
   * Returns the top point of this BoxRecord.
   */
  int getY();
  
  /**
   * Returns the width of this BoxRecord.
   */
  int getWidth();
  /**
   * Returns the height of this BoxRecord.
   */
  int getHeight();
  
protected:
  
  void readFields(DataInputStream& inputStream);
  
};

#endif // __BoxRecord_H
