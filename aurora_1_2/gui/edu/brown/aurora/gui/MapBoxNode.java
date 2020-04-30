package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

import com.sleepycat.db.*;

/**
 * A non-abstract subclass of UnaryBoxNodes that contains semantic information for
 * map boxes.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class MapBoxNode extends UnaryBoxNode 
{
    /**
     * A static label_counter object
     */
    protected static int label_counter = 0;

    /**
     * The string representing an assignment.
     */
    protected String assignmentString = new String();
    
    /**
     * Constructs a new MapBoxNode object.
     *
     * @param boxId The box id.
     * @param m The model that contains this JoinBoxNode object.
     */
    public MapBoxNode(int boxId, int parentId, String modifier, Model m, 
		      boolean useDefinedTypes, boolean outputDefinedTypes)
    {
        super(boxId, parentId, modifier, m, useDefinedTypes, outputDefinedTypes);
        setLabel("Map Box " + label_counter);
        label_counter++;
    }

    /**
     * Returns true if this node gets its box Id set in the lower section of the box id
     * range.
     */
    public boolean isInLowerBoxIds()
    {
        return false;
    }

    
    /**
     * Returns a mapbox node from a box record object that contians a cursor
     * pointing to a particular tuple in the database.
     *
     * @param record The tuple containing the box node information to load.
     * @param mapBoxTable The table containing the map box information to load.
     * @param key
     * @param model The model that contains this box node object.
     */
    public static MapBoxNode loadRecord(BoxRecord record, 
                                        Model model) throws Throwable
    {
        MapBoxNode node = new MapBoxNode(record.getBoxId(),
                                         record.getParentId(),
                                         record.getModifier(),
                                         model,
					 record.isUseDefinedTypes(),
					 record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }


    /**
     * Returns a mapbox node from a box palette record object.
     *
     * @param record The tuple containing the box node information to load.
     * @param model The model to contain this box node object.
     */
    public static MapBoxNode loadRecord(BoxPaletteRecord record, Model model) throws Throwable
    {
        int boxId = setNextUpperIdCounter();

        MapBoxNode node = new MapBoxNode(boxId,
                                         record.getParentId(),
                                         record.getModifier(),
                                         model,
					 record.isUseDefinedTypes(),
					 record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }


    /**
     * Returns the assignments string.
     *
     * @return The assignemts as a string.
     */
    public String getAssignmentsString()
    {
        return assignmentString;
    }

    /**
     * Sets the assignments as a string.
     *
     * @param assignmentsString The string to set the assignments to.
     */
    public void setAssignmentsString(String assignmentString)
    {
        this.assignmentString = assignmentString;
    }
    
    /**
     * Opens up a properties dialog for the map box node.
     *
     * @param tm The typemanager associated with the GUI.
     */
    public void showProperties()
    {
        new MapBoxPropertiesDialog(this);        
    }    

    /**
     * Returns the name of the box.
     *
     * @return the name of the box as a string.
     */
    public String getBoxTypeName()
    {
        return "Map Box";
    }

    public String toString()
    {
        return "Map: " + super.toString();
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.MAP;
    }

}


