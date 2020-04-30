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
 * A box node for the join operator.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class JoinBoxNode extends BinaryBoxNode 
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
     * The operator type.
     */
    protected int operatorType = 0;

    /**
     * The time window.
     */
    protected int timeWindow = 0;

    /**
     * Constructs a new JoinBoxNode object.
     *
     * @param boxId The box id.
     * @param m The model that contains this JoinBoxNode object.
     */
    public JoinBoxNode(int boxId, int parentId, String modifier, 
		       Model m, int timeWindow, boolean useDefinedTypes, 
		       boolean outputDefinedType)
    {
        super(boxId, parentId, modifier, m, useDefinedTypes, outputDefinedType);
        setLabel("Join Box " + label_counter);
        label_counter++;
        this.timeWindow = timeWindow;
    }

    /**
     * Returns true if this node gets its box Id set in the lower section of the box id
     * range.
     */
    public boolean isInLowerBoxIds()
    {
        return true;
    }

    
    /**
     * Returns a joinbox node from a box record object that contains a cursor
     * pointing to a particular tuple in the database.
     *
     * @param record The tuple containing the box node information to load.
     * @param joinBoxTable The table containing the join box information to load.
     * @param key
     * @param model The model that contains this box node object.
     */
    public static JoinBoxNode loadRecord(BoxRecord record,
                                         Model model) throws Throwable
    {

        JoinBoxNode node = new JoinBoxNode(record.getBoxId(),
                                           record.getParentId(),
                                           record.getModifier(),
                                           model, 
                                           0,
					   record.isUseDefinedTypes(),
					   record.isOutputDefinedTypes());  // time window
        BoxNode.loadRecord(node, record, model);
        return node;
    }

    /**
     * Returns a joinbox node from a box palette record object.
     *
     * @param record The tuple containing the box node information to load.
     * @param model The model to contain this box node object.
     */
    public static JoinBoxNode loadRecord(BoxPaletteRecord record, Model model) throws Throwable
    {
        int boxId = setNextLowerIdCounter();

        JoinBoxNode node = new JoinBoxNode(boxId,
                                           record.getParentId(),
                                           record.getModifier(),
                                           model,
                                           0,
					   record.isUseDefinedTypes(),
					   record.isOutputDefinedTypes()); // time window
        BoxNode.loadRecord(node, record, model);
        return node;
    }

    
    /**
     * Returns the time window.
     *
     * @return the time window as an int.
     */
    public int getTimeWindow()
    {
        return timeWindow;
    }
    
    /**
     * Sets the time window.
     *
     * @param timeWindow the value to set the timewindow to.
     */
    public void setTimeWindow(int timeWindow)
    {
        this.timeWindow = timeWindow;
    }
    
    /**
     * Opens up a properties dialog for the join box node.
     *
     */
    public void showProperties()
    {
        new JoinBoxPropertiesDialog(this);        
    }    
    
    /**
     * Returns the name of the box.
     *
     * @return the name of the box as a string.
     */
    public String getBoxTypeName()
    {
        return "Join Box";
    }

    public String toString()
    {
        return "Join: " + super.toString();
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.JOIN;
    }

    
    /**
     * Returns the id of the first arc.
     *
     * @return the id of the first arc as an int.
     */
    public int getLeftArcId()
    {
        BoxNodeInputPort i = ((BoxComponent)getComponent()).getInputPort(0);
        if (i != null) {
            if(i.getArcs().size() == 0) {
                return -1;
            }
            GenericArc arc = (GenericArc)(i.getArcs()).elementAt(0);
            if (arc != null)
                return arc.getArcId();
        }
        return -1;
    }
    
    /**
     * Returns the id of the second arc.
     *
     * @return the id of the second arc as an int.
     */
    public int getRightArcId()
    {
        BoxNodeInputPort i = ((BoxComponent)getComponent()).getInputPort(1);
        if (i != null) {
            if(i.getArcs().size() == 0) {
                return -1;
            }
            GenericArc arc = (GenericArc)(i.getArcs()).elementAt(0);
            if (arc != null) {
                return arc.getArcId();
            }
        }
        return -1;
    }
    
}


