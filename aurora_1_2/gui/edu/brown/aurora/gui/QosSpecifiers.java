
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


/**
 * A class to handle specifying the Qos for outputport nodes.
 *
 * @author Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class QosSpecifiers
{
    protected static String QOSDELIM = "|";
    protected static String INNERQOSDELIM = "~";

    public static String EMPTYQOSPOINTS = "1" + QOSDELIM + "0" + QOSDELIM + "0" + INNERQOSDELIM + "1";
    public static String EMPTYQOS = "1" + Constants.DELIM + EMPTYQOSPOINTS;

    protected int numSpecifiers;
    protected Vector qosSpecifiers;

    public QosSpecifiers(String qos) throws QosSpecifierException
    {
        qosSpecifiers = new Vector();
        if(qos == null) {
            numSpecifiers = 0;
        } else {
            try {
                StringTokenizer tokens = new StringTokenizer(qos, Constants.DELIM);

                numSpecifiers = Integer.parseInt(tokens.nextToken());

                int i=0;
                while(tokens.hasMoreTokens()) {
                    qosSpecifiers.addElement(new QosPoints(tokens.nextToken()));
                    i++;
                }
                if(i != numSpecifiers) {
                    throw new QosSpecifierException("Number of QOS specifiers found " + i +
                                                    " different than QOS specifiers specified " +
                                                    numSpecifiers);
                }
            } catch (Exception e) {
                e.printStackTrace();
                throw new QosSpecifierException("Error parsing qos string : " + qos);
            }
        }
    }

    public int getNumQosSpecifiers() throws QosSpecifierException
    {
        return numSpecifiers;
    }

    public QosPoints getQosPointsIndex(int index)
    {
        return (QosPoints) qosSpecifiers.elementAt(index);
    }

    public void setQosPointsIndex(QosPoints qosPoints, int index)
    {
        qosSpecifiers.setElementAt(qosPoints, index);
    }

    public void setNumSpecifiers(int numSpecifiers)
    {
	int index = 0;
	if (qosSpecifiers == null) {
	    qosSpecifiers = new Vector();
	}
	else {
	    index = this.numSpecifiers;
	    if (numSpecifiers < qosSpecifiers.size()) {
		for (int i = this.numSpecifiers-1; i >= numSpecifiers; i--) {
		    qosSpecifiers.removeElementAt(i);
		}
	    }
	}
	
	this.numSpecifiers = numSpecifiers;
        for(int i=index; i<numSpecifiers; i++) {
            try {
                qosSpecifiers.addElement(new QosPoints(null));
            } catch (QosSpecifierException e) {
                e.printStackTrace();
            }
        }
    }

    public String toString()
    {
        String qosString = ""+numSpecifiers;

        for(int i=0; i<qosSpecifiers.size(); i++) {
            QosPoints points = getQosPointsIndex(i);
            qosString += Constants.DELIM + points.toString();
        }
        return qosString;
    }

}
