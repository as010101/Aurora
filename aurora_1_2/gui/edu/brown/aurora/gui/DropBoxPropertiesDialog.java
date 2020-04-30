
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for drop boxes.  It constructs
 * the dropbox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class DropBoxPropertiesDialog extends UnaryBoxPropertiesDialog
{
    /**
     * The text field that keeps track of the predicate
     */
    private JTextField dropRateField;

    /**
     * Constructs a new DropBoxPropertiesDialog object.
     *
     * @param node The box node that this properties dialog is to be opened for.
     */
    public DropBoxPropertiesDialog(DropBoxNode node)
    {
        super(node);
        setTitle("Drop Box Properties Dialog");
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
     */
    public void setupModifier()
    {
        super.setupModifier();

        DropBoxNode dropnode = (DropBoxNode)node;

        String[] names = getInputTypeAttributeNames();
        String s = getParsedModifier();

        if(names == null ||
		   (dropnode.isUseDefinedTypes() && names.length == 0)) {
            return;
        }

				dropRateField = new JTextField (s, 30);
				JPanel predicatePanel = new JPanel();
				predicatePanel.add (new JLabel ("Drop Rate:"));

				predicatePanel.add (dropRateField);
        tabbedPane.addTab(MODIFIERTABTITLE, predicatePanel);

	/*
        DropBoxNode dropnode = (DropBoxNode)node;

        BoxNodeInputPort port = dropnode.getInputPort();
        if (port == null) {
            Utils.addErrorsText("Port is null.\n");
            return;
        }

        CompositeType c = port.getType();

        if (c == null) {
            Utils.addErrorsText("Composite type is null.\n");
            return;
        }

        GridBagLayout bags = new GridBagLayout();
        JPanel queryPanel = new JPanel(bags);

        JLabel attributeIndex = new JLabel("Attribute Index:");
        addComponent(bags, queryPanel, attributeIndex, 0, 0, 1, 1, 0, 1);

        String[] names = c.getAttributeNames();
        if (names == null) {
            return;
        }

        attributeIndexCombo = new JComboBox(names);
        addComponent(bags, queryPanel, attributeIndexCombo, 1, 0, 1, 1, 1, 1);
        attributeIndexCombo.setSelectedIndex(dropnode.getLeftIndex());

        JLabel operator  = new JLabel("Operator:  ");
        addComponent(bags, queryPanel, operator, 0, 1, 1, 1, 0, 1);

        operatorIndexCombo = new JComboBox(new Object[]
            {"GT", "LT", "EQ", "GEQ", "LEQ", "NEQ"});
        addComponent(bags, queryPanel, operatorIndexCombo, 1, 1, 1, 1, 1, 1);
        operatorIndexCombo.setSelectedIndex(dropnode.getOperatorTypeId());

        JLabel value = new JLabel("Number Of Tuples:  ");
        addComponent(bags, queryPanel, value, 0, 3, 1, 1, 0, 1);
        valueText = new JTextField(dropnode.getValue());
        valueText.setEditable(true);
        addComponent(bags, queryPanel, valueText, 1, 3, 1, 1, 1, 1);

        queryPanel.setBorder(new LineBorder(Color.black, 1));

        tabbedPane.addTab("Predicate", queryPanel);
	*/
    }

    /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     */
    protected boolean set()
    {
        String modifier = "";
        if(isModifiersTabSetup()) {
            modifier = dropRateField.getText();
            System.out.println(modifier);
        }
        if(!super.set() || !super.parse(modifier)) {
            return false;
        }
        return true;
    }

}
