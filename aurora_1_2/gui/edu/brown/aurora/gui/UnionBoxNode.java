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
 * A box node for the Union operator.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class UnionBoxNode extends UnaryBoxNode implements MultipleInputsBoxInterface
{
    /**
     * A static label_counter object
     */
    protected static int label_counter = 0;

    /**
     * Constructs a new UnionBoxNode object.
     *
     * @param boxId The box id.
     * @param m The model that contains this JoinBoxNode object.
     */
    public UnionBoxNode(int boxId, int parentId, String modifier, Model m,
			boolean useDefinedTypes, boolean outputDefinedTypes)
    {
        super(boxId, parentId, "Union Box " + label_counter, m, 
	      useDefinedTypes, outputDefinedTypes);
        setLabel("Union Box " + label_counter);
        label_counter++;
	setModifier(getModifier());
    }

    /**
     * Returns true if this node gets its box Id set in the lower section of the box id
     * range.
     */
    public boolean isInLowerBoxIds()
    {
        return false;
    }

    public String getModifier() {
	return modifier;
    }

    /**
     * Union is special.  We override the inferoutput type method.
     */
    
    public Type inferOutputType()
    {
	Type newType = null;
	// Type newType = super.inferOutputType();
	if(!inputTypesConsistent()) {
	    setModifier("");
	    newType = null;
	} else {
	    setModifier(getLabel());
	    BoxNodeInputPort inputPort = getInputPort();
	    newType = inputPort.getType();
	}

        BoxComponent boxComponent = (BoxComponent) node_component;
        Vector outputPorts = boxComponent.getOutputPorts();
        for(int i=0; i<outputPorts.size(); i++) {
            BoxNodeOutputPort outputPort = (BoxNodeOutputPort) outputPorts.elementAt(i);
            outputPort.setType((CompositeType)newType);
        }


	return newType;
    }
    

    /**
     * Returns a unionbox node from a box record object that contians a cursor
     * pointing to a particular tuple in the database.
     *
     * @param record The tuple containing the box node information to load.
     * @param unionBoxTable The table containing the union box information to load.
     * @param key
     * @param model The model that contains this box node object.
     */
    public static UnionBoxNode loadRecord(BoxRecord record,
                                          Model model) throws Throwable
    {
        UnionBoxNode node = new UnionBoxNode(record.getBoxId(),
					     record.getParentId(),
					     record.getModifier(),
					     model,
					     record.isUseDefinedTypes(),
					     record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }

    /**
     * Returns a unionbox node from a box palette record object.
     *
     * @param record The tuple containing the box node information to load.
     * @param model The model to contain this box node object.
     */
    public static UnionBoxNode loadRecord(BoxPaletteRecord record, Model model) throws Throwable
    {
        int boxId = setNextUpperIdCounter();
        
        UnionBoxNode node = new UnionBoxNode(boxId,
                                             record.getParentId(),
                                             record.getModifier(),
                                             model,
					     record.isUseDefinedTypes(),
					     record.isOutputDefinedTypes());
        BoxNode.loadRecord(node, record, model);
        return node;
    }

    /**
     * Opens up a properties dialog for the union box node.
     *
     */
    public void showProperties()
    {
        new UnionBoxPropertiesDialog(this);        
    } 

    /**
     * Returns the name of the box.
     *
     * @return the name of the box as a string.
     */   
    public String getBoxTypeName()
    {
        return "Union Box";
    }

    public String toString()
    {
        return "Union: " + super.toString();
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.UNION;
    }

    /**
     * Returns true if the input types are all consistent, false otherwise.
     *
     * @return true if the input types are all consistent, false otherwise.
     */
    public boolean inputTypesConsistent()
    {
	
	BoxComponent c = (BoxComponent)getComponent();
	Vector inputPorts = c.getInputPorts();
	GenericBoxNodePort inputPort = (GenericBoxNodePort) inputPorts.elementAt(0);
	Vector arcs = inputPort.getArcs();

	if(arcs.size() == 0) {
	    return false;
	}
		
	Type firstType = getType((GenericArc) arcs.elementAt(0));

	if(firstType == null) {
	    return false;
	}
	
	if(arcs.size() == 1) {
	    return true;
	}
	
	for(int i=1; i<arcs.size(); i++) {	    
	    Type otherType = getType((GenericArc) arcs.elementAt(i));
	    if(otherType == null) {
		return false;
	    }
	    
	    if(!otherType.isCompatible(firstType)) {
		return false;
	    }	    
	}

	return true;	
    }


    public Type getType(GenericArc arc)
    {
	BoxNodeOutputPort outputPort = arc.getOutputPort();
	return outputPort.getType();
    }
}


