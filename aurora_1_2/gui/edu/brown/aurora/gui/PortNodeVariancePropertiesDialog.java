
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
 * A properties dialog for the port nodes.
 * 
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class PortNodeVariancePropertiesDialog 
    extends GenericNodePropertiesDialog implements ActionListener
{

    private class FieldSelectionListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            swapField();
        }
    }

    private class VarianceSelectionListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            swapVariance();
        }
    }

    private class DistributionButtonListener implements ActionListener
    {
        private int buttonNum;
        public DistributionButtonListener(int button)
        {
            buttonNum = button;
        }
        public void actionPerformed(ActionEvent e)
        {
            showDistribution(buttonNum);
        }
    }

    /**
     * The select box of Fields
     */
    protected JComboBox fieldCombo;

    /**
     * The select box of Variance
     */
    protected JComboBox varianceCombo;

    /**
     * The list of labels
     */
    protected JLabel[] labels;

    /**
     * The list of text fields
     */
    protected JButton[] distbuttons;

    private Field previous;
    private FieldSet currentFieldSet;

    private boolean first;

    /**
     * Constructs a new PortNodeVariancePropertiesDialog object.
     *
     * @param node The InputPortNode associated with this 
     * @param fieldset The set of fields to modify
     * properties dialog.
     */
    public PortNodeVariancePropertiesDialog(InputPortNode node, FieldSet fieldset)
    {
        super(node);
	first = true;

        labels = new JLabel[5];
        distbuttons = new JButton[5];
        currentFieldSet = fieldset;
        previous = null;
        // Query tab section
        setupFieldInfo();
	first = false;
        show();
    }

    /**
     * sets up the layout of the panel
     */
    public void setupFieldInfo()
    {
        GridBagLayout bags = new GridBagLayout();
        JPanel srcPanel = new JPanel(bags);

        JLabel info = new JLabel("Attribute: ");
        addComponent(bags, srcPanel, info, 0,0, 1,1, 0,1);

        fieldCombo = new JComboBox(currentFieldSet.getFieldIds());
        addComponent(bags, srcPanel, fieldCombo, 1,0,1,1,1,1);
        
        JLabel varianceLabel = new JLabel("Variance: ");
        addComponent(bags, srcPanel, varianceLabel, 0, 1, 1, 1, 0, 1);

        varianceCombo = new JComboBox(new Object[] {"Constant", "Uniform", "Normal", "Exponential", "Linear", "Random Walk", "Random Directional Walk", "Timestamp"} );
        addComponent(bags, srcPanel, varianceCombo, 1, 1, 1, 1, 1, 1);

        for (int i=0; i<5; ++i) {
            labels[i] = new JLabel("Label"+i);
            addComponent(bags, srcPanel, labels[i], 0, i+2,1,1,0,1);
            
            distbuttons[i] = new JButton("Button"+i);
            addComponent(bags, srcPanel, distbuttons[i], 1,i+2,1,1,1,1);
        }

        srcPanel.setBorder(new LineBorder(Color.black, 1));
        tabbedPane.addTab("Attribute Variance Options", srcPanel);

        fieldCombo.addActionListener(new FieldSelectionListener());
        varianceCombo.addActionListener(new VarianceSelectionListener());

        for (int j=0; j<5; ++j) {
            distbuttons[j].addActionListener(new DistributionButtonListener(j));
        }

        swapField();
        swapVariance();
    }

    /**
     * Swaps the currently selected field
     */
    public void swapField()
    {
        int curvar = varianceCombo.getSelectedIndex();
        boolean getvals = true;
        if (previous!=null) {
            previous.setVariance(curvar);
            /*
            try{
                for (int i=0; i<Constants.VAR_PARAM_NUM[curvar]; ++i) {
                    previous.setParameter(i, new Float(textfields[i].getText()).floatValue());
                }
            } catch (Exception e) {
                JOptionPane.showConfirmDialog(null,
                                              e,
                                              "Exception" ,
                                              JOptionPane.DEFAULT_OPTION, 
                                              JOptionPane.ERROR_MESSAGE);
                getvals=false;
                fieldCombo.setSelectedIndex(currentFieldSet.getFieldIndex(previous));
            }
            */
        }

        if (getvals) {
            int loopvar;
            int loc = fieldCombo.getSelectedIndex();
            Field f = currentFieldSet.getField(loc);
    
            int variance = f.getVariance();
            varianceCombo.setSelectedIndex(variance);
            
            swapVariance();
    
            previous = f;
        }
    }

    /**
     * Swaps the currently selected variance
     */
    public void swapVariance()
    {

        int loopvar;
        DistributionSet ds;

        int loc = fieldCombo.getSelectedIndex();
        Field f = currentFieldSet.getField(loc);
        f.setVariance(varianceCombo.getSelectedIndex());
        
        switch (varianceCombo.getSelectedIndex()) {
        case Constants.VAR_TYPE_TIMESTAMP:
            for (loopvar = 0; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                distbuttons[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_CONSTANT:
            for (loopvar = 0; loopvar<1; ++loopvar){
                labels[loopvar].setVisible(true);
                distbuttons[loopvar].setVisible(true);
            }
            labels[0].setText("Constant value: ");
            distbuttons[0].setText("Constant value Distribution");
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(0,ds);
            for (loopvar = 1; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                distbuttons[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_UNIFORM:
            for (loopvar = 0; loopvar<2; ++loopvar){
                labels[loopvar].setVisible(true);
                distbuttons[loopvar].setVisible(true);
            }
            labels[0].setText("Lower bound: ");
            labels[1].setText("Upper bound: ");
            distbuttons[0].setText("Lower bound Distribution");
            distbuttons[1].setText("Upper bound Distribution");
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,10);
            if (!first) f.setParameter(1,ds);
            for (loopvar = 2; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                distbuttons[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_NORMAL:
            for (loopvar = 0; loopvar<2; ++loopvar){
                labels[loopvar].setVisible(true);
                distbuttons[loopvar].setVisible(true);
            }
            labels[0].setText("Mean: ");
            labels[1].setText("Standard deviation: ");
            distbuttons[0].setText("Mean Distribution");
            distbuttons[1].setText("Standard deviation Distribution");
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,0);
            if (!first) f.setParameter(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(1,ds);
            for (loopvar = 2; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                distbuttons[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_EXPONENTIAL:
            for (loopvar = 0; loopvar<1; ++loopvar){
                labels[loopvar].setVisible(true);
                distbuttons[loopvar].setVisible(true);
            }
            labels[0].setText("Mean: ");
            distbuttons[0].setText("Mean Distribution");
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(0,ds);
            for (loopvar = 1; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                distbuttons[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_LINEAR:
            for (loopvar = 0; loopvar<4; ++loopvar){
                labels[loopvar].setVisible(true);
                distbuttons[loopvar].setVisible(true);
            }
            labels[0].setText("Initial value: ");
            labels[1].setText("Step: ");
            labels[2].setText("Lower bound: ");
            labels[3].setText("Upper bound: ");
            distbuttons[0].setText("Initial value Distribution");
            distbuttons[1].setText("Step Distribution");
            distbuttons[2].setText("Lower bound Distribution");
            distbuttons[3].setText("Upper bound Distribution");
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(1,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(2,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,10);
            if (!first) f.setParameter(3,ds);
            for (loopvar = 4; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                distbuttons[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_RANDOM_WALK:
            for (loopvar = 0; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(true);
                distbuttons[loopvar].setVisible(true);
            }
            labels[0].setText("Initial value: ");
            labels[1].setText("Step: ");
            labels[2].setText("Probability: ");
            labels[3].setText("Lower bound: ");
            labels[4].setText("Upper bound: ");
            distbuttons[0].setText("Initial value Distribution");
            distbuttons[1].setText("Step Distribution");
            distbuttons[2].setText("Probability bound Distribution");
            distbuttons[3].setText("Lower bound Distribution");
            distbuttons[4].setText("Upper bound Distribution");
            ds = new DistributionSet();
            ds.setDistributionType(1);
            ds.setParameter(0,1);
            ds.setParameter(1,100);
            if (!first) f.setParameter(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(1,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,0.5);
            if (!first) f.setParameter(2,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(3,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,100);
            if (!first) f.setParameter(4,ds);
            break;
        case Constants.VAR_TYPE_RANDOMDIR_WALK:
            for (loopvar = 0; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(true);
                distbuttons[loopvar].setVisible(true);
            }
            labels[0].setText("Initial value: ");
            labels[1].setText("Step: ");
            labels[2].setText("Probability: ");
            labels[3].setText("Lower bound: ");
            labels[4].setText("Upper bound: ");
            distbuttons[0].setText("Initial value Distribution");
            distbuttons[1].setText("Step Distribution");
            distbuttons[2].setText("Probability bound Distribution");
            distbuttons[3].setText("Lower bound Distribution");
            distbuttons[4].setText("Upper bound Distribution");
            ds = new DistributionSet();
            ds.setDistributionType(1);
            ds.setParameter(0,1);
            ds.setParameter(1,100);
            if (!first) f.setParameter(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(1,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,0.9);
            if (!first) f.setParameter(2,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) f.setParameter(3,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,100);
            if (!first) f.setParameter(4,ds);
            break;
        }
    }

    public void showDistribution(int button)
    {
        Field f = currentFieldSet.getField(fieldCombo.getSelectedIndex());
        f.setVariance(varianceCombo.getSelectedIndex());
        new PortNodeDistributionDialog((InputPortNode)node, f.getParameter(button), false);
    }

    /**
     * temp
     */
    public boolean set()
    {
        boolean success = true;
        int loc = fieldCombo.getSelectedIndex();
        int curvar = varianceCombo.getSelectedIndex();
        Field f = currentFieldSet.getField(loc);
        f.setVariance(curvar);
        /*
        try{
            for (int i=0; i<Constants.VAR_PARAM_NUM[curvar]; ++i) {
                f.setParameter(i, new Double(textfields[i].getText()).doubleValue());
            }
        } catch (Exception e) {
            JOptionPane.showConfirmDialog(null,
                                          e,
                                          "Exception" ,
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
            success=false;
        }
        */
    
        return success;
    }

    /**
     * Performs the appropriate tasks depending on the event passed in for the dialog.
     *
     * @param e The event that triggered the call of this method.
     */    
    public void actionPerformed(ActionEvent e) {
        if(e.getActionCommand().equals("Apply")) {
            set();
        }
        if(e.getActionCommand().equals("OK")) {
            if (set())
                dispose();
        }
        if(e.getActionCommand().equals("Cancel")) {
            dispose();
        }
    }
}
