
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

import com.sleepycat.db.*;
import edu.brown.aurora.gui.dbts.*;
import edu.brown.aurora.gui.types.*;

/**
 * The superclass of all singular box nodes that have defined operators.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class PrimitiveBoxNode extends BoxNode {

    /**
     * String that keeps track of the modifier for this box
     */
    protected String modifier;

    protected boolean useDefinedType;
    protected boolean outputDefinedType;

    protected boolean typeValid;

    /**
     * Constructs a new PrimitiveBoxNode object.
     *
     * @param boxId The id to assign.
     * @param m The model that contains this box node.
     */
    public PrimitiveBoxNode(int boxId, int parentId, String modifier,
			    Model m, boolean useDefinedType,
			    boolean outputDefinedType) {
        super(boxId, parentId, m);
        setModifier(modifier);

	this.useDefinedType = useDefinedType;
	this.outputDefinedType = outputDefinedType;

        this.typeValid = false;
    }
    
    public String getToolTip(Point at)
    {

	//If the cursor is over one of the ports this returns the
	//Type of the port and the arc Ids connected to it
	GenericBoxNodePort port = getPortOver (at) ;
	if (port != null) {
	    String toolTip = "";
	    CompositeType type = port.getType();
	    if (type != null) {
		toolTip = "TYPE: " + type.toStringInDetail();
	    }
	    Vector arcs = port.getArcs();
	    if (arcs != null && arcs.size() != 0) {
		String arcIds = "ARC ID: " ;
		for (int i = 0; i < arcs.size() ; i++) {
		    arcIds += ((GenericArc) arcs.get (i)).getArcId();
		    if (i + 1 < arcs.size()) {
			arcIds += ",";
		    }
		}
		toolTip += arcIds;
	    }
	    if (toolTip == "") {
		return null;
	    }
	    else {
		return toolTip;
	    }
	}

	if(getModifier() == null || getModifier().equals("")) {
	    return "undefined modifier";
	}




	// this should probably return the description but for now returning the modifier
	// seems infinitely more useful.

	return getModifier();
    }

   

    /**
     * Saves the common attributes of all primitive box nodes.  Currently this saves the
     * node_component attributes.
     *
     * @param boxTable The sleepycat database table containing generic box node
     * fields.
     * @param type The operator type of the box to save into the table.
     */
    public IntegerArrayDbt saveRecord(Db boxTable) throws Throwable
    {
        IntegerArrayDbt key = new IntegerArrayDbt(new int[] {boxId, parentId});
        Rectangle r = getComponent().getBounds();

        BoxRecord boxRecord = new BoxRecord(boxId,
                                            getBoxType(),
                                            getLabel(),
                                            getDescription(),
                                            getModifier(),
                                            parentId,
                                            (float)0.0, // cost
                                            (float)0.0, //  selectivity,
					    useDefinedType,
					    outputDefinedType,
                                            r.x, r.y,
                                            r.width, r.height);
        if (boxTable.put(null, key, boxRecord, 0) == Db.DB_KEYEXIST) {
            System.out.println("BoxNode.saveRecord() - The key already exists.");
        }

        return key;
    }


    /**
     * Returns the name of this box type.
     *
     * @return The name of this box type as a string.
     */
    public abstract String getBoxTypeName();

    /**
     * Opens up a properties dialog box.
     *
     */
    public abstract void showProperties();


    /**
     * Returns a string containing the box modifier
     *
     * @return The box modifier
     */
    public String getModifier () {
	return modifier;
    }

    /**
     * Sets the modifier for this box
     */
    public void setModifier (String modifier) {
	if(modifier == null || modifier.equals("")) {
	    setColor(Constants.NOMODIFIERBOX_COLOR);
	} else {
	    setColor(Constants.UNARYBOX_COLOR);
	}
	this.modifier = modifier;
    }

    public String toString()
    {
        return
	    super.toString() +
	    " useDefinedTypes=" + useDefinedType +
	    " outputDefinedType=" + outputDefinedType +
	    " modifier=" + getModifier();
    }


    /**
     * An input handler for mouse clicks.  A double click allows the
     * user to rename the box node.
     *
     * @param evt The mouse event that invoked this method.
     */
    public boolean handleMouseClick(MouseEvent evt)
    {

        if(evt.getClickCount() == 2) {
            if(node_component instanceof BoxComponent) {
                BoxComponent boxComponent = (BoxComponent)node_component;
                RenamableLabel label = boxComponent.getLabel();
                label.handleRename();
                return true;
            }
        }
        return false;
    }

    /**
     * Sets a boolean indicating whether this node is using a defined type or
     * inferring types.  If switching from defined types to inferred types
     * the current types must also be cleared out
     *
     * @param useDefinedType the boolean to set.
     */
    public void setUseDefinedTypes(boolean useDefinedType)
    {
	if (this.useDefinedType == true && useDefinedType == false) {
	    this.useDefinedType = false;
	    clearInferredInputTypes();
	}
	else {
	    this.useDefinedType = useDefinedType;
	}
    }

    /**
     *  Returns a boolean indicating whether this node is using a defined type or
     *  inferring types.
     *
     * @return a boolean indicating whether this node is using a defined type or
     * inferring types.
     */
    public boolean isUseDefinedTypes()
    {
	return useDefinedType;
    }

    /**
     * Sets a boolean indicating whether this node is outputing a defined
     * type or outputting a a user-defined type.
     *
     * @return a boolean indicating whether this node is using a defined
     * type as its output or whether the user is defining the output type
     * attribute names.
     */
    public void setOutputDefinedTypes(boolean outputDefinedType)
    {
	this.outputDefinedType = outputDefinedType;
    }

    /**
     * Returns a boolean indicating whether this node is outputing a defined
     * type or not.
     *
     * @boolean a boolean indicating whether this node is outputing a defined
     * type or not.
     */
    public boolean isOutputDefinedTypes()
    {
	return outputDefinedType;
    }

    /**
     * Returns true if the modifier is empty, false otherwise;
     **/
    public boolean isModifierEmpty()
    {
        if (getModifier() == null) return true;
        return getModifier().equals("");
    }

    /**
     * Returns the output type of the box node.  If the output type is undefined, then
     * this method returns null.  If two outputports of this box have different types,
     * this method also returns null.
     */

    public Type getOutputType()
    {
        BoxComponent boxComponent = (BoxComponent) node_component;
        Vector outputPorts = boxComponent.getOutputPorts();

        Type outputType = null;

        for(int i=0; i<outputPorts.size(); i++) {
            BoxNodeOutputPort outputPort = (BoxNodeOutputPort) outputPorts.elementAt(i);
            if(outputType == null) {
                outputType = outputPort.getType();
            } else {
                Type otherOutputType = outputPort.getType();
                if(!outputType.isCompatible(otherOutputType)) {
                    Utils.addErrorsText("Error!  node " + toString() +
                                        " has two different output types!");
                    return null;
                }
            }
        }

        return outputType;
    }

    /**
     * Sets the output type of this box.  The type will be inferred from the Parser
     * and the modifier of this box.
     */
    public Type inferOutputType()
    {
        BoxComponent boxComponent = (BoxComponent) node_component;
        Vector outputPorts = boxComponent.getOutputPorts();

        CompositeType outputType = null;
        try {
	    RootFrame.parser.parse(false, this);
	    outputType = RootFrame.parser.getInferredType();
        } catch (Exception e) {
//          e.printStackTrace();
          outputType = null;
        }

	if(outputType != null && outputType.getIsInferred()) {
	    if(RootFrame.typeManager.getInferredType(outputType) == null) {
		try {

		    RootFrame.typeManager.addCompositeType(outputType);
		} catch (Exception e) {
		    System.out.println("Fatal warning #34Df34, report this to Adam Ray Singer.");
		}
	    } else {
		outputType = RootFrame.typeManager.getInferredType(outputType);
	    }
	}

        for(int i=0; i<outputPorts.size(); i++) {
            BoxNodeOutputPort outputPort = (BoxNodeOutputPort) outputPorts.elementAt(i);
            outputPort.setType(outputType);
        }


        return outputType;
    }

    public void setInputType(Type type, int idx)
    {
	if(type != null) {
	    if(type instanceof CompositeType) {
		// System.out.println("\t Setting " + getLabel() + "'s type to " +
		//		   ((CompositeType)type).getName() + " for idx = " + idx);
	    }
	}
        BoxComponent boxComponent = (BoxComponent) node_component;
        BoxNodeInputPort inputPort = boxComponent.getInputPort(idx);

        if(inputPort != null && type != null && type instanceof CompositeType) {
            inputPort.setType((CompositeType) type);
        }
    }

    /**
     * Sets the types of all input ports to null only if this box is
     * inferring types.  If this box is not inferring types, does nothing.
     */
    public void clearInferredInputTypes()
    {
        if(!isUseDefinedTypes()) {
            BoxComponent boxComponent = (BoxComponent) node_component;
            for(int i=0; i<getInputsSize(); i++) {
                BoxNodeInputPort inputPort = boxComponent.getInputPort(i);
                inputPort.setType(null);
            }
        }
    }


    /**
     * Gets the input port type at the specified port index.
     *
     * @param idx The index of the input port from which to retrieve the input type.
     * @return The type specified by that input port type.
     */
    public Type getInputType(int idx)
    {
        BoxComponent boxComponent = (BoxComponent) node_component;
        BoxNodeInputPort inputPort = boxComponent.getInputPort(idx);

        if(inputPort != null) {
            return inputPort.getType();
        } else {
            return null;
        }
    }

    /**
     * Returns the number of input ports on this node.
     */
    public int getInputsSize()
    {
        BoxComponent boxComponent = (BoxComponent) node_component;
        return boxComponent.getNumInputs();
    }

    public boolean typeValid()
    {
        return typeValid;
    }

    public void setTypeValid(boolean typeValid)
    {
        this.typeValid = typeValid;
    }


    /**
     * Returns true if the input ports have been defined, false otherwise.
     */
    public boolean inputPortsDefined()
    {
	BoxComponent c = (BoxComponent) node_component;
	boolean inputPortsDefined = true;
        Vector inputPorts = c.getInputPorts();
	for(int i=0; i<inputPorts.size(); i++) {
	    BoxNodeInputPort inputPort = (BoxNodeInputPort) inputPorts.elementAt(i);
	    if(inputPort.getType() == null) {
		inputPortsDefined = false;
	    }
	}
	return inputPortsDefined;
    }




}
