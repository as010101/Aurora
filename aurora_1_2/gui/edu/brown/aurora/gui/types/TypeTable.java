/*
 * @(#)TypeTable.java	1.0 04/10/2002
 *
 * Copyright 2002 by Jeong-Hyon Hwang (jhhwang@cs.brown.edu).
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import javax.swing.table.*;
import javax.swing.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.awt.*;

/**
 * An TypeTable object is a JTable which represents a specification of a given Compositetype.
 * Therefore, it displays a number of attributes and enables addition, insertion, update, and deletion
 * of attributes.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class TypeTable extends JTable
{

    private class TypeTableListener extends FocusAdapter
    {
	public void focusLost(FocusEvent e) 
	{
	    int row = getRowCount();
	    for (int r = 0; r < row; r++) {
		editCellAt(r, 0);
	    }
	}
    }
	    

    private class TableListener implements ListSelectionListener
    {
        public void valueChanged(ListSelectionEvent e)
        {
            if (type == null) return;

            int minSelectionIndex = getSelectionModel().getMinSelectionIndex();
            int maxSelectionIndex = getSelectionModel().getMaxSelectionIndex();
            int num = type.getNumberOfAttributes();
            if (minSelectionIndex >= num) minSelectionIndex = num-1;
            if (maxSelectionIndex >= num) maxSelectionIndex = num-1;

            if (dlg.removeAttributes != null && dlg.insertAttribute != null) {
                if (minSelectionIndex >=0 && maxSelectionIndex >=0) {
                    dlg.removeAttributes.setEnabled(true);
                    if (minSelectionIndex == maxSelectionIndex)
                        dlg.insertAttribute.setEnabled(true);
                    else
                        dlg.insertAttribute.setEnabled(false);
                } else	{
                    dlg.insertAttribute.setEnabled(false);
                    dlg.removeAttributes.setEnabled(false);
                }
            }
        }
    }
    
    AbstractTableModel dataModel = new AbstractTableModel() 
        {
            public int getColumnCount() 
            { 
                return 3; 
            }
            public int getRowCount() 
            { 
                if (type != null)
                    return type.getNumberOfAttributes();
                else 
                    return 0;
            }
            
            public Object getValueAt(int row, int col) 
            {
                if (col == 0)
                    return type.getAttributeName(row);
                else if (col == 1)
                    return type.getAttributeType(row).getName();
                else if (col == 2)
                    try {
                        return new Integer(type.getAttributeLength(row));
                    } catch (NumberFormatException e) {
                        return new Integer(TypeManager.VARIABLE_SIZE);
                    }

                else return null;
            }
            
            public String getColumnName(int col) 
            {
                if (col == 0)
                    return "name";
                else if (col == 1)
                    return "type";
                else if (col == 2)
                    return "length";
                else return null;
            }
            
            public boolean isCellEditable(int row, int col) 
            {
                Type thisType = type.getAttributeType(row);
                PrimitiveType primType = typeMngr.findPrimitiveType(thisType.getName());
                if (col == 0 || col == 1) {
                    return true;
                } else if (col == 2 && !primType.isFixedLength()) {
                    return true;
                } else {
                    return false;
                }
            }

            public void setValueAt(Object aValue, int row, int col)
            {
                try {
                    if (col == 0) {
                        type.setAttributeName(row, (String)aValue);
		    }
                    else if (col == 1) {
                        type.setAttributeType(row, typeMngr.findPrimitiveType((String)aValue));
			// Forces the length box to be updated
			Rectangle rect = getCellRect(row, 2, false);
			repaint(rect);
		    }
                    else if (col == 2) {
                        int length = 0;
                        try {
                            length = new Integer((String)aValue).intValue();
                            type.setAttributeLength(row, length);
                            //if(length == TypeManager.VARIABLE_SIZE) {
                            //    super.setValueAt("Variable-length", row, col);
                            //}
                        } catch (Exception e) {
                            //type.setAttributeLength(row, TypeManager.VARIABLE_SIZE);
                            throw new TypeException(TypeException.NON_NUMERICAL_LENGTH);
                        }
                    }
                } catch (Exception e) {
                    JOptionPane.showConfirmDialog(null,
                                                  e,
                                                  "Exception" ,
                                                  JOptionPane.DEFAULT_OPTION,
                                                  JOptionPane.ERROR_MESSAGE);
                    e.printStackTrace();
                }
            }
	};
    
    private CompositeType type;
    private TypeManager typeMngr;
    private TypeTableDlg dlg;
    
    /**
     * Constructs a new TypeTable object. 
     * @param typeManager the type manager
     * @param dlg the TypeTableDlg dialog
     */
    public TypeTable(TypeManager typeMngr, TypeTableDlg dlg) 
    {
        super();
        this.typeMngr = typeMngr;
        this.dlg = dlg;
        setModel(dataModel);
        getSelectionModel().addListSelectionListener(new TableListener());

	addFocusListener(new TypeTableListener());
        
        JComboBox comboBox = new JComboBox();
        String[] names = typeMngr.getPrimitiveTypeNames();
        if (names != null)
            for (int i = 0; i < names.length; i++)
                comboBox.addItem(names[i]);
        TableColumn typesColumn = getColumn("type");
        // Use the combo box as the editor in the "Favorite Color" column.
        typesColumn.setCellEditor(new DefaultCellEditor(comboBox));
    }
    
    /**
     * Sets the CompositeType for this type table. 
     * @param type the CompositeType
     */
    public void setType(CompositeType type)
    {
        if (this.type != type) {
            this.type = type;
            getSelectionModel().clearSelection();
            updateUI();
        }

        if (type instanceof CompositeType) {
            dlg.addAttribute.setEnabled(true);
            dlg.insertAttribute.setEnabled(false);
            dlg.removeAttributes.setEnabled(false);
        } else {
            dlg.addAttribute.setEnabled(false);
            dlg.insertAttribute.setEnabled(false);
            dlg.removeAttributes.setEnabled(false);
        }
    }
    
    /**
     * Adds an attribute.
     */
    public void addAttribute()
    {
        if (type != null) {
            int counter = 1;
            String name = "New"+(counter++);
            while (type.findAttribute(name) != null) {
                name = "New"+(counter++);			
            }
            try	{
                type.addAttribute(name, 
                                  typeMngr.getDefaultPrimitiveType(),
                                  typeMngr.getDefaultPrimitiveType().getLength());
            } catch(Exception e) {}
            revalidate();
        }
    }
    
    /**
     * Inserts an attribute to the current position.
     */
    public void insertAttribute()
    {
        if (type != null) {
            int minSelectionIndex = getSelectionModel().getMinSelectionIndex();
            int maxSelectionIndex = getSelectionModel().getMaxSelectionIndex();
            
            int num = type.getNumberOfAttributes();
            if (minSelectionIndex >= num) minSelectionIndex = num-1;
            if (maxSelectionIndex >= num) maxSelectionIndex = num-1;
            
            if (minSelectionIndex >=0 && maxSelectionIndex == minSelectionIndex) {
                int counter = 1;
                String name = "New"+(counter++);
                while (type.findAttribute(name) != null) {
                    name = "New"+(counter++);			
                }
                try {
                    type.insertAttribute(minSelectionIndex, 
                                         name, 
                                         typeMngr.getDefaultPrimitiveType(),
                                         typeMngr.getDefaultPrimitiveType().getLength());
                    getSelectionModel().clearSelection();
                    getSelectionModel().setSelectionInterval(minSelectionIndex+1, minSelectionIndex+1);
                } catch(Exception e) {}
                revalidate();
            } else {
                JOptionPane.showConfirmDialog(null, 
                                              "In order to insert an attribute, you must select one row in the type specification table.", 
                                              "Exception" , 
                                              JOptionPane.DEFAULT_OPTION, 
                                              JOptionPane.ERROR_MESSAGE);
            }
        }
    }	
    
    /**
     * Removes the selected attributes.
     */
    public void removeAttributes()
    {
        if (type != null) {
            int minSelectionIndex = getSelectionModel().getMinSelectionIndex();
            int maxSelectionIndex = getSelectionModel().getMaxSelectionIndex();
            
            int num = type.getNumberOfAttributes();
            if (minSelectionIndex >= num) minSelectionIndex = num-1;
            if (maxSelectionIndex >= num) maxSelectionIndex = num-1;
            
            if (minSelectionIndex >=0 && maxSelectionIndex >=0) {
                if (javax.swing.JOptionPane.showConfirmDialog(null, 
                                                              "Do you really want to remove the selected attributes?", 
                                                              "Question" , 
                                                              JOptionPane.YES_NO_OPTION, 
                                                              JOptionPane.QUESTION_MESSAGE) == JOptionPane.YES_OPTION) {
                    for (int i = maxSelectionIndex; i >= minSelectionIndex; i--) 
                        if (getSelectionModel().isSelectedIndex(i))
                            type.removeAttribute(i);
                }
                getSelectionModel().clearSelection();
                revalidate();
            }
        }
    }
}
