
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * BoxNodeOutputPort is a class that extends PortNode.  It represents an output port
 * within a boxnode that contains all of its semantic information.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class BoxNodeOutputPort extends GenericBoxNodePort 
{

    /**
     * The panel of the parent containing this output port node. This is meant
     * to contain the node_component of the BoxNode.
     */
    protected JPanel parentPanel;


    /**
     * Constructs a new BoxNodeOutputPort.
     *
     * @param where The position of the arc link to this port node.
     * @param panel The panel of the parent component containing this output port
     * node.
     * @param index The index of the parent box that is assigned to this 
     * BoxNodeOutputPort.
     */
    public BoxNodeOutputPort(int where, JPanel panel, int index, BoxNode boxNode)
    {
        super(where, index, boxNode);
        parentPanel = panel;
    }

    /**
     * Returns a string representation of this BoxNodeOutputPort.
     *
     * @return a String representation of this BoxNodeOutputPort.
     */
    public String toString() 
    {
        return "o"+index;
    }

    /**
     * Returns the location of this port on its parent container.
     *
     * @return The location of this port on its parent container as a point.
     */
    public Point getPortLocation()
    {
        Point p1 = parentPanel.getLocation();
        Point p2 = getLocation();

        int x1 = (int)p1.getX();
        int y1 = (int)p1.getY();

        int x2 = (int)p2.getX();
        int y2 = (int)p2.getY();
        return new Point(x1+x2, y1+y2);
    }

    /**
     * Compares a BoxNodeOutputPort object with another object.
     *
     * @return true if the other object and BoxNodeOutputPort are equal, false otherwise.
     */
    public boolean equals(Object other) 
    {
        if(other == null || !(other instanceof BoxNodeOutputPort)) {
            return false;
        }
        BoxNodeOutputPort otherBoxNodePort = (BoxNodeOutputPort)other;
        
        GenericNode thisSource = (GenericNode)getGenericNode();
        GenericNode otherSource = (GenericNode)otherBoxNodePort.getGenericNode();

        return this.index == otherBoxNodePort.index &&
            thisSource.equals(otherSource);
        
    }


}
