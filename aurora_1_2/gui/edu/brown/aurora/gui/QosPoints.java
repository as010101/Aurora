package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.awt.*;
import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import java.util.*;

public class QosPoints
{
    protected Vector points;
    protected int qosType;
    protected int numPoints;

    public QosPoints(String qos) throws QosSpecifierException
    {
	//System.out.println(qos);
        if(qos == null) {
            numPoints = 3;
            qosType = 0;
            points = new Vector();
            points.addElement("0");
            points.addElement("1");
            points.addElement("1");
            points.addElement("1");
            points.addElement("2");
            points.addElement("0");
        } else {
            try {
                StringTokenizer tokens = new StringTokenizer(qos, QosSpecifiers.QOSDELIM);
                numPoints = Integer.parseInt(tokens.nextToken());
                qosType = Integer.parseInt(tokens.nextToken());
                points = new Vector();

                int i=0;
                while(tokens.hasMoreTokens()) {
                    StringTokenizer innerTokens = new StringTokenizer(tokens.nextToken(),
                                                                      QosSpecifiers.INNERQOSDELIM);
                    points.addElement(innerTokens.nextToken());   // X value
                    points.addElement(innerTokens.nextToken());   // Y value
                    i++;
                }
                if(i != numPoints) {
                    throw new QosSpecifierException("qos string : " + qos +
                                                    " specifies " + numPoints +
                                                    " points but actually has " + i);
                }
            } catch(Exception e) {
                System.out.println("Error parsing qos string : " + qos);
                e.printStackTrace();
            }
        }
    }

    public int getNumQosPoints()
    {
        return numPoints;
    }

    public int getQosType()
    {
        return qosType;
    }

    public void setNumQosPoints(int numPoints)
    {
        this.numPoints = numPoints;

        points = new Vector();

        for(int i=0; i<numPoints; i++) {
            points.addElement("0"); // X value
            points.addElement("0"); // Y value
        }
    }

    public String toString()
    {
        String qosString = getNumQosPoints() + QosSpecifiers.QOSDELIM + qosType;

        for(int i=0; i<points.size(); i += 2) {
            String xString = points.elementAt(i).toString();
            String yString = points.elementAt(i+1).toString();

            qosString += QosSpecifiers.QOSDELIM + xString + QosSpecifiers.INNERQOSDELIM + yString;
        }
        return qosString;
    }

    public Vector getPoints()
    {
        return points;
    }

    public void setQosPoints(Vector qosPoints)
    {
        String npString = qosPoints.elementAt(0).toString();
        String qtString = qosPoints.elementAt(1).toString();

        numPoints = Integer.parseInt(npString);
        qosType = Integer.parseInt(qtString);

        qosPoints.removeElementAt(0); // numPoints
        qosPoints.removeElementAt(0); // qosType

        points = qosPoints;
    }
}
