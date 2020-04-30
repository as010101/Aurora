
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.awt.*;
import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import java.util.*;


/**
 * A properties dialog for the port nodes.
 * 
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class PortNodePropertiesDialog 
    extends GenericNodePropertiesDialog implements ActionListener
{
    /**
     * The type list.
     */
    protected JList typeList = null;

    protected boolean useDefinedType;

    /**
     * Constructs a new PortPropertiesDialog object.
     *
     * @param node The GenericPortNode associated with this 
     * properties dialog.
     */
    public PortNodePropertiesDialog(GenericPortNode node)
    {
        super(node);
        
        // Query tab section
        show();
    }

    /**
     * Internal constructor for subclasses who don't want the dialog to
     * show right away
     *
     * @param node The GenericPortNode associated with this
     * @param show whether to show or not
     */
    protected PortNodePropertiesDialog(GenericPortNode node, boolean show)
    {
        super(node);

        if (show) show();
    }
    

    /**
     * Sets the type of this node. Always succeeds.
     * @return boolean
     */
    protected boolean set()
    {
        boolean returnValue = true;

	// ugly...
        return returnValue;
    }

    /**
     * Performs the appropriate tasks depending on the event passed in for the dialog.
     *
     * @param e The event that triggered the call of this method.
     */    
    public void actionPerformed(ActionEvent e) {
        if(e.getActionCommand().equals("Apply")) {
            set();
        }
        if(e.getActionCommand().equals("OK")) {
            if (set())
                dispose();
        }
        if(e.getActionCommand().equals("Cancel")) {
            dispose();
        }
    }
}
