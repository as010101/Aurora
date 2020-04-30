package edu.brown.aurora.gui;

import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;
import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

import com.sleepycat.db.*;
/**
 * Implements a super box.  This is a BoxNode representation of a Model, or a
 * collection of nodes and arcs.  This is an inherently recursive structure,
 * as it is capable of storing a superbox as well as primitive boxes.  
 * This is just a semantic data structure for superboxes,
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class SuperBoxNode extends BoxNode 
{
    /**
     * A static label counter for determining unique superbox labels.
     */
    protected static int label_counter = 0;

    /**
     * The model that this superboxnode represents. (as opposed to the
     * model that contains this superbox, as is for primitive boxes)
     */
    protected Model superBoxModel;

    protected Editor editor;

    /**
     * The catalog manager associated with this superboxnode object.
     */
    CatalogManager cm;

    /**
     * Constructs a new SuperBoxNode object.
     *
     * @param boxId The unique box id for this object determined in the parent
     * class.  
     * @param cm The CatalogManager associated with this SuperBoxNode.
     */
    public SuperBoxNode(int boxId, CatalogManager cm, int parentId) 
    {
        this(boxId, cm, new Model(null, null), null, parentId);
    }
    
    /**
     * Constructs a new SuperBoxNode object.
     *
     * @param boxId The unique box id for this object determined in the
     * parent class.
     * @param superBoxModel The model that this superboxnode represents.
     * @param parentModel The model that contains this superBoxNode.
     */
    public SuperBoxNode(int boxId, CatalogManager cm,
                        Model superBoxModel, Model parentModel) 
    {
        this(boxId, cm, superBoxModel, parentModel, parentModel.getSuperBoxId());
    }

    private SuperBoxNode(int boxId, CatalogManager cm, 
                         Model superBoxModel, Model parentModel, int parentId)
    {
        super(boxId, parentId, parentModel);
        this.cm = cm;
        this.superBoxModel = superBoxModel;
        this.model = parentModel;
        superBoxModel.setSuperBox(this);        
        setColor(Constants.SUPERBOX_COLOR);
        setLabel("Super Box " + label_counter);
        label_counter++;

        ((BoxComponent)node_component).setSize(100, 50);
    }

    /**
     * Returns true if this node gets its box Id set in the lower section of the box id
     * range.
     */
    public boolean isInLowerBoxIds()
    {
        return isRootBox();
    }    

    public String getModifier()
    {
        return "";
    }

    /**
     * Returns the recordDbt object for this box node.
     *
     */
    
    public IntegerArrayDbt saveRecord(Db boxTable) throws Throwable
    {
        String modifier = getModifier();

        IntegerArrayDbt key = new IntegerArrayDbt(new int[]{boxId, parentId});
        Rectangle r = getComponent().getBounds();

        BoxRecord boxRecord = new BoxRecord(boxId,
                                            getBoxType(),
                                            getLabel(),
                                            getDescription(),
                                            modifier,
                                            parentId,
                                            (float)0.0, // cost
                                            (float)0.0, //  selectivity,
					    false,      // isUseDefinedTypes
					    false,      // outputDefinedTypes
                                            r.x, r.y, 
                                            r.width, r.height);
        if (boxTable.put(null, key, boxRecord, 0) == Db.DB_KEYEXIST) {
            System.out.println("SuperBoxNode.saveRecord() - The key already exists.");
        }

        // Saves superboxes recursively.
        CatalogManager.save(superBoxModel, false); 

        return key;
    }

    /**
     * Load a superbox node from the database.
     *
     * @param record The superbox table record that contains the boxnode information
     * for the superbox in the workspace.
     * @param model The model associated with this superBoxNode.
     */
    public static SuperBoxNode loadRecord(BoxRecord record, Model model) throws Throwable
    {        
        CatalogManager cm = RootFrame.catalogManager;

        Model newModel = new Model(null);
        SuperBoxNode node = new SuperBoxNode(record.getBoxId(), cm, 
                                             newModel, model, 
                                             record.getParentId());            
        newModel.setSuperBox(node);
        BoxNode.loadRecord(node, record, model);
        return node;
    }

    /**
     * Load a superbox node from the database.
     *
     * @param record The superbox table record that contains the boxnode information
     * for the superbox in the workspace.
     * @param model The model associated with this superBoxNode.
     */
    public static SuperBoxNode loadRecord(BoxPaletteRecord record, Model model) throws Throwable
    {
        int boxId = setNextUpperIdCounter();

        CatalogManager cm = RootFrame.catalogManager;

        Model newModel = new Model(null);
        SuperBoxNode node = new SuperBoxNode(boxId, cm, 
                                             newModel, model, 
                                             record.getParentId());            

        Editor editor = node.createNewWindow(null);
        node.setLabel(record.getLabel());

        newModel.setSuperBox(node);

        Vector paletteRecords = record.getPaletteRecords();
        // key   - old boxid
        // value - new boxid
        Map boxIdMap = new HashMap(paletteRecords.size());        

        for(int i=0; i<paletteRecords.size(); i++) {
            PaletteRecord paletteRecord = (PaletteRecord) paletteRecords.elementAt(i);
            if(paletteRecord instanceof BoxPaletteRecord) {
                BoxPaletteRecord boxRecord = (BoxPaletteRecord) paletteRecord;
                GenericNode innerNode = boxRecord.getNode(model);

                Integer oldBoxId = new Integer(boxRecord.getBoxId());
                Integer newBoxId = new Integer(innerNode.getBoxId());
                boxIdMap.put(oldBoxId, newBoxId);

                if(innerNode instanceof BoxNode) {
                    innerNode.setParentId(node.getBoxId());
                    newModel.addNode(innerNode, new Point(0,0));
                    BoxComponent c = (BoxComponent) innerNode.getComponent();
                    c.setBounds(boxRecord.getRectangle());
                }
                if(boxRecord.getBoxType() == Constants.INPUTPORT &&
                   boxRecord.getParentId() == record.getId()) {
                    InputPortNode inputPortNode = (InputPortNode) innerNode;
                    innerNode.setParentId(node.getBoxId());
                    newModel.addInputPort(inputPortNode);
                    node.synchronizePortsToModel();
                }
                if(boxRecord.getBoxType() == Constants.OUTPUTPORT &&
                   boxRecord.getParentId() == record.getId()) {
                    OutputPortNode outputPortNode = (OutputPortNode) innerNode;
                    innerNode.setParentId(node.getBoxId());
                    newModel.addOutputPort(outputPortNode);
                    node.synchronizePortsToModel();
                }
            }
        }
        for(int i=0; i<paletteRecords.size(); i++) {
            PaletteRecord paletteRecord = (PaletteRecord) paletteRecords.elementAt(i);

            if(paletteRecord instanceof ArcPaletteRecord) {
                ArcPaletteRecord arcRecord = (ArcPaletteRecord) paletteRecord;
                arcRecord = (ArcPaletteRecord) arcRecord.clone();
                arcRecord.setParentId(node.getBoxId());

                Integer oldSourceBoxId = new Integer(arcRecord.getSourceNodeId());
                Integer oldTargetBoxId = new Integer(arcRecord.getTargetNodeId());

                if(!boxIdMap.containsKey(oldSourceBoxId)) {
                    System.out.println("SuperBoxNode.loadRecord error : boxIdMap does not contain key for source: " + 
                                       oldSourceBoxId);
                    break;
                }
                if(!boxIdMap.containsKey(oldTargetBoxId)) {
                    System.out.println("SuperboxNode.loadRecord error : boxIdMap does not contain key for target: " + 
                                       oldTargetBoxId);
                    break;
                }

                Integer newSourceBoxId = (Integer) boxIdMap.get(oldSourceBoxId);
                Integer newTargetBoxId = (Integer) boxIdMap.get(oldTargetBoxId);

                arcRecord.setSourceNodeId(newSourceBoxId.intValue());
                arcRecord.setTargetNodeId(newTargetBoxId.intValue());

                GenericArc arc = GenericArc.loadRecord(arcRecord, newModel);
                Workspace workspace = newModel.getWorkspace();
                workspace.addArc((GenericNode)arc.getSource(), arc.getOutputPort(),
                                 (GenericNode)arc.getTarget(), arc.getInputPort());
            }
        }

        return node;
    }


    /**
     * 
     */
    public PaletteRecord getPaletteRecord(int rootBoxId)
    {
        PaletteRecord record = super.getPaletteRecord(rootBoxId);
        BoxPaletteRecord boxRecord = (BoxPaletteRecord) record;

        Vector nodes = superBoxModel.getBoxNodes();
        Vector inputPorts = superBoxModel.getInputPorts();
        Vector outputPorts = superBoxModel.getOutputPorts();
        PetalArc[] arcs= superBoxModel.getArcs();

        for(int i=0; i<nodes.size(); i++) {
            BoxNode node = (BoxNode) nodes.elementAt(i);
            PaletteRecord nodeRecord = node.getPaletteRecord(rootBoxId);
            boxRecord.addPaletteRecord(nodeRecord);
            if(Constants.DEBUG) System.out.println("Adding to palette node " + boxRecord);
        }

        for(int i=0; i<inputPorts.size(); i++) {
            InputPortNode port = (InputPortNode) inputPorts.elementAt(i);
            PaletteRecord nodeRecord = port.getPaletteRecord(rootBoxId);
            boxRecord.addPaletteRecord(nodeRecord);
            if(Constants.DEBUG) System.out.println("Adding to palette node " + nodeRecord);
        }
        
        for(int i=0; i<outputPorts.size(); i++) {
            OutputPortNode port = (OutputPortNode) outputPorts.elementAt(i);
            PaletteRecord nodeRecord = port.getPaletteRecord(rootBoxId);
            boxRecord.addPaletteRecord(nodeRecord);
            if(Constants.DEBUG) System.out.println("Adding to palette node " + nodeRecord);
        }

        for(int i=0; i<arcs.length; i++) {
            GenericArc arc = (GenericArc) arcs[i];
            PaletteRecord arcRecord = arc.getPaletteRecord(rootBoxId);
            boxRecord.addPaletteRecord(arcRecord);
            if(Constants.DEBUG) System.out.println("Adding to palette arc " + arcRecord);
        }

        return boxRecord;
    }

    
    public String toString()
    {
        return
            "SuperBox: " + super.toString() + " modifier=" + getModifier();
    }


    /**
     * Sets the catalog manager for this superbox node.
     *
     * @param cm The catalog manager to set.
     */
    public void setCatalogManager(CatalogManager cm)
    {
        this.cm = cm;
    }


    /**
     * Retrieves the model represented by this superboxnode.
     *
     * @return the model that this superboxnode represents.
     */
    public Model getSuperBoxModel()
    {
        return superBoxModel;
    }

    /**
     * Retrieves the parent model of this superboxnode.
     * 
     * @return The parent model that contains this superbox node.
     */
    public Model getParentModel()
    {
        return model;
    }
    
    /**
     * Adds a single input port to this superboxnode.
     *
     * @return The BoxNodeInputPort that was created in this addition.
     */
    public BoxNodeInputPort addInputPort()
    {
        BoxComponent box = (BoxComponent)node_component;
        BoxNodeInputPort boxNodeInputPort = box.addInputPort();

        superBoxModel.synchronizePortsToSuperBoxNode();

        return boxNodeInputPort;
    }

    /**
     * Adds a single output port to this superboxnode.
     *
     * @return The BoxNodeOutputPort that was created in this addition.
     */
    public BoxNodeOutputPort addOutputPort() 
    {

        BoxComponent box = (BoxComponent)node_component;
        BoxNodeOutputPort boxNodeOutputPort = box.addOutputPort();

        superBoxModel.synchronizePortsToSuperBoxNode();

        return boxNodeOutputPort;
    }

    public BoxNodeOutputPort getOutputPort(int idx)
    {
        Vector outputs = ((BoxComponent)node_component).getOutputPorts();
	return (BoxNodeOutputPort) outputs.elementAt(idx);
    }

    public BoxNodeInputPort getInputPort(int idx)
    {
        Vector inputs = ((BoxComponent)node_component).getInputPorts();
	return (BoxNodeInputPort) inputs.elementAt(idx);
    }

    public BoxNodeOutputPort getLastOutputPort()
    {
        Vector outputs = ((BoxComponent)node_component).getOutputPorts();

        return (BoxNodeOutputPort)outputs.elementAt(outputs.size() - 1);
    }


    public BoxNodeInputPort getLastInputPort()
    {
        Vector inputs = ((BoxComponent)node_component).getInputPorts();

        return (BoxNodeInputPort)inputs.elementAt(inputs.size() - 1);
    }




    /**
     * An input handler for mouse clicks.  A double click creates a
     * new superboxnode editor frame.
     *
     * @param evt The mouse event that invoked this method.
     */
    public boolean handleMouseClick(MouseEvent evt)
    {     
        if(evt.getClickCount() == 2) {
            Editor editor = createNewWindow(null);
            if(!editor.isVisible()) {
                editor.setVisible(true);
            }
        }
        return false;
    }

    /**
     * Creates a new window for this superbox.
     *
     * @param name The name of the new window for this superbox.
     * @return The window that was created.
     */
    public Editor createNewWindow(String name) 
    {
        if(name == null) {
            name = "Editor for " + getLabel();
        }

        if(editor == null) {
            PetalEditor petalEditor = new PetalEditor(superBoxModel);
            editor = new Editor(cm, name, petalEditor, superBoxModel);
            superBoxModel.setEditor(editor.getEditor());

            /*
            try {
                CatalogManager.load(superBoxModel);
            } catch (Throwable t) {
                System.out.println("Error loading catalog.");
                t.printStackTrace();
            }
            editor.getWorkspace().resetPortNodePositions();
            */
        }
        return editor;
    }

    /**
     * Sets the number of input and output ports of this SuperBoxNode equal to the
     * number of ports in its associated model.
     
     */
    public void synchronizePortsToModel()    
    {
        BoxComponent box = (BoxComponent)node_component;
        int superBoxInputPorts = box.getNumInputs();
        int superBoxOutputPorts = box.getNumOutputs();
        int modelInputPorts = superBoxModel.getNumberOfInputPorts();
        int modelOutputPorts = superBoxModel.getNumberOfOutputPorts();

        int inputsToAdd = modelInputPorts - superBoxInputPorts;
        int outputsToAdd = modelOutputPorts - superBoxOutputPorts;

        for(int i=0; i<inputsToAdd; i++) {
            addInputPort();
        }
        for(int i=0; i<outputsToAdd; i++) {
            addOutputPort();
        }

        if(inputsToAdd < 0 || outputsToAdd < 0) {
            System.out.println("SuperBoxNode.synchronizePortsToModel() : The code to delete outputports from the model is implemented in Model.delete(), this message should never occur.");
        }
    }


    /**
     * Opens up a popup menu for this superboxnode object.
     *
     * @param tm The TypeManager for the gui
     * @param evt The event that opened up this menu.
     */
    public void showMenu(MouseEvent evt) {
        GenericNode.disableMenu();
        jmenu = new JPopupMenu();

        jmenu.add(new BoxJMenuItem("Delete", this));
        jmenu.add(new BoxJMenuItem("Rename", this));
        jmenu.addSeparator();
        jmenu.add(new SuperBoxJMenuItem("Add Input Port", this));
        jmenu.add(new SuperBoxJMenuItem("Add Output Port", this));       
        jmenu.addSeparator();
        jmenu.add(new SuperBoxJMenuItem("Add to Palette", this));
        jmenu.add(new SuperBoxJMenuItem("Show superbox Window", this));
        jmenu.add(new SuperBoxJMenuItem("Decompose", this));
        jmenu.addSeparator();
        jmenu.add(new SuperBoxJMenuItem("Properties", this));

        Frame frame = JOptionPane.getFrameForComponent(node_component);
        Point pt = frame.getLocation();
        jmenu.setLocation((int)(evt.getX() + pt.getX() + 40), 
                          (int)(evt.getY() + pt.getY()) + 20);  

        Callback.showMenu();
    }

    /**
     * Returns the editor stored by this superboxnode object.
     *
     * @return The editor frame representing this superboxnode.
     */
    public Editor getEditor()
    {
        return editor;
    }

    /**
     * Opens up a visual properties dialog for this super box node.
     *
     * @param tm The TypeManager for the editor.
     */
   public void showProperties()
    {
        new SuperBoxPropertiesDialog(this);
    }

    /**
     * Returns the name of this superbox class.
     *
     * @return The name as a string.
     */
    public String getBoxTypeName()
    {
        return "Super Box";
    }

    /**
     * Returns the int value for the type of this box.  Refer to the box type values
     * @see edu.brown.aurora.gui.Constants
     *
     * @return An int value for the type of this box.
     */
    public int getBoxType()
    {
        return Constants.SUPERBOX;
    }

    /**
     * Decomposes a superbox.
     */
    public void decompose()
    {
	TypeInferencer.setEnabled(false);
        if(this.getParentModel() == null) {
            JOptionPane.showConfirmDialog(null, 
                                          "This is the root superbox, cannot be decomposed.",
                                          "Error!", 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);         
        } else {
            Model parentModel = this.getParentModel();
            PetalNode[] nodes = superBoxModel.getNodes();
	    Workspace workspace = parentModel.getWorkspace();

            for(int i=0; i<nodes.length; i++) {
                
                Point point = new Point(0,0);
                if(nodes[i] instanceof GenericNode) {
                    GenericNode genericNode = (GenericNode)nodes[i];
                    Component component = genericNode.getNodeComponent();
                    point = component.getLocation(null);
                    genericNode.setParentId(this.getParentId());
                    genericNode.setModel(parentModel);
                }


                if(nodes[i] instanceof BoxNode) {
                    parentModel.addNode(nodes[i], point);                    
                }
            }

	    PetalArc[] arcs = superBoxModel.getArcs();
	    for(int i=0; i<arcs.length; i++) {
		GenericArc arc = (GenericArc)arcs[i];

		GenericNode sourceNode = (GenericNode)arc.getSource();
		GenericNode targetNode = (GenericNode)arc.getTarget();
		
		boolean sourceInside = !(sourceNode instanceof GenericPortNode);
		boolean targetInside = !(targetNode instanceof GenericPortNode);

		if(sourceInside && targetInside) {
		    workspace.addArc(sourceNode, arc.getOutputPort(),
				     targetNode, arc.getInputPort());
		}
                		
                if(!sourceInside) {
                    GenericPortNode sourcePortNode = (GenericPortNode)sourceNode;
                    int index = sourcePortNode.getIndex();
                    PetalArc[] parentArcs = parentModel.getArcs();
                    for(int j=0; j<parentArcs.length; j++) {
                        GenericArc parentArc = (GenericArc)parentArcs[j];
                        
                        BoxNodeInputPort superBoxInputPort = parentArc.getInputPort();
                        int superBoxIndex = superBoxInputPort.getIndex();
                        
                        if(index == superBoxIndex && parentArc.getTarget().equals(this)) {
                            GenericNode parentSourceNode = (GenericNode)parentArc.getSource();
                            workspace.addArc(parentSourceNode, parentArc.getOutputPort(),
                                             targetNode, arc.getInputPort());
                        }
                        
                        
	 	    }
		}
                if(!targetInside) {
                    GenericPortNode targetPortNode = (GenericPortNode)targetNode;
                    int index = targetPortNode.getIndex();
                    PetalArc[] parentArcs = parentModel.getArcs();
                    for(int j=0; j<parentArcs.length; j++) {
                        GenericArc parentArc = (GenericArc)parentArcs[j];
                        
                        BoxNodeOutputPort superBoxOutputPort = parentArc.getOutputPort();
                        int superBoxIndex = superBoxOutputPort.getIndex();

                        if(index == superBoxIndex && parentArc.getSource().equals(this)) {
                            GenericNode parentTargetNode = (GenericNode)parentArc.getTarget();
                            workspace.addArc(sourceNode, arc.getOutputPort(),
                                             parentTargetNode, parentArc.getInputPort());
                        }
                    }
                }		
	    }

	    for(int i=0; i<nodes.length; i++) { 
                superBoxModel.removeNode(nodes[i]);
            }

            Editor editor = superBoxModel.getAuroraEditor();
            if(editor != null) {
                editor.dispose();
	    }

	    parentModel.removeNode(this);
            parentModel.deselectAll();
            Editor parentEditor = parentModel.getAuroraEditor();
            if(parentEditor != null) {
                parentEditor.updateWorkspace();
            }
        }
	TypeInferencer.setEnabled(true);

    }

    /**
     * Inner class for describing the jmenu items for a popupmenu for each 
     * super box node.
     *
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */
    private class SuperBoxJMenuItem extends BoxJMenuItem implements ActionListener {
        /**
         * Constructs a new BoxJMenuItem for the popupmenu on a box node.
         *
         * @param text The string representation of the menu item
         * @param node The BoxNode that raised the popupmenu that contains
         * this menu item.
         */
        public SuperBoxJMenuItem(String text, BoxNode node) {
            super(text, node);
        }
        
        /**
         * Handles events on the popup menus.
         *
         * @param e The event that occurred on the popup menu.
         */
        public void actionPerformed(ActionEvent e) 
        {
            super.actionPerformed(e);
            BoxNode.disableMenu();
            if(e.getActionCommand().equals("Add Input Port")) {
                ((SuperBoxNode)node).addInputPort();
            }
            if(e.getActionCommand().equals("Add Output Port")) {            
                ((SuperBoxNode)node).addOutputPort();
            }
            if(e.getActionCommand().equals("Show superbox Window")) {
                Editor editor = ((SuperBoxNode)node).createNewWindow(null);
                editor.setVisible(true);                
            }
            if(e.getActionCommand().equals("Decompose")) {
                ((SuperBoxNode)node).decompose();
            }
        }
    }


}
