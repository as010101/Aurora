
package edu.brown.aurora.gui;

import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;
import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

import com.sleepycat.db.*;

/**
 * The abstract class that all Boxes are meant to subclass.  BoxNodes contain
 * semantic information about their ports, operators, predicates, etc.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public abstract class BoxNode extends GenericNode
{
    /**
     * The string description of this box node.  This should provide no semantic
     * information about the box itself.
     */
    protected String description;

    /*********************************************************/
    /*	Constructors                                         */
    /*********************************************************/

    /**
     * Constructs a new BoxNode object.
     *
     * @param boxId The unique Box Id identifier of this box node.
     * @param m The model that contains this box node/
     */
    public BoxNode(int boxId, int parentId, Model m) {
        super(boxId, parentId, m);
        node_component = new BoxComponent(this);
        description = new String();
    }


    public Vector getOutputPorts()
    {
	return ((BoxComponent)node_component).getOutputPorts();
    }

    /**
     * Saves the box node port information for this Box Node into the
     * sleepycat database.
     *
     * @param inputPortTable The table containing input port information for boxes.
     * @param outputPortTable The table containing output port information for boxes.
     * @param model The model containing this BoxNode object.
     * @param key
     */
    public void savePortRecords(Db portTable, Model model)
        throws Throwable
    {
        TypeManager tm = RootFrame.typeManager;
        BoxComponent c = (BoxComponent)node_component;

        Vector iPorts = c.getInputPorts();
        Vector oPorts = c.getOutputPorts();

        if (iPorts != null) {
            for (int k = 0; k < iPorts.size(); k++) {
                int typeId = tm.findCompositeTypeIndex(c.getInputPort(k).getType());
                PortRecord inputPort =  new PortRecord(getBoxId(),
                                                       k,
                                                       typeId,
                                                       Constants.INPUTPORTTYPE);
                if(Constants.VERBOSE) System.out.println(inputPort);

                IntegerArrayDbt key = new IntegerArrayDbt(new int[] {boxId,
                                                                     k,
                                                                     Constants.INPUTPORTTYPE,
                                                                     parentId});

                if (portTable.put(null, key, inputPort, 0) == Db.DB_KEYEXIST) {
                    System.out.println("The key already exists.");
                }
            }
        }

        if (oPorts != null) {
            for (int k = 0; k < oPorts.size(); k++) {
                int typeId = tm.findCompositeTypeIndex(c.getOutputPort(k).getType());
                PortRecord outputPort = new PortRecord(getBoxId(),
                                                       k,
                                                       typeId,
                                                       Constants.OUTPUTPORTTYPE);


                if(Constants.VERBOSE) System.out.println(outputPort);

                IntegerArrayDbt key = new IntegerArrayDbt(new int[] {boxId,
                                                                     k,
                                                                     Constants.OUTPUTPORTTYPE,
                                                                     parentId});

                if (portTable.put(null, key, outputPort, 0) == Db.DB_KEYEXIST) {
                    System.out.println("The key already exists.");
                }
            }
        }
    }

    /**
     * Returns a PaletteRecord to save to the palette.
     *
     * @param rootBoxId The root box id that of the palette that contains this record.
     * @return The paletterecord of this primitive box node.
     */
    public PaletteRecord getPaletteRecord(int rootBoxId)
    {
        Rectangle r = getComponent().getBounds();

        BoxPaletteRecord record = new BoxPaletteRecord(boxId,
                                                       getBoxType(),
                                                       getLabel(),
                                                       getDescription(),
                                                       getModifier(),
                                                       parentId,
                                                       (float)0.0,
                                                       (float)0.0,
                                                       r.x, r.y,
                                                       r.width, r.height,
                                                       rootBoxId);


        TypeManager tm = RootFrame.typeManager;
        BoxComponent c = (BoxComponent)node_component;
        Vector iPorts = c.getInputPorts();
        Vector oPorts = c.getOutputPorts();
        if(iPorts != null) {
            for (int k = 0; k < iPorts.size(); k++) {
                int typeId = tm.findCompositeTypeIndex(c.getInputPort(k).getType());
                record.addPaletteRecord(new PortPaletteRecord(getBoxId(), k, typeId,
                                                              Constants.INPUTPORTTYPE,
                                                              rootBoxId));
            }
        }
        if (oPorts != null) {
            for (int k = 0; k < oPorts.size(); k++) {
                int typeId = tm.findCompositeTypeIndex(c.getOutputPort(k).getType());
                record.addPaletteRecord(new PortPaletteRecord(getBoxId(), k, typeId,
                                                              Constants.OUTPUTPORTTYPE,
                                                              rootBoxId));
            }
        }

        return record;
    }


    /**
     * Miscellaneous palette loading method for all of the subclasses.
     *
     * @param node the node that gets modified for all common palette node loading operations.
     * @param record The record that contains the information to load into node.
     * @param model The model which contains this node.
     */
    public static void loadRecord(BoxNode node,
                                  BoxPaletteRecord record,
                                  Model model) throws Throwable
    {
        node.setParentId(model.getSuperBox().getBoxId());
        node.setLabel(record.getLabel());
        node.setDescription(record.getDescription());
        BoxNode.loadPalettePortRecords(node, record, model);
    }

    /**
     * Miscellaneous database loading method for all of the subclasses.
     *
     * @param node the node that gets modified for all common node loading operations.
     * @param record The record that contains the information to load into node.
     * @param model The model which contains this node.
     */
    public static void loadRecord(BoxNode node,
                                  BoxRecord record,
                                  Model model) throws Throwable
    {
        node.setLabel(record.getLabel());
        node.setDescription(record.getDescription());
        node.getComponent().setBounds(record.getRectangle());

        Point location = node.getComponent().getLocation(null);
        model.addNode(node, location);
    }

    /**
     * Loads the box node port information from the sleepycat database.
     *
     * @param portTable The table containing port information for this box node.
     * @param key The key to the portTable that contains the port records for this box.
     * @param model The model that contains this boxnode object.
     */
    public void loadPortRecords(Db portTable, Model model) throws Throwable
    {
        Dbc innerIterator = portTable.cursor(null, 0);
        PortRecord portRecord = new PortRecord();
        BoxComponent c = (BoxComponent) node_component;

        IntegerArrayDbt innerKey = new IntegerArrayDbt(new int[] {boxId,
                                                                  0,
                                                                  0,
                                                                  parentId});
        if (innerIterator.get(innerKey, portRecord, Db.DB_SET) == 0) {
            do {
                if (innerKey.getInteger(0) != boxId) break;
                portRecord.parse();
                if (Constants.VERBOSE) System.out.println(portRecord);
                int typeId = portRecord.getTypeId();
                if (typeId >= 0) {
                    CompositeType types = RootFrame.typeManager.getAllCompositeTypes()[typeId];
                    if(portRecord.getPortType() == Constants.INPUTPORTTYPE) {
                        c.getInputPort(innerKey.getInteger(1)).setType(types);
                    }
                    if(portRecord.getPortType() == Constants.OUTPUTPORTTYPE) {
                      int idx = innerKey.getInteger(1);
                      BoxNodeOutputPort output = c.getOutputPort(idx);
                      while (output == null)
                      {
                        c.addOutputPort();
                        output = c.getOutputPort(idx);
                      }
                      output.setType(types);
                    }
                }
            } while (innerIterator.get(innerKey, portRecord, Db.DB_NEXT) == 0);
        }
        innerIterator.close();
    }

    /**
     * Loads the box node port palette information from the sleepycat database.  This method
     * modifies the port vectors in the record object.
     *
     * @param inputPortTable The table containing input port information for this box node.
     * @param outputPortTable The table containing output port information for this box node.
     * @param Model the model that contains the ports
     * @param record The record to load the port information into.
     */
    public static void loadPalettePortRecords(BoxNode node,
                                              BoxPaletteRecord boxRecord,
                                              Model model) throws Throwable
    {
        Vector ports = boxRecord.getPaletteRecords();
        BoxComponent c = (BoxComponent) node.getNodeComponent();

        for(int i=0; i<ports.size(); i++) {
            PortPaletteRecord portRecord = (PortPaletteRecord) ports.elementAt(i);
            int boxId = portRecord.getBoxId();
            int portIndex = portRecord.getPortIndex();
            int typeId = portRecord.getTypeId();
            int portType = portRecord.getPortType();

            if(typeId >= 0) {
                CompositeType types = RootFrame.typeManager.getAllCompositeTypes()[typeId];
                if(portType == Constants.INPUTPORTTYPE) {
                    c.getInputPort(portIndex).setType(types);

                }
                if(portType == Constants.OUTPUTPORTTYPE) {
                    c.getOutputPort(portIndex).setType(types);
                }
            }
        }
    }


    /**
     * Returns the string description of this box node.
     *
     * @return The String description of this box node.
     */
    public String getDescription()
    {
        return description;
    }


    /**
     * Sets the string description of this box node.
     *
     * @param description the string description for this box.
     */
    public void setDescription(String description)
    {
        this.description = description;
    }

    /**
     * Sets the visual GUI label for this box node.
     *
     * @param labelstr The label to set.
     */
    public void setLabel(String labelstr) {
        RenamableLabel label = ((BoxComponent)(node_component)).getLabel();
        label.setText(labelstr);
    }


    /**
     * Sets the visual GUI label colorfor this box node.
     *
     * @param color The color to set.
     */
    public void setColor(Color color) {
        RenamableLabel label = ((BoxComponent)(node_component)).getLabel();
        label.setColor(color);
    }


    /**
     * Given a point, returns the port which the point is over.
     *
     * @param p The (x,y) point over the BoxNode
     * @return The PortNode which the Port is over.  If the point is over
     * no port node, returns null.
     */
    public GenericBoxNodePort getPortOver(Point p)
    {
        return ((BoxComponent)node_component).getPortOver(p);
    }

    /**
     * Returns the string representation of the box type.
     *
     * @return The string representation of the box type.
     */
    public abstract String getBoxTypeName();

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public abstract int getBoxType();


    /**
     * Returns a string representation of this box node.
     *
     * @return  A string representation of this box node.
     */
    public String toString()
    {
        return
            "label=" + getLabel() +
            " id=" + boxId +
            " parent=" + parentId;
    }

    /**
     * Shows a popup menu for this Box Node
     *
     * @param evt The mouse event that triggered this method
     */
    public void showMenu(MouseEvent evt) {
        GenericNode.disableMenu();
        model.deselectAll();

        jmenu = new JPopupMenu();

        jmenu.add(new BoxJMenuItem("Delete", this));
        jmenu.add(new BoxJMenuItem("Rename", this));
        jmenu.addSeparator();
        jmenu.add(new BoxJMenuItem("Add to Palette", this));
        jmenu.add(new BoxJMenuItem("Properties", this));

        Frame frame = JOptionPane.getFrameForComponent(node_component);
        Point pt = frame.getLocation();
        jmenu.setLocation((int)(evt.getX() + pt.getX() + 40),
                          (int)(evt.getY() + pt.getY()) + 20);

        Callback.showMenu();
    }


    /**
     * Returns the visual gui component associated with this BoxNode object.
     *
     * @return The visual gui component associated with this boxnode object.
     */
    public Component getComponent() {
            return node_component;
    }

    /**
     * An input handler for this PetalNode object.  Handles mouse clicks
     * on the node component.
     *
     * @param evt The mouse event positioned relative to the editor frame.
     */
    public boolean handleMouseClick(MouseEvent evt) {
        return false;
    }

    /**
     * An input handler for this PetalNode object.  Handles keyboard events
     * on this node component.
     *
     * @param evt the keyboard event.
     */
    public boolean handleKeyInput(KeyEvent evt) {
        return false;
    }


    /**
     * Displays a tooltip for this box node.
     *
     * @return the string tool tip for this box node.
     */
    public String getToolTip(Point at) {
        return null;
    }


    /**
     * Returns the label contained by the component on this BoxNode
     * object.
     *
     * @return A string representation of the label.
     */
    public String getLabel()
    {
        BoxComponent boxComponent = (BoxComponent)node_component;
        RenamableLabel label = boxComponent.getLabel();
        return label.getLabel();
    }

    /**
     * Returns the modifier contained by this box node.
     *
     * @return A string representation of the modifier.
     */
    abstract public String getModifier();

    /**
     * A method that saves this node into the database box table.
     *
     * @param boxTable the table to save this node into.
     * @return IntegerDbt the key produced by saving this node into the database.
     */
    abstract public IntegerArrayDbt saveRecord(Db boxTable) throws Throwable;


    /**
     * Opens up a visual properties dialog for this box node.
     *
     */
    public abstract void showProperties();

    /**
     * Inner class for describing the jmenu items for a popupmenu for each box node.
     *
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */
    protected class BoxJMenuItem extends JMenuItem implements ActionListener {
        /**
         * The label associated with the BoxJMenuItem
         */
        protected String label;
        /**
         * The node associated with this BoxJMenuItem's popup menu
         */
        protected BoxNode node;

        /**
         * Constructs a new BoxJMenuItem for the popupmenu on a box node.
         *
         * @param text The string representation of the menu item
         * @param node The BoxNode that raised the popupmenu that contains
         * this menu item.
         */
        public BoxJMenuItem(String text, BoxNode node) {
            super(text);
            label = text;
            this.node = node;
            addActionListener(this);
        }

        /**
         * Handles events on the pop up menus
         *
         * @param e The event that occurred on the popup menu.
         */
        public void actionPerformed(ActionEvent e)
        {
            Model model = node.getModel();
            BoxNode.disableMenu();
            if(e.getActionCommand().equals("Rename")) {
                GenericNode.disableMenu();
                Component component = node.getNodeComponent();
                if(component instanceof BoxComponent) {
                    RenamableLabel label = ((BoxComponent)component).getLabel();
                    label.handleRename();
                }
            }
            if(e.getActionCommand().equals("Delete")) {
                GenericNode.disableMenu();
                model.removeNode(BoxNode.this);
		model.getAuroraEditor().updateWorkspace();
		model.deselectAll();
            }
            if(e.getActionCommand().equals("Properties")) {
                GenericNode.disableMenu();
                node.showProperties();
            }
            if(e.getActionCommand().equals("Add to Palette")) {
                Editor editor = Utils.getRootEditor(model);
                Toolboxspace toolbox = editor.getToolboxspace();
                PalettePanel palette = toolbox.getPalettePanel();
                palette.addPaletteRecord(node.getPaletteRecord(node.getBoxId()));
                Utils.addMessagesText("Box node \"" + node.getLabel() +
                                      "\" successfully added to the palette.\n");
            }
        }
    }
}
