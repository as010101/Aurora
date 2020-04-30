package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * An abstract superclass of a properties dialog for primitive boxes.  It constructs 
 * the visual dialog box along with one tab for labels and descriptions.  It also 
 * constructs the tab panels that describe port functionality.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class UnaryBoxPropertiesDialog 
    extends PrimitiveBoxPropertiesDialog implements ActionListener
{
    /**
     * Constructs a new UnaryBoxPropertiesDialog object.
     *
     * @param node The box node that this properties dialog is to be opened for.
     */ 
    public UnaryBoxPropertiesDialog(UnaryBoxNode node)
    {
        super(node);
        setTitle("Unary Box Properties Dialog");
    }
}
