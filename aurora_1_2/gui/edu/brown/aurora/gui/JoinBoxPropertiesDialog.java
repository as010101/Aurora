
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for join boxes.  It constructs
 * the filterbox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */

public class JoinBoxPropertiesDialog extends BinaryBoxPropertiesDialog
{

  JTextArea editingArea;

  JTextField windowSizeText;

  /**
   * The sortByAttributes
   */
  private DefaultListModel orderByAttributesLeft;

  /**
   * The attribute combo box
   */
  private JComboBox orderByAttributeLeftComboBox;

  private JTextField slackLeftText;

  /**
   * The sortByAttributes
   */
  private DefaultListModel orderByAttributesRight;

  /**
   * The attribute combo box
   */
  private JComboBox orderByAttributeRightComboBox;

  private JTextField slackRightText;

  /**
   * The possible time options
   */
  private DefaultListModel timeOptions;  

  private JComboBox timeOptionsBox;

    /**
     * Constructs a new JoinBoxPropertiesDialog object
     *
     * @param node The JoinBoxNode that this properties dialog should be opened
     * up for.
     */

    public JoinBoxPropertiesDialog(JoinBoxNode node) {
        super(node);
        setTitle("Join Box Properties Dialog");
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

       JoinBoxNode joinNode = (JoinBoxNode)node;

       BoxNodeInputPort leftPort = joinNode.getLeftInputPort();
       if (leftPort == null) return;
       CompositeType leftType = leftPort.getType();
       if (leftType == null) return;

       BoxNodeInputPort rightPort = joinNode.getRightInputPort();
       if (rightPort == null) return;
       CompositeType rightType = rightPort.getType();
       if (rightType == null) return;

       String[] leftNames = leftType.getAttributeNames();
       if (leftNames == null) return;

       String[] rightNames = rightType.getAttributeNames();
       if (rightNames == null) return;

       String[] names = new String[leftNames.length+rightNames.length];
       for (int i = 0; i < leftNames.length; i++)
         names[i] = "LEFT."+leftNames[i];
       for (int i = 0; i < rightNames.length; i++)
         names[leftNames.length+i] = "RIGHT."+rightNames[i];

       GridBagLayout bags = new GridBagLayout();

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
       }

       StringTokenizer modifierTokenizer = new StringTokenizer(modifier, Constants.MODIFIER_DELIM);

       String predicate = "";
       String windowSizeStr = "1";
       String orderByAttributeLeft = "";
       String slackLeft = "0";
       String orderByAttributeRight = "";
       String slackRight = "0";

       try {
           predicate = modifierTokenizer.nextToken();
           windowSizeStr = modifierTokenizer.nextToken();
           orderByAttributeLeft = modifierTokenizer.nextToken();
           int index = orderByAttributeLeft.indexOf('.');
           if (index >= 0)
             orderByAttributeLeft = orderByAttributeLeft.substring(index+1);
           slackLeft = modifierTokenizer.nextToken();
           orderByAttributeRight = modifierTokenizer.nextToken();
           index = orderByAttributeRight.indexOf('.');
           if (index >= 0)
             orderByAttributeRight = orderByAttributeRight.substring(index+1);
           slackRight = modifierTokenizer.nextToken();
       } catch (Exception e) {}

       JPanel leftPanel = new JPanel(bags);

       addComponent(bags, leftPanel, new JLabel("Predicate"), 0,0, 5,1, 1,1);
       editingArea = new JTextArea(predicate);
       editingArea.setRows (3);
       editingArea.setLineWrap (true);
       addComponent(bags, leftPanel, editingArea, 0,1, 5,4, 1,1);

       timeOptions = new DefaultListModel();
       timeOptions.addElement("Secs");
       timeOptions.addElement("Mins");
       timeOptions.addElement("Hours");
       timeOptions.addElement("Days");
       timeOptionsBox = new JComboBox(timeOptions.toArray());
       timeOptionsBox.setSelectedIndex(0);

       windowSizeText = new JTextField(windowSizeStr);
       addComponent(bags, leftPanel, new JLabel("Window Size"), 0,7, 2,1, 1,1);
       addComponent(bags, leftPanel, timeOptionsBox, 1,7, 1,1, 1,1, 
		    new Insets(0,0,15,0));
       addComponent(bags, leftPanel, windowSizeText, 2,7, 3,1, 1,1,
		    new Insets(0,0,15,0));

