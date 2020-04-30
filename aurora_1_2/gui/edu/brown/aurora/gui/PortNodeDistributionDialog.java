
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
 * A properties dialog for the port nodes' variance distributions.
 * 
 *
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 09/04/2002
 */
public class PortNodeDistributionDialog 
    extends GenericNodePropertiesDialog implements ActionListener
{

    private class VarianceSelectionListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            swapDistribution(false);
        }
    }

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
    protected JTextField[] textfields;

    /**
     * The DistributionSet to be modified
     */
    protected DistributionSet distSet;

    private boolean microEnabled;

    /**
     * Constructs a new PortNodeDistributionDialog object.
     *
     * @param node The InputPortNode associated with this 
     * @param fieldset The set of fields to modify
     * properties dialog.
     */
    public PortNodeDistributionDialog(InputPortNode node, DistributionSet dist, boolean micro)
    {
        super(node);

        labels = new JLabel[5];
        textfields = new JTextField[5];
        distSet = dist;
	microEnabled = micro;
        // Query tab section
        setupDistInfo();
        show();
    }

    /**
     * sets up the layout of the panel
     */
    public void setupDistInfo()
    {
        GridBagLayout bags = new GridBagLayout();
        JPanel srcPanel = new JPanel(bags);

        JLabel distributionLabel = new JLabel("Distribution: ");
        addComponent(bags, srcPanel, distributionLabel, 0, 0, 1, 1, 0, 1);

        varianceCombo = new JComboBox(new Object[] {"Constant", "Uniform", "Normal", "Exponential", "Linear", "Random Walk", "Random Directional Walk"} );
        addComponent(bags, srcPanel, varianceCombo, 1, 0, 1, 1, 1, 1);

        boolean first = false;
        int curType = distSet.getDistributionType();
        if (curType!=-1) {
            first = true;
            varianceCombo.setSelectedIndex(curType);
        }

        for (int i=0; i<5; ++i) {
            labels[i] = new JLabel("Label"+i);
            addComponent(bags, srcPanel, labels[i], 0, i+1,1,1,0,1);
            
            textfields[i] = new JTextField("0.0");
            textfields[i].setEditable(true);
            addComponent(bags, srcPanel, textfields[i], 1,i+1,1,1,1,1);
        }

        srcPanel.setBorder(new LineBorder(Color.black, 1));
        tabbedPane.addTab("Distribution Options", srcPanel);

        varianceCombo.addActionListener(new VarianceSelectionListener());

        swapDistribution(first);

    }

    /**
     * Swaps the currently selected variance
     */
    public void swapDistribution(boolean first)
    {
        int loopvar;
        int curDist = varianceCombo.getSelectedIndex();
        if (!first) distSet.setDistributionType(curDist);
        switch (curDist) {
        case Constants.VAR_TYPE_CONSTANT:
            for (loopvar = 0; loopvar<1; ++loopvar){
                labels[loopvar].setVisible(true);
                textfields[loopvar].setVisible(true);
            }
            labels[0].setText("Constant value: ");
	    if (!first) distSet.setParameter(0,1.0);
            textfields[0].setText(""+distSet.getParameter(0));
            for (loopvar = 1; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                textfields[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_UNIFORM:
            for (loopvar = 0; loopvar<2; ++loopvar){
                labels[loopvar].setVisible(true);
                textfields[loopvar].setVisible(true);
            }
            labels[0].setText("Lower bound: ");
            labels[1].setText("Upper bound: ");
	    if (!first) distSet.setParameter(0,1.0);
	    if (!first) distSet.setParameter(1,10.0);
            textfields[0].setText(""+distSet.getParameter(0));
            textfields[1].setText(""+distSet.getParameter(1));
            for (loopvar = 2; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                textfields[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_NORMAL:
            for (loopvar = 0; loopvar<2; ++loopvar){
                labels[loopvar].setVisible(true);
                textfields[loopvar].setVisible(true);
            }
            labels[0].setText("Mean: ");
            labels[1].setText("Standard deviation: ");
	    if (!first) distSet.setParameter(0,0.0);
	    if (!first) distSet.setParameter(1,1.0);
            textfields[0].setText(""+distSet.getParameter(0));
            textfields[1].setText(""+distSet.getParameter(1));
            for (loopvar = 2; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                textfields[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_EXPONENTIAL:
            for (loopvar = 0; loopvar<1; ++loopvar){
                labels[loopvar].setVisible(true);
                textfields[loopvar].setVisible(true);
            }
            labels[0].setText("Mean: ");
	    if (!first) distSet.setParameter(0,1.0);
            textfields[0].setText(""+distSet.getParameter(0));
            for (loopvar = 1; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                textfields[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_LINEAR:
            for (loopvar = 0; loopvar<4; ++loopvar){
                labels[loopvar].setVisible(true);
                textfields[loopvar].setVisible(true);
            }
            labels[0].setText("Initial value: ");
            labels[1].setText("Step: ");
            labels[2].setText("Lower bound: ");
            labels[3].setText("Upper bound: ");
	    if (!first) distSet.setParameter(0,1.0);
	    if (!first) distSet.setParameter(1,1.0);
	    if (!first) distSet.setParameter(2,1.0);
	    if (!first) distSet.setParameter(3,10.0);
            textfields[0].setText(""+distSet.getParameter(0));
            textfields[1].setText(""+distSet.getParameter(1));
            textfields[2].setText(""+distSet.getParameter(2));
            textfields[3].setText(""+distSet.getParameter(3));
            for (loopvar = 4; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(false);
                textfields[loopvar].setVisible(false);
            }
            break;
        case Constants.VAR_TYPE_RANDOM_WALK:
            for (loopvar = 0; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(true);
                textfields[loopvar].setVisible(true);
            }
            labels[0].setText("Initial value: ");
            labels[1].setText("Step: ");
            labels[2].setText("Probability: ");
            labels[3].setText("Lower bound: ");
            labels[4].setText("Upper bound: ");
	    if (!first) distSet.setParameter(0,50.0);
	    if (!first) distSet.setParameter(1,1.0);
	    if (!first) distSet.setParameter(2,0.5);
	    if (!first) distSet.setParameter(3,1.0);
	    if (!first) distSet.setParameter(4,100.0);
            textfields[0].setText(""+distSet.getParameter(0));
            textfields[1].setText(""+distSet.getParameter(1));
            textfields[2].setText(""+distSet.getParameter(2));
            textfields[3].setText(""+distSet.getParameter(3));
            textfields[4].setText(""+distSet.getParameter(4));
            break;
        case Constants.VAR_TYPE_RANDOMDIR_WALK:
            for (loopvar = 0; loopvar<5; ++loopvar){
                labels[loopvar].setVisible(true);
                textfields[loopvar].setVisible(true);
            }
            labels[0].setText("Initial value: ");
            labels[1].setText("Step: ");
            labels[2].setText("Probability: ");
            labels[3].setText("Lower bound: ");
            labels[4].setText("Upper bound: ");
	    if (!first) distSet.setParameter(0,50.0);
	    if (!first) distSet.setParameter(1,1.0);
	    if (!first) distSet.setParameter(2,0.9);
	    if (!first) distSet.setParameter(3,1.0);
	    if (!first) distSet.setParameter(4,100.0);
            textfields[0].setText(""+distSet.getParameter(0));
            textfields[1].setText(""+distSet.getParameter(1));
            textfields[2].setText(""+distSet.getParameter(2));
            textfields[3].setText(""+distSet.getParameter(3));
            textfields[4].setText(""+distSet.getParameter(4));
            break;
        }

    }

    /**
     * temp
     */
    public boolean set()
    {
        boolean success = true;
        int curDist = varianceCombo.getSelectedIndex();
        distSet.setDistributionType(curDist);
        try{
            for (int i=0; i<Constants.VAR_PARAM_NUM[curDist]; ++i) {
                distSet.setParameter(i, new Double(textfields[i].getText()).doubleValue());
            }
        } catch (Exception e) {
            JOptionPane.showConfirmDialog(null,
                                          e,
                                          "Exception" ,
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
            success=false;
        }

        if (success) {
	    int numParam = Constants.VAR_PARAM_NUM[curDist];
            double testVal = 0.001;
	    if (microEnabled) testVal = 0.000001;
	    for (int j=0; j<numParam; ++j) {
	        double curVal = distSet.getParameter(j);
		if (testVal > curVal) {
		    success=false;
		    if (microEnabled) {
                        JOptionPane.showConfirmDialog(null,
                                    "Specified value is below threshold (0.000001)",
                                    "Error", JOptionPane.DEFAULT_OPTION,
                                    JOptionPane.ERROR_MESSAGE);
		    } else {
                        JOptionPane.showConfirmDialog(null,
                                    "Specified value is below threshold (0.001)",
                                    "Error", JOptionPane.DEFAULT_OPTION,
                                    JOptionPane.ERROR_MESSAGE);
		    }
		    return success;
		}
	    }
            switch (curDist) {
            case Constants.VAR_TYPE_UNIFORM:
                if (distSet.getParameter(0)>distSet.getParameter(1)) {
                    success=false;
                    JOptionPane.showConfirmDialog(null,
                                "Lower bound must be less than upper bound",
                                "Error", JOptionPane.DEFAULT_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                }
                break;
            case Constants.VAR_TYPE_LINEAR:
                if (distSet.getParameter(2)>distSet.getParameter(3)) {
                    success=false;
                    JOptionPane.showConfirmDialog(null,
                                "Lower bound must be less than upper bound",
                                "Error", JOptionPane.DEFAULT_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                }
                break;
            case Constants.VAR_TYPE_NORMAL:
                if (distSet.getParameter(1)<0) {
                    success=false;
                    JOptionPane.showConfirmDialog(null,
                                "Standard deviation must be greater than 0",
                                "Error", JOptionPane.DEFAULT_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                }
            case Constants.VAR_TYPE_EXPONENTIAL:
                if (distSet.getParameter(0)<0) {
                    success=false;
                    JOptionPane.showConfirmDialog(null,
                                "Mean must be greater than 0",
                                "Error", JOptionPane.DEFAULT_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                }
                break;
            case Constants.VAR_TYPE_RANDOM_WALK:
            case Constants.VAR_TYPE_RANDOMDIR_WALK:
                if (distSet.getParameter(3)>distSet.getParameter(4)) {
                    success=false;
                    JOptionPane.showConfirmDialog(null,
                                "Lower bound must be less than upper bound",
                                "Error", JOptionPane.DEFAULT_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                }
                if ((distSet.getParameter(2)>1)||(distSet.getParameter(2)<0)) {
                    success=false;
                    JOptionPane.showConfirmDialog(null,
                                "Probability must be between 0 and 1",
                                "Error", JOptionPane.DEFAULT_OPTION,
                                JOptionPane.ERROR_MESSAGE);
                }
                break;
            }
        }

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
