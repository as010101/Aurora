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
 * GenericBoxNodePort is an abstract class that implements the visual and the semantic
 * ports on the box nodes.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class GenericBoxNodePort extends JPanel { //implements MouseListener{

    /**
     * The index of this GenericBoxNodePort within its parent box node.
     */
    protected int index = 0;
    
    /**
     * The arc connection point for this box node port.
     */
    protected BoxPortConnectionPoint boxPort;

    /** 
     * The cardinality.
     */
    protected int cardinality;

    /**
     * The type assigned to this port.
     */
    protected CompositeType type;

    /**
     * The arcs attached to this port.
     */
    protected Vector arcs;

    /**
     * The GenericNode object that contains this input port.
     */
    protected GenericNode genericNode;    

    /**
     * Constructs a new BoxNortPode objet.
     *
     * @param where The connection point of an arc.  See 
     * PetalPort for details.
     * @param index The index assigned to this object from the
     * parent boxnode.     
     */
    public GenericBoxNodePort(int where, int index, GenericNode genericNode)
    {    
        super();
        this.index = index;
        setBackground(Constants.PORTNODE_COLOR);

        this.genericNode = genericNode;
        
        boxPort = new BoxPortConnectionPoint(where, this);
        arcs = new Vector();
    }

    public boolean equals(Object other) 
    {
        if(!(other instanceof GenericBoxNodePort)) {
            return false;
        }
        GenericBoxNodePort otherBoxNodePort = (GenericBoxNodePort)other;
        return this.index == otherBoxNodePort.index;
    }

    /**
     * Sets the arc connection to this GenericBoxNodePort.
     *
     * @param arc The arc to connect to this GenericBoxNodePort.
     */
    public void addArc(GenericArc arc)
    {
        arcs.addElement(arc);
    }


    public void removeArc(GenericArc arc)
    {
        arcs.remove(arc);
    }

    /**
     * Retrieves arcs that are connected to this GenericBoxNodePort.
     *
     * @param The arcs connected to this GenericBoxNodePort.
     */    
    public Vector getArcs()
    {
        return arcs;
    }
    
    /**
     * Gets the index of this GenericBoxNodePort within its containing BoxNode.
     *
     * @return the index of this GenericBoxNodePort.
     */
    public int getIndex()
    {
        return index;
    }
    
    /**
     * Sets the index of this GenericBoxNodePort within its containing BoxNode.
     *
     * @param The index of this GenericBoxNodePort to set.
     */
    public void setIndex(int index)
    {
        this.index = index;
    }
    
    /**
     * Returns the box node that contains this node port object.
     *
     * @return The box node that contains this node port object.
     */

    public GenericNode getGenericNode()
    {
        return this.genericNode;
    }

    /**
     * Sets the box node that contains this node port.
     *
     * @param GenericNode the box node to set.
     */
    public void setGenericNode(GenericNode genericNode)
    {
        this.genericNode = genericNode;
    }



    /**
     * Returns the BoxPort contained within this GenericBoxNodePort.
     *
     * @return The BoxPort contained within this GenericBoxNodePort.
     */
    public BoxPortConnectionPoint getPort()
    {
        return boxPort;
    }
    
    /**
     * Sets the BoxPort for this GenericBoxNodePort object.
     *
     * @param p The BoxPort to set.
     */
    public void setPort(BoxPortConnectionPoint p)
    {
        boxPort = p;
    }
    
    /**
     * Returns true if point p is contained by this GenericBoxNodePort,
     * false otherwise.
     *
     * @param p The point which is or is not contained by this
     * GenericBoxNodePort visual component.
     */
    public boolean contains(Point p) 
    {
        Point origin = getLocation();
        Dimension dim = getSize();

        int x1 = (int)origin.getX();
        int y1 = (int)origin.getY();

        int x2 = (int)dim.getWidth() + x1;
        int y2 = (int)dim.getHeight() + y1;

        int xp = (int)p.getX();
        int yp = (int)p.getY();

        return 
            (x1 <= xp && xp <= x2) && 
            (y1 <= yp && yp <= y2);
    }

    /**
     * Returns the location of this port.
     *
     * @return A point representing the location of this port.
     */
    public abstract Point getPortLocation();


    /**
     * Sets the cardinality.
     *
     * @param cardinality The cardinality to set.
     */
    public void setCardinality(int cardinality)
    {
        this.cardinality = cardinality;
    }

    /**
     * Retrieves the cardinality.
     *
     * @param The cardinality to return.
     */
    public int getCardinality()
    {
        return cardinality;
    }

    /**
     * Sets the type of this port. 
     * @param type the type of this port.
     */
    public void setType(CompositeType type)
    {
	/*
	if(type instanceof CompositeType) {
	    System.out.println("\t Setting " + toString() + "'s type of " + 
	    		       genericNode.toString() + " to " + type.getName());
	}
	*/
        this.type = type;
    }
    
    /**
     * Reeturns this port's type. 
     * @return the type of this port.
     */
    public CompositeType getType()
    {
        return type;
    }
    
    /**
     * Returns a String object representing this GenericWorkspacePortNode's
     * type name.  If the type is not yet assigned, an empty string is 
     * returned.
     * @return a string representation of the name of this port's type.
     */
    public String getTypeName()
    {
        if (type == null)
            return new String("");
        else
            return type.getName();
    }

}
