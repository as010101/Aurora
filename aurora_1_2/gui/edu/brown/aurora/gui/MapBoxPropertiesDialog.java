
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
 * A class that implements a properties dialog for map boxes.  It constructs
 * the mapbox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class MapBoxPropertiesDialog extends UnaryBoxPropertiesDialog
{

    private class AttributeButtonListener implements ActionListener {
	public void actionPerformed (ActionEvent e) {
	    try {
		if (e.getActionCommand().equals ("Add")) {
		    addAttribute ("Attribute" + functionAreas.size(),"", false);
		}else if (e.getActionCommand().equals ("Remove")){
		    int last = functionAreas.size() - 1;
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
	    }else if(e.getActionCommand().equals ("abs")) {
		editingArea.replaceSelection (" abs(  ) ");
		editingArea.setCaretPosition (editingArea.getCaretPosition() - 3);
	    }else if(e.getActionCommand().equals ("<")) {
		editingArea.replaceSelection (" < ");
	    }else if(e.getActionCommand().equals (">")) {
		editingArea.replaceSelection (" > ");
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
     * Constructs a new MapBoxPropertiesDialog object.
     *
     * @param tm The typemanager contained in the GUI.
     * @param node The box node that this properties dialog is to be opened for.
     */
    public MapBoxPropertiesDialog(MapBoxNode node)
    {
        super(node);
        setTitle("Map Box Properties Dialog");
    }

    /** 
     * Returns that this box does need to specify output types
     * @return this box does need to specify output types
     */
    
    public boolean specifyOutput () {
	return true;
    }
    


    /**
     * Sets up the modifier panel for the tabbed pane.
     */
    public void setupModifier()
    {
        super.setupModifier();

	MapBoxNode mapnode = (MapBoxNode)node;

        String[] names = getInputTypeAttributeNames();
        String[] oNames = getOutputTypeAttributeNames();

        if(names == null ||
	   (mapnode.isUseDefinedTypes() && names.length == 0)) {
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
	JLabel attributeLabel = new JLabel ("Tuples");
	addComponent (bags, attributePanel, attributeLabel, 0,0, 1,1, 0,0);
	addComponent (bags, attributePanel, (new JScrollPane (attributeList)), 0,1, 1,1, 1,1);

	//creates the functions panel
	JPanel functionsPanel = new JPanel (bags);
	GridLayout layout = new GridLayout (1,1);
	textAreaPanel = new JPanel(layout);
	functionAreas = new LinkedList();
	attributeNameFields = new LinkedList();
	JScrollPane scrollPane = 
	    new JScrollPane(textAreaPanel,
			    JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
			    JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
	JScrollBar bar = scrollPane.getVerticalScrollBar();
	// Controls how fast the pane scrolls
	bar.setUnitIncrement(20);
	addComponent(bags, functionsPanel, scrollPane, 0,0, 2,1, 1,1);
	if (!isOutputNamed) {
	    AttributeButtonListener buttonListener = new AttributeButtonListener ();
	    JButton addAttribute = new JButton ("Add");
	    addAttribute.addActionListener (buttonListener);
	    JButton removeAttribute = new JButton ("Remove");
	    removeAttribute.addActionListener (buttonListener);
	    addComponent (bags, functionsPanel, addAttribute, 0,1, 1,1, .5,0);
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
	
	StringTokenizer tokenizer =  new StringTokenizer (modifier, "=,");
	if (modifier.equals("") && isOutputNamed) {
	    for (int i = 0; i < oNames.length ;i++ ) {
		addAttribute ( oNames[i], "", isOutputNamed);
	    }
	}
	if (modifier != null) {
	    while (tokenizer.hasMoreElements())
			addAttribute (tokenizer.nextToken(),tokenizer.nextToken(), isOutputNamed );
	}
	
	//adds all the components to the PredicatePanel
	modifierPanel = new JPanel(bags);
	Insets padding = new Insets (10,10,10,10);
	addComponent (bags, modifierPanel, functionsPanel, 0,0, 1,2, 1,1, padding);
	addComponent (bags, modifierPanel, attributePanel, 1,0, 1,1, 0,1, padding);
	addComponent (bags, modifierPanel, createButtonPanel(), 1,1, 1,1, 0,0, padding);
        tabbedPane.addTab(MODIFIERTABTITLE, modifierPanel);
    }

    /**
     * Sets up the modifier window if the output type is specified
     */
    public void setupSpecifiedOutputTypeTab (String[] iNames, String[] oNames) {

	MapBoxNode mapnode = (MapBoxNode)node;
	GridBagLayout bags = new GridBagLayout();

	//creates a JList with the items in the names array
	JList attributeList = new JList (iNames);

	//do not allow multiple selections
	attributeList.setSelectionMode (ListSelectionModel.SINGLE_SELECTION);

	//set up the event handler
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
	JLabel attributeLabel = new JLabel ("Tuples");
	addComponent (bags, attributePanel, attributeLabel, 0,0, 1,1, 0,0);
	addComponent (bags, attributePanel, (new JScrollPane (attributeList)), 0,1, 1,1, 1,1);

	//creates the functions panel
	JPanel functionsPanel = new JPanel (bags);
	GridLayout layout = new GridLayout (1,1);
	textAreaPanel = new JPanel(layout);
	functionAreas = new LinkedList();
	addComponent (bags, functionsPanel, (new JScrollPane (textAreaPanel,
							      JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,
							      JScrollPane.HORIZONTAL_SCROLLBAR_NEVER))
		      , 0,0, 2,1, 1,1);

	//tokenizes the modifiers String
	String modifier = new String();
	try
	    {
		Parser p = new Parser();	
                    p.parse(false, (BoxNode)node);
                    System.out.println(p.getInferredType().toStringInDetail());
		modifier = p.getTransformedString();
	    } catch (Exception e) {}

       	LinkedList modifiers = new LinkedList();
	StringTokenizer tokenizer =  new StringTokenizer (modifier, Constants.INNER_MODIFIER_DELIM);
	if (modifier != null) {
	    try 
		{
		    for (int i = 0; i < oNames.length ; i++ ) {
			if (tokenizer.hasMoreElements()) {
			    addAttribute (tokenizer.nextToken(), oNames[i]);
			} else {
			    addAttribute ("", oNames[i]);
			}
		    }
		} catch (Exception e) {}
	}

	//adds all the components to the PredicatePanel
	modifierPanel = new JPanel(bags);
	Insets padding = new Insets (10,10,10,10);
	addComponent (bags, modifierPanel, functionsPanel, 0,0, 1,2, 1,1, padding);
	addComponent (bags, modifierPanel, attributePanel, 1,0, 1,1, 0,1, padding);
	addComponent (bags, modifierPanel, createButtonPanel(), 1,1, 1,1, 0,0, padding);
        tabbedPane.addTab("Modifier", modifierPanel);

    }

    private void addAttribute (String attributeName, String text, boolean isOutputNamed) {
	try {
	    GridBagLayout bags = new GridBagLayout();
	    Insets padding = new Insets (4,0,2,0);
	    JPanel tempPanel = new JPanel (bags);
	    JTextField tempTextField = new JTextField (attributeName, 12);
	    if (isOutputNamed) {
		tempTextField.setEditable (false);
	    }
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
    
    private void addAttribute (String text, String attributeName) {
	try {
	    GridBagLayout bags = new GridBagLayout();
	    Insets padding = new Insets (4,0,2,0);
	    JPanel tempPanel = new JPanel (bags);
	    addComponent (bags, tempPanel, ( new JLabel (attributeName)),
			  0,0,1,1,0,0, padding);
	    JTextArea tempTextArea = new JTextArea();
	    tempTextArea.setRows (3);
	    tempTextArea.addFocusListener (new TextAreaFocusListener());
	    tempTextArea.setLineWrap (true);
	    tempTextArea.setText (text);
	    functionAreas.add (tempTextArea);
	    padding = new Insets (2,0,4,0);
	    addComponent (bags, tempPanel, tempTextArea, 0,1, 1,1, 1,0, padding);
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
                    modifier = modifier.concat (((JTextField)attributeNameFields.get(i)).getText() +
                                                "=" + ((JTextArea)functionAreas.get(i)).getText());
                } else {
                    modifier = modifier.concat (((JTextField)attributeNameFields.get(i)).getText() +
                                                "=" + ((JTextArea)functionAreas.get(i)).getText() +
                                                Constants.INNER_MODIFIER_DELIM);
                }
            }
        }

        if(!super.set() || !super.parse(modifier)) {
            return false;
        }
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
	JButton absoluteButton = new JButton ("abs");
	absoluteButton.addActionListener (pActionListener);
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
	addComponent (bags, buttonPanel, absoluteButton, 1,6, 1,1, 0,0, padding);
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







