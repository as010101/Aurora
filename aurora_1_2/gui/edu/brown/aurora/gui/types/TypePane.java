/*
 * @(#)TypePane.java	1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved. 
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import edu.brown.aurora.gui.*;
import javax.swing.tree.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 * A TypePane object is a scroll pane which has a TypeManager and a TypeList object associated with the TypeManager in it. 
 * When a typePane object is created, its TypeManager and TypeList objects are constructed automatically.  
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class TypePane extends JPanel
{
    private TypeManager typeMngr = null;
    private TypeList typeList;
    
    private static final String ADD_BUTTON = "Add";
    private static final String REMOVE_BUTTON = "Remove";

    /**
     * Constructs a new TypePane. 
     */
    public TypePane()
    {
        typeMngr = RootFrame.typeManager;
        
        if(typeMngr.getTypeList() == null) {
            typeList = new TypeList(typeMngr);
        } else {
            DefaultListModel oldTypeList = typeMngr.getTypeList();
            typeList = new TypeList(oldTypeList);
        }

	GridBagLayout bags = new GridBagLayout();
	setLayout(bags);
	
	JScrollPane typeListScroll = new JScrollPane();
	typeListScroll.setViewportView(typeList);
	JButton addTypeBtn = new JButton(ADD_BUTTON);
	JButton removeTypeBtn = new JButton(REMOVE_BUTTON);
	addTypeBtn.addActionListener(new TypesButtonListener());
	removeTypeBtn.addActionListener(new TypesButtonListener());

	Utils.addComponent(bags, this, typeListScroll, 0,0,1,1,1,20);
	Utils.addComponent(bags, this, addTypeBtn, 0,1,1,1,1,1);
	Utils.addComponent(bags, this, removeTypeBtn, 0,2,1,1,1,1);

	addTypeBtn.setFont(Constants.TOOLBOX_BUTTON_FONT);
	removeTypeBtn.setFont(Constants.TOOLBOX_BUTTON_FONT);
    }

    protected class TypesButtonListener implements ActionListener
    {
	public TypesButtonListener() {}
	
	public void actionPerformed(ActionEvent e) 
	{
	    if(ADD_BUTTON.equals(e.getActionCommand())) {
		typeList.addType();
	    }
	    if(REMOVE_BUTTON.equals(e.getActionCommand())) {
		if (typeList.getSelectedIndex() == -1) {
		    JOptionPane.showMessageDialog(null,
						  "Select a type to remove.",
						  "Information",
						  JOptionPane.INFORMATION_MESSAGE);
		}
		else {
		    if (JOptionPane.showConfirmDialog(null, 
						      "Do you really want to remove the selected type?", 
						      "Question" , 
						      JOptionPane.YES_NO_OPTION, 
						      JOptionPane.QUESTION_MESSAGE)
			== JOptionPane.YES_OPTION) {
			
			typeList.removeType();
		    }
		}
	    }
	}
    }
    
    /**
     * Returns this pane's TypeManager. 
     * @return this pane's TypeManager 
     */
    public TypeManager getTypeManager()
    {
        return typeMngr;
    }
    
    /**
     * Returns this pane's TypeList. 
     * @return this pane's TypeList 
     */
    public TypeList getTypeList()
    {
        return typeList;
    }
    
}
