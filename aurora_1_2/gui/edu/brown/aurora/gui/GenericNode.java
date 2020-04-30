package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * An abstract class that implements PetalNode.  To be extended by all of the node
 * classes in the gui.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */

public abstract class GenericNode implements PetalNode, java.io.Serializable  {

    /**
     * A popup menu associated with this generic node object.
     */
    protected static JPopupMenu jmenu = null;

    /**
     * A unique box id identifier for this node object.
     */
    protected int boxId;

    /**
     * A counter integer to set boxId's for restream, xsection, join, and 
     * root box input ports
     */
    protected static int lowerBoxIdCounter = 0;

    /**
     * A counter integer to set boxId's for boxes other than restream, xsection, 
     * join, and root box ports.
     */
    protected static int upperBoxIdCounter = Constants.MAX_STREAM_BOX_ID;

    /**
     * The link to this box node.  This should be removed.
     */
    protected PetalLink	node_link;

    /** 
     * The arc that links to this box node.  This should be removed.
     */
    protected PetalArc	link_arc;

    /**
     * The visual component representation of the port in the workspace
     */
    protected Component	node_component;

    /**
     * The model that contains this generic node object.
     */ 
    protected Model model = null;

    /**
     * The id of the parent superbox that contains this GenericNode object.
     */
    protected int parentId;

    

