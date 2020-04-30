/*
 * @(#)QoS.H    1.0 07/15/2002
 *
 * Copyright 2002 Nesime Tatbul (tatbul@cs.brown.edu), Jeong-Hyon Hwang
 * (jhhwang@cs.brown.edu) and Alexander Rasin (alexr@cs.brown.edu).
 * All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef QOS_H
#define QOS_H

#include <vector>
#include <string>
#include <stdio.h>
// #include <strstream> // commenting out - Nesime
#include <iostream>
// STRSTREAM is deprecated - can we fix this? - eddie
//  fix by not using strstream but ostrstream and other funky stuff ..
//  that is, dont use strstream, use ostringstream n stuff like that, i think...
using namespace std;

typedef struct QoSEntry
{
  float x;
  float utility;
} QoSEntry;

typedef vector<QoSEntry*>	QoSEntryVector;

/**
 * An QoS (Quality of Service) object consists a number of points which enable to find the
 * estimated utility for a specified x point.
 *
 * @author Nesime Tatbul (tatbul@cs.brown.edu) and Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class QoS
{
  // Attribute(s)
  protected:
   QoSEntryVector m_graph;
   int m_qosType;
//    float            _cursor;

  // Method(s)
public:

    /**
     * Constructs a QoS object consisting of a number of points specified in the given string modifier.
     * @param modifer the modifer representing the qos.
     */
    QoS(string modifier);

    /**
     * Destroys a QoS object.
     */
    virtual ~QoS();

    /**
     * Returns the utility at the specified point.
     * @param point a point in the qos's domain.
     * @return the utility at the specified point.
     */
    float getUtility( float point );

    /**
     * Returns this QoS's graph consisting of points.
     * @return this QoS's graph consisting of points.
     */
    QoSEntryVector& getGraph();

    /**
     * Returns the type of this QoS.
     * @return the type of this QoS.
     */
    int getQosType();

    /**
     * Returns the string representation of this QoS.
     * @return the string representation of this QoS.
     */
    string toString();

/*
    float   getSlope(float point);
    void    setSlope(float point, float slope);

    void    setUtility(float point, float utility);

    float   getCursor();
    void    setCursor(float cursor);

	void			setGraph(QoSEntryVector g);	

	int		getGraphSize();

    virtual void printQoS();
*/
};

/*
class MessageQoS : public QoS {

public:

    MessageQoS(QoSEntryVector g) : QoS(g) { _cursor = 100; }
    MessageQoS() : QoS() { _cursor = 100; }
    ~MessageQoS() { }

    void    printQoS();

private:

};                              

class DelayQoS : public QoS {

public:

    DelayQoS(QoSEntryVector g, float threshold, float upper);
	DelayQoS(QoSEntryVector g) : QoS(g) { _cursor = 0; }
    DelayQoS() : QoS() { _cursor = 0; }
    ~DelayQoS() { }

    float   getThreshold();
    void    setThreshold(float threshold);

    float   getUpper();
    void    setUpper(float upper);

    void    printQoS();

private:

    float   _threshold;   // if delay <= _threshold, utility = 1
    float   _upper;       // else if delay >= _high, utility = 0
                          // else, utility defined by the _graph
};

class ValueQoS : public QoS { 

public:

    ValueQoS(QoSEntryVector g, int index, float size, float upper, float lower);
	ValueQoS(QoSEntryVector g) : QoS(g) { }	// no cursor initialization
    ValueQoS() : QoS() { }
    ~ValueQoS() { }

    int		getAttrIndex();
    void    setAttrIndex(int index);

    float   getSize();
    void    setSize(float size);

    float   getUpper();
    void    setUpper(float upper);
    
    float   getLower();
    void    setLower(float lower);

    void    printQoS();
    
private:

    int      _attr_index;   // stream attribute for which this qos is defined
    float    _intSize;      // interval units to be used in Load Shedding
    float    _upper;        // [lower, upper] specifies the value range for attr
    float    _lower;        // assume value is of type float for now
                            // also assume there is one value_based qos graph
};
*/
#endif // QOS_H

