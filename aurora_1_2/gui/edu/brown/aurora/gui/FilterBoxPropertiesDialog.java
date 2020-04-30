
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
 * A class that implements a properties dialog for filter boxes.  It constructs
 * the filterbox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class FilterBoxPropertiesDialog extends UnaryBoxPropertiesDialog
{

  ButtonGroup bGroup;
  JRadioButton dropButton;
  JRadioButton passButton;

    private class AttributeButtonListener implements ActionListener {
	public void actionPerformed (ActionEvent e) {
	    try {
		if (e.getActionCommand().equals ("Add")) {
		    addOutput ("Output" + functionAreas.size(),"");
		}else if (e.getActionCommand().equals ("Remove")){
		    int last = functionAreas.size() - 1;
                    if (last == 0) return;
		    textAreaPanel.remove (last);
		    functionAreas.removeLast();
		    attributeNameFields.removeLast();
		    textAreaPanel.repaint();
		}
	    } catch ( ArrayIndexOutOfBoundsException ex) {}
	}
    }

    private class PredicateActionListener implements ActionListener {
	public void actionPerformed (ActionEvent e) {
	    if (editingArea == null) {
		return;
	    }else if(e.getActionCommand().equals ("AND")) {
		editingArea.replaceSelection (" AND(  ,  ) ");
		editingArea.setCaretPosition (editingArea.getCaretPosition() - 6);
	    }else if(e.getActionCommand().equals ("OR")) {
		editingArea.replaceSelection (" OR(  ,  ) ");
		editingArea.setCaretPosition (editingArea.getCaretPosition() - 6);
	    }else if(e.getActionCommand().equals ("NOT")) {
		editingArea.replaceSelection (" NOT(  ) ");
		editingArea.setCaretPosition (editingArea.getCaretPosition() - 3);
	    }else if(e.getActionCommand().equals ("+")) {
		editingArea.replaceSelection (" + ");
	    }else if(e.getActionCommand().equals ("-")) {
		editingArea.replaceSelection (" - ");
	    }else if(e.getActionCommand().equals ("*")) {
		editingArea.replaceSelection (" * ");
	    }else if(e.getActionCommand().equals ("/")) {
		editingArea.replaceSelection (" / ");
	    }else if(e.getActionCommand().equals ("<")) {
		editingArea.replaceSelection (" < ");
	    }else if(e.getActionCommand().equals (">")) {
		editingArea.replaceSelection (" > ");
		/*
		  }else if(e.getActionCommand().equals ("GEQ")) {
		  editingArea.replaceSelection (" GEQ ");
		  }else if(e.getActionCommand().equals ("LEQ")) {
		  editingArea.replaceSelection (" LEQ ");
		  }else if(e.getActionCommand().equals ("NEQ")) {
		  editingArea.replaceSelection (" NEQ ");
		*/
	    }else if(e.getActionCommand().equals (">=")) {
		editingArea.replaceSelection (" >= ");
	    }else if(e.getActionCommand().equals ("<=")) {
		editingArea.replaceSelection (" <= ");
	    }else if(e.getActionCommand().equals ("!=")) {
		editingArea.replaceSelection (" != ");
	    }else if(e.getActionCommand().equals ("=")) {
		editingArea.replaceSelection (" = ");
	    }
	    editingArea.grabFocus();
	}
    }

    private class TextAreaFocusListener extends FocusAdapter {
	public void focusGained (FocusEvent e) {
	    editingArea = (JTextArea) e.getSource();
	}
    }



    /**
     * The text area that keeps track of the last text area to have focus
     */
    JTextArea editingArea;

    /**
     * The LinkedList of JTextAreas used to keep track of the functions for each attribute
     */
    LinkedList functionAreas;

    /**
     * The LinkedList of JTextFields used to keep track of the attribute names
     */
    LinkedList attributeNameFields;

    /**
     * The JPanel that contains the output attribute functions
     */
    JPanel textAreaPanel;

    /**
     * Constructs a new FilterBoxPropertiesDialog object.
     *
     * @param node The box node that this properties dialog is to be opened for.
     */
    public FilterBoxPropertiesDialog(FilterBoxNode node)
    {
        super(node);
        setTitle("Filter Box Properties Dialog");
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

        FilterBoxNode filternode = (FilterBoxNode)node;

        String[] names = getInputTypeAttributeNames();
        String s = getParsedModifier();

        if(names == null ||
	   (filternode.isUseDefinedTypes() && names.length == 0)) {
            return;
        }

 GridBagLayout bags = new GridBagLayout();


	//creates a JList with the items in the names array
	JList attributeList = new JList (names);
	attributeList.setSelectionMode (ListSelectionModel.SINGLE_SELECTION);
	attributeList.addMouseListener (
	    new MouseAdapter(){
		public void mouseReleased (MouseEvent e)
		{
		    try {
			editingArea.replaceSelection (((JList)e.getSource())
						      .getSelectedValue().toString());
			editingArea.grabFocus();
		    } catch (NullPointerException npe) {}
		}
	    }
	    );

	//creates the panel that contains the attribute list
	JPanel attributePanel = new JPanel (bags);
	JLabel attributeLabel = new JLabel ("Attributes");
	addComponent (bags, attributePanel, attributeLabel, 0,0, 1,1, 0,0);
	addComponent (bags, attributePanel, (new JScrollPane (attributeList)), 0,1, 1,1, 1,1);

	//creates the functions panel
	JPanel functionsPanel = new JPanel (bags);
	GridLayout layout = new GridLayout (1,1);
	textAreaPanel = new JPanel(layout);
	functionAreas = new LinkedList();
	attributeNameFields = new LinkedList();
	addComponent (bags, functionsPanel, (new JScrollPane (textAreaPanel,
							      JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
							      JScrollPane.HORIZONTAL_SCROLLBAR_NEVER))
		      , 0,0, 2,1, 1,1);

        JPanel radioPanel = new JPanel(bags);
        dropButton = new JRadioButton("Drop");
        passButton = new JRadioButton("Pass on 'FALSE' stream");
        bGroup = new ButtonGroup();
        bGroup.add(dropButton);
        bGroup.add(passButton);
        radioPanel.add(dropButton);
        radioPanel.add(passButton);
	dropButton.setSelected(true);

	if (!isOutputNamed) {
	    AttributeButtonListener buttonListener = new AttributeButtonListener ();
	    JButton addAttributeButton = new JButton ("Add");
	    addAttributeButton.addActionListener (buttonListener);
	    JButton removeAttribute = new JButton ("Remove");
	    removeAttribute.addActionListener (buttonListener);
	    addComponent (bags, functionsPanel, addAttributeButton, 0,1, 1,1, .5,0);
	    addComponent (bags, functionsPanel, removeAttribute, 1,1, 1,1, .5,0);
	}

	//tokenizes the modifiers String
	String modifier = new String();
	try {
            Parser p = new Parser();
            p.parse(false, (BoxNode)node);
            if (Constants.VERBOSE) System.out.println(p.getInferredType().toStringInDetail());
            modifier = p.getTransformedString();
        } catch (Exception e) {}

	if (modifier != null ) {
	StringTokenizer tokenizer1 =  new StringTokenizer (modifier, "~");
          if (tokenizer1.hasMoreElements()) {
            int i = 0;
            StringTokenizer tokenizer =  new StringTokenizer (tokenizer1.nextToken(), "&");
	    while (tokenizer.hasMoreElements())
            {
              addOutput ("Output"+i,tokenizer.nextToken());
              i++;
            }
            String str = tokenizer1.nextToken();
            if (str.lastIndexOf("DROP") >= 0)
              bGroup.setSelected(dropButton.getModel(), true);
            else
              bGroup.setSelected(passButton.getModel(), true);
    }
    else
    {
      addOutput("Output0", "");
    }
	}

	//adds all the components to the PredicatePanel
	modifierPanel = new JPanel(bags);
	Insets padding = new Insets (10,10,10,10);
	addComponent (bags, modifierPanel, functionsPanel,      0,0, 1,3, 1,1, padding);
	addComponent (bags, modifierPanel, attributePanel,      1,0, 1,1, 0,1, padding);
        addComponent (bags, modifierPanel, createButtonPanel(), 1,1, 1,1, 0,0, padding);
        addComponent (bags, modifierPanel, radioPanel,          1,2, 1,1, 0,0, padding);
        tabbedPane.addTab(MODIFIERTABTITLE, modifierPanel);
    }


    private void addOutput (String attributeName, String text) {
	try {
	    GridBagLayout bags = new GridBagLayout();
	    Insets padding = new Insets (4,0,2,0);
	    JPanel tempPanel = new JPanel (bags);
	    JTextField tempTextField = new JTextField (attributeName, 12);
      tempTextField.setEditable (false);
	    attributeNameFields.add (tempTextField);
	    addComponent (bags, tempPanel, tempTextField, 0,0,1,1,0,0, padding);
	    addComponent (bags, tempPanel, (new JLabel ("=")), 1,0, 1,1, 0,0);
	    JTextArea tempTextArea = new JTextArea();
	    tempTextArea.setRows (3);
	    tempTextArea.addFocusListener (new TextAreaFocusListener());
	    tempTextArea.setLineWrap (true);
	    tempTextArea.setText (text);
	    functionAreas.add (tempTextArea);
	    padding = new Insets (2,0,4,0);
	    addComponent (bags, tempPanel, tempTextArea, 0,1, 2,1, 1,0, padding);
	    ((GridLayout)textAreaPanel.getLayout())
		.setRows (((GridLayout)textAreaPanel.getLayout()).getRows() + 1);
	    textAreaPanel.add (tempPanel);
	    textAreaPanel.getRootPane().repaint();
	    tempTextArea.grabFocus();
	} catch (NullPointerException ex) {}
    }

    /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     */
    protected boolean set()
    {
        String modifier = "";
        if(isModifiersTabSetup()) {
            for (int i = 0; i < functionAreas.size() ; i++ ) {
                if (i == functionAreas.size() - 1) {
                    modifier = modifier.concat (((JTextArea)functionAreas.get(i)).getText());
                } else {
                    modifier = modifier.concat (((JTextArea)functionAreas.get(i)).getText() +
                                                "&");
                }
            }
            if (bGroup.isSelected(dropButton.getModel()))
              modifier = modifier.concat (""+Constants.MODIFIER_DELIM+"DROP");
            else
              modifier = modifier.concat (""+Constants.MODIFIER_DELIM+"PASS");
        }
        if(!super.set() || !super.parse(modifier)) {
            return false;
        }
        PrimitiveBoxNode primBox = (PrimitiveBoxNode) node;
        Parser p = RootFrame.parser;
        try
        {
          p.parse(false, primBox);
        } catch (Exception e) {}
        int numOutputs = ((Integer)(p.getParsedObject())).intValue();
        BoxComponent box = (BoxComponent)(node.getNodeComponent());
        int currentNumOutputs = box.getNumOutputs();
        if (currentNumOutputs > numOutputs)
          for (int i = currentNumOutputs-1; i >= numOutputs; i--)
          {
            BoxNodeOutputPort outputPort = box.getOutputPort(i);
            java.util.Vector v = outputPort.getArcs();
            for (int j = 0; j < v.size(); j++)
              node.getModel().removeArc((GenericArc)(v.elementAt(j)));
            box.removeOutputPort(i);
          }
        else
          for (int i = currentNumOutputs; i < numOutputs; i++)
            box.addOutputPort();
        return true;
    }



    /**
     * Creates a new button panel
     */

    private JPanel createButtonPanel () {
	GridBagLayout bags = new GridBagLayout ();
	Insets padding = new Insets (0,2,0,2);
	PredicateActionListener pActionListener = new PredicateActionListener();
	JPanel buttonPanel = new JPanel (bags);

	JLabel predicatesLabel = new JLabel ("Predicates");
	JLabel arithmeticLabel = new JLabel ("Arithmetic");
	JLabel arithmeticOperatorsLabel = new JLabel ("Operators");
	JLabel relationalLabel = new JLabel ("Relational");
	JLabel relationalOperatorsLabel = new JLabel ("Operators");
	JButton andButton = new JButton ("AND");
	andButton.addActionListener (pActionListener);
	JButton orButton = new JButton ("OR");
	orButton.addActionListener (pActionListener);
	JButton notButton = new JButton ("NOT");
	notButton.addActionListener (pActionListener);
	JButton plusButton = new JButton ("+");
	plusButton.addActionListener (pActionListener);
	JButton minusButton = new JButton ("-");
	minusButton.addActionListener (pActionListener);
	JButton multiplyButton = new JButton ("*");
	multiplyButton.addActionListener (pActionListener);
	JButton divideButton = new JButton ("/");
	divideButton.addActionListener (pActionListener);
	JButton greaterThanButton = new JButton (">");
	greaterThanButton.addActionListener (pActionListener);
	JButton lessThanButton = new JButton ("<");
	lessThanButton.addActionListener (pActionListener);
	//JButton GEQButton = new JButton ("GEQ");
	JButton GEQButton = new JButton (">=");
	GEQButton.addActionListener (pActionListener);
	//JButton LEQButton = new JButton ("LEQ");
	JButton LEQButton = new JButton ("<=");
	LEQButton.addActionListener (pActionListener);
	//JButton NEQButton = new JButton ("NEQ");
	JButton NEQButton = new JButton ("!=");
	NEQButton.addActionListener (pActionListener);
	JButton equalsButton = new JButton ("=");
	equalsButton.addActionListener (pActionListener);
	addComponent (bags, buttonPanel, predicatesLabel, 0,0, 1,2, 0,0, padding);
	addComponent (bags, buttonPanel, andButton, 0,2, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, orButton, 0,3, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, notButton, 0,4, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, arithmeticLabel, 1,0, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, arithmeticOperatorsLabel, 1,1, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, plusButton, 1,2, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, minusButton, 1,3, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, multiplyButton, 1,4, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, divideButton, 1,5, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, relationalLabel, 2,0, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, relationalOperatorsLabel, 2,1, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, greaterThanButton, 2,2, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, lessThanButton, 2,3, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, GEQButton, 2,4, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, LEQButton, 2,5, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, NEQButton, 2,6, 1,1, 0,0, padding);
	addComponent (bags, buttonPanel, equalsButton, 2,7, 1,1, 0,0, padding);

	return buttonPanel;

    }


}



