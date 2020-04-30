package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import javax.swing.event.*;
import edu.brown.aurora.gui.types.*;


/**
 * An abstract superclass of a properties dialog for boxes.  It constructs the visual
 * dialog box along with one tab for labels and descriptions.  It also constructs the
 * tab panels that describe port functionality.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class BoxPropertiesDialog 
    extends GenericNodePropertiesDialog implements ActionListener
{
    /**
     * A Swing textfield object for setting the label of the box node.
     */ 
    protected JTextField labelText;

    /**
     * A Swing textarea object for setting the description of the box node.
     */
    protected JTextArea descriptionText;

    /**
     * Formatting constant.  Sets the number of rows in the label/description tab area
     * of the properties dialog.
     */
    protected final static int TEXTROWS = 5;

    /**
     * Formatting constant.  Sets the number of columns in the label/description tab area
     * of the properties dialog.
     */
    protected final static int TEXTCOLS = 6;

    /**
     * Formatting constant.  The index of the label's JLabel object.
     */
    protected static final int LABELLABEL = 0;

    /**
     * Formatting constant.  The index of the label's JTextField object.
     */
    protected static final int LABELTEXT = 1;

    /**
     * Formatting constant.  The index of the description's JLabel object.
     */
    protected static final int DESCLABEL = 2;

    /**
     * Formatting constant.  The index of the description's textarea object.
     */
    protected static final int DESCTEXT = 3;
    
    protected Vector portLists;
    protected Vector typeLists;
    protected JCheckBox useDefinedInputTypeCheck;
    protected JCheckBox useDefinedOutputTypeCheck;
    protected boolean useInputDefinedType;
    protected boolean useOutputDefinedType;
    protected boolean isOutputNamed;


    /**
     * Constructs a BoxPropertiesDialog object.  Creates one tab for the label
     * and the description.
     *
     * @param node The BoxNode associated with this properties dialog box.
     */
    public BoxPropertiesDialog(BoxNode node) {
        super(node);

	portLists = new Vector();
	typeLists = new Vector();
        
        // Set Label section
        setupLabel();
        
        // Set up Ports section
        setupPortsPanel();
    }  
 


    /**
     * Abstract class which returns whether or not to specify output
     * @return whether or not to specify output
     */
    abstract boolean specifyOutput();

    public void setIsOutputNamed(boolean isOutputNamed) 
    {
        this.isOutputNamed = isOutputNamed;
	
        if(this instanceof PrimitiveBoxPropertiesDialog) {
            PrimitiveBoxPropertiesDialog pbp = (PrimitiveBoxPropertiesDialog) this;
            pbp.resetModifier();
        }
	
	
    }



    /**
     * Sets up the primary tab of the properties dialog box.  Objects in this
     * tab include textcomponents for box ids, type, label, and description of
     * the box.
     */
    public void setupLabel()
    {
        GridBagLayout bags = new GridBagLayout();
        JPanel labelPanel = new JPanel(bags);
        
        JLabel boxId  = new JLabel("Box Id:  ");
        addComponent(bags, labelPanel, boxId, 0,0, 1,1, 0,1);
        
        JTextField labelBoxId = new JTextField(""+node.getBoxId());
        labelBoxId.setEditable(false);
        addComponent(bags, labelPanel, labelBoxId, 1,0, 1, 1, 1, 1);
        
        
        JLabel type  = new JLabel("Type:  ");
        addComponent(bags, labelPanel, type, 0,1, 1,1, 0, 1);
        
        JTextField labelType = new JTextField(((BoxNode)node).getBoxTypeName());
        labelType.setEditable(false);
        addComponent(bags, labelPanel, labelType, 1,1, 1,1, 1, 1);
        
        JLabel label  = new JLabel("Label:  ");
        addComponent(bags, labelPanel, label, 0, 2, 1,1, 0, 1);
        
        labelText = new JTextField(((BoxNode)node).getLabel());
        addComponent(bags, labelPanel, labelText, 1, 2, 1, 1, 1, 1);
        
        
        JLabel desc = new JLabel("Description:  ");
        addComponent(bags, labelPanel, desc, 0, 3, 1, 1, 0, 1);
        
        descriptionText = new JTextArea("");
        descriptionText.setRows(TEXTROWS);
        descriptionText.setLineWrap(true);
        descriptionText.setText(((BoxNode)node).getDescription());
        JScrollPane scroller = new JScrollPane(descriptionText);
        addComponent(bags, labelPanel, scroller, 1, 3, 1, 1, 1, 1);
        
        labelPanel.setBorder(new LineBorder(Color.black, 1));
        tabbedPane.addTab("Label", labelPanel);
    }
   
    /**
     * An inner class that allows the list of ports within the box properties
     * dialog to be associated with function calls.
     *
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */
    protected class PortListListener implements ListSelectionListener
    {
        
        JList portList;
        JList typeList;
        
        PortListListener(JList portList, JList typeList)
        {
            this.portList = portList;
            this.typeList = typeList;
        }
        
        public void valueChanged(ListSelectionEvent e) 
        {
            int index = this.portList.getSelectedIndex();
            if (index >= 0) {
                GenericBoxNodePort port = (GenericBoxNodePort)(this.portList.getModel().getElementAt(index));
                CompositeType t = port.getType();
                for (int i = 0; i < this.typeList.getModel().getSize(); i++) {
                    Object maybeType = typeList.getModel().getElementAt(i);
		    if(maybeType instanceof CompositeType) {
			CompositeType compositeType = (CompositeType)maybeType;
			if(compositeType.equals(t)) {
			    this.typeList.setSelectedIndex(i);
			    return;	
			}
		    }
                }
	    }
            this.typeList.removeSelectionInterval(0, this.typeList.getMaxSelectionIndex());
        }
    }

    /**
     * An inner class that allows the list of types within the box properties
     * dialog to be associated with function calls.
     *
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */    
    protected class TypeListListener implements ListSelectionListener
    {
        
        JList portList;
        JList typeList;
        BoxPropertiesDialog dialog;

        TypeListListener(JList portList, JList typeList, BoxPropertiesDialog dialog)
        {
            this.portList = portList;
            this.typeList = typeList;
            this.dialog = dialog;
        }
        
        public void valueChanged(ListSelectionEvent e) 
        {
            int index = this.typeList.getSelectedIndex();
            if (index >= 0) {
                int portIndex = this.portList.getSelectedIndex();
                if (portIndex >= 0) {
                    GenericBoxNodePort port = (GenericBoxNodePort)(this.portList.getModel().getElementAt(portIndex));
                    port.setType((CompositeType)(this.typeList.getModel().getElementAt(index)));
                                        
                    if(dialog instanceof PrimitiveBoxPropertiesDialog) {
                        PrimitiveBoxPropertiesDialog primDialog = (PrimitiveBoxPropertiesDialog) dialog;
                        primDialog.resetModifier();
                    }
                } else {
                    this.typeList.removeSelectionInterval(0, this.typeList.getMaxSelectionIndex());
                }
            }
        }
    }

    /**
     * Sets up a panel that displays ports as a JList object for the properties dialog.  
     *
     * @param ports The Vector that contains the ports.
     * @return A JPanel that contains JList objects for the properties dialog.
     */
    protected JPanel setupPortsPanel(Vector ports, boolean enabled)
    {	
        GridBagLayout bags = new GridBagLayout();
        JPanel portsPanel = new JPanel(bags);

        JLabel numberOfPorts  = new JLabel("Number of Ports:  ");
        addComponent(bags, portsPanel, numberOfPorts, 0,0, 1,1, 0,0);
        
        JTextField numberOfPortsText = new JTextField(""+ports.size());
        numberOfPortsText.setEditable(false);
        addComponent(bags, portsPanel, numberOfPortsText, 1,0, GridBagConstraints.REMAINDER,1, 0, 0);
        
        JList portList = null;
        JList typeList = null;
        
        if (ports.size() > 0) {
            portList = new JList(ports);
            portList.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
            addComponent(bags, portsPanel, portList, 0,1, 1,1, 0, 1);
	    portLists.addElement(portList);
        }
        
        CompositeType[] types = RootFrame.typeManager.getCompositeTypes();
        if (types != null && types.length > 0) {
            typeList = new JList(types);
            typeList.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    typeLists.addElement(typeList);
            addComponent(bags, portsPanel, typeList, 1,1, 1,1, 1, 1);
	    typeList.setEnabled(enabled);
        }
        
        if (portList != null && typeList != null) {
            portList.addListSelectionListener(new PortListListener(portList, 
                                                                   typeList));
            typeList.addListSelectionListener(new TypeListListener(portList, 
                                                                   typeList, this));
            portList.setSelectedIndex(0);
	    portList.setEnabled(enabled);
        }
        
        return portsPanel;
    }
    
    /**
     * Sets up a panel that displays ports as a JList object for the properties dialog.  
     *
     */
    public void setupPortsPanel()
    {
        GridBagLayout bags = new GridBagLayout();
        JPanel portsPanel = new JPanel(bags);

	if(node instanceof PrimitiveBoxNode) {
	    useInputDefinedType = ((PrimitiveBoxNode)node).isUseDefinedTypes(); 
	    useOutputDefinedType = ((PrimitiveBoxNode)node).isOutputDefinedTypes();
	} else if (node instanceof GenericPortNode) {
	    useInputDefinedType = ((GenericPortNode)node).isUseDefinedTypes();
	    useOutputDefinedType = useInputDefinedType;
	} else {
	    useInputDefinedType = false;
	    useOutputDefinedType = false;
	}

        isOutputNamed = useOutputDefinedType;

        
        portsPanel.setBorder(new LineBorder(Color.black, 1));
        tabbedPane.addTab("Ports", portsPanel);

	useDefinedInputTypeCheck = new JCheckBox("Use defined input type", 
                                                           useInputDefinedType);
	useDefinedInputTypeCheck.addChangeListener(new DefinedTypesListener(this));

	addComponent(bags, portsPanel, useDefinedInputTypeCheck, 0,0,1,1,1,1);
        if(specifyOutput()) {
            useDefinedOutputTypeCheck = new JCheckBox("Use defined output type",
                                                      useOutputDefinedType);
            useDefinedOutputTypeCheck.addChangeListener(new DefinedTypesListener(this));
        
            addComponent(bags, portsPanel, useDefinedOutputTypeCheck, 1, 0, 1, 1, 1, 1); 
        }

        if (node.getComponent() instanceof BoxComponent) {
            BoxComponent component = (BoxComponent)(node.getComponent());
            addComponent(bags, 
                         portsPanel, 
                         setupPortsPanel(component.getInputPorts(), 
                                         useInputDefinedType), 
                         0,1, 
                         1,1, 
                         1,7);
	    if (specifyOutput()) {
		addComponent(bags, 
			     portsPanel, 
			     setupPortsPanel(component.getOutputPorts(), 
                                             useOutputDefinedType), 
			     1,1, 
			     1,1, 
			     1,7);
	    }
        }

	if(portLists.size() > 0 && typeLists.size() > 0) {
	    JList typeList = (JList)typeLists.elementAt(0);
	    typeList.setEnabled(useInputDefinedType);
	    JList portList = (JList)portLists.elementAt(0);
	    portList.setEnabled(useInputDefinedType);
	    
	    if (specifyOutput()) {
		typeList = (JList)typeLists.elementAt(1);
		typeList.setEnabled(useOutputDefinedType);
		portList = (JList)portLists.elementAt(1);
		portList.setEnabled(useOutputDefinedType);
	    }
	}
    }


    private class DefinedTypesListener implements ChangeListener
    {
        protected BoxPropertiesDialog dialog;

        public DefinedTypesListener(BoxPropertiesDialog dialog)
        {
            this.dialog = dialog;
        }

        public void stateChanged(ChangeEvent e) 
        {
            if(e.getSource().equals(useDefinedInputTypeCheck)) {
                useInputDefinedType = !useInputDefinedType;
		if(typeLists.size() > 0) {
		    JList typeList = (JList)typeLists.elementAt(0);
		    typeList.setEnabled(useInputDefinedType);
		    if (!useInputDefinedType) {
			typeList.clearSelection();
		    }
		}
		JList portList = (JList)portLists.elementAt(0);
                portList.setEnabled(useInputDefinedType);


            }

            if(e.getSource().equals(useDefinedOutputTypeCheck)) {
                useOutputDefinedType = !useOutputDefinedType;
		if(typeLists.size() > 1) {
		    JList typeList = (JList)typeLists.elementAt(1);
		    typeList.setEnabled(useOutputDefinedType);	    
		    if (!useOutputDefinedType) {
			typeList.clearSelection();
		    }
		}
		JList portList = (JList)portLists.elementAt(1);
		portList.setEnabled(useOutputDefinedType);

                setIsOutputNamed(useOutputDefinedType);
            }
            TypeInferencer.typeCheck();
               

            /*
            if(dialog instanceof PrimitiveBoxPropertiesDialog) {
                PrimitiveBoxPropertiesDialog pmDialog = (PrimitiveBoxPropertiesDialog) dialog;
                pmDialog.removeModifier();

                PrimitiveBoxNode pmNode = (PrimitiveBoxNode) node;
                // Query tab section
                if(pmNode.isUseDefinedTypes() && pmDialog.inputPortsDefined() && 
                   !pmDialog.isModifiersTabSetup()) {
                    pmDialog.setupModifier();
                }
                
                if(pmNode.defineModifiersEnabled() && !pmDialog.isModifiersTabSetup()) {
                    pmDialog.setupModifier();
                }

            }
            */

        }
        
    }

}
