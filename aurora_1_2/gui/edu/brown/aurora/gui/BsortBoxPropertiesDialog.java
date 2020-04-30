package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for Bsort boxes.  It constructs
 * the Bsortbox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class BsortBoxPropertiesDialog extends UnaryBoxPropertiesDialog {

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

    class CActionListener implements ActionListener {
            public void actionPerformed (ActionEvent e) {
              handleSortByAttributeChange();
            }
    }

    /**
     * The attribute combo box
     */
    private JComboBox sortByAttributeComboBox;

    /**
     * The attributes
     */
    private DefaultListModel attributes;

    /**
     * The input list
     */
    private JList inputList;

    /**
     * The output list
     */
    private JList outputList;

    /**
     * The input model
     */
    private DefaultListModel inputModel;

    /**
     * The list model for the output list
     */
    private DefaultListModel outputModel;

    /**
     * The text area for slack
     */
    protected JTextField slackText;

    /**
     * Constructs a new BsortBoxPropertiesDialog object.
     *
     * @param tm The typemanager contained in the GUI.
     * @param node The box node that this properties dialog is to be opened for.
     */
    public BsortBoxPropertiesDialog(BsortBoxNode node)
    {
        super(node);
        setTitle("Bsort Box Properties Dialog");
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

	BsortBoxNode Bsortnode = (BsortBoxNode)node;

        String[] iNames = getInputTypeAttributeNames();
        String modifier = new String();
        try {
            Parser p = new Parser();
            p.parse(false, Bsortnode);
            if (Constants.VERBOSE) System.out.println(p.getInferredType().toStringInDetail());
            modifier = p.getTransformedString();
        } catch (Exception e) {}
        if(iNames == null ||
	   (Bsortnode.isUseDefinedTypes() && iNames.length == 0)) {
            return;
        }

	GridBagLayout bags = new GridBagLayout();

	StringTokenizer modifierTokenizer = new StringTokenizer(modifier,
								Constants.MODIFIER_DELIM);

        String attributeStr = "";
        String slackStr = "0";
	String groupByStr = "";

	try {
            attributeStr = modifierTokenizer.nextToken();
	    slackStr = modifierTokenizer.nextToken();
            groupByStr = modifierTokenizer.nextToken();
	} catch (Exception e) {}

	StringTokenizer outputTokenizer = new StringTokenizer(groupByStr,
							      Constants.INNER_MODIFIER_DELIM);
	StringTokenizer inputTokenizer = new StringTokenizer(groupByStr,
							     Constants.INNER_MODIFIER_DELIM);

        JPanel attributePanel = new JPanel(bags);
        attributes = new DefaultListModel();
        for (int i = 0; i < iNames.length; i++ ) {
            attributes.addElement (iNames[i]);
        }
        sortByAttributeComboBox = new JComboBox (attributes.toArray());
        if (attributeStr.equals(""))
          sortByAttributeComboBox.setSelectedIndex(0);
        else
          sortByAttributeComboBox.setSelectedItem(attributeStr);

        sortByAttributeComboBox.addActionListener (new CActionListener ());

        JLabel sortAttributeLabel = new JLabel("Sort Attribute");
        addComponent(bags, attributePanel, sortAttributeLabel, 0,0, 1,1, 1,1);
        addComponent(bags, attributePanel, sortByAttributeComboBox, 1,0, 1,1, 1,1);

	//creates the output panel
	JPanel outputPanel = new JPanel (bags);
	outputModel = new DefaultListModel ();
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

        handleSortByAttributeChange();

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


	JPanel bottomPanel = new JPanel(bags);
	slackText = new JTextField(slackStr);
	JLabel slackLabel = new JLabel("Slack");
        addComponent(bags, bottomPanel, slackLabel, 0,0, 1,1, 1,1);
        addComponent(bags, bottomPanel, slackText, 1,0, 1,1, 1,1);

	//adds the components
	modifierPanel = new JPanel(bags);
        addComponent (bags, modifierPanel, attributePanel, 0,0, 2,1, 0,0);
        addComponent (bags, modifierPanel, inputPanel, 0,1, 1,1, 0,7);
	addComponent (bags, modifierPanel, outputPanel, 1,1, 1,1, 0,7);
	addComponent (bags, modifierPanel, bottomPanel, 0,2, 2,1, 0,0);
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
            BsortBoxNode Bsortnode = (BsortBoxNode)node;
            modifier = sortByAttributeComboBox.getSelectedItem().toString();
            modifier = modifier.concat(Constants.MODIFIER_DELIM + slackText.getText() + Constants.MODIFIER_DELIM  );
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

    void handleSortByAttributeChange()
    {
      inputModel.removeAllElements();
      for (int i = 0; i < attributes.size(); i++ ) {
          Object item = attributes.elementAt(i);
          if (!item.equals(sortByAttributeComboBox.getSelectedItem()))
            inputModel.addElement(item);
      }
      for (int i = outputModel.size()-1; i >= 0; i-- ) {
          Object item = outputModel.elementAt(i);
          if (item.equals(sortByAttributeComboBox.getSelectedItem()))
            outputModel.removeElementAt(i);
          else
            for (int j = inputModel.size()-1; j >= 0; j--)
              if (inputModel.elementAt(j).equals(item))
                inputModel.removeElementAt(j);
      }
    }

}




