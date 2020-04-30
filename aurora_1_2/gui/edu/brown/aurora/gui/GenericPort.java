/*
 * @(#)GenericPort.java	1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved. 
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui;
import edu.brown.aurora.gui.types.*;
import java.awt.*;

/**
 * The GenericPort class is a super class for InputPort and OutputPort classes.
 * An GenericPort has its own type specified by a CompositeType object.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class GenericPort
{    
    protected CompositeType type;
    
    /**
     * Constructs a new GenericPort object. 
     * The type is not assigned.
     */
    public GenericPort()
    {
        type = null;
    }
    
    /**
     * Constructs a new GenericPort object having the same contents as the specified 
     * GenericPort object. 
     */
    public GenericPort(GenericPort port)
    {
        type = port.type;
    }


    
    /**
     * Returns a String object representing this port. 
     * @return a string representation of this port.
     */
    public String toString()
    {
        if (type == null)
            return "";
    	else 
    	    return type.toString();
    }
    
    /**
     * Sets the type of this port. 
     * @param type the type of this port.
     */
    public void setType(CompositeType type)
    {
        this.type = type;
    }
    
    /**
     * Returns this port's type. 
     * @return the type of this port.
     */
    public CompositeType getType()
    {
        return type;
    }
    
    /**
     * Returns a String object representing this GenericPort's type name. If the type 
     * is not yet assigned, an empty string is returned.
     * @return a string representation of the name of this GenericPort's type.
     */
    public String getTypeName()
    {
        if (type == null)
            return new String("");
        else
            return type.getName();
    }
    
    /**
     * Sets the properties of this port using a properties dialog box.
     * @param rame the parent frame of the properties box. It can be null.
     */
    
    abstract void setProperties(Frame frame);
    
}
