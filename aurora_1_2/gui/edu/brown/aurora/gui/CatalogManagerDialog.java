/*
 * A basic implementation of the JDialog class.
 *
 * Revised heavily on 6/9/03 by Matthew Hatoun (mhatoun@cs.brown.edu)
 */
package edu.brown.aurora.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

public class CatalogManagerDialog extends JDialog
{
    CatalogManager cm;

    // ok I was too lazy to do this right.
    // the intention of this variable is to only allow users to delete
    // catalogs in the first dialog, afterwards deleting catalogs could
    // result in strange behavior.  rly
    protected static boolean firstCMD = true;  

  
    // Used by addNotify
    boolean frameSizeAdjusted = false;
    
    //JList used to keep track of possible catalog names
    private JList catalogList = null; 

    
    public CatalogManagerDialog(CatalogManager cm, Frame parent)
    {
        super(parent);
        this.cm = cm;
        if(Constants.VERBOSE) System.out.println(this.cm);

        setSize(473, 390);
        setVisible(false); 
        setResizable(false);
        setModal(true);
        setTitle("Catalog Manager");
        setLocationRelativeTo(parent);
	getContentPane().setLayout (new BorderLayout(50, 50));

	CatalogWindowAdaptor cWindowAdaptor = new CatalogWindowAdaptor();
        this.addWindowListener(cWindowAdaptor);
        CatalogButtonActionListener cActionListener = 
	    new CatalogButtonActionListener();

	//bags is the layout used to set up all the layouts
	GridBagLayout bags = new GridBagLayout();

	//the JPanel used to store all the components of the catalog manager dialog
	JPanel catalogPanel = new JPanel (bags);	
      
	//sets up the JList with the possible catalog names
        catalogList = new JList(cm.getCatalogNames());

	//allows the user to double click on a name to select it
	catalogList.addMouseListener (new MouseAdapter(){
		public void mouseClicked (MouseEvent e)
		{
		    if (e.getClickCount() == 2) {
			selectButton_ActionPerformed();
		    }
			
		}
	    }
	    );
	JScrollPane catalogScroller = new JScrollPane (catalogList);

	//sets up the select button
        JButton selectButton = new JButton ("Select Catalog");

        selectButton.setFont(Constants.DIALOG_FONT);
	selectButton.addActionListener (cActionListener);

	//sets up the cancel button
        JButton cancelButton = new JButton ("Cancel");
        cancelButton.setFont(Constants.DIALOG_FONT);
	cancelButton.addActionListener (cActionListener);
	
	//sets up the new button
        JButton newButton = new JButton ("New Catalog");
        newButton.setFont(Constants.DIALOG_FONT);
	newButton.addActionListener (cActionListener);
        
	//sets up the delete button
        JButton deleteButton = new JButton ("Delete Catalog");
	deleteButton.setFont(Constants.DIALOG_FONT);
	deleteButton.addActionListener (cActionListener);
	
	//creates a panel to take up extra space (to make the buttons reasonable size)
	JPanel spacerPanel = new JPanel();

	//adds all the components to the content pane
	addComponent (bags, catalogPanel, catalogScroller , 0,0, 1,5, .8,1, 
		      new Insets (40, 20, 40, 10));
	addComponent (bags, catalogPanel, selectButton, 1,0, 1,1, .2,.2,
		      new Insets (40, 10, 5, 20));
	addComponent (bags, catalogPanel, newButton, 1,1, 1,1, .2,.2, 
		      new Insets (5,10,5,20));
	addComponent (bags, catalogPanel, deleteButton, 1,2, 1,1, .2,.2,
		      new Insets (5,10,5,20));
	addComponent (bags, catalogPanel, cancelButton, 1,3, 1,1, .2,.2, 
		      new Insets (5,10,5,20));
	addComponent (bags, catalogPanel, spacerPanel, 1,4, 1,1, .2,.4, 
		      new Insets (5,5,10,5));
	


	if(!firstCMD) {
	    deleteButton.setEnabled(false);
	}
	firstCMD = false;
        
	getContentPane().add (catalogPanel, BorderLayout.CENTER);
    }
    
