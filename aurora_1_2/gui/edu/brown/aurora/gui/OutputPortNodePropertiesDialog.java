
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.table.*;
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
public class OutputPortNodePropertiesDialog extends PortNodePropertiesDialog
{
    protected QosSpecifiers qosSpecifiers;

    protected OutputPortNode outputPortNode;

    protected JTextField numQosSpecifiersText;
    protected int numQosSpecifiers;
    protected Vector qosSpecifiersPanels;
    protected CompositeType type;
    


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
		return false;
            }
	    
	};






    /**
     * Constructs a new OutputPortNodePropertiesDialog object.
     *
     * @param node The GenericPortNode associated with this 
     * properties dialog.
     */
    public OutputPortNodePropertiesDialog(OutputPortNode node)
    {
        super(node, false);
        outputPortNode = (OutputPortNode) node;
	type = node.getType();
        setupGeneral();
        setupQosSpecifiers();   
        show();      
    }
    

    /**
     * Sets up the general tab for the properties dialogs.
     */
    public void setupGeneral()
    {
        GridBagLayout bags = new GridBagLayout();
        JPanel generalPanel = new JPanel(bags);

	type = outputPortNode.getType();

	//sets up the box Id label
	JLabel boxIdLabel = new JLabel ("Box Id: " + (new Integer (node.getBoxId())).toString()); 
	

	//sets up the type panel
	JPanel typePanel = new JPanel (bags);
	if (type == null) {
	    addComponent (bags, typePanel, new JLabel ("No type specified"), 0,0, 1,1, 0,0);
	}
	else {
	    JScrollPane typeTable = new JScrollPane (new JTable (dataModel));
	    String typeName = type.getName();
	    if (typeName == "" || typeName == null) {
		typeName = "ANONYMOUS";
	    }
	    JLabel typeNameLabel = new JLabel ("Type: " + typeName);
	    addComponent (bags, typePanel, typeNameLabel, 0,0, 1,1, .01, .01);
	    addComponent (bags, typePanel, typeTable, 0,1, 1,1, .1,.1);
	}


	//sets up the QoS specifiers
        qosSpecifiers = outputPortNode.getQosSpecifiers();

        try {
            numQosSpecifiers = qosSpecifiers.getNumQosSpecifiers();
        } catch (QosSpecifierException e) {
            System.out.println(e.toString());
            e.printStackTrace();
            numQosSpecifiers = 0;
        }
        numQosSpecifiersText = new JTextField("" + numQosSpecifiers);
        numQosSpecifiersText.addKeyListener(new TextKeyListener());


	//adds all the components
	addComponent(bags, generalPanel, boxIdLabel, 0,0, 2,1, .01,.01);
	addComponent(bags, generalPanel, typePanel, 0,1, 2,1, .1, .1, 
		     new Insets (10,10,10,10));
	addComponent(bags, generalPanel, new JLabel ("Number of Qos Specifiers: "), 0,2, 1,1, .01, .01);
	addComponent(bags, generalPanel, numQosSpecifiersText, 1,2, 1,1, .01, .01);
        tabbedPane.addTab("General", generalPanel);

    }

    protected void setupQosSpecifiers()
    {
        if(qosSpecifiersPanels != null) {
            for(int i=0; i<qosSpecifiersPanels.size(); i++) {
	      tabbedPane.remove((Component)qosSpecifiersPanels.elementAt(i));
            }
	}
	else {
	    qosSpecifiersPanels = new Vector();
	}

        for(int i=0; i<numQosSpecifiers; i++) {
            QosPoints points = qosSpecifiers.getQosPointsIndex(i);
            
            QosSpecifiersPanel qosPanel = new QosSpecifiersPanel(points);
            qosSpecifiersPanels.addElement(qosPanel);

            tabbedPane.addTab("Qos " + i, qosPanel);
        }
    }
    
    /**
     *  Sets the values of the output port node when the user clicks "apply" or "ok".
     */
    protected boolean set()
    {
        boolean returnValue = super.set();

        try {
            if(numQosSpecifiers != 
	       Integer.parseInt(numQosSpecifiersText.getText())) {
                numQosSpecifiers = 
		    Integer.parseInt(numQosSpecifiersText.getText());
                qosSpecifiers.setNumSpecifiers(numQosSpecifiers);
                setupQosSpecifiers();
            }
        } catch(Exception parseExc) {
            JOptionPane.showConfirmDialog(null, 
                                          "Integer parsing error : " + parseExc.toString(), 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
            return false;
        }
     
        for(int i=0; i<numQosSpecifiers; i++) {
            QosSpecifiersPanel panel = (QosSpecifiersPanel) qosSpecifiersPanels.elementAt(i);
            
            if(!panel.set()) {
                return false;
            }
        }

        outputPortNode.setQosSpecifiers(qosSpecifiers);

        return returnValue;
    }

    protected class TextKeyListener implements KeyListener
    {
        public TextKeyListener()
        {
        }
        
        public void keyPressed(KeyEvent e)
        {
        }
        public void keyReleased(KeyEvent e)
        {
        }
        public void keyTyped(KeyEvent e)
        {
            if(e.getKeyChar() == '\n') {
                try {
                    numQosSpecifiers = Integer.parseInt(numQosSpecifiersText.getText());
                    qosSpecifiers.setNumSpecifiers(numQosSpecifiers);
                    setupQosSpecifiers();
                } catch(Exception exc) {
                    JOptionPane.showConfirmDialog(null, 
                                                  "Integer parsing error : " + exc.toString(), 
                                                  "Exception" , 
                                                  JOptionPane.DEFAULT_OPTION, 
                                                  JOptionPane.ERROR_MESSAGE);
                }
            }
        }
    }


}
