package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;

import javax.swing.*;
import javax.swing.undo.*;
import javax.swing.event.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.awt.Rectangle;
import edu.brown.aurora.gui.dbts.*;
import edu.brown.aurora.gui.types.*;
import com.sleepycat.db.*;
import java.io.*;

import java.util.*;

/**
 * The model of the GUI diagram.  This class contains the semantic information of the
 * diagram, which primarily includes all of the box nodes and their arcs.  Other components
 * such as the types are implicitly contained within each of the components.  This
 * class reflects the data within a superbox.
 * <br><br>
 * This class implements PetalModel and is the key component of the data representation
 * of the petal node/arc data structure.  It allows the capability of adding, removing,
 * and modifying the nodes and arcs of the diagram.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Model implements PetalModel, java.io.Serializable
{
    private Vector node_list;
    private Vector arc_list;
    private Vector inputport_list;
    private Vector outputport_list;

    transient private PetalSelectionSet select_set;
    transient private PetalLayoutMethod layout_method;

    transient private PetalEditor the_editor;
    transient private Editor editor;
    transient private Workspace workspace;
    private SuperBoxNode superBox;

    /**
     * Stateful storage variable.  This variable stores the first node when an
     * arc creation event occurs.
     */
    private GenericNode start_node;

    /**
     * Stateful storage variable.  This variable stores the first port when an
     * arc creation event occurs.
     */
    private PetalPort start_port;

    /**
     * Stateful storage variable.  This variable stores the first boxnode port when
     * an arc creation event occurs.
     */
    private BoxNodeOutputPort start_portNode;

    /**
     * Stateful storage variable.  This variable stores whether the workspace should
     * be editable or not.
     */
    private boolean edit;


    /**
     * Constructs a new Model object.
     *
     * @param editor The Editor frame to which this model is associated.
     * @param superBox The superBox to which this model represents.
     */
    public Model(Editor editor, SuperBoxNode superBox)
    {
        this.editor = editor;
        this.superBox = superBox;
        node_list = new Vector();
        arc_list = new Vector();

        select_set = new PetalSelectionSet(this);
        layout_method = null;
        start_node = null;
        start_port = null;
        edit = true;

        inputport_list = new Vector();
        outputport_list = new Vector();
    }

    /**
     * Constructs a new Model object without an association to a particular
     * frame.
     *
     * @param superBox The superBox to which this model represents.
     */
    public Model(SuperBoxNode superBox)
    {
        this(null, superBox);
    }


    public void setSelectSet()
    {
        select_set = new PetalSelectionSet(this);
    }

    /**
     * Returns a string representation of this Model object.
     *
     * @return A string represention of this model object.
     */
    public String toString()
    {
        String s = "";
        for (int i = 0; i < node_list.size(); i++) {
            s += node_list.elementAt(i) +"\r\n";
        }
        return s;
    }

    /**
     * Sets attributes of this model to another. (ie: cloning)
     *
     * @param model The model to set this model's attributes to.
     */
    public void set(Model model)
    {
        this.node_list = model.node_list;
        this.arc_list = model.arc_list;
        this.inputport_list = model.inputport_list;
        this.outputport_list = model.outputport_list;
    }

    /**
     * Clears all of the nodes and arcs from the model.
     */
    public void clear()
    {
        clearArcs();
        clearNodes();
        clearPorts();
    }


    /**
     * Adds a node to the model.
     *
     * @param n The PetalNode to add.
     * @param p
     */
    public synchronized void addNode(PetalNode n, Point p)
    {
        if (!node_list.contains(n)) {
            node_list.addElement(n);
        }
    }

    /**
     * Retrieves the number of output ports in this model.
     *
     * @return an int specifyin the number of output ports in this model.
     */
    public synchronized int getNumberOfOutputPorts()
    {
        return outputport_list.size();
    }


    /**
     * Retrieves the number of input ports in this model.
     *
     * @return an int specifying the number of input ports in this model.
     */
    public synchronized int getNumberOfInputPorts()
    {
        return inputport_list.size();
    }

    /**
     * Deletes a single node from the model.  This method guarantees that all inputport nodes
     * and outputport nodes will have indices that increase by 1.
     *
     * @param node The node to delete from the model.
     */
    public synchronized void delete(GenericPortNode node)
    {
        Vector port_list = new Vector();
        BoxComponent boxComponent = (BoxComponent)superBox.getComponent();
        GenericBoxNodePort nodePort = null;
        int index = 0;


        if(node instanceof InputPortNode) {
            port_list = inputport_list;
            index = port_list.indexOf(node);
            if(superBox.getBoxId() != 0) {
                nodePort = boxComponent.removeInputPort(index);
            }
        }
        if(node instanceof OutputPortNode) {
            port_list = outputport_list;
            index = port_list.indexOf(node);
            if(superBox.getBoxId() != 0) {
                nodePort = boxComponent.removeOutputPort(index);
            }
        }

        if(index >= 0) {
            // remove arcs from the parent model.
            Model parentModel = superBox.getParentModel();
            if(parentModel != null) {
                Vector arcs = nodePort.getArcs();
                while(arcs.size() != 0) {
                    parentModel.removeArc((PetalArc)arcs.elementAt(0));
                }
            }

            port_list.removeElementAt(index);


            // remove arcs from this model.
            removeArcs(node,null);
            removeArcs(null,node);

            // reset all of the indices of the port list.
            for(int i=0; i<port_list.size(); i++) {
                GenericPortNode next = (GenericPortNode)port_list.elementAt(i);
                next.setIndex(i);
            }
        }

        workspace.resetPortNodePositions();
    }

    /**
     * Retrieves the Vector of input ports contained in this model.
     *
     * @return A vector of input ports contained in this model.
     */
    public Vector getInputPorts()
    {
        return inputport_list;
    }

    /**
     * Retrieves the vector of output ports contained in this model.
     *
     * @return A vector of output ports contained in this model.
     */
    public Vector getOutputPorts()
    {
        return outputport_list;
    }

    /**
     * Retrieves the vector of box nodes contained in this model.
     *
     * @return A vector of box nodes contained in this model.
     */
    Vector getBoxNodes()
    {
        return node_list;
    }

    /**
     * Retrieves the vector of arcs contained in this model.
     *
     * @return A vector of arcs contained in this model.
     */
    Vector getArcsVector()
    {
        return arc_list;
    }

    /**
     * Add one output port to the model.
     *
     * @return the index of the added port.
     */
    public synchronized OutputPortNode addOutputPort()
    {
        BoxNodeInputPort boxNodeInputPort = new BoxNodeInputPort(PetalPort.WEST,
                                                                 0,
                                                                 null);
        OutputPortNode output = new OutputPortNode(-1, this, getSuperBoxId(),
                                                   getNumberOfOutputPorts(),
                                                   boxNodeInputPort,
                                                   OutputPortNode.EMPTYQOSSTRING);
        boxNodeInputPort.setGenericNode(output);

        outputport_list.addElement(output);
        if(workspace != null) {
            workspace.resetPortNodePositions();
        }

        return output;
    }

    /**
     * Add one output port to the model.
     *
     * @param output The output port to add.
     */
    public synchronized void addOutputPort(OutputPortNode output)
    {
        BoxNodeInputPort boxNodeInputPort = new BoxNodeInputPort(PetalPort.WEST,
                                                                 0,
                                                                 null);
        boxNodeInputPort.setGenericNode(output);

        outputport_list.addElement(output);
        if(workspace != null) {
            workspace.resetPortNodePositions();
        }

    }



    /**
     * Add one output port to the model.
     *
     * @return the index of the added port.
     */
    public synchronized InputPortNode addInputPort()
    {
        BoxNodeOutputPort boxNodeOutputPort = new BoxNodeOutputPort(PetalPort.EAST,
                                                                    new JPanel(),
                                                                    0,
                                                                    null);
        InputPortNode input = new InputPortNode(-1, -1, this, getSuperBoxId(),
                                                getNumberOfInputPorts(),
                                                boxNodeOutputPort,
                                                0, true, true, -1, false);
        boxNodeOutputPort.setGenericNode(input);

        inputport_list.addElement(input);
        if(workspace != null) {
            workspace.resetPortNodePositions();
        }

        return input;
    }


    /**
     * Add one output port to the model.
     *
     * @param input the inputportnode to add
     */
    public synchronized void addInputPort(InputPortNode input)
    {
        BoxNodeOutputPort boxNodeOutputPort = new BoxNodeOutputPort(PetalPort.EAST,
                                                                    new JPanel(),
                                                                    0,
                                                                    null);
        boxNodeOutputPort.setGenericNode(input);

        inputport_list.addElement(input);
        if(workspace != null) {
            workspace.resetPortNodePositions();
        }
    }



    /**
     * Sets the number of input and output ports of this Model equal to the
     * number of ports in its associated SuperBoxNode.
     */
    public void synchronizePortsToSuperBoxNode()
    {
        BoxComponent box = (BoxComponent)superBox.getNodeComponent();
        int superBoxInputPorts = box.getNumInputs();
        int superBoxOutputPorts = box.getNumOutputs();

        int modelInputPorts = getNumberOfInputPorts();
        int modelOutputPorts = getNumberOfOutputPorts();

        int inputsToAdd = superBoxInputPorts - modelInputPorts;
        int outputsToAdd = superBoxOutputPorts - modelOutputPorts;

        for(int i=0; i<inputsToAdd; i++) {
            addInputPort();
        }
        for(int i=0; i<outputsToAdd; i++) {
            addOutputPort();
        }

        if(inputsToAdd < 0) {
            System.out.println("Model.synchronizePortsToSuperBoxNode() : The capability of deleting input ports from the superbox is not yet supported.");
            //System.out.println("SuperboxInputPorts = " + superBoxInputPorts +
            //                   ", modelInputPorts = " + modelInputPorts);
            //Exception e = new Exception("");
            //e.printStackTrace();
        }
        if(outputsToAdd < 0) {
            System.out.println("Model.synchronizePortsToSuperBoxNode() : The capability of deleting output ports from the superbox is not yet supported.");
            //System.out.println("SuperboxOutputPorts = " + superBoxOutputPorts +
            //                   ", modelOutputPorts = " + modelOutputPorts);
            //Exception e = new Exception("");
            //e.printStackTrace();

        }

    }


    /**
     * Removes nodes from the model along with all of its connected arcs.
     *
     * @param n The node to remove from the model.
     */
    public synchronized void removeNode(PetalNode n)
    {
        node_list.removeElement(n);
        removeArcs(n,null);
        removeArcs(null,n);
    }

    /**
     * Retrieves an array of PetalNodes from the model.
     *
     * @return the array of petal nodes contained in this model.
     */
    public synchronized PetalNode [] getNodes()
    {
        Vector allNodes = new Vector();
        allNodes.addAll(node_list);
        allNodes.addAll(inputport_list);
        allNodes.addAll(outputport_list);
        int ct = allNodes.size();
        PetalNode [] nds = new PetalNode[ct];
        allNodes.copyInto(nds);

        return nds;
    }

    /**
     * Handler for dropping a node onto the PetalEditor using (DnD) (Drag & Drop)
     *
     * @param o The object to drop (dragged from an external source).
     * @param p The point where the object is to be dropped.
     * @param pn The PetalNode to that is created from this dropping process.
     * @param pa
     *
     * @return true if a node has been added, false if this method does not
     * handle the call and a node has not been added.
     */
    public synchronized boolean dropNode(Object o, Point p,
                                         PetalNode pn, PetalArc pa)
    {
        Component pane = editor.getToolboxspace().getSelectedTabbedPane();

	boolean useDefinedTypes = false;
	boolean outputDefinedType = false;

        if(!(o instanceof StringReader)) {
            if (o.toString().equals(BoxesPanel.ADD_SUPER_BOX)) {
                Model newModel = new Model(null);
                SuperBoxNode superBox = new SuperBoxNode(-1, null, newModel, this);
                superBox.setCatalogManager(editor.getCatalogManager());
                newModel.setSuperBox(superBox);
                workspace.addNode(superBox, p);
            } else if (o.toString().equals(BoxesPanel.ADD_FILTER_BOX)) {
                workspace.addNode(new FilterBoxNode(-1, getSuperBoxId(), "", this,
						    useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_DROP_BOX)) {
                workspace.addNode(new DropBoxNode(-1, getSuperBoxId(), "", this,
						    useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_MAP_BOX)) {
                workspace.addNode(new MapBoxNode(-1, getSuperBoxId(), "", this,
						 useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_UNION_BOX)) {
                workspace.addNode(new UnionBoxNode(-1, getSuperBoxId(), "", this,
						   useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_AGGREGATE_BOX)) {
                workspace.addNode(new AggregateBoxNode(-1, getSuperBoxId(), "", this,
						    useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_UPDATE_RELATION_BOX)) {
                workspace.addNode(new UpdateRelationBoxNode(-1, getSuperBoxId(), "", this,
                                                   useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_READ_RELATION_BOX)) {
                workspace.addNode(new ReadRelationBoxNode(-1, getSuperBoxId(), "", this,
                                                   useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_BSORT_BOX)) {
                workspace.addNode(new BsortBoxNode(-1, getSuperBoxId(), "", this,
                                                   useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_RESTREAM_BOX)) {
                workspace.addNode(new RestreamBoxNode(-1, getSuperBoxId(), "", this,
						      useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_JOIN_BOX)) {
                workspace.addNode(new JoinBoxNode(-1, getSuperBoxId(), "", this,
						  0, useDefinedTypes, outputDefinedType), p);
            } else if (o.toString().equals(BoxesPanel.ADD_RESAMPLE_BOX)) {
                workspace.addNode(new ResampleBoxNode(-1, getSuperBoxId(), "", this,
						      useDefinedTypes, outputDefinedType), p);
              } else if (o.toString().equals(BoxesPanel.ADD_UNKNOWN_BOX)) {
                  workspace.addNode(new UnknownBoxNode(-1, getSuperBoxId(), "", this,
                                                   useDefinedTypes, outputDefinedType), p);
            } else if (pane instanceof PalettePanel) {
                try {
                    PalettePanel palette = (PalettePanel) pane;
                    BoxPaletteRecord record = (BoxPaletteRecord) palette.getSelectedValue();
                    GenericNode node = record.getNode(this);
                    workspace.addNode(node, p);
                    if(Constants.VERBOSE) System.out.println("adding to palette : " + node);
                } catch (Throwable t) {
                    t.printStackTrace();
                }
            } else {
            }
            Utils.updateHierarchy(this);
        }
        return false;
    }


    /**
     * Adds an arc to the model.  Note: don't call this method.  Call
     * The addArc method from workspace.  That method will automatically
     * call this one as well.
     *
     * @param a The arc to add to the model.
     */
    public synchronized void addArc(PetalArc a)
    {
	// TODO: JH, can we whack this line as superfluous?  - Matt
	if (arc_list.contains(a)) return;

        arc_list.addElement(a);
        TypeInferencer.typeCheck();
    }

    /**
     * Removes an arc from the model.
     *
     * @param a The arc to add to the model.
     */
    public synchronized void removeArc(PetalArc a)
    {
        arc_list.removeElement(a);
        if(a instanceof GenericArc) {
            GenericArc arc = (GenericArc)a;
            BoxNodeInputPort inputPort = arc.getInputPort();
            BoxNodeOutputPort outputPort = arc.getOutputPort();
            inputPort.removeArc(arc);
            outputPort.removeArc(arc);
	    TypeInferencer.typeCheck();
        }
    }

    /**
     * Removes an arc from the model.
     *
     * @param from The "from" node that the arc is attached to.
     * @param to The "to" node that the arc is attached to.
     */
    public synchronized void removeArcs(PetalNode from,PetalNode to)
    {
        int j = 0;
        int ct = arc_list.size();

        for (int i = 0; i < ct; ++i) {
            PetalArc a = (PetalArc) arc_list.elementAt(i);

            if ((from == null || a.getSource() == from) &&
                (to == null || a.getTarget() == to)) {
		++j;

		if(a instanceof GenericArc) {
		    GenericArc ga = (GenericArc) a;
		    BoxNodeInputPort inputPort = ga.getInputPort();
		    BoxNodeOutputPort outputPort = ga.getOutputPort();
		    inputPort.removeArc(ga);
		    outputPort.removeArc(ga);
		}

	    }
            else if (j > 0) arc_list.setElementAt(a,i-j);
        }

        if (j != 0) arc_list.setSize(ct-j);
    }

    /**
     * Returns a list of arcs as an array that are contained in this model.
     *
     * @return An array of arcs contained in this model.
     */

    public synchronized PetalArc [] getArcs()
    {
        int ct = arc_list.size();
        PetalArc [] arcs = new PetalArc[ct];
        arc_list.copyInto(arcs);

        return arcs;
    }


    /**
     * Returns a list of "from" arcs that are associated with a particular node.
     *
     * @param n The node whose connected arcs are to be returned.
     * @return An array of PetalArcs connected to n.
     */
    public synchronized PetalArc [] getArcsFromNode(PetalNode n)
    {
        int ct = 0;

        GenericNode node = (GenericNode)n;
        for(int i=0; i<arc_list.size(); i++) {
            PetalArc a = (PetalArc) arc_list.elementAt(i);
            GenericNode other = (GenericNode)(a.getSource());

            if (other.equals(node)) {
                ++ct;
            }
        }

        PetalArc [] arcs = new PetalArc[ct];
        ct = 0;
        for(int i=0; i<arc_list.size(); i++) {
            PetalArc a = (PetalArc) arc_list.elementAt(i);
            GenericNode other = (GenericNode)(a.getSource());

            if(other.equals(node)) {
                arcs[ct++] = a;
            }
        }

        return arcs;
    }

    /**
     * Returns a list of "to" arcs that are associated with a particular node.
     *
     * @param n The node whose connected arcs are to be returned.
     * @return An array of PetalArcs associated with the PetalNode.
     */
    public synchronized PetalArc [] getArcsToNode(PetalNode n)
    {
        int ct = 0;
        GenericNode node = (GenericNode)n;

        for(int i=0; i<arc_list.size(); i++) {
            PetalArc a = (PetalArc) arc_list.elementAt(i);
            GenericNode other = (GenericNode)(a.getTarget());

            if (other.equals(node)) {
                ++ct;
            }
        }

        PetalArc [] arcs = new PetalArc[ct];
        ct = 0;
        for(int i=0; i<arc_list.size(); i++) {
            PetalArc a = (PetalArc) arc_list.elementAt(i);
            GenericNode other = (GenericNode)(a.getTarget());

            if(other.equals(node)) {
                arcs[ct++] = a;
            }
        }

        return arcs;
    }


    /**
     * Creates an arc.  Not implemented.
     *
     *
     */
    public void createArc(PetalNode f,PetalNode t)
    {
    }

/************************************************************************/
/*	Methods for handling cut and paste				*/
/************************************************************************/

    /**
     * Returns a copied object. (arc)
     *
     *
     */
    public Object getCopyObject(PetalArc pa)
    {
        return pa;
    }

    /**
     * Returns a copied object. (node)
     *
     *
     */
    public Object getCopyObject(PetalNode pn)
    {
        return pn;
    }

    /**
     * Adds a node by paste.  Not implemented.
     *
     *
     */
    public PetalNode addPasteNode(Object o,boolean dofg)
    {
        return null;
    }

    /**
     * Adds a node by paste.  Not implemented.
     *
     *
     */
    public PetalNode addPasteNode(Object o,PetalClipSet pcs,boolean dofg)
    {
        return null;
    }

    /**
     * Adds an arc by paste.  Not implemented.
     *
     *
     */
    public PetalArc addPasteArc(Object o,PetalClipSet pcs,boolean dofg)
    {
        return null;
    }


    /**
     * Sets the layout method for this model.
     *
     * @param m The layoutmethod to set on this model.
     */
    public void setLayoutMethod(PetalLayoutMethod m)
    {
        layout_method = m;
    }

    /**
     * Retrieves the layout method.
     *
     * @return A PetalLayoutMethod set in this model.
     */
    public PetalLayoutMethod getLayoutMethod()
    {
        return layout_method;
    }

    /**
     * Updates the model using the layout set in this model.
     */
    public void updateLayout()
    {
        if (layout_method != null) layout_method.doLayout(this);
    }


/************************************************************************/
/*	Methods for handling selection					*/
/************************************************************************/

    /**
     * Selects an arc or a node in the workspace.
     *
     * @param o The object selected in the workspace.
     */
    public synchronized void select(Object o)
    {
        Toolbarspace toolbar = editor.getToolbarspace();
        if(!(o instanceof GenericPortNode) && !toolbar.isAddArcMode()) {
            select_set.select(o);
        }
    }

    /**
     * Selects all objects within a rectangular box in the workspace.
     *
     * @param box The rectangle specified to select objects.
     */
    public synchronized void selectInBox(Rectangle box)
    {
        select_set.selectInBox(box);
        for(int i=0; i<inputport_list.size(); i++) {
            deselect(inputport_list.elementAt(i));
        }
        for(int i=0; i<outputport_list.size(); i++) {
            deselect(outputport_list.elementAt(i));
        }
    }

    /**
     * Selects all of the objects within the workspace.
     */
    public synchronized void selectAll()
    {
        select_set.selectAll();
    }

    /**
     * De-selects all of the objects within the workspace.
     */
    public synchronized void deselectAll()
    {
        select_set.deselectAll();
    }

    /**
     * De-selects one object within the workspace.
     */
    public synchronized void deselect(Object o)
    {
        select_set.deselect(o);
    }

    /**
     * Returns an array of selected nodes from the workspace.
     *
     * @return an array of selected nodes from the workspace.
     */
    public synchronized PetalNode [] getSelectedNodes()
    {
        return select_set.getSelectedNodes();
    }

    /**
     * Returns an array of selected arcs from the workspace.
     *
     * @return an array of selected arcs from the workspace.
     */
    public synchronized PetalArc [] getSelectedArcs()
    {
        return select_set.getSelectedArcs();
    }

    /**
     * Returns a boolean determining whether a particular node has been selected or not.
     *
     * @param n A petalNode which is either selected or not.
     * @return True if petalNode n is selected, false otherwise.
     */
    public synchronized boolean isSelected(PetalNode n)
    {
        return select_set.isSelected(n);
    }

    /**
     * Returns a boolean determining whether a particular arc has been selected or not.
     *
     * @param n A petalArc which is either selected or not.
     * @return True if petalArc n is selected, false otherwise.
     */
    public synchronized boolean isSelected(PetalArc a)
    {
        return select_set.isSelected(a);
    }

    /**
     * Returns an int specifying the number of selections in the workspace.
     *
     * @return an int specifying the number of selections.
     */
    public int getNumSelections()
    {
        return select_set.getNumSelections();
    }

    /**
     * Finds correlations.  Given a point, finds whether a node occupies that point.
     *
     * @param p The point to test correlation,
     * @return A PetalNode if p exists within that node, null otherwise.
     */
    public PetalNode findNodeAtLocation(Point p)
    {
        for (Enumeration e = node_list.elements(); e.hasMoreElements(); ) {
            PetalNode n = (PetalNode) e.nextElement();
            Component c = n.getComponent();
            Rectangle r = c.getBounds();
            if (r != null && r.contains(p)) return n;
        }

        return null;
    }


    /**
     * Finds correlations.  Given a point, finds whether a arc occupies that point.
     *
     * @param p The point to test correlation,
     * @return A PetalArc if p exists within that arc, null otherwise.
     */
    public PetalArc findArcAtLocation(Point p)
    {
        for (Enumeration e = arc_list.elements(); e.hasMoreElements(); ) {
            PetalArc a = (PetalArc) e.nextElement();
            if (a.contains(p)) return a;
        }

        return null;
    }

    /**
     * Creates new arcs between nodes.  This method is stateful -- If an
     * arc is not in the process of being created, then a call of this method
     * begins a process of creating the arc and changnes state.  If an
     * arc is in the process of being created, then the second call of this
     * method attempts to finish creating an arc.
     *
     * @param pn The PetalNode that was selected which made a call to this
     * method.  This method's behavior on this PetalNode object depends on
     * its state.
     * @param p The point at which the event occurred that triggered this
     * method call. Usually a point within a box node.
     * @param mode The mode or state of the the call. Values are specified
     * in the PetalModel interface that this class implements.
     * @param evt The mouse event relative to the petal_editor component.
     */
    public boolean handleArcEndPoint(PetalNode pn,Point p,
                                     int mode,MouseEvent evt)
    {

        if (edit && mode == ARC_MODE_START && pn != null) {
	    deselectAll();
            if(pn instanceof BoxNode) {
                BoxNode node = (BoxNode)pn;
                GenericBoxNodePort port = node.getPortOver(p);

                if (port != null && port instanceof BoxNodeOutputPort) {
                    String label = node.getLabel();
                    Utils.addMessagesText("Starting an arc at " + label + "\n");

                    start_node = (GenericNode)pn;

                    start_port = port.getPort();
                    start_portNode = (BoxNodeOutputPort)port;
                }
            }   else if (pn instanceof InputPortNode) {
                Utils.addMessagesText("Starting an arc at input port\n");
                start_node = (InputPortNode)pn;
                start_portNode = ((InputPortNode)pn).getOutputPortNode();
                start_port = start_portNode.getPort();

                InputPortNode inputNode = (InputPortNode) pn;
                inputNode.setOutputPortNode(start_portNode);
            }   else if (pn instanceof OutputPortNode) {
                JOptionPane.showConfirmDialog(null,
                                              "Cannot start an arc from an output port node.",
                                              "Error!",
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
        }
        if (start_node != null && pn != null && edit && mode == ARC_MODE_END) {
            if(pn instanceof BoxNode) {
                BoxNode node = (BoxNode)pn;
                GenericBoxNodePort port = node.getPortOver(p);

                if(port != null && port instanceof BoxNodeInputPort) {
		    if (port.getArcs().size() != 0 && !(node instanceof MultipleInputsBoxInterface)) {
			JOptionPane.showConfirmDialog(null,
						      "Cannot end an arc on an input port that already has an arc.",
						      "Error!",
						      JOptionPane.DEFAULT_OPTION,
						      JOptionPane.ERROR_MESSAGE);
		    }
		    else {
			String label = ((BoxNode) pn).getLabel();
			Utils.addMessagesText("Ending an arc at " + label + "\n");
			PetalArc arc = workspace.addArc(start_node, start_portNode,
							node, (BoxNodeInputPort)port);
		    }
                }
            } else if (pn instanceof InputPortNode) {
                JOptionPane.showConfirmDialog(null,
                                              "Cannot end an arc at an input port node.",
                                              "Error!",
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            } else if (pn instanceof OutputPortNode) {
                Utils.addMessagesText("Ending an arc at output port\n");

                if(start_node instanceof InputPortNode) {
                    JOptionPane.showConfirmDialog(null,
                                                  "Cannot create an arc from an inputportnode to an outputportnode",
                                                  "Error!",
                                                  JOptionPane.DEFAULT_OPTION,
                                                  JOptionPane.ERROR_MESSAGE);
                    return edit;
                }

                OutputPortNode outputNode = (OutputPortNode) pn;

		if(outputNode.getInputPortNode().getArcs().size() > 0) {
			JOptionPane.showConfirmDialog(null,
						      "Cannot end an arc on an output port node that already has an arc.",
						      "Error!",
						      JOptionPane.DEFAULT_OPTION,
						      JOptionPane.ERROR_MESSAGE);
			return edit;
		}

		outputNode.setColor();
                BoxNodeInputPort end_portNode = ((OutputPortNode) pn).getInputPortNode();
                outputNode.setInputPortNode(end_portNode);

                PetalArc arc = workspace.addArc(start_node, start_portNode,
                                                outputNode, end_portNode);
            }
            start_node = null;
            start_port = null;
            start_portNode = null;
        }

        return edit;
    }

    protected GenericBoxNodePort portOver = null;
    protected GenericPortNode nodeOver = null;

    /**
     * Event handler for a mouse over a node or arc.  Not implemented.
     *
     *
     */
    public void handleMouseOver(PetalNode pn,PetalArc pa,Point p)
    {
        if(pn != null) {
            if(pn instanceof BoxNode) {
                BoxNode boxnode = (BoxNode) pn;
                GenericBoxNodePort gbnp = boxnode.getPortOver(p);
                if(gbnp != null) {
                    setEdit(true);
                    gbnp.setBackground(Constants.PORTNODE_COLOR_OVER);
                    portOver = gbnp;
                } else if (portOver != null) {
                    setEdit(false);
                    portOver.setBackground(Constants.PORTNODE_COLOR);
                    portOver = null;
                }
            }
            if(pn instanceof GenericPortNode) {
                nodeOver = (GenericPortNode) pn;
                nodeOver.setBackground(Constants.PORTNODE_COLOR_OVER);
                setEdit(true);
            }
        } else if (portOver != null) {
            setEdit(false);
            portOver.setBackground(Constants.PORTNODE_COLOR);
            portOver = null;
        } else if (nodeOver != null) {
            setEdit(false);
	    if(nodeOver instanceof GenericPortNode) {
		((GenericPortNode)nodeOver).setColor();
	    } else {
		nodeOver.setBackground(Constants.PORTNODE_COLOR);
	    }
            nodeOver = null;
        }
    }

    /**
     * Handles a pop up menu request.
     *
     * @param pn The petal node to which this popup was requested.
     * @param pa The petal arc to which this popup was requested.
     * @param evt The event that triggered the popup menu request.  The mouse
     * event relative to the petal_editor component
     */
    public boolean handlePopupRequest(PetalNode pn,
                                      PetalArc pa,
                                      MouseEvent evt)
    {

        if (pn instanceof GenericNode) {
            GenericNode node = (GenericNode)pn;
            node.showMenu(evt);
            return false;
        }
        return true;
    }

    /**
     * Sets the workspace for this model.
     *
     * @param ws the workspace to set for this model.
     */
    public void setWorkspace(Workspace ws)
    {
        workspace = ws;
    }

    /**
     * Gets the workspace from this model.
     *
     * @return The workspace stored in this model.
     */
    public Workspace getWorkspace()
    {
        return workspace;
    }

    /**
     * Sets the PetalEditor for this model.
     *
     * @param ed The PetalEditor to set for this model.
     */
    public void setEditor(PetalEditor ed)
    {
        the_editor = ed;
    }

    /**
     * Retrieves the PetalEditor contained by this model.
     *
     * @return The PetalEditor contained by this model.
     */
    public PetalEditor getEditor()
    {
        return the_editor;
    }

    /**
     * Sets the aurora gui editor frame for this model.
     *
     * @param editor the editor to set.
     */
    public void setAuroraEditor(Editor editor)
    {

        this.editor = editor;
    }


    /**
     * Gets the aurora gui editor frame for this model.
     *
     * @return The Editor frame.
     */
    public Editor getAuroraEditor()
    {
        return editor;
    }

    /**
     * Gets this superbox from the model.
     *
     * @return A superboxnode associated with this model.
     */
    public SuperBoxNode getSuperBox()
    {
        return superBox;
    }

    /**
     * Gets the boxId of the superBox that this model contains.
     *
     * @return the id of the superbox that this model contains.
     */
    public int getSuperBoxId()
    {
        return superBox.getBoxId();
    }

    /**
     * Sets this model to a superbox.
     *
     * @param superBox The superbox to set this model to.
     */
    public void setSuperBox(SuperBoxNode superBox)
    {
        this.superBox = superBox;
    }

    /**
     * Sets the edit mode of this Model.  If set to false, then arcs cannot be added.
     * Else, arcs may be added to this model.
     *
     * @param edit The edit mode to set for this model.
     */
    public void setEdit(boolean edit)
    {
        this.edit = edit;
    }

    // TODO: JH - This isn't getting called!!!!!  What havoc does that wreak?  - Matt
    /**
     * Sets the port connection points of the boxes.  Used in conjunction with loading
     * catalogs.
     */
    public void adjust()
    {
        // handling contained boxes
        for (int i = 0; i < node_list.size(); i++) {
            Object node = node_list.elementAt(i);
            if (node instanceof BoxNode) {
                BoxNode box = (BoxNode)node;
                BoxComponent c = (BoxComponent)(box.getComponent());
                RenamableLabel label = c.getLabel();
                label.finishRename();

                Vector ports = c.getInputPorts();
                for (int k = 0; k < ports.size(); k++) {
                    BoxNodeInputPort port = (BoxNodeInputPort)ports.elementAt(k);
                    port.setPort(new BoxPortConnectionPoint(PetalPort.NORTH_WEST, port));
                }

                ports = c.getOutputPorts();
                for (int k = 0; k < ports.size(); k++) {
                    BoxNodeOutputPort port = (BoxNodeOutputPort)ports.elementAt(k);
                    port.setPort(new BoxPortConnectionPoint(PetalPort.NORTH_WEST, port));
                }
            }
        }

        for(int i=0; i<inputport_list.size(); i++) {
            InputPortNode portnode = (InputPortNode)inputport_list.elementAt(i);
            BoxNodeOutputPort boxport = portnode.getOutputPortNode();
            boxport.setPort(new BoxPortConnectionPoint(PetalPort.EAST, boxport));
        }
        for(int i=0; i<outputport_list.size(); i++) {
            OutputPortNode portnode = (OutputPortNode)outputport_list.elementAt(i);
            BoxNodeInputPort boxport = portnode.getInputPortNode();
            boxport.setPort(new BoxPortConnectionPoint(PetalPort.WEST, boxport));
        }

	// Since this method isn't getting called, I put the following 
	// functionality into CatalogManager.loadModel(...). -cjc
        for (int i = 0; i < arc_list.size(); i++) {
            GenericArc arc = (GenericArc)(arc_list.elementAt(i));
            if (GenericArc.getCurrentIdCounter() <= arc.getArcId())
		{
		    GenericArc.setCurrentIdCounter(arc.getArcId()+1);
		}
        }
    }

    /**
     * Returns a boxnode contained in this model.
     *
     * @param id the boxId of the boxnode to return.
     * @return the Boxnode with BoxId id, null if this model doesn't contain any
     * boxnode with that id.
     */
    public BoxNode getBoxNode(int id)
    {
        for (int i = 0; i < node_list.size(); i++) {
            BoxNode node = (BoxNode)(node_list.elementAt(i));

            if (node.getBoxId() == id)
                return node;
        }
        return null;
    }

    /**
     * Returns an inputportnode given an id.
     *
     * @param The id of the inputportnode to return.
     * @return an inputportnode with the id, if no such inputportnode exists in
     * this model, returns null.
     */
    public InputPortNode getInputPortNode(int id)
    {
        for (int i = 0; i < inputport_list.size(); i++) {
            InputPortNode node = (InputPortNode)(inputport_list.elementAt(i));
            if (node.getBoxId() == id)
                return node;
        }
        return null;
    }

    /**
     * Returns a vector of all inputportnodes
     * @return all input ports
     */
    public Vector getAllInputPortNodes()
    {
        //copy so that people don't mess with it
        //
        //this just makes the workload generator much
        //easier to code
        return (Vector)(inputport_list.clone());
    }

    /**
     * Returns an outputportnode given an id
     *
     * @param The id of the outputportnode to return.
     * @return an outputportnode with the id, if no such outputportnode exists
     * in this model, returns null.
     */
    public OutputPortNode getOutputPortNode(int id)
    {
        for (int i = 0; i < outputport_list.size(); i++) {
            OutputPortNode node = (OutputPortNode)(outputport_list.elementAt(i));
            if (node.getBoxId() == id)
                return node;
        }
        return null;
    }


    /*******************************************************************
     * Private methods                                                 *
     *******************************************************************/

    private synchronized void clearNodes()
    {
        node_list.removeAllElements();
    }


    private synchronized void clearArcs()
    {
        arc_list.removeAllElements();
    }

    private synchronized void clearPorts()
    {
        inputport_list.removeAllElements();
        outputport_list.removeAllElements();
    }


}
