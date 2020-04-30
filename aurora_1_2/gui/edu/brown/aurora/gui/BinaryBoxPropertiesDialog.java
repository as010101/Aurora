package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * Implements a properties dialog for all binary boxes in general.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class BinaryBoxPropertiesDialog 
    extends PrimitiveBoxPropertiesDialog implements ActionListener
{

    /**
     * Constructs a new BinaryBoxPropertiesDialog object
     *
     * @param node The BinaryBoxNode that this properties dialog should be opened up for.
     */
    public BinaryBoxPropertiesDialog(BinaryBoxNode node)
    {
        super(node);
        setTitle("Binary Box Properties Dialog");
    }
}
