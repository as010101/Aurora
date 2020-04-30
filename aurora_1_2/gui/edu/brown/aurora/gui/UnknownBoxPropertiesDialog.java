
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for unknown boxes.  It constructs
 * the unknown box-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class UnknownBoxPropertiesDialog extends UnaryBoxPropertiesDialog
{
  /**
   * The text area to edit box type id
   */
  JTextField boxTypeEditingArea;

  /**
   * The text area for predicate editing
   */
  JTextField predicateEditingArea;

    /**
     * The JPanel that contains the output attribute functions
     */
    JPanel textAreaPanel;

    /**
     * Constructs a new UnknownBoxPropertiesDialog object.
     *
     * @param tm The typemanager contained in the GUI.
     * @param node The box node that this properties dialog is to be opened for.
     */
    public UnknownBoxPropertiesDialog(UnknownBoxNode node)
    {
        super(node);
        setTitle("Unknown Box Properties Dialog");
    }

    /**
     * Returns that this box does need to specify output types
     * @return this box does need to specify output types
     */

    public boolean specifyOutput () {
	return true;
    }

    /**
     * Sets up the modifier panel for the tabbed pane.
     */
    public void setupModifier()
    {
        super.setupModifier();

	UnknownBoxNode unknownBoxNode = (UnknownBoxNode)node;

	GridBagLayout bags = new GridBagLayout();

	//creates a JList with the items in the names array
        modifierPanel = new JPanel(bags);

        boxTypeEditingArea = new JTextField();

	addComponent (bags, modifierPanel, new JLabel ("Box Type:"), 0,0, 1,1, 1,1);
	addComponent (bags, modifierPanel, boxTypeEditingArea, 1,0, 1,1, 1,1);

        predicateEditingArea = new JTextField();

        addComponent (bags, modifierPanel, new JLabel ("Predicate :"), 0,1, 1,1, 1,1);
        addComponent (bags, modifierPanel, predicateEditingArea, 1,1, 1,1, 1,1);

	//tokenizes the modifiers String
	String modifier = new String();
	try {
            boxTypeEditingArea.setText(""+unknownBoxNode.getBoxType());
            Parser p = new Parser();
            p.parse(false, (BoxNode)node);
            if (Constants.VERBOSE) System.out.println(p.getInferredType().toStringInDetail());
            modifier = p.getTransformedString();
            predicateEditingArea.setText(modifier);
        } catch (Exception e) {e.printStackTrace();}

	//adds all the components to the PredicatePanel
        tabbedPane.addTab(MODIFIERTABTITLE, modifierPanel);
    }

    /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     */
    protected boolean set()
    {
        String modifier = "";
        if(isModifiersTabSetup()) {
          modifier = predicateEditingArea.getText();
        }
        if(!super.set() || !super.parse(modifier)) {
          System.out.println(super.set());
            return false;
        }
        try {
          ((UnknownBoxNode)node).setBoxTypeId(Integer.parseInt(boxTypeEditingArea.getText()));
        }
        catch (Exception e) {
          System.out.println("Invalid Box Type");
        }
        return true;
    }

}


