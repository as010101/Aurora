
package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * A versatile panel object that displays a textfield in edit mode, and a label
 * in a non-editting mode.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class RenamableLabel extends JPanel {
    /**
     * The label contained in this panel.
     */
    protected JLabel label;

    /**
     * The textfield contained in this panel.
     */
    protected JTextField textfield;

    /**
     * Constructs a new RenamableLabel object given a default string.
     *
     * @param s the default string to set this renanamable label to.
     */
    public RenamableLabel(String s) {
        this(s, Color.lightGray);
    }

    /**
     * Constructs a new RenamableLabel object given a default string
     * and a color for the background.
     *
     * @param s The default string to set this renanamablelabel to.
     * @param c The default color for the background of this RenamamableLabel
     * object.
     */
    public RenamableLabel(String s, Color c) {
        setLayout(new GridLayout(1,1));

        label = new JLabel(s, SwingConstants.CENTER);
        label.setFont(Constants.LABEL_FONT);
        label.setBackground(c);
        label.setText(s);
        label.setMinimumSize(label.getPreferredSize());
        
        label.setOpaque(true);
        
        setBorder(new LineBorder(Color.black,1));

        setSize(new Dimension(100, 25));

        setOpaque(true);
        setMaximumSize(new Dimension(400,400));

        textfield = new JTextField();
        textfield.addActionListener(new RenameListener());
        textfield.setSize(textfield.getPreferredSize());

        // add(label);
        // handleRename();

        add(textfield);
        textfield.setText("");
        //set it to accept the focus

        setRequestFocusEnabled(true);
        textfield.setRequestFocusEnabled(true);
        finishRename();

    }

    /**
     * Grabs the focus for the textfield.
     */
    public void getTextFieldFocus() {
        textfield.grabFocus();
        textfield.requestFocus();
        textfield.requestFocusInWindow();
    }

    /**
     * Sets the background color of the label.
     *
     * @param c The background color to set for this label.
     */
    public void setColor(Color c) {
        label.setBackground(c);
    }

    /**
     * Sets the text of the textfield and the label of this object.
     *
     * @param t The text to set.
     */
    public void setText(String t) {
        textfield.setText(t);
        label.setText(t);
    }

    /**
     * Begins the renaming process for this object.  Hides the label
     * object and shows the textfield.
     */
    public void handleRename() {
        remove(label);
        add(textfield);

        textfield.setText(label.getText());

        getTextFieldFocus();

        update(this.getGraphics());
    }

    /**
     * Ends the renaming process for this object.  Hides the text
     * field and shows the label.
     */
    public void finishRename() {
        String newLabel = textfield.getText();
        label.setText(newLabel);

        remove(textfield);
        add(label);

        label.updateUI();
        Graphics g = getGraphics();
        if (g != null)
            update(g);     
    }

    /**
     * Returns the label of this object as a string.
     *
     * @return The label of this object as a string.
     */
    public String getLabel() {
        return label.getText();
    }

    /**
     * A listener for the textfield componentn of RenamableLabel.  Finishes
     * the textfield editting process when it receives an event.
     * 
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */

    protected class RenameListener implements ActionListener {

        /**
         * Constructs a new RenameListener object.
         */
        public RenameListener() {
        }

        /**
         * Finishes the the renaming scheme.
         * 
         * @param e ignored.
         */
        public void actionPerformed(ActionEvent e) {
            finishRename();
        }
    }


}