       orderByAttributesLeft = new DefaultListModel();
       for (int i = 0; i < leftNames.length; i++ ) {
           orderByAttributesLeft.addElement(leftNames[i]);
       }

       orderByAttributeLeftComboBox = new JComboBox (orderByAttributesLeft.toArray());
       if (orderByAttributeLeft.equals("")) {
         orderByAttributeLeftComboBox.setSelectedIndex(0);
       }
       else {
         orderByAttributeLeftComboBox.setSelectedItem(orderByAttributeLeft); 
       }
       orderByAttributeLeftComboBox.addActionListener(new CActionListener());

       addComponent(bags, leftPanel, new JLabel("Order on"), 0,5, 1,1, 1,1);
       addComponent(bags, leftPanel, orderByAttributeLeftComboBox, 1,5, 1,1, 1,1);
       addComponent(bags, leftPanel, new JLabel(" "), 2,5, 1,1, 1,1);
       slackLeftText = new JTextField(slackLeft);
       addComponent(bags, leftPanel, new JLabel("Slack"), 3,5, 1,1, 1,1);
       addComponent(bags, leftPanel, slackLeftText, 4,5, 1,1, 1,1);

       orderByAttributesRight = new DefaultListModel();
       for (int i = 0; i < rightNames.length; i++ ) {
           orderByAttributesRight.addElement(rightNames[i]);
       }

       orderByAttributeRightComboBox = new JComboBox (orderByAttributesRight.toArray());
       if (orderByAttributesRight.equals("")) {
         orderByAttributeRightComboBox.setSelectedIndex(0);
       }
       else {
         orderByAttributeRightComboBox.setSelectedItem(orderByAttributeRight);
       }
       orderByAttributeRightComboBox.addActionListener(new CActionListener());

       if (isOrderOnTypeTime()) {
	   enableTimeComboBoxes();
       }  
       else {
	   disableTimeComboBoxes();
       }

       addComponent(bags, leftPanel, new JLabel("Order on"), 0,6, 1,1, 1,1);
       addComponent(bags, leftPanel, orderByAttributeRightComboBox, 1,6, 1,1, 1,1);
       addComponent(bags, leftPanel, new JLabel(" "), 2,6, 1,1, 1,1);
       slackRightText = new JTextField(slackRight);
       addComponent(bags, leftPanel, new JLabel("Slack"), 3,6, 1,1, 1,1);
       addComponent(bags, leftPanel, slackRightText, 4,6, 1,1, 1,1);

