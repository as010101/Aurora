package edu.brown.aurora.gui;

import edu.brown.aurora.gui.types.*;
import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;


/**
 * The superclass of all Unary box nodes that have defined operators. 
 * Unary boxes contain one input port and one output port.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class UnaryBoxNode extends PrimitiveBoxNode {

    /**
     * Constructs a new UnaryBoxNode object.  
     *
     * @param boxId The id to assign.
     * @param parentId The id of the parent box of this unarybox node.
     * @param modifier the modifier for this unary box node.
     * @param m The model that contains this box node.
     * @param useDefinedTypes a boolean indicating whether this unaryboxnode is
     * using a defined type or inferring types.
     */
    public UnaryBoxNode(int boxId, int parentId, String modifier, 
			Model m, boolean useDefinedTypes, boolean outputDefinedType) {
        super(boxId, parentId, modifier,  m, useDefinedTypes, outputDefinedType);
        // setColor(Constants.UNARYBOX_COLOR);
        BoxComponent box = (BoxComponent)node_component;
        box.addInputPort();
        box.addOutputPort();
    }

    
    /**
     * Returns the input port of this unaryboxnode class.
     *
     * @return The BoxNodeInputPort.
     */
    public BoxNodeInputPort getInputPort()
    {
        BoxComponent c = (BoxComponent)getComponent();
        return c.getInputPort(0);
    }

    /**
     * Returns the input port of this unaryboxnode class.
     *
     * @return The BoxNodeInputPort.
     */
    public BoxNodeOutputPort getOutputPort()
    {
        BoxComponent c = (BoxComponent)getComponent();
        return c.getOutputPort(0);
    }

}


