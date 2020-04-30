/*
 * @(#)  BinaryBoxNode.java	1.0 05/15/2002
 *
 * Authors Jeong-Hyon Hwang and Robin Yan (jhhwangcs.brown.edu, rly@cs.brown.edu)
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * An abstract class node for which all primitive boxes with two input ports and
 * one output port subclass.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class BinaryBoxNode extends PrimitiveBoxNode {

    /**
     * Constructs a new BinaryBoxNode object.
     *
     * @param boxId The unique int identifier for this box object.
     * @param parentId The identifier of the parent box of this box object.
     * @param modifier The modifier for this box node.
     * @param m The model that contains this binary box node object.
     * @param useDefinedType A boolean indicating whether this binary node is
     * using defined types or inferring types.
     */
    public BinaryBoxNode(int boxId, int parentId, String modifier,
			 Model m, boolean useDefinedType, boolean outputDefinedType) {
        super(boxId, parentId, modifier, m, useDefinedType, outputDefinedType);
        // setColor(Constants.BINARYBOX_COLOR);
        BoxComponent box = (BoxComponent)node_component;
        box.addInputPort();
        box.addInputPort();
        box.addOutputPort();
    }

    /**
     * Returns the first input port of this BinaryBoxNode object.
     *
     * @return The first InputPortNode of this BinaryBoxNode object.
     */
    public BoxNodeInputPort getLeftInputPort()
    {
        BoxComponent c = (BoxComponent)getComponent();
        return c.getInputPort(0);
    }

    /**
     * Returns the second input port of this BinaryBoxNode object.
     *
     * @return The second InputPortNode of this BinaryBoxNode object.
     */
    public BoxNodeInputPort getRightInputPort()
    {
        BoxComponent c = (BoxComponent)getComponent();
        return c.getInputPort(1);
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
