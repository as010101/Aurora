package edu.brown.aurora.gui;

import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;
import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

import com.sleepycat.db.*;


/**
 * A non-abstract subclass of UnaryBoxNodes that contains semantic information for
 * drop boxes.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class DropBoxNode extends UnaryBoxNode
{

    /**
     * A static int that counts the number of drop boxes.
     */
    protected static int label_counter = 0;

    /**
     * Constructs a new DropBoxNode.
     */
    public DropBoxNode(int boxId, int parentId, String modifier, 
		       Model m, boolean useDefinedTypes, boolean outputDefinedType)
    {
        super(boxId, parentId, modifier, m, useDefinedTypes, outputDefinedType);
        setLabel("Drop Box " + label_counter);
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
     * Returns a dropbox node from a box record object that contians a cursor
     * pointing to a particular tuple in the database.
     *
     * @param record The tuple containing the box node information to load.
     * @param key
     * @param model The model that contains this box node object.
     */
    public static DropBoxNode loadRecord(BoxRecord record, 
                                           Model model) throws Throwable
    {

        DropBoxNode node = new DropBoxNode(record.getBoxId(),
                                           record.getParentId(),
                                           record.getModifier(),
                                           model,
					   record.isUseDefinedTypes(),
					   record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }

    /**
     * Returns a dropbox node from a box palette record object.
     *
     * @param record The tuple containing the box node information to load.
     * @param model The model to contain this box node object.
     */
    public static DropBoxNode loadRecord(BoxPaletteRecord record, Model model) throws Throwable
    {
        int boxId = setNextUpperIdCounter();

        DropBoxNode node = new DropBoxNode(boxId,
                                           record.getParentId(),
                                           record.getModifier(),
                                           model,
					   record.isUseDefinedTypes(),
					   record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }


    /**
     * Returns a string representation of this DropBoxNode.
     *
     * @return A string representation of this DropBoxNode.
     */
    public String toString()
    {
        return "Drop Box : " + super.toString();
    }

    /**
     * Opens up a properties dialog box.
     *
     */
    public void showProperties()
    {
        new DropBoxPropertiesDialog(this);        
    }    
    
    /**
     * Returns the type name of this box.
     *
     * @return the type name of this box as a string.
     */
    public String getBoxTypeName()
    {
        return "Drop Box";
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.DROP;
    }

}


