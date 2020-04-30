
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
 * A Panel containing one set of Qos Specifiers for output port node properties dialog.
 * 
 *
 * @author Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class QosSpecifiersPanel extends JPanel
{
    protected QosPoints points;
    protected JTextField numPointsText;
    protected JTextField qosTypeText;

    protected Vector qosValues;
    protected JPanel qosValuesPanel;

    public QosSpecifiersPanel(QosPoints points)
    {
        this.points = points;
        setLayout(new GridLayout(1,1));
        GridBagLayout bags = new GridBagLayout();
        JPanel appPanel = new JPanel(bags);
        qosValuesPanel = new JPanel();
        qosValues = new Vector();

        JLabel numPointsLabel = new JLabel("Number of Points: ");
        
        numPointsText = new JTextField("" + points.getNumQosPoints());
        numPointsText.addKeyListener(new TextKeyListener(this, numPointsText));
        
        JLabel qosTypeLabel = new JLabel("Qos Type: ");

        qosTypeText = new JTextField("" + points.getQosType());

        createQosValuesPanel();

        Utils.addComponent(bags, appPanel, numPointsLabel, 0, 0, 1, 1, 4, 1);
        Utils.addComponent(bags, appPanel, numPointsText, 1, 0, 1, 1, 1, 1);
        Utils.addComponent(bags, appPanel, qosTypeLabel, 0, 1, 1, 1, 4, 1);
        Utils.addComponent(bags, appPanel, qosTypeText, 1, 1, 1, 1, 1, 1);
        Utils.addComponent(bags, appPanel, qosValuesPanel, 0, 2, 2, 1, 5, 5);          

        createQosValuesPanel();

        add(appPanel);
    }

    protected void createQosValuesPanel()
    {
        qosValuesPanel.removeAll();
        qosValues.removeAllElements();

        Vector qosPoints = points.getPoints();
        int numPoints = points.getNumQosPoints();

        qosValuesPanel.setLayout(new GridLayout(numPoints+1, 3));

        qosValuesPanel.add(new JLabel(""));
        qosValuesPanel.add(new JLabel("X values"));
        qosValuesPanel.add(new JLabel("Y values"));

        for(int i=0; i<numPoints; i++) {
            JTextField qosTextFieldX = new JTextField(qosPoints.elementAt(2*i).toString());
            JTextField qosTextFieldY = new JTextField(qosPoints.elementAt(2*i+1).toString());

            qosValuesPanel.add(new JLabel("Qos values " + i));
            qosValuesPanel.add(qosTextFieldX);
            qosValuesPanel.add(qosTextFieldY);
            qosValues.addElement(qosTextFieldX);
            qosValues.addElement(qosTextFieldY);
        }

        qosValuesPanel.setVisible(false);
        qosValuesPanel.setVisible(true);
    }
    
    public boolean set()
    {
        try {
            Integer.parseInt(numPointsText.getText());
            Integer.parseInt(qosTypeText.getText());
        } catch(Exception e) {
            JOptionPane.showConfirmDialog(null, 
                                          "Integer parsing error : " + e.toString(), 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
            return false;
        }

        if(Integer.parseInt(numPointsText.getText()) != points.getNumQosPoints()) {
            int numPoints = Integer.parseInt(numPointsText.getText());
            points.setNumQosPoints(numPoints);
            createQosValuesPanel();
            return false;
        }
            
        Vector textFieldStrings = new Vector();

        textFieldStrings.addElement(numPointsText.getText());
        textFieldStrings.addElement(qosTypeText.getText());

        for(int i=0; i<qosValues.size(); i++) {
            String tfString = ((JTextField)qosValues.elementAt(i)).getText();
            try {
                Double.parseDouble(tfString);
            } catch(Exception e) {
                JOptionPane.showConfirmDialog(null, 
                                              "Double parsing error : " + e.toString(), 
                                              "Exception" , 
                                              JOptionPane.DEFAULT_OPTION, 
                                              JOptionPane.ERROR_MESSAGE);
                return false;                
            }
            textFieldStrings.addElement(tfString);
        }

        points.setQosPoints(textFieldStrings);

        return true;
    }

    protected class TextKeyListener implements KeyListener
    {
        JPanel qosValuesPanel;
        JTextField numPointsText;

        public TextKeyListener(JPanel qosValuesPanel, JTextField numPointsText)
        {
            this.qosValuesPanel = qosValuesPanel;
            this.numPointsText = numPointsText;
        }
        
        public void keyPressed(KeyEvent e)
        {
        }
        public void keyReleased(KeyEvent e)
        {
        }
        public void keyTyped(KeyEvent e)
        {
            if(e.getKeyChar() == '\n') {
                int numPoints = Integer.parseInt(numPointsText.getText());
                points.setNumQosPoints(numPoints);
                
                createQosValuesPanel();
            }
        }
    }
}
