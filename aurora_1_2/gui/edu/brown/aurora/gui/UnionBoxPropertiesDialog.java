
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for union boxes.  It constructs 
 * the unionbox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class UnionBoxPropertiesDialog extends UnaryBoxPropertiesDialog
{
    /**
     * Constructs a new UnionBoxPropertiesDialog object.
     *
     * @param node The box node that this properties dialog is to be opened for.
     */ 
    public UnionBoxPropertiesDialog(UnionBoxNode node) 
    {
        super(node);
        setTitle("Union Box Properties Dialog");
    }

    /** 
     * Returns that this box does not need to specify output types
     * @return this box does not need to specify output types
     */
    public boolean specifyOutput () {
	return false;
    }

    /**
     * Sets up the modifier panel for the tabbed pane.
     * Not yet implemented...
     */
    public void setupModifier()
    {
        UnionBoxNode unionnode = (UnionBoxNode)node;
        
    }

    /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     * Not yet Implemented...
     */
    protected boolean set()
    {
        if(!super.set()) {
            return false;
        }

        UnionBoxNode unionnode = (UnionBoxNode)node;

        return true;
    }
    
}