    public CatalogManagerDialog(CatalogManager cm)
    {
        this(cm, (Frame) null);
    }
    
    public CatalogManagerDialog(CatalogManager cm, String sTitle)
    {
        this(cm);
        setTitle(sTitle);
    }
    
    public void addNotify()
    {
        // Record the size of the window prior to calling parents addNotify.
        Dimension size = getSize();
        
        super.addNotify();
        
        if (frameSizeAdjusted)
            return;
        frameSizeAdjusted = true;
        
        // Adjust size of frame according to the insets
        Insets insets = getInsets();
        setSize(insets.left + insets.right + size.width, insets.top + insets.bottom + size.height);
    }
      
    //Window adaptor that handles any window events for the JDialog
    class CatalogWindowAdaptor extends WindowAdapter
    {
        public void windowClosing(WindowEvent event)
        {
            Object object = event.getSource();
            if (object == CatalogManagerDialog.this)
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
    
    //ActionListener that handles the buttons for the CatalogMangerDialog
    class CatalogButtonActionListener implements ActionListener
    {
        public void actionPerformed(ActionEvent e)
        {
            if (e.getActionCommand().equals ("Select Catalog")){
                selectButton_ActionPerformed();
	    }
            else if (e.getActionCommand().equals ("New Catalog")) {
                newButton_ActionPerformed();
	    }
            else if (e.getActionCommand().equals ("Delete Catalog")) {
                deleteButton_ActionPerformed();
	    }
            else if (e.getActionCommand().equals ("Cancel")) {
                cancelButton_ActionPerformed();
	    }
        }
    }
    
    void selectButton_ActionPerformed()
    {

        // to do: code goes here.
        try {
            // JAboutDialog Hide the JAboutDialog
            Object o = catalogList.getSelectedValue();
            if (o == null) {
                JOptionPane.showConfirmDialog(null, 
                                              "Please select one catalog", 
                                              "Exception" , 
                                              JOptionPane.DEFAULT_OPTION, 
                                              JOptionPane.ERROR_MESSAGE);
            } else {
                String name = cm.selectCatalog((String)o);
                if (name == null) {
                    JOptionPane.showConfirmDialog(null, 
                                                  "The Catalog couldn't be selected!", 
                                                  "Exception" , 
                                                  JOptionPane.DEFAULT_OPTION, 
                                                  JOptionPane.ERROR_MESSAGE);
                    
                } else {
                    catalogList.setListData(cm.getCatalogNames());
                    try {
                        // JAboutDialog Hide the JAboutDialog
                        this.setVisible(false);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
            }
            
        } catch (Exception e) {
            e.printStackTrace();
            JOptionPane.showConfirmDialog(null, 
                                          e, 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
        }
    }
    
    void newButton_ActionPerformed()
    {
        // to do: code goes here.
        CatalogNameDialog d = new CatalogNameDialog(null);
        d.show();
        if (d.getResponse().equals("OK")) {
            String newName = d.nameTextField.getText();
	    boolean success = cm.newCatalog(newName);
            if (!success) {
                JOptionPane.showConfirmDialog(null, 
                                              "New Catalog couldn't be inserted!", 
                                              "Exception" , 
                                              JOptionPane.DEFAULT_OPTION, 
                                              JOptionPane.ERROR_MESSAGE);
                
            } else {
                catalogList.setListData(cm.getCatalogNames());
            }
        }
    }
    
    void deleteButton_ActionPerformed()
    {
        // to do: code goes here.
        Object o = catalogList.getSelectedValue();
        if(o == null) {
            JOptionPane.showConfirmDialog(null, 
                                          "Please select one catalog to delete", 
                                          "Exception" , 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);
        } else {
            cm.deleteCatalog(o.toString());
            catalogList.setListData(cm.getCatalogNames());
        }
    }
    
    void cancelButton_ActionPerformed()
    {
        // to do: code goes here.
        try {
            // JAboutDialog Hide the JAboutDialog
            this.setVisible(false);
        } catch (Exception e) {
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
