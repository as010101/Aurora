
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;


/**
 * BoxNodeInputPort is a class that extends PortNode.  It represents an input port
 * within a boxnode and contains the all of its semantic information.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class BoxNodeInputPort extends GenericBoxNodePort 
{
    /**
     * The timeWindow for this port.
     */
    protected long timeWindow;


    /**
     * Constructs a new BoxNodeInputPort.
     *
     * @param index The index of the parent box that is assigned to this 
     * BoxNodeInputPort.
     * @param where The position of the arc link to this port node.
     */
    public BoxNodeInputPort(int where, int index, BoxNode boxNode)
    {
        super(where, index, boxNode);
    }

    /**
     * Sets the timewindow for this BoxNodeInputPort.
     *
     * @param timeWindow The timeWindow to set.
     */
    public void setTimeWindow(long timeWindow)
    {
        this.timeWindow = timeWindow;
    }

    /**
     * Retrieves the timewindow for this BoxNodeInputPort.
     *
     * @return the timewindow for this BoxNodeInputPort.
     */
    public long getTimeWindow()
    {
        return timeWindow;
    }

    /**
     * Returns a string representation of this BoxNodeInputPort.
     *
     * @return a String representation of this BoxNodeInputPort.
     */
    public String toString() 
    {
        return "i"+index;
    }

    /**
     * Returns the location of this port on its parent container.
     *
     * @return The location of this port on its parent container as a point.
     */
    public Point getPortLocation()
    {
        return getLocation();
    } 


    /**
     * Compares a BoxNodeInputPort object with another object.
     *
     * @return true if the other object and BoxNodeInputPort are equal, false otherwise.
     */
    public boolean equals(Object other) 
    {
        
        if(other == null || !(other instanceof BoxNodeInputPort)) {
            return false;
        }
        BoxNodeInputPort otherBoxNodePort = (BoxNodeInputPort)other;

        //return this.index == otherBoxNodePort.index;
        
        GenericNode thisTarget = (GenericNode)getGenericNode();
        GenericNode otherTarget = (GenericNode)otherBoxNodePort.getGenericNode();
        
        return this.index == otherBoxNodePort.index &&
            thisTarget.equals(otherTarget);
    }
   
}
