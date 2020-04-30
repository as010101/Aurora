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
 * OutputPortNode implements the output port on the workspace of the GUI.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class OutputPortNode extends GenericPortNode
{
     /**
     * The superbox's input port node associated with this OutputPortNode.
     */
    protected BoxNodeInputPort boxNodeInputPort;

    protected QosSpecifiers qosSpecifiers;

    protected String qos;

    public static String EMPTYQOSSTRING = "1:3|0|0~1|1~1|2~0";

    /**
     * Constructs a new OutputPortNode object.
     *
     * @param boxId The unique int identifier of this OutputPortNode.
     * @param m The containing model of that this OutputPortNode belongs to.
     * @param index The index of this OutputPortNode within its container.
     * @param boxNodeInputPort The boxNodeInputPort of the containing superbox associated
     * with this InputWorkspacePortNode.
     */
    public OutputPortNode(int boxId, Model m, int parentId,
                          int index, BoxNodeInputPort boxNodeInputPort,
                          String qos)
    {
        super(boxId, m, parentId);
        setIndex(index);
        this.boxNodeInputPort = boxNodeInputPort;
        this.qos = qos;
        try {
            qosSpecifiers = new QosSpecifiers(qos);
        } catch(QosSpecifierException e) {
            JOptionPane.showConfirmDialog(null,
                                          e,
                                          "Exception" ,
                                          JOptionPane.DEFAULT_OPTION,
                                          JOptionPane.ERROR_MESSAGE);
            e.printStackTrace();
        }
	setColor();
    }

    /**
     * Returns true if this node gets its box Id set in the lower section of the box id
     * range.
     */
    public boolean isInLowerBoxIds()
    {
        return false;
    }


    public void setColor()
    {
	if(boxNodeInputPort.getArcs().size() == 0) {
	    lbl.setBackground(Constants.NOMODIFIERBOX_COLOR);
	    lbl.setForeground(Constants.NOMODIFIERPORTNODE_FONT_COLOR);
	} else {
	    GenericArc arc = (GenericArc) boxNodeInputPort.getArcs().elementAt(0);
	    BoxNodeOutputPort outputPort = arc.getOutputPort();
	    if(outputPort.getType() == null) {
		lbl.setBackground(Constants.NOMODIFIERBOX_COLOR);
		lbl.setForeground(Constants.NOMODIFIERPORTNODE_FONT_COLOR);
	    } else {
		lbl.setBackground(Constants.PORTNODE_COLOR);
		lbl.setForeground(Constants.PORTNODE_FONT_COLOR);
	    }
	}
    }







    /**
     * Gets the modifier for an outputportnode
     */
    public String getModifier()
    {
        String modifier =
            getTypeId() + Constants.DELIM +
            qosSpecifiers.toString();
        return modifier;
    }

    public QosSpecifiers getQosSpecifiers()
    {
        return qosSpecifiers;
    }

    public void setQosSpecifiers(QosSpecifiers qosSpecifiers)
    {
        this.qosSpecifiers = qosSpecifiers;
    }

    /**
     * Returns whether this port uses defined types.  Output port nodes *never* specify
     * their incoming tuple types.
     */
    public boolean isUseDefinedTypes()
    {
	return false;
    }



    /**
     * Opens up the properties dialog box for this GenericPortNode  object.
     *
     */
    public void showProperties()
    {
        new OutputPortNodePropertiesDialog(this);
    }

    /**
     * Returns an OutputPortNode object that is loaded using the information from
     * a database table in sleepycat.
     *
     * @param record the record containing the generic port node information to load.
     * @param sourceStreamTable the table containing the output port node-specific
     * source stream information.
     * @param key
     * @model The model that contains this output port node.
     * @return A new output port node object corresponding to the information in
     * the database.
     */
    public static OutputPortNode loadRecord(BoxRecord record,
                                            Model model) throws Throwable
    {
        StringTokenizer tokens = new StringTokenizer(record.getModifier(), Constants.DELIM);

        String typeIdToken = tokens.nextToken();
        int typeId = Integer.parseInt(typeIdToken);
        String qos = record.getModifier().substring(typeIdToken.length()+Constants.DELIM.length());

        BoxNodeInputPort boxNodeInputPort = new BoxNodeInputPort(PetalPort.WEST,
                                                                 0,
                                                                 null);
        OutputPortNode node = new OutputPortNode(record.getBoxId(),
                                                 model,
                                                 record.getParentId(),
                                                 model.getNumberOfOutputPorts(),
                                                 boxNodeInputPort,
                                                 qos);


        boxNodeInputPort.setGenericNode(node);

        int index = typeId;
        if (index >= 0) {
            try {
                CompositeType types = RootFrame.typeManager.getAllCompositeTypes()[index];
                node.setType(types);
            } catch (Exception e) {
                System.out.println(e.toString());
                e.printStackTrace();
            }
        }
        return node;
    }

    /**
     * Returns an OutputPortNode object that is loaded using the information from
     * a database table in sleepycat.
     *
     * @param record the record containing the generic port node information to load.
     * @param sourceStreamTable the table containing the output port node-specific
     * source stream information.
     * @param key
     * @model The model that contains this output port node.
     * @return A new output port node object corresponding to the information in
     * the database.
     */
    public static OutputPortNode loadRecord(BoxPaletteRecord record,
                                            Model model) throws Throwable
    {
        int boxId = setNextUpperIdCounter();

        int typeId = -1;
        String qos = "";

        if(!record.getModifier().equals("")) {
            StringTokenizer tokens = new StringTokenizer(record.getModifier(), Constants.DELIM);

            String typeIdToken = tokens.nextToken();
            typeId = Integer.parseInt(typeIdToken);
            qos = record.getModifier().substring(typeIdToken.length()+Constants.DELIM.length());

        }

        BoxNodeInputPort boxNodeInputPort = new BoxNodeInputPort(PetalPort.WEST,
                                                                 0,
                                                                 null);
        OutputPortNode node = new OutputPortNode(boxId,
                                                 model,
                                                 record.getParentId(),
                                                 model.getNumberOfOutputPorts(),
                                                 boxNodeInputPort,
                                                 qos);


        boxNodeInputPort.setGenericNode(node);

        int index = typeId;
        if (index >= 0) {
            try {
                CompositeType types = RootFrame.typeManager.getAllCompositeTypes()[index];
                node.setType(types);
            } catch (Exception e) {
                System.out.println(e.toString());
                e.printStackTrace();
            }
        }
        return node;
    }



    /**
     * Returns a String representation of this OutputPortNode.
     *
     * @return a String representation of this OutputPortNode.
     */
    public String toString()
    {
        return
            "OutputPortNode : " +
            " boxId=" + boxId +
            ", parent=" + parentId +
            ", typeId=" + getTypeId() +
            ", modifier=" + getModifier();
    }


    /**
     * Sets the index of the this OutputPortNode, as well as the title
     * accordingly.
     *
     * @param index The index to set this OutputPortNode to.
     */
    public void setIndex(int index)
    {
        super.setIndex(index);
        setLabel("o"+index);
    }

    /**
     * Returns the index of this outputport node.
     *
     * @return the index of this outputport node in relation to its superbox.
     */
    public int getIndex()
    {
	return index;
    }

    /**
     * Sets the boxNodeInputPort of the containing Superclass for this
     * InputWorkspacePortNode.
     *
     * @param ipt The boxnodeinputport.
     */
    public void setInputPortNode(BoxNodeInputPort opt)
    {
        boxNodeInputPort = opt;
    }

    /**
     * Returns the boxNodeInputPort associated with this OutputPortNode.
     *
     * @return The boxNodeInputPort associated with this OutputPortNode.
     */
    public BoxNodeInputPort getInputPortNode()
    {
        return boxNodeInputPort;
    }

    /**
     * Returns the type defined in this OutputPortNode.
     *
     * @return the type defined in this OutputPortNode.
     */
    public CompositeType getType()
    {
        return boxNodeInputPort.getType();
    }

    /**
     * Gets the tooltip associated with an OutputPortNode
     *
     * @return the tooltip associated with an OutputPortNode
     */
   public String getToolTip(Point at) 
    {
	if(getType() == null) {
	    return "Undefined output type";
	}
	return getType().toStringInDetail();
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.OUTPUTPORT;
    }


    /**
     * Sets the type of this OutputPortNode.
     *
     * @param type The type to set this OutputPortNode to.
     */
    public void setType(CompositeType type)
    {
        boxNodeInputPort.setType(type);
	SuperBoxNode parentSuperBox = model.getSuperBox();
	Vector outputPorts = parentSuperBox.getOutputPorts();
	if(getIndex() < outputPorts.size() && getIndex() != -1) {
	    BoxNodeOutputPort boxNodeOutputPort = (BoxNodeOutputPort) outputPorts.elementAt(getIndex());
	    boxNodeOutputPort.setType(type);
	}
    }

    /**
     * Returns the Type Id
     *
     * @return The Type id
     */
    public int getTypeId()
    {
        return RootFrame.typeManager.findCompositeTypeIndex(getType());
    }

    /**
     * Returns the cardinality of this OutputPortNode.
     *
     * @return The cardinality of this OutputPortNode.
     */
    public int getCardinality()
    {
        return boxNodeInputPort.getCardinality();
    }


    public String getQos()
    {
        return qos;
    }
}
