
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;
import edu.brown.aurora.gui.types.*;


/**
 * A class that implements a properties dialog for resample boxes.  It constructs
 * the resamplebox-specific properties components for the properties dialog.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class ResampleBoxPropertiesDialog extends BinaryBoxPropertiesDialog
{

  JTextField attributeText;

    /**
     * The aggregate combo box
     */
    private JComboBox aggregateComboBox;

    /**
     * The attribute combo box
     */
    private JComboBox attributeComboBox;

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
     * Constructs a new ResampleBoxPropertiesDialog object.
     *
     * @param node The box node that this properties dialog is to be opened for.
     */
    public ResampleBoxPropertiesDialog(ResampleBoxNode node)
    {
        super(node);
        setTitle("Resample Box Properties Dialog");
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

       ResampleBoxNode resampleNode = (ResampleBoxNode)node;

       BoxNodeInputPort leftPort = resampleNode.getLeftInputPort();
       if (leftPort == null) return;
       CompositeType leftType = leftPort.getType();
       if (leftType == null) return;

       BoxNodeInputPort rightPort = resampleNode.getRightInputPort();
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

        StringTokenizer modifierTokenizer = new StringTokenizer(modifier, Constants.MODIFIER_DELIM+"=");

        String attribute = "";
        String aggregate = "";
        String aggregateAttribute = "";
        String windowSizeStr = "1";
        String orderByAttributeLeft = "";
        String slackLeft = "0";
        String orderByAttributeRight = "";
        String slackRight = "0";

       try {
           attribute = modifierTokenizer.nextToken();
           aggregate = modifierTokenizer.nextToken();
           aggregateAttribute = modifierTokenizer.nextToken();
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
       
       String[] aggregateOptions = {"AVG", "COUNT", "MAX", "MIN", "SUM", "LASTVAL", "LASTTWO"};
       aggregateComboBox = new JComboBox (aggregateOptions);
       if (aggregate.equals(""))
	   aggregateComboBox.setSelectedIndex(0);
       else
          aggregateComboBox.setSelectedItem (aggregate);
       
       attributeText = new JTextField(attribute);
       
       addComponent(bags, leftPanel, new JLabel("Target Attribute"), 0,0, 1,1, 1,1);
       addComponent(bags, leftPanel, attributeText, 1,0, 1,1, 1,1);
       
       addComponent(bags, leftPanel, new JLabel("Aggregate Function"), 0,1, 1,1, 1,1);
       addComponent(bags, leftPanel, aggregateComboBox, 1,1, 1,1, 1,1);
       
       while(aggregateAttribute.indexOf(' ') == 0)
	   aggregateAttribute = aggregateAttribute.substring(1, aggregateAttribute.length());
       attributeComboBox = new JComboBox (names);
       if (aggregateAttribute.equals(""))
	   aggregateAttribute = names[0];
       attributeComboBox.setSelectedItem (aggregateAttribute);
       
       addComponent(bags, leftPanel, new JLabel("Aggregate Attribute"), 0,2, 1,1, 1,1);
       addComponent(bags, leftPanel, attributeComboBox, 1,2, 1,1, 1,1);
       
       timeOptions = new DefaultListModel();
       timeOptions.addElement("Secs");
       timeOptions.addElement("Mins");
       timeOptions.addElement("Hours");
       timeOptions.addElement("Days");
       timeOptionsBox = new JComboBox(timeOptions.toArray());
       timeOptionsBox.setSelectedIndex(0);

       windowSizeText = new JTextField(windowSizeStr);
       addComponent(bags, leftPanel, new JLabel("Window Size"), 0,5, 1,1, 1,1);
       addComponent(bags, leftPanel, timeOptionsBox, 1,5, 1,1, 1,1);
       addComponent(bags, leftPanel, windowSizeText, 2,5, 3,1, 1,1);

       orderByAttributesLeft = new DefaultListModel();
       for (int i = 0; i < leftNames.length; i++ ) {
           orderByAttributesLeft.addElement(leftNames[i]);
       }

       orderByAttributeLeftComboBox = new JComboBox (orderByAttributesLeft.toArray());
       if (orderByAttributeLeft.equals(""))
         orderByAttributeLeftComboBox.setSelectedIndex(0);
       else
         orderByAttributeLeftComboBox.setSelectedItem(orderByAttributeLeft);
       orderByAttributeLeftComboBox.addActionListener(new CActionListener());

       addComponent(bags, leftPanel, new JLabel("Order on"), 0,3, 1,1, 1,1);
       addComponent(bags, leftPanel, orderByAttributeLeftComboBox, 1,3, 1,1, 1,1);
       addComponent(bags, leftPanel, new JLabel(" "), 2,3, 1,1, 1,1);
       slackLeftText = new JTextField(slackLeft);
       addComponent(bags, leftPanel, new JLabel("Slack"), 3,3, 1,1, 1,1);
       addComponent(bags, leftPanel, slackLeftText, 4,3, 1,1, 1,1);

       orderByAttributesRight = new DefaultListModel();
       for (int i = 0; i < rightNames.length; i++ ) {
           orderByAttributesRight.addElement(rightNames[i]);
       }

       orderByAttributeRightComboBox = new JComboBox (orderByAttributesRight.toArray());
       if (orderByAttributesRight.equals(""))
         orderByAttributeRightComboBox.setSelectedIndex(0);
       else
         orderByAttributeRightComboBox.setSelectedItem(orderByAttributeRight);
       orderByAttributeRightComboBox.addActionListener(new CActionListener());

       if (isOrderOnTypeTime()) {
	   enableTimeComboBoxes();
       }  
       else {
	   disableTimeComboBoxes();
       }

       addComponent(bags, leftPanel, new JLabel("Order on"), 0,4, 1,1, 1,1);
       addComponent(bags, leftPanel, orderByAttributeRightComboBox, 1,4, 1,1, 1,1);

       addComponent(bags, leftPanel, new JLabel(" "), 2,4, 1,1, 1,1);

       slackRightText = new JTextField(slackRight);
       addComponent(bags, leftPanel, new JLabel("Slack"), 3,4, 1,1, 1,1);
       addComponent(bags, leftPanel, slackRightText, 4,4, 1,1, 1,1);

       modifierPanel = new JPanel(bags);
       Insets padding = new Insets (1,1,1,1);
       addComponent (bags, modifierPanel, leftPanel, 0,0, 1,2, 1,1, padding);
       tabbedPane.addTab(MODIFIERTABTITLE, modifierPanel);

   }


   /**
     * Sets the values of a box node to the values specified
     * in the properties dialog.
     */
    protected boolean set()
    {
      String modifier = "";
      if(isModifiersTabSetup()) {
        ResampleBoxNode resampleNode = (ResampleBoxNode)node;
        modifier = attributeText.getText()+"="+
	    aggregateComboBox.getSelectedItem().toString()+Constants.MODIFIER_DELIM;
        modifier = modifier.concat(attributeComboBox.getSelectedItem().toString() + 
				   Constants.MODIFIER_DELIM  );
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
	ResampleBoxNode resampleNode = (ResampleBoxNode)node;
	if (!resampleNode.getInputType(0).isPrimitive()) {
	    CompositeType compType = (CompositeType)resampleNode.getInputType(0);
	    int index = orderByAttributeLeftComboBox.getSelectedIndex();
	    if (index < compType.getNumberOfAttributes()) {
		PrimitiveType primType = 
		    (PrimitiveType)compType.getAttributeType(index);
		if (primType.equals(TypeManager.TIMESTAMP_TYPE)) {
		    return true;
		}
	    }
	}
	if (!resampleNode.getInputType(1).isPrimitive()) {
	    CompositeType compType = (CompositeType)resampleNode.getInputType(1);
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
    
}