    /**
     * Constructs a generic node.
     *
     * @param boxId The box id identifier to be associated with this generic node. If
     * boxId < 0, then the boxId of this GenericNode is set to the static counter value
     * and the static counter value is incremented by one.
     * @param m The model that contains this generic node.
     */
    public GenericNode(int boxId, int parentId, Model m)
    {
        this.model = m;
        this.parentId = parentId;

        if (boxId < 0) {
            if(isInLowerBoxIds()) {
                this.boxId = lowerBoxIdCounter;
                lowerBoxIdCounter++;
            } else {
                this.boxId = upperBoxIdCounter;
                upperBoxIdCounter++;
            }
        } else {
            this.boxId = boxId;
            if(isInLowerBoxIds()) {
                if(boxId >= Constants.MAX_STREAM_BOX_ID) {
                    System.out.println("Warning!  Setting id " + boxId + 
                                       " of a streaming box " + getLabel() +
				       " greater than max " + 
                                       Constants.MAX_STREAM_BOX_ID);

                    if(Constants.DEBUG) new Exception().printStackTrace();
                }
                if(boxId+1 > lowerBoxIdCounter) {
                    lowerBoxIdCounter = this.boxId + 1;
                }
            } else {
                if(boxId < Constants.MAX_STREAM_BOX_ID) {
                    System.out.println("Warning!  Setting id " + boxId + 
                                       " of non-streaming box " + getLabel() +
				       " less than max " + 
                                       Constants.MAX_STREAM_BOX_ID);

                    if(Constants.DEBUG) new Exception().printStackTrace();
                }
                if(boxId+1 > upperBoxIdCounter) {
                    upperBoxIdCounter = this.boxId + 1;
                }                
            }
        }
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public abstract int getBoxType();

    public abstract boolean isInLowerBoxIds();

    /**
     * Disables the popupmenu
     */
    public static void disableMenu() {
        if(jmenu != null) {
            jmenu.setVisible(false);
        }
    }


    /**
     * Returns the value of the static counter.
     *
     * @return the value of the static counter.
     */
    public int getCurrentIdCounter()
    {
        if(isInLowerBoxIds()) {
            return lowerBoxIdCounter;
        } else {
            return upperBoxIdCounter;
        }
    }
    
    /**
     * Sets the value of the static counter.
     *
     * @param id The value of the static counter to be set.
     * @return The previous value.
     */
    public static int setCurrentLowerIdCounter(int id)
    {
        int temporary = lowerBoxIdCounter;
        lowerBoxIdCounter = id;
        return temporary;
    }

    /**
     * Sets the value of the static counter.
     *
     * @param id The value of the static counter to be set.
     * @return The previous value.
     */
    public static int setCurrentUpperIdCounter(int id)
    {
        int temporary = upperBoxIdCounter;
        upperBoxIdCounter = id;
        return temporary;
    }


    /**
     * Increments the counter by one and returns the previous value.
     *
     * @return The previous value.
     */
    public static int setNextLowerIdCounter()
    {
        int temporary = lowerBoxIdCounter;
        lowerBoxIdCounter++;
        return temporary;
    }

    /**
     * Increments the counter by one and returns the previous value.
     *
     * @return The previous value.
     */
    public static int setNextUpperIdCounter()
    {
        int temporary = upperBoxIdCounter;
        upperBoxIdCounter++;
        return temporary;
    }


    public static int getLowerIdCounter()
    {
        return lowerBoxIdCounter;
    }
    public static int getUpperIdCounter()
    {
        return upperBoxIdCounter;
    }


    /**
     * Returns the unique box id of this box.
     *
     * @return The unique box id of this box.
     */
    public int getBoxId()
    {
        return boxId;
    }
    
    public void setBoxId(int boxId) 
    {
        this.boxId = boxId;
    }

    public Model getModel()
    {
        return model;
    }

    public void setModel(Model model)
    {
        this.model = model;
    }


    /**
     * Returns the id of the parent box.
     *
     * @return the id of the parent superbox.
     */
    public int getParentId()
    {
        return parentId;
    }

    /**
     * Sets the id of the parent box
     *
     * @int parentId The id of the parent box to set
     */
    public void setParentId(int parentId) 
    {
        this.parentId = parentId;
    }

    /**
     * Returns a string representation of this Generic Node object.
     *
     * @return a String representation of this GenericNode object.
     */
    public String toString()
    {
        return ""+boxId;
    }

    /**
     * Returns a record Dbt object for insertion into the palette.
     */
    abstract public PaletteRecord getPaletteRecord(int rootBoxId);


    /**
     * Returns the label contained by the component on this generic
     * object.
     *
     * @return A string representation of the label.
     */
    abstract public String getLabel();


    /**
     * Opens up a popup menu for this generic node object.
     *
     * @param evt The event that opened up this menu.
     */
    abstract public void showMenu(MouseEvent evt);


    /**
     * Gets a petal link associated with this box node.  This implements the interface
     * for PetalNode.
     */
    public PetalLink getLink() {
        return node_link; 
    }
    /**
     * True if this box is the root super box, false otherwise.
     *
     * @return True if this box is the root super box, false otherwise.
     */
    public boolean isRootBox()
    {
        return boxId == Constants.ROOTBOXID;
    }

    /** 
     * Gets the petal arc associated with this box node.  This implements the
     * interface for PetalNode.
     */
    public PetalArc getLinkArc() { 
        return link_arc; 
    }

    /**
     * Sets the link and arcs for this box node.  This implements the interface
     * for PetalNode.
     */
    public void setLink(PetalLink lnk,PetalArc arc) {
        node_link = lnk;
        link_arc = arc;
    }

    /**
     * Returns the visual node component object contained in 
     * this GenericNode object.
     *
     * @return the node_component of this GenericNode
     */
    public Component getNodeComponent()
    {
        return node_component;
    }

    /**
     * Returns a point associated with the port for this box node.  This
     * implements the interface for PetalNode.
     */    
    public Point findPortPoint(Point at,Point from) {
        if(node_component == null) {
            System.out.println("Node_component is null!");
            return new Point(1,1);
        }
        return PetalHelper.findPortPoint(node_component.getBounds(),at,from);
    }

    /**
     * Returns true if the object passed in is equal to this object.
     * Returns false otherwise.
     *
     * @param other An object to test equality with.
     * @return true if other is equal to this GenericNode object, 
     * false otherwise.
     */
    public boolean equals(Object other) {
        if(!(other instanceof GenericNode)) {
            return false;
        }
        GenericNode otherNode = (GenericNode)other;
        return otherNode.boxId == this.boxId;
    }

    
}
