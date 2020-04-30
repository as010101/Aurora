/*
 * @(#)InputPort.java	1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved. 
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui;
import edu.brown.aurora.gui.types.*;
import java.awt.*;

/**
 * An InputPort object is used as a means to feed an Aurora box. 
 * Each InputPort object has its window defined by the specified timewindow (in millisecond) 
 * and cardinarlity (maximum # of tuples can be stored in the port). 
 * In addition, as a descendent of AbstractPort, InputPort has a type field of the CompositeType 
 * class.  The properties of an InputPort can be changed via the setProperties(Frame frame) 
 * method.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class InputPort extends GenericPort
{
    
    long timeWindow = 0;
    int cardinality = 0;
    
    /**
     * Constructs a default InputPort object. The type is not assigned and the timeWindow 
     * and cardinality are set to 0.
     */
    public InputPort()
    {
        super();
    }
    
    /**
     * Constructs a new InputPort object having the same contents as the specified InputPort 
     * object. 
     */
    public InputPort(InputPort port)
    {
        super(port);
        timeWindow = port.timeWindow;
        cardinality = port.cardinality;
    }
    
    /**
     * Returns a String object representing this port. 
     * @return a string representation of this port.
     */
    public String toString()
    {
        return super.toString()+": time window ("+timeWindow+"ms), cardinality (" + cardinality +")";
    }
    
    
    /**
     * Sets the time window of this port. 
     * @param timeWindow for this port.
     */
    public void setTimeWindow(long timeWindow)
    {
        this.timeWindow = timeWindow;
    }
    
    /**
     * Sets the cardinality of this port. 
     * @param the cardinality for this port.
     */
    public void setCardinality(int cardinality)
    {
        this.cardinality = cardinality;
    }
    
    /**
     * Returns the time window of this port. 
     * @return the time window of this port.
     */
    public long getTimeWindow()
    {
        return timeWindow;
    }
    
    /**
     * Returns the cardinality of this port. 
     * @return the cardinality of this port.
     */
    public int getCardinality()
    {
        return cardinality;
    }
    
    /**
     * Sets the properties of this port using a properties dialog box.
     * @param frame the parent frame of the properties box. It can be null.
     */
    public void setProperties(Frame frame)
    {
        // new InputPortPropertiesDialog(this, frame);
    }
   
}
