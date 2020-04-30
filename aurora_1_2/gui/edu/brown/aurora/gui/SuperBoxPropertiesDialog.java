
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
/**
 * An implementation of the properties dialog for superboxes.  It constructs
 * the visual dialog box.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class SuperBoxPropertiesDialog
    extends BoxPropertiesDialog implements ActionListener
{
    /**
     * Constructs a new SuperBoxPropertiesDialog box.
     *
     * @param node the node that this dialog is opend up for.
     */
    public SuperBoxPropertiesDialog(SuperBoxNode node)
    {
        super(node);
    }

    /** 
     * Returns that this box does not need to specify output types
     * @return this box does not need to specify output types
     */
    public boolean specifyOutput () {
	return false;
    }

    /**
     * Performs the appropriate tasks depending on the event passed in for the dialog.
     *
     * @param e The event that triggered the call of this method.
     */    
    public void actionPerformed(ActionEvent e) {
        if(e.getActionCommand().equals("Apply")) {
            ((SuperBoxNode)node).setLabel(labelText.getText());
        }
        if(e.getActionCommand().equals("OK")) {
            ((SuperBoxNode)node).setLabel(labelText.getText());
            dispose();
        }
        if(e.getActionCommand().equals("Cancel")) {

            dispose();
        }
    }
}
