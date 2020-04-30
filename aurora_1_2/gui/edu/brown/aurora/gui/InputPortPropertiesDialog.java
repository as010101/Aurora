
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import javax.swing.event.*;
import java.awt.*;
import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;

import java.util.*;


/**
 * A properties dialog for the input port nodes.
 * 
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 08/06/2002
 */
public class InputPortPropertiesDialog 
    extends PortNodePropertiesDialog
{

    private class WorkloadListener implements ChangeListener
    {
        public void stateChanged(ChangeEvent e) 
        {
            setWorkloadState(true);
        }
        
    }

    private class RateOptionsListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
	    if(typeList == null || typeList.getSelectedValue()==null) {
		JOptionPane.showConfirmDialog(null,
					      "Input Stream type undefined." ,
					      "Error",
					      JOptionPane.DEFAULT_OPTION,
					      JOptionPane.ERROR_MESSAGE);
	    } else {
		showRateOptions();
	    }
        }
    }

    private class VarianceOptionsListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
	    if(typeList == null || typeList.getSelectedValue()==null) {
		JOptionPane.showConfirmDialog(null,
					      "Input Stream type undefined." ,
					      "Error",
					      JOptionPane.DEFAULT_OPTION,
					      JOptionPane.ERROR_MESSAGE);
	    } else {
		showVarianceOptions();
	    }
        }
    }


    
    /**
     * Sets up the type for this Portnode.
     */
    public void setupGeneral()
    {


        GridBagLayout bags = new GridBagLayout();
        JPanel generalPanel = new JPanel(bags);
        tabbedPane.addTab("General", generalPanel);
        
        CompositeType[] types = RootFrame.typeManager.getCompositeTypes();

        if (types != null && types.length > 0 && node instanceof GenericPortNode) {
	    GenericPortNode genericPortNode = (GenericPortNode) node;
    	    useDefinedType = genericPortNode.isUseDefinedTypes();
    	    JCheckBox useDefinedTypeCheck = new JCheckBox("Use defined type", useDefinedType);
    	    useDefinedTypeCheck.addChangeListener(new DefinedTypesListener());
	    
	    //sets up the type list
	    typeList = new JList(types);
	    typeList.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    typeList.setSelectedValue(((GenericPortNode)node).getType(), true);
	    typeList.addListSelectionListener(new TypeChangeListener());
	    
	    //sets up the box Id label
	    JLabel boxIdLabel = new JLabel ("Box Id: " + (new Integer (node.getBoxId())).toString()); 
	 
	    addComponent (bags, generalPanel, boxIdLabel, 0,0, 1,1, .01, .01);	
    	    Model model = node.getModel();
    	    SuperBoxNode superBox = model.getSuperBox();
    	    if(superBox.isRootBox()) {
		addComponent(bags, generalPanel, new JLabel ("Type:"), 0,1, 1,1, .01,.01);
		addComponent(bags, generalPanel, typeList, 0,2, 1,1, .8, .8, new Insets(0,20,20,20));
    	    } else {
		addComponent(bags, generalPanel, new JLabel ("Type:"), 0,1, 1,1, .1,.1);
		addComponent(bags, generalPanel, useDefinedTypeCheck, 0,2,1,1,1,1);
		addComponent(bags, generalPanel, typeList, 0,3, 1,1, 1, 7, new Insets(5,20,5,20));
		typeList.setEnabled(useDefinedType);
	    }	    
        }
    }

    private class TypeChangeListener implements ListSelectionListener
    {
        public void valueChanged(ListSelectionEvent e)
        {
            workload.doClick();
            workload.doClick();
        }
    }
    private class DefinedTypesListener implements ChangeListener
    {
        public DefinedTypesListener() {}

        public void stateChanged(ChangeEvent e) 
        {	    
	    useDefinedType = !useDefinedType;
	    typeList.setEnabled(useDefinedType);
        }
        
    }




    /**
     * The Workload Checkbox .
     */
    protected JCheckBox workload=null;
    /**
     * The periodicity.
     */
    protected JComboBox rateCombo = null;
    /**
     * The adjustability combo box.
     */
    protected JComboBox adjustabilityCombo = null;
    /**
     * The number of tuples text field.
     */
    protected JTextField numTuplesTextField = null;
    /**
     * The number of tuples text field.
     */
    protected JTextField numStreamsTextField = null;
    /**
     * The Rate options button
     */
    protected JButton rateOptions = null;
    /**
     * The variance options button
     */
    protected JButton varianceOptions = null;

    private boolean isWorkloadOn;

    /**
     * Constructs a new InputPortPropertiesDialog object.
     *
     * @param node The InputPortNode associated with this 
     * properties dialog.
     */
    public InputPortPropertiesDialog(InputPortNode node)
    {
        super(node, false);
        
        // Query tab section
        setupGeneral();
        setupSourceStream();

        show();
    }



    
    /**
     * Creates swing objects to obtain info about the desired specs for the
     * Workload generator, if it is currently used.
     */
    public void setupSourceStream()
    {
        GridBagLayout bags = new GridBagLayout();
        JPanel srcPanel = new JPanel(bags);

        isWorkloadOn = ((InputPortNode)node).usesWorkload();
        workload = new JCheckBox("Use workload generator", isWorkloadOn);
        addComponent(bags, srcPanel, workload, 1,0, 1,1, 0,1);
        WorkloadListener wfListener = new WorkloadListener();
        workload.addChangeListener(wfListener);
            
        JLabel rate  = new JLabel("Rate Distribution:  ");
        addComponent(bags, srcPanel, rate, 0,1, 1,1, 0, 1);
            
        rateCombo = new JComboBox(new Object[] {"Periodic", "Exponential"});
        if (((InputPortNode)node).isPeriodic()) 
            rateCombo.setSelectedIndex(1);
        else
            rateCombo.setSelectedIndex(0);
        rateCombo.setEditable(false);
        //addComponent(bags, srcPanel, rateCombo, 1,1, 1, 1, 1, 1);

        rateOptions = new JButton("Rate Options");
        addComponent(bags, srcPanel, rateOptions, 1, 1, 1, 1, 1, 1);
        rateOptions.addActionListener(new RateOptionsListener());
        
        JLabel numTuples = new JLabel("Number Of Tuples:  ");
        addComponent(bags, srcPanel, numTuples, 0, 2, 1, 1, 0, 1);
        numTuplesTextField = new JTextField(""+((InputPortNode)node).getNumberOfTuples());
        numTuplesTextField.setEditable(true);
        addComponent(bags, srcPanel, numTuplesTextField, 1, 2, 1, 1, 1, 1);
        
        JLabel numStreams = new JLabel("Number Of Streams:  ");
        addComponent(bags, srcPanel, numStreams, 0, 3, 1, 1, 0, 1);
        numStreamsTextField = new JTextField(""+((InputPortNode)node).getNumberOfStreams());
        numStreamsTextField.setEditable(true);
        addComponent(bags, srcPanel, numStreamsTextField, 1, 3, 1, 1, 1, 1);
        
        srcPanel.setBorder(new LineBorder(Color.black, 1));
        tabbedPane.addTab("Source Stream Specification", srcPanel);

        varianceOptions = new JButton("Attribute Variance Options");
        addComponent(bags, srcPanel, varianceOptions, 1, 4, 1, 1, 1, 1);
        varianceOptions.addActionListener(new VarianceOptionsListener());
        
        this.setWorkloadState(false);
    }

    /**
     * Sets the values of the node associated with this properties dialog.
     * Shows any error messages and returns false if there is a failure, else it
     * returns true and sets all values.
     * @return success of the attempt to set data
     */
    protected boolean set()
    {
        boolean returnValue = true;
        super.set();
	InputPortNode inputNode = (InputPortNode) node;


	inputNode.setUseDefinedTypes(useDefinedType);
	
	if (typeList != null) {
            inputNode.setType( (CompositeType)(typeList.getSelectedValue()));
	}



        try {
            inputNode.setNumberOfTuples(new Integer(numTuplesTextField.getText()).intValue());
        } catch (Exception e) {
            JOptionPane.showConfirmDialog(null, 
                                          e, 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
            returnValue = false;
        }
            
        try {
            inputNode.setNumberOfStreams(new Integer(numStreamsTextField.getText()).intValue());
            if (isWorkloadOn&&(inputNode.getNumberOfStreams()<=0)) {
                JOptionPane.showConfirmDialog(null,
                                              "Number of streams must be positve",
                                              "Error",
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
                returnValue = false;
            }
        } catch (Exception e) {
            JOptionPane.showConfirmDialog(null, 
                                          e, 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
            returnValue = false;
        }

        if(typeList != null && typeList.getSelectedValue() != null) {
            inputNode.setPeriodicity(rateCombo.getSelectedIndex()==1);
            
            inputNode.setUsesWorkload(isWorkloadOn);
            
            inputNode.setColor();
        }
        return returnValue;
    }

    /**
     * Swap the availability of the workload items for editing.
     */
    public void setWorkloadState(boolean swap)
    {
        if (swap) {
            isWorkloadOn = !isWorkloadOn;
            if (typeList!=null&&typeList.getSelectedValue()!=null) {
                ((InputPortNode)node).setType((CompositeType)(typeList.getSelectedValue()));
                ((InputPortNode)node).setUsesWorkload(isWorkloadOn);
            }
        }
        rateCombo.setEnabled(isWorkloadOn);
        rateOptions.setEnabled(isWorkloadOn);
        varianceOptions.setEnabled(isWorkloadOn);
        numTuplesTextField.setEditable(isWorkloadOn);
        numStreamsTextField.setEditable(isWorkloadOn);
    }

    /**
     * Shows a Rate Options dialog according to the current
     * settings for rate.
     */
    public void showRateOptions()
    {
        int rateType;
        new PortNodeRatePropertiesDialog((InputPortNode)node,
                                         ((InputPortNode)node).getFieldSet());
    }
    
    /**
     * Shows a Variance Options dialog according to the current
     * settings for rate.
     */
    public void showVarianceOptions()
    {
        if (typeList!=null&&typeList.getSelectedValue()!=null) {
            ((InputPortNode)node).setType((CompositeType)(typeList.getSelectedValue()));
            new PortNodeVariancePropertiesDialog((InputPortNode)node, 
                                        ((InputPortNode)node).getFieldSet());
        }
    }
    
}
