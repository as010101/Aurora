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
 * A box node for the Bsort operator.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class BsortBoxNode extends UnaryBoxNode
{
    /**
     * A static label_counter object
     */
    protected static int label_counter = 0;

    /**
     * Constructs a new BsortBoxNode object.
     *
     * @param boxId The box id.
     * @param m The model that contains this JoinBoxNode object.
     */
    public BsortBoxNode(int boxId, int parentId, String modifier,
			Model m, boolean useDefinedTypes, boolean outputDefinedTypes)
    {
        super(boxId, parentId, modifier, m, useDefinedTypes, outputDefinedTypes);
        setLabel("Bsort Box " + label_counter);
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
     * Returns a Bsortbox node from a box record object that contians a cursor
     * pointing to a particular tuple in the database.
     *
     * @param record The tuple containing the box node information to load.
     * @param BsortBoxTable The table containing the Bsort box information to load.
     * @param key
     * @param model The model that contains this box node object.
     */
    public static BsortBoxNode loadRecord(BoxRecord record,
                                          Model model) throws Throwable
    {
        BsortBoxNode node = new BsortBoxNode(record.getBoxId(),
                                             record.getParentId(),
                                             record.getModifier(),
                                             model,
					     record.isUseDefinedTypes(),
					     record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }


    /**
     * Returns a Bsortbox node from a box palette record object.
     *
     * @param record The tuple containing the box node information to load.
     * @param model The model to contain this box node object.
     */
    public static BsortBoxNode loadRecord(BoxPaletteRecord record, Model model) throws Throwable
    {
        int boxId = setNextUpperIdCounter();

        BsortBoxNode node = new BsortBoxNode(boxId,
                                             record.getParentId(),
                                             record.getModifier(),
                                             model,
					     record.isUseDefinedTypes(),
					     record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }


    /**
     * Opens up a properties dialog for the Bsort box node.
     *
     * @param tm The typemanager associated with the GUI.
     */
    public void showProperties()
    {
        Utils.addErrorsText("Warning - Bsort Box properties dialog not yet implemented.\n");
        new BsortBoxPropertiesDialog(this);
    }

    /**
     * Returns the name of the box.
     *
     * @return the name of the box as a string.
     */
    public String getBoxTypeName()
    {
        return "Bsort Box";
    }

    public String toString()
    {
        return "Bsort: " + super.toString();
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.BSORT;
    }

}