       modifierPanel = new JPanel(bags);
       Insets padding = new Insets (1,1,1,1);
       addComponent (bags, modifierPanel, leftPanel,      0,0, 1,2, 1,1, padding);
       addComponent (bags, modifierPanel, createAttributePanel(names), 1,0, 1,1, 1,1, padding);
       addComponent (bags, modifierPanel, createButtonPanel(), 1,1, 1,1, 1,1, padding);
       tabbedPane.addTab(MODIFIERTABTITLE, modifierPanel);

   }

    
    /**
     * enables the combo boxes the specify the type of time output
     */
    private void enableTimeComboBoxes()
    {
	timeOptionsBox.setEnabled (true);
	timeOptionsBox.setSelectedIndex (0);	
    }

    /**
     * disables the combo boxes the specify the type of time output
     */
    private void disableTimeComboBoxes()
    {
	timeOptionsBox.setEnabled (false);
	timeOptionsBox.setSelectedIndex (-1);	
    }    


    /**
     * Returns the primitive type for the currently selected type in the group
     * by combo box
     */
    private boolean isOrderOnTypeTime() 
    {
	JoinBoxNode joinNode = (JoinBoxNode)node;
	if (!joinNode.getInputType(0).isPrimitive()) {
	    CompositeType compType = (CompositeType)joinNode.getInputType(0);
	    int index = orderByAttributeLeftComboBox.getSelectedIndex();
	    if (index < compType.getNumberOfAttributes()) {
		PrimitiveType primType = 
		    (PrimitiveType)compType.getAttributeType(index);
		if (primType.equals(TypeManager.TIMESTAMP_TYPE)) {
		    return true;
		}
	    }
	}
	if (!joinNode.getInputType(1).isPrimitive()) {
	    CompositeType compType = (CompositeType)joinNode.getInputType(1);
	    int index = orderByAttributeRightComboBox.getSelectedIndex();
	    if (index < compType.getNumberOfAttributes()) {
		PrimitiveType primType = 
		    (PrimitiveType)compType.getAttributeType(index);
		if (primType.equals(TypeManager.TIMESTAMP_TYPE)) {
		    return true;
		}
	    }
	}
	return false;
    }
    

   /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     */
    protected boolean set()
    {
      String modifier = "";
      if(isModifiersTabSetup()) {
        JoinBoxNode joinNode = (JoinBoxNode)node;
        modifier = editingArea.getText() + Constants.MODIFIER_DELIM;

	String windowSizeModifier;
	int windowSizeInt = (new Integer(windowSizeText.getText())).intValue();
	if (timeOptionsBox.getSelectedIndex() == 1) {
	    windowSizeInt = windowSizeInt * 60;
	    windowSizeModifier = (new Integer(windowSizeInt)).toString();
	}
	else if (timeOptionsBox.getSelectedIndex() == 2) {
	    windowSizeInt = windowSizeInt * 60 * 60;
	    windowSizeModifier = (new Integer(windowSizeInt)).toString();
	}
	else if (timeOptionsBox.getSelectedIndex() == 3) {
	    windowSizeInt = windowSizeInt * 60 * 60 * 24;
	    windowSizeModifier = (new Integer (windowSizeInt)).toString();
	}  
	else {
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
        modifier = 
	    modifier.concat("LEFT."+
			    orderByAttributeLeftComboBox.getSelectedItem().toString()
			    + Constants.MODIFIER_DELIM  );
        modifier = modifier.concat(slackLeftText.getText() + 
				   Constants.MODIFIER_DELIM  );
        modifier = 
	    modifier.concat("RIGHT."+
			    orderByAttributeRightComboBox.getSelectedItem().toString()
			    + Constants.MODIFIER_DELIM  );
        modifier = modifier.concat(slackRightText.getText());
      }
      if(!super.set() || !super.parse(modifier)) {
          return false;
      }
      return true;
    }


    private JPanel createAttributePanel(String[] attributeNames) {
      GridBagLayout bags = new GridBagLayout ();
      Insets padding = new Insets (0,2,0,2);
      JList attributeList = new JList (attributeNames);
      attributeList.setSelectionMode (ListSelectionModel.SINGLE_SELECTION);
      attributeList.addMouseListener ( new MouseAdapter(){
        public void mouseReleased (MouseEvent e) {
            try {
                editingArea.replaceSelection (((JList)e.getSource()).getSelectedValue().toString());
                editingArea.grabFocus();
            } catch (NullPointerException npe) {}
        }
      });
      JPanel attributePanel = new JPanel (bags);
      JScrollPane attributeListScroll = new JScrollPane();
      attributeListScroll.setViewportView(attributeList);
      addComponent (bags, attributePanel, new JLabel ("Attributes"), 0,0, 1,1, 1, 1);
      //addComponent (bags, attributePanel, attributeList, 0,1, 1,1, 1, 1);
      addComponent (bags, attributePanel, attributeListScroll, 0, 1, 1, 1, 1, 1);
      return attributePanel;
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
    
    class CActionListener implements ActionListener {
	public void actionPerformed (ActionEvent e) {
	    if (isOrderOnTypeTime()) {
		enableTimeComboBoxes();
	    }  
	    else {
		disableTimeComboBoxes();
	    }
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
	    }else if(e.getActionCommand().equals (">=")) {
                editingArea.replaceSelection (" >= ");
            }else if(e.getActionCommand().equals ("<=")) {
                editingArea.replaceSelection (" <= ");
            }else if(e.getActionCommand().equals ("!=")) {
                editingArea.replaceSelection (" != ");
            }else if(e.getActionCommand().equals ("=")) {
                editingArea.replaceSelection (" = ");
            }
	    /*
	      }else if(e.getActionCommand().equals ("GEQ")) {
	      editingArea.replaceSelection (" GEQ ");
	      }else if(e.getActionCommand().equals ("LEQ")) {
	      editingArea.replaceSelection (" LEQ ");
	      }else if(e.getActionCommand().equals ("NEQ")) {
	      editingArea.replaceSelection (" NEQ ");
	      }
	    */
            editingArea.grabFocus();
        }
    }


}
