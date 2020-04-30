/*
 * @(#)TypeTableDlg.java	1.0 04/10/2002
 *
 * Copyright 2002 by Jeong-Hyon Hwang (jhhwang@cs.brown.edu).
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import edu.brown.aurora.gui.TypeInferencer;
import java.awt.event.*;
import java.awt.event.ActionEvent;
import java.awt.*;
import javax.swing.*;

/**
 * A TypeTableDlg has a TypeTable which represents a specification of a given CompositeType.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Matthew Hatoun (mhatoun@cs.brown.edu)
 * @version 1.1 07/08/2002
 */
public class TypeTableDlg extends javax.swing.JDialog
{



    class SymWindow extends java.awt.event.WindowAdapter
    {
	public void windowClosing(java.awt.event.WindowEvent event)
	{
	    Object object = event.getSource();
	    if (object == TypeTableDlg.this)
		jAboutDialog_windowClosing(event);
	}
    }
    
    
    class SymAction implements ActionListener
    {
	public void actionPerformed(ActionEvent event)
	{
	    Object object = event.getSource();
	    if (object == okButton) {
		if (set()) {
		    dispose();
		}
	    }
	    else if (object == applyButton) {
		set();
	    }
	    else if (object == cancelButton) {
		dispose();
	    }
	    else if (object == addAttribute) {
		typeTable.addAttribute();
	    }
	    else if (object == insertAttribute) {
		typeTable.insertAttribute();
	    }
	    else if (object == removeAttributes) {
		typeTable.removeAttributes();
	    }
	}
    }
    
    private TypeManager typeMngr;
    private TypeTable typeTable;
    private CompositeType originalType;
    private CompositeType workingType;
    private GridBagLayout gbLayout;
    private GridBagConstraints gbConstraints;    
    
    //buttons
    private JButton okButton = new JButton("OK");
    private JButton applyButton = new JButton("Apply");
    private JButton cancelButton = new JButton("Cancel");
    JButton addAttribute = new JButton("Add Attribute");
    JButton insertAttribute = new JButton("Insert Attribute");
    JButton removeAttributes = new JButton("Delete Attribute");
    
    //text field
    private JTextField nameField;

    // Used for addNotify check.
    boolean fComponentsAdjusted = false;
    
    
    /**
     * Constructs a new TypeTableDlg that is based on the given TypeManager and CompositeType
     * @param typeMngr the TypeManager
     * @param type the CompositeType
     */

    public TypeTableDlg(TypeManager typeMngr, CompositeType type)
    {
	if (type == null) return;
	this.typeMngr = typeMngr;
	originalType = type;
	workingType = originalType.deepCopy();
	   //makes a deep copy of the passed type so that changes can be canceled

	setTitle("Type Specification");
	setModal(true);
	typeTable = new TypeTable(typeMngr, this);
	typeTable.setType(workingType);
	JScrollPane typeDefinition = new JScrollPane(typeTable);
	
	gbLayout = new GridBagLayout();
	getContentPane().setLayout( gbLayout);
	
	setSize(520,310);
	setResizable(false);
	setVisible(false);


	//adds window listener
	SymWindow aSymWindow = new SymWindow();
	this.addWindowListener(aSymWindow);

	//adds action listeners to buttons
	SymAction lSymAction = new SymAction();
	okButton.addActionListener(lSymAction);
	applyButton.addActionListener(lSymAction);
	cancelButton.addActionListener(lSymAction);
	addAttribute.addActionListener(lSymAction);
	insertAttribute.addActionListener(lSymAction);
	removeAttributes.addActionListener(lSymAction);

	//creates button panel
	JPanel buttonPanel = new JPanel();
        buttonPanel.setLayout(new GridLayout(1,3,10,15));
        buttonPanel.add(applyButton);
        buttonPanel.add(okButton);
        buttonPanel.add(cancelButton);

	//create attributes button panel
	JPanel attributesButtonPanel = new JPanel();
	attributesButtonPanel.setLayout (new GridLayout (3, 1, 5, 10) );
	attributesButtonPanel.add (addAttribute);
	attributesButtonPanel.add (insertAttribute);
	attributesButtonPanel.add (removeAttributes);

	//creates name editing panel
	JPanel namePanel = new JPanel();
	JLabel nameLabel = new JLabel("Type name: ");
	nameField = new JTextField (type.getName(), 20);
	namePanel.add (nameLabel);
	namePanel.add (nameField);



	//adds name panel to content pane
	gbConstraints = new GridBagConstraints();
	gbConstraints.fill = GridBagConstraints.BOTH;
	gbConstraints.gridwidth = 2;
	gbLayout.setConstraints (namePanel, gbConstraints);
	getContentPane().add (namePanel);

	//adds typeDefinition to content pane
	gbConstraints.gridy = 1;
	gbConstraints.weightx = 1;
	gbConstraints.weighty = 1;
	gbConstraints.gridwidth = 1;
	gbLayout.setConstraints (typeDefinition, gbConstraints);
	getContentPane().add (typeDefinition);

	//adds attributesButtonPanel to content pane
	gbConstraints.gridx = 1;
	gbConstraints.fill = GridBagConstraints.NONE;
	gbConstraints.weightx = 0;
	gbConstraints.weighty = 0;
	gbLayout.setConstraints (attributesButtonPanel, gbConstraints);
	getContentPane().add (attributesButtonPanel);

	//adds buttonPanel to the content pane
	gbConstraints.gridx = 0;
	gbConstraints.gridy = 2;
	gbConstraints.gridwidth = 2;
	gbLayout.setConstraints (buttonPanel, gbConstraints);
	getContentPane().add (buttonPanel);


	setLocationRelativeTo(null);
    }

