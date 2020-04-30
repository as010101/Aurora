package edu.brown.aurora.gui;
import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

import com.sleepycat.db.*;

/**
 * This is a super class for InputPortNode and OutputPortNode
 * classes.   A GenericPortNode has its own type specified by a 
 * CompositeType object.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class GenericPortNode extends GenericNode
{
    /**
     * The label representing the the port.
     */
    protected JLabel lbl;

    /**
     * A string description of this port node.
     */
    protected String description;

    /**
     * The index associated with this GenericPortNode object.
     */
    protected int index = 0;
    
    /**
     * Constructs a new GenericPortNode object. 
     * The type is not assigned.
     *
     * @param id The unique int identifier of this port node
     * @param m The model containing this GenericPortNode object/
     */
    public GenericPortNode(int id, Model m, int parentId)
    {
        super(id, parentId, m);
        JPanel panel = new JPanel(new GridLayout(1,1));
        lbl = new JLabel("xxxxxxx",SwingConstants.CENTER);
        
        lbl.setBackground(Constants.PORTNODE_COLOR);
        lbl.setForeground(Constants.PORTNODE_FONT_COLOR);
        lbl.setFont(Constants.LABEL_FONT);
        lbl.setBorder(new LineBorder(Color.black,1));
        lbl.setMinimumSize(lbl.getPreferredSize());
        lbl.setSize(lbl.getPreferredSize());
        lbl.setMaximumSize(new Dimension(400,400));
        lbl.setOpaque(true);
        
        panel.add(lbl);
        
        node_component = panel;
        panel.setSize(lbl.getPreferredSize());

        description =  new String();
        
    }

    /**
     * Constructs a new GenericPortNode object having the same 
     * contents as the specified GenericPortNode object. 
     */
    public GenericPortNode(GenericPortNode port, Model m, int parentId)
    {
        super(-1, parentId, m);
        String s = ((JLabel)port.node_component).getText();
        
        JLabel lbl = new JLabel(s,SwingConstants.CENTER);
        node_component = lbl;
        
        lbl.setBackground(Constants.PORTNODE_COLOR);
        //lbl.setText(s);
        lbl.setText("boxId=" + boxId);
        lbl.setForeground(Constants.PORTNODE_FONT_COLOR);
        lbl.setFont(Constants.LABEL_FONT);
        lbl.setBorder(new LineBorder(Color.black,1));
        lbl.setMinimumSize(lbl.getPreferredSize());
        lbl.setSize(lbl.getPreferredSize());
        lbl.setMaximumSize(new Dimension(400,400));
        lbl.setOpaque(true);

        this.description = port.description;
    }


    /**
     * Saves the common attributes of all primitive box nodes.  Currently this saves the
     * node_component attributes.
     *
     * @param boxTable The sleepycat database table containing generic box node
     * fields.
     * @param type The operator type of the box to save into the table.
     */
    protected IntegerArrayDbt saveRecord(Db boxTable) throws Throwable
    {
        IntegerArrayDbt key = new IntegerArrayDbt(new int[] {boxId, parentId});
        Rectangle r = getComponent().getBounds();

        BoxRecord boxRecord = new BoxRecord(boxId,
                                            getBoxType(),
                                            getLabel(),
                                            getDescription(),
                                            getModifier(),  // modifier
                                            parentId,
                                            (float)0.0, // cost
                                            (float)0.0, //  selectivity,
					    isUseDefinedTypes(),
					    false,
                                            r.x, r.y, 
                                            r.width, r.height);
        if (boxTable.put(null, key, boxRecord, 0) == Db.DB_KEYEXIST) {
            System.out.println("PortNode.saveRecordDbt() - The key already exists.");
        }
        
        return key;
    }

    abstract public boolean isUseDefinedTypes();


    /**
     * Gets the modifier.
     */
    public abstract String getModifier();

    /**
     * Sets the color for this generic port node
     */
    public abstract void setColor();

    /**
     * Returns the index of this GenericPortNode object.
     *
     * @return The index of this GenericPortNode object.
     */
    public int getIndex()
    {
        return index;
    }

    /**
     * Sets the index of this GenericPortNode object.
     *
     * @param index The index to set on this GenericPortNode object.
     */
    public void setIndex(int index)
    {
        this.index = index;
    }
    
    /**
     * Returns a composite type contained by this  port node.
     *
     * @return a composite type contained by this  port node.
     */
    abstract public CompositeType getType();

    /**
     * Sets a type for this composite  port node.
     *
     * @param type The composite tyoe to assicate with this port node.
     */
    abstract public void setType(CompositeType type);
    
    /**
     * Sets the label of this GenericPortNode object.
     *
     * @param s The label to set this GenericPortNode object to.
     */
    public void setLabel(String s)
    {
        lbl.setText(s);
    }

    /**
     * Gets the label of this GenericPortNode object.
     *
     * @return The label that this GenericPortNode contains.
     */
    public String getLabel()
    {
	if(lbl == null) {
	    return "port node";
	} else {
	    return lbl.getText();
	}
    }

    /**
     * Gets the description of this GenericPortNode object.
     *
     * @return The description that this GenericPortNode contains.
     */
    public String getDescription()
    {
        return description;
    }

    /**
     * Returns the type Id of this port node.
     *
     * @return the type id of this port node.
     */
    public abstract int getTypeId();

    /**
     * Gets the visual GUI representation of the this GenericPortNode object.
     *
     * @return The JComponent that visually represents this GenericPortNode object.
     */
    public Component getComponent()
    {
        return node_component;
    }

    /**
     * An input handler for this PetalNode object.  Handles mouse clicks
     * on the node component.
     *
     * @param evt The mouse event positioned relative to the editor frame.
     */    
    public boolean handleMouseClick(MouseEvent evt)
    {
        return false;
    }


    /**
     * Sets the background color of this generic port node.
     */
    public void setBackground(Color color)
    {
        lbl.setBackground(color);
    }
    


    /**
     * An input handler for this PetalNode object.  Handles keyboard events
     * on this node component.
     *
     * @param evt the keyboard event.
     */    
    public boolean handleKeyInput(KeyEvent evt)
    {
        return false;
    }
    
    /**
     * Displays a tooltip for this box node.
     *
     * @return the string tool tip for this box node.
     */    
    public String getToolTip(Point at)
    {
        return null;
    }


    /*
     * Sets the properties of this port using a properties dialog box.
     * @frame the parent frame of the properties box. It can be null.
     */

    //abstract void setProperties(Frame frame);
 
    /**
     * Returns the height of the visual GUI component.
     *
     * @return The height of the visual GUI component as an int.
     */
    public int getHeight()
    {
        return node_component.getHeight();
    }

    /**
     * Returns the width of the visual GUI component.
     *
     * @return the width of the visual GUI component as an int.
     */
    public int getWidth()
    {
        return node_component.getWidth();
    }

    /**
     * Opens up the properties dialog box for this GenericPortNode object.
     *
     */
    public void showProperties()
    {
        new PortNodePropertiesDialog(this);        
    } 
    
    /**
     * Opens up the struct dialog for this GenericPortNode object.
     *
     */
    public void showStructDialog()
    {
        new PortNodeStructDialog(this);        
    }    


    /**
     * Shows the pop up menu associated with the GenericPortNode.
     *
     * @param evt The Mouse Event that popped up the popup menu.
     */
    public void showMenu(MouseEvent evt) {

        BoxNode.disableMenu();
        jmenu = new JPopupMenu();
        
        jmenu.add(new PortMenuItem("Delete", this));
        jmenu.addSeparator();
        jmenu.add(new PortMenuItem("Create C++ Struct", this));
        jmenu.addSeparator();
        jmenu.add(new PortMenuItem("Properties", this));
        
        Frame frame = JOptionPane.getFrameForComponent(node_component);
        Point pt = frame.getLocation();
        jmenu.setLocation((int)(evt.getX() + pt.getX() + 40), 
                          (int)(evt.getY() + pt.getY()) + 20);        
        
        Callback.showMenu();
    }


    /**
     * Saving GenericPortNodes to the palette not implemented yet.
     */
    public PaletteRecord getPaletteRecord(int rootBoxId)
    {
        String label = "<n/a>";
        if(getBoxType() == Constants.INPUTPORT) {
            label = 
                "InputPortNode id=" + boxId + 
                ", parentId=" + parentId + 
                ", rootBox=" + rootBoxId;
        }
        if(getBoxType() == Constants.OUTPUTPORT) {
            label = 
                "OutputPortNode id=" + boxId + 
                ", parentId=" + parentId + 
                ", rootBox=" + rootBoxId;
        }

        
        return new BoxPaletteRecord(boxId,
                                    getBoxType(),
                                    getLabel(), // label
                                    getDescription(),
                                    getModifier(), // modifier
                                    parentId,
                                    0, // cost
                                    0, // selectivity
                                    0, // x
                                    0, // y
                                    0, // width
                                    0, // height
                                    rootBoxId);
    }

    /**
     * Inner class for describing the jmenu items for a popupmenu for each 
     * GenerocPortNode.
     *
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */
    protected class PortMenuItem extends JMenuItem implements ActionListener {

        protected GenericPortNode node;

        /**
         * Constructs a new PortMenuItem for the popupmenu on the 
         * GenericPortNode.
         *
         * @param text The text to be displayed by the PortMenuItem.
         */
        public PortMenuItem(String text, GenericPortNode node) {
            super(text);
            addActionListener(this);
            this.node = node;
        }
       
        /**
         * Handles events on the pop up menus
         *
         * @param e The event associated with the popup menu.
         */ 
        public void actionPerformed(ActionEvent e) 
        {
            Model model = node.getModel();
            if(e.getActionCommand().equals("Properties")) {
                disableMenu();
                showProperties();
            }
            else if(e.getActionCommand().equals("Delete")) {
                disableMenu();
                model.removeNode(GenericPortNode.this);
                model.delete(GenericPortNode.this);
            }
	    else if(e.getActionCommand().equals("Create C++ Struct")) {
		disableMenu();
		showStructDialog();
		    
	    }
        }
    }
}
