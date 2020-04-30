/*
 * @(#)TypeTree.java	1.0 04/10/2002
 *
 * Copyright 2002 by Jeong-Hyon Hwang (jhhwang@cs.brown.edu).
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import edu.brown.aurora.gui.*;
import javax.swing.tree.*;
import javax.swing.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.event.*;
import java.awt.Point;

/**
 * Displays the contents of the type manager in a list form.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */

public class TypeList extends JList
{
    protected DefaultListModel typeList;

    protected class TypeListListener extends MouseAdapter
    {
        public void mouseClicked(MouseEvent e) {
	    if(e.getButton() == MouseEvent.BUTTON3 || e.getClickCount() == 2) {
		int index = locationToIndex(new Point(e.getX(), e.getY()));
		setSelectedIndex(index);
		CompositeType type = getCurrentType();
		if(type != null) {
		    TypeTableDlg dlg = new TypeTableDlg(RootFrame.typeManager, type);
		    dlg.setModal(true);
		    dlg.show();
		    
		}
	    }
	    // Forces the name of the composite type to change in the display
	    e.getComponent().requestFocus();
        }        
    }

    /**
     * Constructs a TypeList object whose data model is the specified TypeManager object. 
     * The instantiated TypeList object (1) is editable, 
     * (2) selects only one node at a time, 
     * (3) runs a mouse listener in order to display the properties dialog box 
     * for the selected node whenever the right mouse button is clicked.
     * @param typeManager the data model for this TypeList object
     */
    public TypeList(TypeManager typeManager) 
    {
	typeList = typeManager.getTypeList();
	setModel(typeList);
	setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        addMouseListener(new TypeListListener());
	setFont(Constants.PALETTE_LIST_FONT);
    }

    public TypeList(DefaultListModel listModel)
    {
	typeList = listModel;
	setModel(typeList);
	setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	addMouseListener(new TypeListListener());
	setFont(Constants.PALETTE_LIST_FONT);
    }

    /**
     * Returns the type manager. 
     * @return the type manager (the data model for this TypeList object)
     */
    public TypeManager getTypeManager()
    {
        return RootFrame.typeManager;
    }
    
    /**
     * Returns the CompositeType object chosen by user. 
     * @return the CompositeType object chosen by user
     */
    public CompositeType getCurrentType()
    {
	if(getSelectedValue() instanceof CompositeType) {
	    return (CompositeType)getSelectedValue();
	}
        return null;
    }
    
    /**
     * Adds a new CompositeType object in this tree. 
     * The newly created type in this tree will be highlighted and the corresponding name of it will be "New"+[number].
     * This method is exception-free.
     */
    public void addType()
    {
        String name = RootFrame.typeManager.getNewTypeName(Constants.DEFINEDTYPENAME);

        try {
            CompositeType type = new CompositeType(name, false);
            RootFrame.typeManager.addCompositeType(type);
	    setSelectedValue(type, true);
        } catch(Exception e) {
            JOptionPane.showConfirmDialog(null, 
                                          e, 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
            e.printStackTrace();
        }
    }
    
    /**
     * Removes the current CompsiteType object (the chosen node).
     */
    public void removeType()
    {
	int selectedIndex = getSelectedIndex();
	CompositeType deletedType = (CompositeType)getSelectedValue();
	//System.out.print("deletedType: " + deletedType.toStringInDetail() + "\n");
	if(selectedIndex != -1) {
	    RootFrame.typeManager.removeType(deletedType, selectedIndex);
	    System.out.println("Removed deleted type in typeManager");
	}
	/*
	int selectedIndex = getSelectedIndex();
	if(selectedIndex != -1) {
	    typeList.removeElementAt(selectedIndex);
	    System.out.println("typeList removed element at " + selectedIndex);
	}
	*/
    }


}
