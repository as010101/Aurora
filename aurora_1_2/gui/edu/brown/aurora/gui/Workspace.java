package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;

import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.dnd.*;

import java.util.*;

import edu.brown.aurora.gui.types.*;


/**
 * The workspace of the editor gui.  This component primarily contains the
 * PetalEditor object in which all of the diagram editing (nodes/arcs) are
 * performed.  It has the capability of adding/removing nodes and arcs.
 * 
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Workspace extends JPanel
{
    private Model petal_model;
    transient private PetalEditor petal_editor;
    private Editor editor;


    /**
     * Constructs a new Workspace object.  
     *
     * @param petal_editor The petal editor that this workspace contains.
     * @param petal_model The model
     * @param Editor The parent frame that contains this workspace object.
     */
    public Workspace(PetalEditor petal_editor, Model petal_model) {
        super();
        
        this.petal_model = petal_model;
        this.petal_editor = petal_editor;
        this.editor = editor;

        setDoubleBuffered(true);
        
        GridLayout layout = new GridLayout(1,1);

        setLayout(layout);

        add(petal_editor);

        ComponentAdapter c = new ComponentAdapter () 
            {
                public void componentResized(ComponentEvent e) 
                {
                    resetPortNodePositions();
                }
            };
        
        addComponentListener(c);
    }

    /**
     * Adds a node to the model.
     *
     * @param node The node to be added.
     * @param p The location point where the node should be added.
     */
    public PetalNode addNode(PetalNode node, Point p) {
        petal_model.addNode(node, p);
        node.getComponent().setLocation(p);
        
        petal_editor.update();
        return node;
    }

    /**
     * Resets the port node positions.
     */
    public void resetPortNodePositions()
    {
        int editorHeight = petal_editor.getHeight();        
        int editorWidth = petal_editor.getWidth();

        Vector inputPortNodes = petal_model.getInputPorts();
        Vector outputPortNodes = petal_model.getOutputPorts();

        for(int i=0; i<inputPortNodes.size(); i++) {
            InputPortNode port = (InputPortNode)inputPortNodes.elementAt(i);
            Point p = new Point(0, (i+1)*editorHeight/(inputPortNodes.size()+1));
            port.getComponent().setLocation(p);
        }
        for(int i=0; i<outputPortNodes.size(); i++) {
            OutputPortNode port = (OutputPortNode)outputPortNodes.elementAt(i);
            Point p = new Point(editorWidth - port.getWidth()-5, 
                                (i+1)*editorHeight/(outputPortNodes.size()+1));
            port.getComponent().setLocation(p);
        }

        update();
    }

    /**
     * Adds an arc to the workspace.
     *
     * @param n1 The source node for the arc.
     * @param opt The output port that the arc should be connected to of
     * of the source node.
     * @param n2 The target node for the arc.
     * @param ipt The input port that the the arc should be connected to
     * of the target node.
     */
    public GenericArc addArc(GenericNode n1, BoxNodeOutputPort opt, 
                             GenericNode n2, BoxNodeInputPort ipt)
    {
        GenericArc pa = new GenericArc(n1, opt, n2, ipt, petal_model.getSuperBox().getBoxId());

        pa.setTargetEnd(new ArcEnd(PetalConstants.PETAL_ARC_END_ARROW));
        petal_model.addArc(pa);

        return pa;
    }

    /**
     * Redraws all the objects on the workspace.
     *
     */
    public void update() {
        petal_editor.update();
    }

    /**
     * Sets the parent editor of this object.
     *
     * @param editor The Editor object to set.
     */
    public void setEditor(Editor editor)
    {
        this.editor = editor;
    }


}
