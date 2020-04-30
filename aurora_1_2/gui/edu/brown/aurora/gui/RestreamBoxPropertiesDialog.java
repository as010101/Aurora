package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for restream boxes.  It constructs
 * the restreambox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class RestreamBoxPropertiesDialog extends UnaryBoxPropertiesDialog {

    private class buttonListener implements ActionListener {
	public void actionPerformed (ActionEvent e) {
	    if(e.getActionCommand().equals ("Select") && inputList.getSelectedIndex() != -1) {
		try {
		    outputModel.addElement (inputModel.remove (inputList.getSelectedIndex()));
		} catch (NullPointerException ex ) {}
	    }else if(e.getActionCommand().equals ("Remove")) {
		try {
		    inputModel.addElement (outputModel.remove (outputModel.size() -1));
		} catch (ArrayIndexOutOfBoundsException ex) {}
	    }
	}
    }

    private class specificationCellRenderer extends JLabel implements ListCellRenderer {
	public specificationCellRenderer() {
	    setOpaque(true);
	}
	public Component getListCellRendererComponent(
						      JList list,
						      Object value,
						      int index,
						      boolean isSelected,
						      boolean cellHasFocus)
	{
	    setText(value.toString());
	    setBackground(Color.white);
	    setForeground(Color.black);
	    return this;
	}
    }

    /**
     * The input list
     */
    private JList inputList;

    /**
     * The input model
     */
    private DefaultListModel inputModel;

    /**
     * The output list
     */
    private JList outputList;

    /**
     * The list model for the output list
     */
    private DefaultListModel outputModel;

    /**
     * Constructs a new RestreamBoxPropertiesDialog object.
     *
     * @param tm The typemanager contained in the GUI.
     * @param node The box node that this properties dialog is to be opened for.
     */
    public RestreamBoxPropertiesDialog(RestreamBoxNode node)
    {
        super(node);
        setTitle("Restream Box Properties Dialog");
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

	RestreamBoxNode restreamnode = (RestreamBoxNode)node;

        String[] iNames = getInputTypeAttributeNames();
        String m = getParsedModifier();
        
        if(iNames == null ||
	   (restreamnode.isUseDefinedTypes() && iNames.length == 0)) {
            return;
        }

	GridBagLayout bags = new GridBagLayout();

	StringTokenizer outputTokenizer = new StringTokenizer(m,
							      Constants.INNER_MODIFIER_DELIM);
	StringTokenizer inputTokenizer = new StringTokenizer(m,
							     Constants.INNER_MODIFIER_DELIM);

	//creates the output panel
	JPanel outputPanel = new JPanel (bags);
	outputModel = new DefaultListModel ();
	String modifier = restreamnode.getModifier();
	while (outputTokenizer.hasMoreElements()) {
	    outputModel.addElement (outputTokenizer.nextToken());
	}
	outputList = new JList (outputModel);
	specificationCellRenderer sCellRenderer = new specificationCellRenderer();
	outputList.setCellRenderer (sCellRenderer);
	outputList.setFixedCellWidth (215);
	JButton removeButton = new JButton ("Remove");
	removeButton.addActionListener (new buttonListener());
	addComponent (bags, outputPanel, (new JLabel ("Group by")), 0,0, 1,1, 0,0);
	addComponent (bags, outputPanel, (new JScrollPane (outputList)), 0,1, 0,1, 1,1);
	addComponent (bags, outputPanel, removeButton, 0,2, 1,1, 0,0);

	//creates the input panel
	JPanel inputPanel = new JPanel (bags);
	inputModel = new DefaultListModel();
	for (int i = 0; i < iNames.length; i++ ) {
	    inputModel.addElement (iNames[i]);
	}
	while (inputTokenizer.hasMoreElements()) {
	    inputModel.removeElement (inputTokenizer.nextToken());
	}
	inputList = new JList (inputModel);
	inputList.setSelectionMode (ListSelectionModel.SINGLE_SELECTION);
	inputList.addMouseListener (
		new MouseAdapter() {
		    public void mouseReleased (MouseEvent e)
		    {
			if (e.getClickCount() == 2) {
			    try {
				outputModel.addElement (inputModel.remove (inputList.getSelectedIndex()));
			    } catch (NullPointerException ex ) {}
			}
		    }
		});
	inputList.setFixedCellWidth (215);
	JButton selectButton = new JButton ("Select");
	selectButton.addActionListener (new buttonListener());
	addComponent (bags, inputPanel, (new JLabel ("Input")), 0,0, 1,1, 0,0);
	addComponent (bags, inputPanel, (new JScrollPane (inputList)), 0,1, 0,1, 1,1);
	addComponent (bags, inputPanel, selectButton, 0,2, 1,1, 0,0);



	//adds the components
	modifierPanel = new JPanel(bags);
	addComponent (bags, modifierPanel, inputPanel, 0,0, 1,1, 0,1);
	addComponent (bags, modifierPanel, outputPanel, 1,0, 1,1, 0,1);
        tabbedPane.addTab(MODIFIERTABTITLE, modifierPanel);
    }

    /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     * Not yet Implemented...
     */
    protected boolean set()
    {
        String modifier = "";
        if(isModifiersTabSetup()) {
            RestreamBoxNode restreamnode = (RestreamBoxNode)node;
            Object[] output = outputModel.toArray();
            for (int i = 0; i < output.length ; i++ ) {
                if (i == output.length - 1) {
                    modifier = modifier.concat (output[i].toString());
                } else {
                    modifier = modifier.concat (output[i].toString() +
						Constants.INNER_MODIFIER_DELIM);
                }
            }
        }

        if(!super.set() || !super.parse(modifier)) {
            return false;
        }

	return true;
    }

}




