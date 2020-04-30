
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * An abstract superclass of a properties dialog for primitive boxes.  It constructs
 * the visual dialog box along with one tab for labels and descriptions.  It also
 * constructs the tab panels that describe port functionality.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class PrimitiveBoxPropertiesDialog
    extends BoxPropertiesDialog implements ActionListener
{
    protected final String NULL_ATTR = "<null>";


    /**
     * The attribute index combo box.
     */
    JComboBox attributeIndexCombo = null;

    /**
     * The left attribute index combo box.
     */
    JComboBox leftAttributeIndexCombo = null;

    /**
     * The right attribute index combo box.
     */
    JComboBox rightAttributeIndexCombo = null;

    /**
     * The operator attribute index combo box.
     */
    JComboBox operatorIndexCombo = null;

    /**
     * The value textfield.
     */
    JTextField valueText = null;

    /**
     * The timeout textfield.
     */
    JTextField timeOutText = null;

    /**
     * The time window textfield.
     */
    JTextField timeWindowText = null;

    /**
     * Boolean that keeps track of whether or not the box has to specify an output type
     */
    boolean specifyOutputType;

    JPanel modifierPanel;

    public static final String MODIFIERTABTITLE = "Modifier";

    /**
     * Constructs a new PrimitiveBoxPropertiesDialog object.
     *
     * @param node The PrimitiveBoxNode associated with this dialog.
     */
    public PrimitiveBoxPropertiesDialog(PrimitiveBoxNode node)
    {
        super(node);

        // Query tab section

        setupAssignments();
        setupEmissionPolicy();
        setupTimeWindow();

        modifierTabHandler();

        show();
    }

    /**
     * Sets up the modifier panel for the tabbed pane.
     */
    public void setupModifier()
    {

    }

    /**
     * Removes the modifier panel from the tabbed pane.
     */
    public void removeModifier()
    {
	if(modifierPanel != null) {
            PrimitiveBoxNode primBox = (PrimitiveBoxNode) node;
            primBox.setModifier("");
	    tabbedPane.remove(modifierPanel);
	}
    }


    /**
     * Sets up the assignments panel for the tabbed pane.
     */
    public void setupAssignments()
    {
        if (node instanceof AssignmentsSpecifier) {
            GridBagLayout bags = new GridBagLayout();
            JPanel assignmentsPanel = new JPanel(bags);
            tabbedPane.addTab("Assignments", assignmentsPanel);
        }
    }

    /**
     * Sets up the emission policy panel for the tabbed pane.
     */
    public void setupEmissionPolicy()
    {
        if (node instanceof EmissionPolicySpecifier) {
            GridBagLayout bags = new GridBagLayout();
            JPanel emissionPolicyPanel = new JPanel(bags);

            JLabel label = new JLabel("Time Out:");
            addComponent(bags, emissionPolicyPanel, label, 0, 1, 1,1, 1,1);

            timeOutText = new JTextField();
            timeOutText.setText("" +
                                ((EmissionPolicySpecifier)node).getTimeOut());
            addComponent(bags, emissionPolicyPanel, timeOutText,
                         1, 1, 1, 1, 3, 1);

            tabbedPane.addTab("Emission Policy", emissionPolicyPanel);
        }
    }

    /**
     * Sets up the timewindow panel for the tabbed pane.
     */
    public void setupTimeWindow()
    {
        if (node instanceof TimeWindowSpecifier) {
            GridBagLayout bags = new GridBagLayout();
            JPanel timeWindowPanel = new JPanel(bags);

            JLabel label = new JLabel("Time Window:");
            addComponent(bags, timeWindowPanel, label, 0, 1, 1, 1, 1, 0.1);

            timeWindowText = new JTextField();
            timeWindowText.setText(""+ ((TimeWindowSpecifier)node).getTimeWindow());
            addComponent(bags, timeWindowPanel, timeWindowText, 1, 1, 1, 1, 1, 0.1);

            tabbedPane.addTab("Time Window", timeWindowPanel);
        }
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
            if (set()) {
                dispose();
            }
        }
        if(e.getActionCommand().equals("Cancel")) {
            dispose();
        }
    }

    public boolean isModifiersTabSetup()
    {
        return tabbedPane.indexOfTab(MODIFIERTABTITLE) != -1;
    }

    //used when poping up a new properties dialog.  Basically just 
    //checks a few conditions to see whether or not the modifiers
    //tab should appear yet
    public boolean modifierTabHandler()
    {
        PrimitiveBoxNode primBox = (PrimitiveBoxNode) node;
	/*
	  System.out.println("\nINSIDE MODIFIERTABHANDLER: \n" +
	       "isModifiersTabSetup() = " + isModifiersTabSetup() +
	       "\ninputPortsDefined() = " + primBox.inputPortsDefined() +
	       "\nisUseDefinedTypes() = " + primBox.isUseDefinedTypes() + 
	       "\nisModifierEmpty() = " + primBox.isModifierEmpty());
	*/
        if(isModifiersTabSetup()) {
            if(!primBox.inputPortsDefined()) {
	
		if (!primBox.isUseDefinedTypes()) {
		    removeModifier();
		}
		
	
            }
        } else {
            if(primBox.inputPortsDefined()) {
                setupModifier();
            } else {
                if(!primBox.isModifierEmpty() && !primBox.isUseDefinedTypes()) {
                    setupModifier();
                }
            }
        }
	

        if(!primBox.typeValid()) {
            TypeInferencer.typeCheck();
            return false;
        }
        return true;
    }


    public String[] getOutputTypeAttributeNames()
    {
        String[] names = new String[0];
        PrimitiveBoxNode primBox = (PrimitiveBoxNode) node;

	Type outputType = primBox.getOutputType();
        if(outputType != null && outputType instanceof CompositeType) {
            CompositeType oc = (CompositeType) outputType;
            names = oc.getAttributeNames();
        }

        return names;
    }

    public String[] getInputTypeAttributeNames()
    {
        String[] names = new String[0];
        PrimitiveBoxNode primBox = (PrimitiveBoxNode) node;
        String s = primBox.getModifier();

        Vector allAttrNames = new Vector();

        for(int i=0; i<primBox.getInputsSize(); i++) {
            Type currentType = primBox.getInputType(i);
            if(currentType != null && currentType instanceof CompositeType) {
                CompositeType c = (CompositeType)currentType;
                String[] typeNames = c.getAttributeNames();
		if(typeNames == null) {
		    return null;
		}
                for(int j=0; j<typeNames.length; j++) {
                    allAttrNames.addElement(typeNames[j]);
                }
            }

        }

        if(allAttrNames.size() > 0) {
            names = new String[allAttrNames.size()];
            for(int i=0; i<allAttrNames.size(); i++) {
                names[i] = (String) allAttrNames.elementAt(i);
            }
        }


        return names;
    }

    public String getParsedModifier()
    {
        String s = "";
        try {
            PrimitiveBoxNode primBox = (PrimitiveBoxNode) node;
            Parser p = RootFrame.parser;
            p.parse(false, primBox);
            s = p.getTransformedString();
            if (Constants.VERBOSE)
                System.out.println(p.getInferredType().toStringInDetail());
        } catch (Exception e) {
            s = "";
        }
        return s;
    }


    public void resetModifier()
    {
        if(isModifiersTabSetup()) {
            PrimitiveBoxNode primBox = (PrimitiveBoxNode) node;
            primBox.setModifier("");
            removeModifier();
            setupModifier();
        }
    }

    protected boolean parse(String modifierText)
    {
        PrimitiveBoxNode primBox  = (PrimitiveBoxNode)node;
	if(isModifiersTabSetup()) {
            if(modifierText.equals("")) {
                primBox.setModifier("");
            } else {
                try {
                    Parser p = RootFrame.parser;
                    p.parse(true, (BoxNode)node, modifierText);
                    if(Constants.VERBOSE) {
                        System.out.println(p.getInferredType().toStringInDetail());
                    }
                    primBox.setModifier(p.getTransformedString());
                } catch (Exception e) {
                    JOptionPane.showConfirmDialog(null,
                                                  "Invalid Predicate: " + e,
                                                  "Exception" ,
                                                  JOptionPane.DEFAULT_OPTION,
                                                  JOptionPane.ERROR_MESSAGE);
                    return false;
                }
            }
	}

        TypeInferencer.typeCheck();

        return true;
    }

    /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     */
    protected boolean set()
    {
        PrimitiveBoxNode primBox = (PrimitiveBoxNode)node;

        boolean returnValue = true;
        primBox.setDescription(descriptionText.getText());
        primBox.setLabel(labelText.getText());
	primBox.setUseDefinedTypes(useInputDefinedType);
	primBox.setOutputDefinedTypes(isOutputNamed);


        if(!modifierTabHandler()) {
            return false;
        }

       if (node instanceof AssignmentsSpecifier) {
            /*
            try {
                AssignmentsSpecifier assNode = (AssignmentsSpecifier)node;
                assNode.setPredicateString(predicateText.getText());
            } catch (Exception e) {
    	       	JOptionPane.showConfirmDialog(null,
                                              e,
                                              "Exception" ,
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);

                returnValue = false;
            }
            */
        }
        if (node instanceof EmissionPolicySpecifier) {
            try {
                EmissionPolicySpecifier emNode = (EmissionPolicySpecifier)node;
                emNode.setTimeOut(Integer.parseInt(timeOutText.getText()));
            } catch (Exception e) {
    	       	JOptionPane.showConfirmDialog(null,
                                              e,
                                              "Exception" ,
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
                returnValue = false;
            }
        }
        if (node instanceof TimeWindowSpecifier) {
            try {
                TimeWindowSpecifier timeNode = (TimeWindowSpecifier)node;
                timeNode.setTimeWindow(Integer.parseInt(timeWindowText.getText()));
            } catch (Exception e) {
                JOptionPane.showConfirmDialog(null,
                                              e,
                                              "Exception" ,
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
                returnValue = false;
            }
        }

        return returnValue;
    }
}