    /**
     * controls whether or not the dialog is visible 
     * @param b the boolean that sets if it is visible or not
     */
    public void setVisible(boolean b)
    {
	if (b) {
            Rectangle bounds = (getParent()).getBounds();
            Dimension size = getSize();
            setLocation(bounds.x + (bounds.width - size.width)/2,
                        bounds.y + (bounds.height - size.height)/2);
        }
	super.setVisible(b);
    }
    
    public void addNotify()
    {
	// Record the size of the window prior to calling parents addNotify.
	Dimension d = getSize();
	
	super.addNotify();
	
	if (fComponentsAdjusted)
	    return;
	// Adjust components according to the insets
	Insets insets = getInsets();
	setSize(insets.left + insets.right + d.width, insets.top + insets.bottom + d.height);
	Component components[] = getContentPane().getComponents();
	for (int i = 0; i < components.length; i++) {
            Point p = components[i].getLocation();
            p.translate(insets.left, insets.top);
            components[i].setLocation(p);
        }
	fComponentsAdjusted = true;
    }
    
    
    void jAboutDialog_windowClosing(java.awt.event.WindowEvent event)
    {
        // to do: code goes here.
        
        jAboutDialog_windowClosing_Interaction1(event);
    }
    
    void jAboutDialog_windowClosing_Interaction1(java.awt.event.WindowEvent event) {
	try {
	    // JAboutDialog Hide the JAboutDialog
	    this.setVisible(false);
	} catch (Exception e) {
	}
    }
    
    
    /**
     * called by the apply and OK buttons to make all changes in the dialog final
     */   
    private boolean set ()
    {  
	if (nameField.getText().equalsIgnoreCase ("")) {
	    JOptionPane.showConfirmDialog(null, 
					  "Type needs a type name",
					  "Error!", 
					  JOptionPane.DEFAULT_OPTION,
					  JOptionPane.ERROR_MESSAGE);
	    return false;
	} 
	else if ( typeMngr.findCompositeType (nameField.getText()) != null && 
		  typeMngr.findCompositeType (nameField.getText()) != originalType) {
	    JOptionPane.showConfirmDialog(null, 
					  "Type name is already used",
					  "Error!", 
					  JOptionPane.DEFAULT_OPTION,
					  JOptionPane.ERROR_MESSAGE);
	    return false;
	}
	else {
	    try {
		originalType.deepCopyAttributes ( workingType);
	    } catch ( TypeException te) {
		JOptionPane.showConfirmDialog(null, 
					      "Type Exception",
					      "Error!", 
					      JOptionPane.DEFAULT_OPTION,
					      JOptionPane.ERROR_MESSAGE);
		return false;
	    }
	    originalType.setName (nameField.getText());
            TypeInferencer.typeCheck();

	    return true;
	}
    }
}
