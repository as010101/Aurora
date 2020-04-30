package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.awt.*;

import java.util.*;

/**
 * BoxPortConnectionPoint is a subclass for PetalPort.  It provides the functionality 
 * for a distinct port in a boxcomponent to locate its connection points.  It allows 
 * arc ends to dynamically locate their ends when connected to specific ports of boxes. 
 * It is not meant be the visual Swing component representation of a port, only the 
 * abstract semantic representation of the port.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class BoxPortConnectionPoint extends PetalPort implements java.io.Serializable
{
    /**
     * The visual representation of this port
     */
    protected GenericBoxNodePort port;
    

    /**
     * Constructs a BoxPortConnectionPoint object.
     *
     * @param x The x position of this BoxPortConnectionPoint.
     * @param y The y position of this BoxPortConnectionPoint.
     * @param port The PortNode associated with this BoxPortConnectionPoint.
     */
    public BoxPortConnectionPoint(double x, double y, GenericBoxNodePort port)
    {
        super(x, y);
        this.port = port;
    }

    /**
     * Constructs a BoxPortConnectionPoint object.
     *
     * @param where The spatial location of the connection point into this port.
     * Please refer to PetalPort for constant values for this parameter.
     * @param port The PortNode associated with this BoxPortConnectionPoint.
     */
    public BoxPortConnectionPoint(int where, GenericBoxNodePort port)
    {
        super(where);
        this.port = port;
    }

    /**
     * Implements the getconnectionPoint method required by the PetalPort interfance.
     * Returns the point of the connection of an arc specified by this 
     * BoxPortConnectionPoint object.
     * 
     * @param b The rectangle bordering the visual component of this box port.
     *
     * @return The point of connection of an arc specified by this 
     * BoxPortConnectionPoint object.
     */
    public Point getConnectionPoint(Rectangle b)
    {
        Point p1 = super.getConnectionPoint(b);
        Point p2 = port.getPortLocation();
        
        double x1 = p1.getX();
        double y1 = p1.getY();
        
        double x2 = p2.getX();
        double y2 = p2.getY();         
        
        return new Point((int)(x1+x2) + port.getWidth()/2, 
                         (int)(y1+y2) + port.getHeight()/2);
    }
}
