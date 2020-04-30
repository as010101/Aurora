
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
 * A properties dialog for the port nodes rate.
 * 
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 07/22/2002
 */
public class PortNodeRatePropertiesDialog 
    extends GenericNodePropertiesDialog implements ActionListener
{
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
     * The select box of Variance
     */
    protected JComboBox varianceCombo;

    /**
     * The first label
     */
    protected JLabel[] labels;
    
    /**
     * The first textfield
     */
    protected JButton[] distbuttons;

    /**
     * The second textfield
     */
    protected JButton secondParamButton;

    /**
     * The field set to be modified
     */
    protected FieldSet currentFieldSet;

    private boolean first;

    /**
     * Constructs a new PortNodeRatePropertiesDialog object.
     *
     * @param node The InputPortNode associated with this 
     * @param fieldset The set of fields to modify
     * @param dist The distribution used with the rate
     * properties dialog.
     */
    public PortNodeRatePropertiesDialog(InputPortNode node, FieldSet fieldset)
    {
        super(node);
	first = true;

        currentFieldSet = fieldset;
        //currentFieldSet.setDistributionType();

        // Query tab section
        setupRateData();
        swapVariance();
	first = false;
        show();
    }

    protected void swapVariance() {
        int loopvar;
        DistributionSet ds;
        int curvar = varianceCombo.getSelectedIndex();
        currentFieldSet.setDistributionType(curvar);

        switch (curvar) {
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
            if (!first) currentFieldSet.setDistributionParam(0,ds);
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
            if (!first) currentFieldSet.setDistributionParam(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,10);
            if (!first) currentFieldSet.setDistributionParam(1,ds);
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
            if (!first) currentFieldSet.setDistributionParam(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) currentFieldSet.setDistributionParam(1,ds);
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
            if (!first) currentFieldSet.setDistributionParam(0,ds);
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
            if (!first) currentFieldSet.setDistributionParam(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) currentFieldSet.setDistributionParam(1,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) currentFieldSet.setDistributionParam(2,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,10);
            if (!first) currentFieldSet.setDistributionParam(3,ds);
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
            if (!first) currentFieldSet.setDistributionParam(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) currentFieldSet.setDistributionParam(1,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,0.5);
            if (!first) currentFieldSet.setDistributionParam(2,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) currentFieldSet.setDistributionParam(3,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,100);
            if (!first) currentFieldSet.setDistributionParam(4,ds);
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
            if (!first) currentFieldSet.setDistributionParam(0,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) currentFieldSet.setDistributionParam(1,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,0.9);
            if (!first) currentFieldSet.setDistributionParam(2,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,1);
            if (!first) currentFieldSet.setDistributionParam(3,ds);
            ds = new DistributionSet();
            ds.setDistributionType(0);
            ds.setParameter(0,100);
            if (!first) currentFieldSet.setDistributionParam(4,ds);
            break;
        }
    }

    /**
     * Creates a tab pane to input rate specific information
     */
    public void setupRateData()
    {
        int i;
        GridBagLayout bags = new GridBagLayout();
        JPanel srcPanel = new JPanel(bags);

        JLabel varianceLabel = new JLabel("Variance: ");
        addComponent(bags, srcPanel, varianceLabel, 0, 0, 1, 1, 0, 1);

        varianceCombo = new JComboBox(new Object[] {"Constant", "Uniform", "Normal", "Exponential", "Linear", "Random Walk", "Random Directional Walk"} );
        addComponent(bags, srcPanel, varianceCombo, 1, 0, 1, 1, 1, 1);

        /*
        if (currentFieldSet.getDistributionType()==-1) {
            currentFieldSet.setDistributionType(1);
        }
        */
        int curDist = currentFieldSet.getDistributionType();
        if (curDist>0) {
            varianceCombo.setSelectedIndex(curDist);
        }

        labels = new JLabel[5];
        distbuttons = new JButton[5];
        for (i = 0; i<5; ++i){
            labels[i] = new JLabel("Text");
            distbuttons[i] = new JButton("Text");
            addComponent(bags, srcPanel, labels[i], 0, i+1, 1, 1, 0, 1);
            addComponent(bags, srcPanel, distbuttons[i], 1, i+1, 1, 1, 0, 1);
        }

        varianceCombo.addActionListener(new VarianceSelectionListener());

        for (int j=0; j<5; ++j) {
            distbuttons[j].addActionListener(new DistributionButtonListener(j));
        }

        
        srcPanel.setBorder(new LineBorder(Color.black, 1));
        tabbedPane.addTab("Rate Options", srcPanel);

    }


    public void showDistribution(int button)
    {
        new PortNodeDistributionDialog((InputPortNode)node, currentFieldSet.getDistributionParam(button), true);
    }
    
    /**
     * Attempts to set data from the dialog into the object.
     */
    private boolean set()
    {
        boolean success = true;
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
