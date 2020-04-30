package edu.brown.aurora.gui;

import edu.brown.aurora.gui.dbts.PaletteRecord;
import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;


import javax.swing.*;
import javax.swing.border.*;
import javax.swing.tree.*;
import javax.swing.event.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.dnd.*;
import java.awt.datatransfer.*;

import java.util.*;

/**
 * The panel implementing the palette.  Not yet implemented.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class PalettePanel extends JPanel 
    implements DragGestureListener, DragSourceListener
{
    private Model petal_model;
    transient private PetalEditor petal_editor;
    private Editor editor;

    private JList palette_list;
    private DragSource drag_source;

    private ListModel listData;

    private static final String REMOVE_BUTTON = "Remove";

    /**
     * Constructs a new Palette Panel.
     *
     * @param petal_editor The PetalEditor that this PalettePanel visually adds 
     * nodes/arcs to.
     * @param petal_model The PetalModel that this PalettePanel semantically adds 
     * nodes/arcs to.
     * @param editor The editor frame that this BoxesPanel is contained within.
     */
    public PalettePanel(PetalEditor petal_editor,
                        Model petal_model,                        
                        Editor editor) {
        this(petal_editor, petal_model, editor, null);
    }

    /**
     * Constructs a new Palette Panel.
     *
     * @param petal_editor The PetalEditor that this PalettePanel visually adds 
     * nodes/arcs to.
     * @param petal_model The PetalModel that this PalettePanel semantically adds 
     * nodes/arcs to.
     * @param editor The editor frame that this BoxesPanel is contained within.
     * @param palette_list A palette list to put into this palette panel.
     */
    public PalettePanel(PetalEditor petal_editor, Model petal_model, 
                        Editor editor, ListModel listModel) 
    {
        super();
        setDoubleBuffered(true);

        this.petal_model = petal_model;
        this.petal_editor = petal_editor;
        this.editor = editor;
        this.palette_list = palette_list;
        
        setLayout(new GridLayout(1,1));

        if(listModel == null) {
            listData = new DefaultListModel();
        } else {
            listData = listModel;
        }
        palette_list = new JList(listData);
        palette_list.setFont(Constants.PALETTE_LIST_FONT);
        palette_list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        
        drag_source = DragSource.getDefaultDragSource();
        drag_source.createDefaultDragGestureRecognizer(palette_list, 
                                                       DnDConstants.ACTION_COPY_OR_MOVE, 
                                                       this);
        palette_list.addListSelectionListener(new PaletteListListener());

	GridBagLayout bags = new GridBagLayout();
	setLayout(bags);

	JButton removeBtn = new JButton(REMOVE_BUTTON);

	removeBtn.setFont(Constants.TOOLBOX_BUTTON_FONT);

	removeBtn.addActionListener(new PaletteButtonListener());
        Utils.addComponent(bags, this, new JScrollPane(palette_list), 0,0,1,1,1,20);
	Utils.addComponent(bags, this, removeBtn, 0,2,1,1,1,1);

    }

    protected class PaletteButtonListener implements ActionListener
    {
	public PaletteButtonListener () {}
	
	public void actionPerformed(ActionEvent e)
	{
	    if(REMOVE_BUTTON.equals(e.getActionCommand())) {
		int selectedItem = palette_list.getSelectedIndex();
		if(selectedItem == -1) {
		    JOptionPane.showConfirmDialog(null, 
						  "No palette item selected.",
						  "Error!", 
						  JOptionPane.DEFAULT_OPTION, 
						  JOptionPane.ERROR_MESSAGE);                
		} else {
		    DefaultListModel model = (DefaultListModel) palette_list.getModel();
		    model.removeElementAt(selectedItem);
		}
		
		
	    }
	}
    }
    
    
    public void addPaletteRecord(PaletteRecord record)
    {
        if(record != null) {
            ((DefaultListModel)listData).addElement(record);
        } else {
            Utils.addMessagesText("Addition of this box to the palette not yet implemented.");
        }
    }

    /**
     * Returns the list object that is currently being selected.
     *
     * @return The list object that is currently being selected.
     */
    public Object getSelectedValue() 
    {
        return palette_list.getSelectedValue();
    }

    /**
     * Returns the index of the selected object in the list.
     *
     * @return the index of the selected object in the list.
     */
    public int getSelectedIndex() 
    {
        return palette_list.getSelectedIndex();
    }

    /**
     * Returns the list contained in the palette.
     *
     * @return the list contained in the palette.
     */
    public JList getPaletteList()
    {
        return palette_list;
    }

    /**
     * Handles selection during drag and drop. (not implemented)
     *
     * @param o The object being selected.
     * @param ws The workspace where the object exists in.
     */
    public void handleSelect(Object o, Workspace ws)
    {
    }

    /**
     * Sets the editability of the model depending on whether an arc
     * is being added or not.  This method depends on the statefulness
     * of this object -- if the selected value in the list is the 
     * "add arc" item, then the model changes to the add arc mode.  Otherwise
     * the model defaults to the select box mode.
     */
    public void setEdit()
    {
    }

    /**
     * Implements the DnD interface.  Returns the DragSource object contained
     * in this object.  See java.awt.dnd.DragSourceListener.
     *
     * @return the DragSource object contained in this object.
     */
    public DragSource getDragSource() {
        return drag_source;
    }

 
    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dragDropEnd(DragSourceDropEvent dsde)
    {
    }
    
    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dragEnter(DragSourceDragEvent dsde)
    {
    }
    
    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */ 
   public void dragExit(DragSourceEvent dse)
    {
    }
    
    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dragOver(DragSourceDragEvent dsde)
    {
    }
    
    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dropActionChanged(DragSourceDragEvent dsde)
    {
    }

    /**
     * Implements the DnD interface.  Detects a platform-dependent drag initiating
     * mouse gesture and notifies the listener in order for it to initiate an
     * action for the user.
     * See java.awt.dnd.DragGestureListener.
     */
    public void dragGestureRecognized(DragGestureEvent dge) {
        Object selected = getSelectedValue();
        if ( selected != null ) {
            StringSelection text = new StringSelection(selected.toString());
            drag_source.startDrag(dge, DragSource.DefaultMoveDrop, text, this);
        } else {
            System.out.println("Nothing was selected");  
        }
    }

    /**
     * A list selection listener.  Mainly changes the editability of the workspace
     * when a the JList selected item has changed.
     *
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */
    protected class PaletteListListener implements ListSelectionListener {

        /**
         * Constructs a new PaletteListListener.
         *
         * @param panel the panel that contains the list items that this
         * class listens to.
         */
        public PaletteListListener() {
        }
        
        
        /**
         * Implements the ListSelectionListener interface.  Changes the
         * editability of the workspace when a different item has been selected
         * in the list.
         *
         * @param e The list selection change event that was passed into this
         * class.
         */
        public void valueChanged(ListSelectionEvent e) {
        }        
    }
}
