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
 * A box node for the UpdateRelation operator.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class UpdateRelationBoxNode extends UnaryBoxNode
{
    /**
     * A static label_counter object
     */
    protected static int label_counter = 0;

    /**
     * Constructs a new UpdateRelationBoxNode object.
     *
     * @param boxId The box id.
     * @param m The model that contains this JoinBoxNode object.
     */
    public UpdateRelationBoxNode(int boxId, int parentId, String modifier,
			Model m, boolean useDefinedTypes, boolean outputDefinedTypes)
    {
        super(boxId, parentId, modifier, m, useDefinedTypes, outputDefinedTypes);
        setLabel("UpdateRelation Box " + label_counter);
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
     * Returns a UpdateRelationbox node from a box record object that contians a cursor
     * pointing to a particular tuple in the database.
     *
     * @param record The tuple containing the box node information to load.
     * @param UpdateRelationBoxTable The table containing the UpdateRelation box information to load.
     * @param key
     * @param model The model that contains this box node object.
     */
    public static UpdateRelationBoxNode loadRecord(BoxRecord record,
                                          Model model) throws Throwable
    {
        UpdateRelationBoxNode node = new UpdateRelationBoxNode(record.getBoxId(),
                                             record.getParentId(),
                                             record.getModifier(),
                                             model,
					     record.isUseDefinedTypes(),
					     record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }


    /**
     * Returns a UpdateRelationbox node from a box palette record object.
     *
     * @param record The tuple containing the box node information to load.
     * @param model The model to contain this box node object.
     */
    public static UpdateRelationBoxNode loadRecord(BoxPaletteRecord record, Model model) throws Throwable
    {
        int boxId = setNextUpperIdCounter();

        UpdateRelationBoxNode node = new UpdateRelationBoxNode(boxId,
                                             record.getParentId(),
                                             record.getModifier(),
                                             model,
					     record.isUseDefinedTypes(),
					     record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }


    /**
     * Opens up a properties dialog for the UpdateRelation box node.
     *
     * @param tm The typemanager associated with the GUI.
     */
    public void showProperties()
    {
        Utils.addErrorsText("Warning - UpdateRelation Box properties dialog not yet implemented.\n");
        new UpdateRelationBoxPropertiesDialog(this);
    }

    /**
     * Returns the name of the box.
     *
     * @return the name of the box as a string.
     */
    public String getBoxTypeName()
    {
        return "UpdateRelation Box";
    }

    public String toString()
    {
        return "UpdateRelation: " + super.toString();
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.UPDATE_RELATION;
    }

}


