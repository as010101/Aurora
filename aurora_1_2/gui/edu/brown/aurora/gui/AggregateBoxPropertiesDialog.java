package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for Aggregate boxes.  It constructs
 * the Aggregatebox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class AggregateBoxPropertiesDialog extends UnaryBoxPropertiesDialog {

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
	      if (getSelectedGroupByType() != null && 
		  getSelectedGroupByType().equals (TypeManager.TIMESTAMP_TYPE) &&
		  valuesButton != null && valuesButton.isSelected()) {
		  enableTimeComboBoxes();
	      }  
	      else {
		  disableTimeComboBoxes();
	      }
            }
    }

    /**
     * The text area for slack
     */
    protected JTextField attributeText;

    private String[] iNames;
    /**
     * The sortByAttributes
     */
    private DefaultListModel sortByAttributes;

    /**
     * The aggregate combo box
     */
    private JComboBox aggregateComboBox;

    /**
     * The attribute combo box
     */
    private JComboBox attributeComboBox;

    /**
     * The text area for window size
     */
    protected JTextField windowSizeText;

    /**
     * The combo box for window size
     */
    private JComboBox windowSizeComboBox;

    /**
     * The radio buttons for  for window size
     */
    protected JRadioButton valuesButton;

    protected JRadioButton tuplesButton;

    ButtonGroup bGroup;

    /**
     * The text area for advance
     */
    protected JTextField advanceText;
    
    /**
     * The combo box for advance
     */
    private JComboBox advanceComboBox;
    
    /**
     * The attribute combo box
     */
    private JComboBox sortByAttributeComboBox;

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
     * The text area for slack
     */
    protected JTextField timeoutText;

    /**
     * The possible time options
     */
    private DefaultListModel timeOptions;

    /**
     * Constructs a new AggregateBoxPropertiesDialog object.
     *
     * @param tm The typemanager contained in the GUI.
     * @param node The box node that this properties dialog is to be opened for.
     */
    public AggregateBoxPropertiesDialog(AggregateBoxNode node)
    {
        super(node);
        setTitle("Aggregate Box Properties Dialog");
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

	AggregateBoxNode Aggregatenode = (AggregateBoxNode)node;

        iNames = getInputTypeAttributeNames();
        String modifier = new String();
        try {
            Parser p = new Parser();
            p.parse(false, Aggregatenode);
            if (Constants.VERBOSE) System.out.println(p.getInferredType().toStringInDetail());
            modifier = p.getTransformedString();
        } catch (Exception e) {}
        if(iNames == null ||
	   (Aggregatenode.isUseDefinedTypes() && iNames.length == 0)) {
            return;
        }

	GridBagLayout bags = new GridBagLayout();

	StringTokenizer modifierTokenizer = new StringTokenizer(modifier, "="+Constants.MODIFIER_DELIM);

        String attribute = "";
        String aggregate = "";
        String aggregateAttribute = "";
        String windowSizeStr = "1";
        String windowSizeType = "VALUES";
        String advanceStr = "0";
        String timeoutStr = "0.0";
        String sortByAttributeStr = "";
        String slackStr = "0";
        String groupByStr = "";

	try {
            attribute = modifierTokenizer.nextToken();
            aggregate = modifierTokenizer.nextToken();
            aggregateAttribute = modifierTokenizer.nextToken();
            windowSizeStr = modifierTokenizer.nextToken();
            windowSizeType = modifierTokenizer.nextToken();
            advanceStr = modifierTokenizer.nextToken();
            timeoutStr = modifierTokenizer.nextToken();
            sortByAttributeStr = modifierTokenizer.nextToken();
	    slackStr = modifierTokenizer.nextToken();
            groupByStr = modifierTokenizer.nextToken();
	} catch (Exception e) {}

        String[] aggregateOptions = new String[edu.brown.aurora.gui.types.Parser.aggregateFunctions.length];
        for (int i = 0; i < aggregateOptions.length; i++)
          aggregateOptions[i] = edu.brown.aurora.gui.types.Parser.aggregateFunctions[i].getName();

        aggregateComboBox = new JComboBox (aggregateOptions);
        if (aggregate.equals(""))
          aggregateComboBox.setSelectedIndex(0);
        else
          aggregateComboBox.setSelectedItem (aggregate);

        JPanel attributePanel = new JPanel(bags);

	//sets up the default list model for the time options
	timeOptions = new DefaultListModel();
	timeOptions.addElement ("Secs");
	timeOptions.addElement ("Mins");
	timeOptions.addElement ("Hours");
	timeOptions.addElement ("Days");
	


        attributeText = new JTextField(attribute);

        addComponent(bags, attributePanel, new JLabel("Target Attribute"), 0,0, 1,1, 1,1);
        addComponent(bags, attributePanel, attributeText, 1,0, 1,1, 1,1);

        addComponent(bags, attributePanel, new JLabel("Aggregate Function"), 0,1, 1,1, 1,1);
        addComponent(bags, attributePanel, aggregateComboBox, 1,1, 1,1, 1,1);

        while(aggregateAttribute.indexOf(' ') == 0)
          aggregateAttribute = aggregateAttribute.substring(1, aggregateAttribute.length());
        attributeComboBox = new JComboBox (iNames);
        if (aggregateAttribute.equals(""))
          aggregateAttribute = iNames[0];
        attributeComboBox.setSelectedItem (aggregateAttribute);
        attributeComboBox.addActionListener (new CActionListener ());

        addComponent(bags, attributePanel, new JLabel("Aggregate Attribute"), 0,2, 1,1, 1,1);
        addComponent(bags, attributePanel, attributeComboBox, 1,2, 1,1, 1,1);

	//sets up group by:
        sortByAttributes = new DefaultListModel();
        for (int i = 0; i < iNames.length; i++ ) {
            sortByAttributes.addElement(iNames[i]);
        }

        sortByAttributeComboBox = new JComboBox (sortByAttributes.toArray());
        sortByAttributeComboBox.addItem("TUPLENUM");

        if (sortByAttributeStr.equals("")){
          sortByAttributeComboBox.setSelectedIndex(0);
	}
        else {
          sortByAttributeComboBox.setSelectedItem(sortByAttributeStr);
	}
	
        sortByAttributeComboBox.addActionListener (new CActionListener ());
	
	addComponent(bags, attributePanel, new JLabel("Order on"), 0,3, 1,1, 1,1);
	addComponent(bags, attributePanel, sortByAttributeComboBox, 1,3, 1,1, 1,1);
	
	addComponent(bags, attributePanel, new JLabel("    "), 2,3, 1,1, 1,1);
	
	slackText = new JTextField(slackStr);
	addComponent(bags, attributePanel, new JLabel("Slack"), 3,3, 1,1, 1,1);
        addComponent(bags, attributePanel, slackText, 4,3, 1,1, 1,1);


	//sets up window size
        windowSizeText = new JTextField(windowSizeStr);
        addComponent(bags, attributePanel, new JLabel("Window Size"), 0,4, 1,1, 1,1);
        addComponent(bags, attributePanel, windowSizeText, 1,4, 1,1, 1,1);
	windowSizeComboBox = new JComboBox (timeOptions.toArray());
	addComponent (bags, attributePanel, windowSizeComboBox, 2,4, 1,1, 1,1);
        valuesButton = new JRadioButton("Values");
	valuesButton.addChangeListener (   
	   new ChangeListener() {
		public void stateChanged (ChangeEvent evt)
		{
		    if (!valuesButton.isSelected()) {
			disableTimeComboBoxes();
		    }
		    else {
			if ( getSelectedGroupByType() != null && 
			     getSelectedGroupByType().equals (TypeManager.TIMESTAMP_TYPE)) {
			    enableTimeComboBoxes();
			}
		    }
		}
	    }
	    );
        addComponent(bags, attributePanel, valuesButton, 3,4, 1,1, 1,1);
        tuplesButton = new JRadioButton("Tuples");
        addComponent(bags, attributePanel, tuplesButton, 4,4, 1,1, 1,1);
        bGroup = new ButtonGroup();
        bGroup.add(valuesButton);
        bGroup.add(tuplesButton);

	//sets up the advance
        advanceText = new JTextField(advanceStr);
        addComponent(bags, attributePanel, new JLabel("Advance"), 0,5, 1,1, 1,1);
        addComponent(bags, attributePanel, advanceText, 1,5, 1,1, 1,1);
	advanceComboBox = new JComboBox (timeOptions.toArray());
	addComponent (bags, attributePanel, advanceComboBox, 2,5, 1,1, 1,1);

	//sets the advance and window size extra fields to the correct starting values:
	if (windowSizeType.lastIndexOf("VALUES") >= 0) {
	    bGroup.setSelected(valuesButton.getModel(), true);
	}
        else {
	    bGroup.setSelected(tuplesButton.getModel(), true);
	}


	if (getSelectedGroupByType() != null && 
	    getSelectedGroupByType().equals (TypeManager.TIMESTAMP_TYPE) &&
	    valuesButton != null && valuesButton.isSelected()) {  
	    enableTimeComboBoxes();
	}
	else {
	    disableTimeComboBoxes();
	}


	//sets up the timeout 
        timeoutText = new JTextField(timeoutStr);
        addComponent(bags, attributePanel, new JLabel("Timeout"), 0,6, 1,1, 1,1);
        addComponent(bags, attributePanel, timeoutText, 1,6, 1,1, 1,1);

	//creates the output panel
	JPanel outputPanel = new JPanel (bags);
	outputModel = new DefaultListModel ();
        StringTokenizer outputTokenizer = new StringTokenizer(groupByStr,Constants.INNER_MODIFIER_DELIM);

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


	//adds the components
	modifierPanel = new JPanel(bags);
        addComponent (bags, modifierPanel, attributePanel, 0,0, 2,1, 0,0);
        addComponent (bags, modifierPanel, inputPanel, 0,1, 1,1, 0,7);
	addComponent (bags, modifierPanel, outputPanel, 1,1, 1,1, 0,7);
 	tabbedPane.addTab(MODIFIERTABTITLE, modifierPanel);
	}
	
    /**
     * Returns the primitive type for the currently selected type in the group
     * by combo box
     */
    private PrimitiveType getSelectedGroupByType() 
    {

	AggregateBoxNode Aggregatenode = (AggregateBoxNode)node;
	if (!Aggregatenode.getInputType (0).isPrimitive()) {
	    CompositeType compType = (CompositeType)Aggregatenode.getInputType (0);
	    if (sortByAttributeComboBox.getSelectedIndex() < 
		compType.getNumberOfAttributes()) {
		PrimitiveType primType = (PrimitiveType) compType.getAttributeType 
		    (sortByAttributeComboBox.getSelectedIndex());
		return primType;
	    }
	}
	return null;
    }

	
    /**
     * enables the combo boxes the specify the type of time output
     */
    private void enableTimeComboBoxes()
    {
	windowSizeComboBox.setEnabled (true);
	windowSizeComboBox.setSelectedIndex (0);
	advanceComboBox.setEnabled (true);
	advanceComboBox.setSelectedIndex (0);	
    }

    /**
     * disables the combo boxes the specify the type of time output
     */
    private void disableTimeComboBoxes()
    {
	windowSizeComboBox.setEnabled (false);
	windowSizeComboBox.setSelectedIndex (-1);
	advanceComboBox.setEnabled (false);
	advanceComboBox.setSelectedIndex (-1);	
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
            AggregateBoxNode Aggregatenode = (AggregateBoxNode)node;

            modifier = attributeText.getText()+"="+aggregateComboBox.getSelectedItem().toString()+Constants.MODIFIER_DELIM;
            modifier = modifier.concat(attributeComboBox.getSelectedItem().toString() + Constants.MODIFIER_DELIM  );
	    //adjusts window size if a differnt time unit has been selected
	    String windowSizeModifier;
	    int windowSizeInt;
	    if (windowSizeComboBox.getSelectedIndex() == 1) {
		windowSizeInt =(new Integer (windowSizeText.getText())).intValue() * 60;
		windowSizeModifier = (new Integer (windowSizeInt)).toString();
	    }
	    else if (windowSizeComboBox.getSelectedIndex() == 2) {
		windowSizeInt =(new Integer (windowSizeText.getText())).intValue() * 60 * 60;
		windowSizeModifier = (new Integer (windowSizeInt)).toString();
	    }
	    else if (windowSizeComboBox.getSelectedIndex() == 3) {
		windowSizeInt =(new Integer (windowSizeText.getText())).intValue() *60*60*24;
		windowSizeModifier = (new Integer (windowSizeInt)).toString();
	    }  
	    else {
		windowSizeInt =(new Integer (windowSizeText.getText())).intValue();
		windowSizeModifier = windowSizeText.getText();
	    }

	    if (windowSizeInt <= 0) {
		JOptionPane.showConfirmDialog(null, 
					      "Window size must be a positive number",
					      "Exception" , 
					      JOptionPane.DEFAULT_OPTION, 
					      JOptionPane.ERROR_MESSAGE);
		return false;
	    }
		
            modifier = modifier.concat(windowSizeModifier + Constants.MODIFIER_DELIM);
            if (bGroup.isSelected(valuesButton.getModel()))
              modifier = modifier.concat ("VALUES" + Constants.MODIFIER_DELIM);
            else
              modifier = modifier.concat ("TUPLES" + Constants.MODIFIER_DELIM);	    
	    //adjusts advance if a differnt time unit has been selected
	    String advanceModifier = "";
	    int advanceInt;
	    if (advanceComboBox.getSelectedIndex() == 1) {
		advanceInt = (new Integer ( advanceText.getText())).intValue() * 60 ;
		advanceModifier = (new Integer (advanceInt)).toString();
	    }
	    else if (advanceComboBox.getSelectedIndex() == 2) {
		advanceInt =(new Integer (advanceText.getText())).intValue() *60*60;
		advanceModifier = (new Integer (advanceInt)).toString();
	    }
	    else if (advanceComboBox.getSelectedIndex() == 3) {
		advanceInt =(new Integer (advanceText.getText())).intValue() *60*60*24;
		advanceModifier = (new Integer (advanceInt)).toString();
	    }	   
	    else {
		advanceModifier = advanceText.getText();
	    }
	    
            modifier = modifier.concat(advanceModifier + Constants.MODIFIER_DELIM  );
            modifier = modifier.concat(timeoutText.getText()+Constants.MODIFIER_DELIM);
            modifier = modifier.concat(sortByAttributeComboBox.getSelectedItem().toString() + Constants.MODIFIER_DELIM  );
            modifier = modifier.concat(slackText.getText() + Constants.MODIFIER_DELIM  );
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
      for (int i = 0; i < sortByAttributes.size(); i++ ) {
          Object item = sortByAttributes.elementAt(i);
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




