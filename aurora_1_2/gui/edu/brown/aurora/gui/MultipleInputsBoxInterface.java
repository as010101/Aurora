package edu.brown.aurora.gui;


/**
 * All boxes that may have multiple arcs per input port must implement this interface.  Otherwise
 * it is assumed that boxes may only have one arc per input port.  handleArcEndPoint() in 
 * edu.brown.aurora.gui.Model must check for instances of this interface in order to determine 
 * whether a second or more arc may be added to an input port.
 *
 * @author Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/1
 */

public interface MultipleInputsBoxInterface
{
}
