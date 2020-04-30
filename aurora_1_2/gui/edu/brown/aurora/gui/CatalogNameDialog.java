package edu.brown.aurora.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;


/**
 * A basic implementation of the JDialog class.
 *
 * revised by Matthew Hatoun (mhatoun@cs.brown.edu) on 6/20/03
 */

public class CatalogNameDialog extends JDialog
{
        
    // Used for addNotify check.
    boolean fComponentsAdjusted = false;
    
    public JTextField nameTextField;
    JButton okButton;
    JLabel nameLabel;
    JButton cancelButton ;
    
    String pressedButton = "Cancel";
    
    public CatalogNameDialog(Frame parentFrame)
    {
        super();
        setTitle("Connection Specification");
        setResizable(false);
        setModal(true);
        setSize(285,134);
        setVisible(false);


	getContentPane().setLayout (new BorderLayout(50, 50));

	//bags is the layout used to set up all the layouts
	GridBagLayout bags = new GridBagLayout();

	//the JPanel used to store all the components 
	JPanel nameDialogPanel = new JPanel (bags);

	//sets up the name text field
	nameTextField = new JTextField (15);

	//sets up Okay button
        okButton = new JButton ("  OK  ");
        okButton.setActionCommand("OK");
        okButton.setOpaque(false);
        okButton.setMnemonic((int)'O');

	//sets up the name pane
        JLabel nameLabel = new JLabel ("Name:");

	//sets up the cancel button
        cancelButton = new JButton("Cancel");
        cancelButton.setActionCommand("Cancel");
        cancelButton.setOpaque(false);
        cancelButton.setMnemonic((int)'C');
        
        //REGISTER LISTENERS
        NameDialogWindowAdapter nWindowAdapter = new NameDialogWindowAdapter();
        this.addWindowListener(nWindowAdapter);
        NameDialogButtonListener bListener = new NameDialogButtonListener();
        okButton.addActionListener(bListener);
        cancelButton.addActionListener(bListener);
        nameTextField.addKeyListener(new TextKeyListener(this));

	//adds all the components to the name dialog panel
	JPanel buttonPanel = new JPanel (bags);
	JPanel namePanel = new JPanel (bags);
	addComponent (bags, buttonPanel, okButton, 0,0, 1,1, .2,.2, 
		      new Insets (10,20,10,5));
	addComponent (bags, buttonPanel, cancelButton, 1,0, 1,1, .2,.2,
		      new Insets (10,5,10,20));
	addComponent (bags, nameDialogPanel, nameLabel, 0,0, 1,1, .0,.0,
		      new Insets (10,10,10,5));
	addComponent (bags, nameDialogPanel, nameTextField, 1,0, 1,1, .0,.0,
		      new Insets (10,0,10,10));
	addComponent (bags, nameDialogPanel, buttonPanel, 0,1, 2,1, .0,.0,
		      new Insets (0,0,0,0));

       
        setLocationRelativeTo(parentFrame);
	
	getContentPane().add (nameDialogPanel, BorderLayout.CENTER);
    }
    
    public void setVisible(boolean b)
    {
        if (b) {
            Rectangle bounds = (getParent()).getBounds();
            Dimension size = getSize();
            setLocation(bounds.x + (bounds.width - size.width)/2,
                        bounds.y + (bounds.height - size.height)/2);
        }
        
        super.setVisible(b);
    }
    
    public void addNotify()
    {
        // Record the size of the window prior to calling parents addNotify.
        Dimension d = getSize();
        
        super.addNotify();
        
        if (fComponentsAdjusted)
            return;
        // Adjust components according to the insets
        Insets insets = getInsets();
        setSize(insets.left + insets.right + d.width, insets.top + insets.bottom + d.height);
        Component components[] = getContentPane().getComponents();
        for (int i = 0; i < components.length; i++) {
            Point p = components[i].getLocation();
            p.translate(insets.left, insets.top);
            components[i].setLocation(p);
        }
        fComponentsAdjusted = true;
    }

    class NameDialogWindowAdapter extends WindowAdapter
    {
        public void windowClosing(WindowEvent event)
        {
            Object object = event.getSource();
            if (object == CatalogNameDialog.this)
                jAboutDialog_windowClosing(event);
        }
    }
    
    void jAboutDialog_windowClosing(WindowEvent event)
    {
        // to do: code goes here.
        
        jAboutDialog_windowClosing_Interaction1(event);
    }
    
    void jAboutDialog_windowClosing_Interaction1(WindowEvent event) {
        try {
            // JAboutDialog Hide the JAboutDialog
            this.setVisible(false);
        } catch (Exception e) {
        }
    }
    
    class NameDialogButtonListener implements ActionListener
    {
        public void actionPerformed(ActionEvent event)
        {
	    if (event.getActionCommand().equals ("OK")) {
                okButton_actionPerformed();
	    }
            else if (event.getActionCommand().equals ("Cancel")) {
                cancelButton_actionPerformed();
	    }
        }
    }
    
    void okButton_actionPerformed()
    {
        // to do: code goes here.
        
        okButton_actionPerformed_Interaction1();
    }
    
    void okButton_actionPerformed_Interaction1() {
        pressedButton = "OK";
        if (nameTextField.getText().equals("")) {
            JOptionPane.showConfirmDialog(null, 
                                          "The catalog name should not be null!", 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
        } 
	else if (nameTextField.getText().length() > 20) {
	    JOptionPane.showConfirmDialog(null, 
                                          "The catalog name should be less than 20 characters!", 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
	}
	else {
            try {
                // JAboutDialog Hide the JAboutDialog
                this.setVisible(false);
            } catch (Exception e) {
            }
        }
    }
    
    void cancelButton_actionPerformed()
    {
        // to do: code goes here.
        pressedButton = "Cancel";
        try {
            // JAboutDialog Hide the JAboutDialog
            this.setVisible(false);
        } catch (Exception e) {
        }
    }
    
    String getResponse()
    {
        return pressedButton;    
    }


    protected class TextKeyListener implements KeyListener
    {
        protected CatalogNameDialog cnd;
        public TextKeyListener(CatalogNameDialog cnd)
        {
            this.cnd = cnd;
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
		okButton_actionPerformed();
            }
            if((int)e.getKeyChar() == 27) {
		cancelButton_actionPerformed();
            }

        }
    }


    /**
     * A generic helper method for adding components in a GridBagLayout layout.
     * gridx, gridy, gridwidth, gridheight, weightx, weighty, and inset are GridBagConstraints
     * arguments.
     * @see java.awt.GridBagConstraints
     *
     * @param bags The GridBagLayout object passed into the parent container object
     * @param parent The parent container object
     * @param child The child container object to add into the parent
     * @param gridx The gridx value.
     * @param gridy The gridy value.
     * @param gridwidth The gridwidth value.
     * @param gridheight The gridheight value.
     * @param weightx The weightx value.
     * @param weighty The weighty value.
     * @param inset The insets value
     */
    private void addComponent(GridBagLayout bags,
                                JComponent parent, JComponent child,
                                int gridx, int gridy,
                                int gridwidth, int gridheight,
                                double weightx, double weighty,
				Insets inset )
    {
        GridBagConstraints c = new GridBagConstraints(gridx, gridy,
                                                      gridwidth, gridheight,
                                                      weightx, weighty,
                                                      GridBagConstraints.CENTER,
                                                      GridBagConstraints.BOTH,
                                                      inset,
                                                      0,0);
        bags.setConstraints(child, c);
        parent.add(child);
    }
}

