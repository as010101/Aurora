package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * BoxComponent is the visual Java Swing object that represents a box in the PetalEditor.
 * It is not meant to store any semantic information for boxes, it is only meant to 
 * draw and store the visual components for the GUI.  It is capable of displaying the
 * components for any type of box, whether it is a unary box, binary box, or a super box.
 * Besides containing the label that  identifies the nodes, it also contains and draws 
 * multiple input and output ports for the box. 
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class BoxComponent extends JPanel 
{
    /**
     * The label that is displayed for the box.
     */
    protected RenamableLabel label;

    /**
     * The left-hand-side panel that contains all of the input ports for this component.
     */
    protected JPanel westPanel;

    /**
     * The right-hand-side panel that contains all of the output ports for this components.
     */
    protected JPanel eastPanel;

    /**
     * The box node that this box component represents.
     */
    protected GenericNode boxNode;

    /**
     * Multiple input ports for this box component.
     */
    protected Vector inputPorts;

    /**
     * Multiple output ports for this box component.
     */
    protected Vector outputPorts;


    /**
     * Constructs a new boxcomponent object that represents a given BoxNode
     *
     * @param bnode The boxnode which this visual box component object represents
     */
    public BoxComponent(GenericNode bnode) 
    {
        super();
        
        label = new RenamableLabel("");
        label.setColor(Color.lightGray);
        
        westPanel = new JPanel();
        eastPanel = new JPanel();
        
        boxNode = bnode;
        
        this.setLayout(new BorderLayout());
        add(this.label, BorderLayout.CENTER);
        setSize(100,25);

        westPanel.setLayout(new GridLayout(0,1));
        eastPanel.setLayout(new GridLayout(0,1));
        add(westPanel, BorderLayout.WEST);
        add(eastPanel, BorderLayout.EAST);    

        inputPorts = new Vector();
        outputPorts = new Vector();
        
        setRequestFocusEnabled(true);
    }

    /**
     * Returns the RenamableLabel object that this BoxComponent contains.
     *
     * @return The RenamableLabel object that this BoxComponent contains.
     */
    public RenamableLabel getLabel(){
        return this.label;
    }

    /**
     * Returns the GenericNode object that this BoxComponent visually represents.
     *
     * @return The GenericNode object that this BoxComponent visuually represents.
     */
    public GenericNode getGenericNode() {
        return boxNode;
    }

    /**
     * Returns the left/west panel that contains all of the input ports for this
     * visual component object.
     *
     * @return The left/west panel that contains all of the input ports for this
     * visual component object.
     */
    public JPanel getWest() {
        return westPanel;
    }

    /**
     * Returns the right/east panel that contains all of the output ports for this
     * visual component object.
     *
     * @return The right/east panel that contains all of the output ports for this
     * visual component object.
     */
    public JPanel getEast() {
        return eastPanel;
    }

    /**
     * Returns the Vector of input ports that this object contains.
     *
     * @return The Vector of input ports that this object contains.
     */
    public Vector getInputPorts()
    {
        return inputPorts;
    }

    /**
     * Returns the Vector of output ports that this object contains.
     *
     * @return The Vector of output ports that this object contains.
     */
    public Vector getOutputPorts()
    {
        return outputPorts;
    }

    /**
     * Returns a port that a given point is over.  If the point is not over any
     * port, returns null.
     *
     * @param p The point that may or may not be over a particular port.
     * @return The port that the input port is over.
     */
    public GenericBoxNodePort getPortOver(Point p)
    {
        Point outputPoint = new Point((int)(p.getX() - westPanel.getLocation().getX()),
                                      (int)(p.getY() - westPanel.getLocation().getY()));

        for(int i=0; i<inputPorts.size(); i++) {
            GenericBoxNodePort port = (GenericBoxNodePort)inputPorts.elementAt(i);
            if(port.contains(p)) {
                return port;
            }
        }

        outputPoint = new Point((int)(p.getX() - eastPanel.getLocation().getX()),
                                (int)(p.getY() - eastPanel.getLocation().getY()));

        for(int i=0; i<outputPorts.size(); i++) {
            GenericBoxNodePort port = (GenericBoxNodePort)outputPorts.elementAt(i);
            
            if(port.contains(outputPoint)) {
                return port;
            }
        }
        return null;
    }

    /**
     * Returns the InputPortNode given an index.  If no InputPortNode is specified
     * by the index, then returns null.
     *
     * @param idx The index of the inputportnode to return.
     * @return The InputPortNode specified by idx.  If idx is not within the range
     * of the vector, returns null.
     */
    public BoxNodeInputPort getInputPort(int idx) 
    {
        if(inputPorts == null) {
            return null;
        }
        if(idx < 0 || idx >= inputPorts.size()) {
            return null;
        }

        return (BoxNodeInputPort)inputPorts.elementAt(idx);
    }

    /**
     * Returns the OutputPortNode given an index.  If no OutputPortNode is specified
     * by the index, then returns null.
     *
     * @param idx The index of the outputportnode to return.
     * @return The OutputPortNode specified by idx.  If idx is not within the range
     * of the vector, returns null.
     */
    public BoxNodeOutputPort getOutputPort(int idx)
    {
        if(outputPorts == null) {
            return null;
        }
        if(idx < 0 || idx >= outputPorts.size()) {
            return null;
        }

        return (BoxNodeOutputPort)outputPorts.elementAt(idx);
    }    

    /**
     * Returns the index of the InputPort Vector given an InputPortNode.
     *
     * @param input The InputPortNode that the returned index specifies.
     * @return The integer index into the InputPort Vector that specifies
     * the parameter InputPortNode.
     */
    public int getInputPortIndex(BoxNodeInputPort input) 
    {
        return (inputPorts.indexOf(input));
    }

    /**
     * Returns the index of the OutputPort Vector given an OutputPortNode.
     *
     * @param output The OutputPortNode that the returned index specifies.
     * @return The integer index into the OutputPort Vector that specifies
     * the parameter OutputPortNode.
     */
    public int getOutputPortIndex(BoxNodeOutputPort output)
    {
        return (outputPorts.indexOf(output));
    }    

    /**
     * Returns the number of inputPorts of the box
     *
     * @return The number of inputports of the box.
     */
    public int getNumInputs() 
    {
        return inputPorts.size();
    }

    /**
     * Returns the number of outputPorts of the box.
     *
     * @return the number of output ports of the box.
     */
    public int getNumOutputs()
    {
        return outputPorts.size();
    }


    public BoxNodeOutputPort removeOutputPort(int index) {
        BoxNodeOutputPort outputPort = (BoxNodeOutputPort)outputPorts.elementAt(index);

        if(outputPorts.size() == 1) {
            eastPanel.removeAll();
        } else {
            eastPanel.remove((index*2)+1); // remove the port
            eastPanel.remove((index*2)+1); // remove the space after the port
        }
        outputPorts.removeElementAt(index);

        setVisible(false);
        setVisible(true);

        return outputPort;
    }


    public BoxNodeInputPort removeInputPort(int index) {
        BoxNodeInputPort inputPort = (BoxNodeInputPort)inputPorts.elementAt(index);

        if(inputPorts.size() == 1) {
            westPanel.removeAll();
        } else {
            westPanel.remove((index*2)+1); // remove the port
            westPanel.remove((index*2)+1); // remove the space after the port
        }
        inputPorts.removeElementAt(index);

        setVisible(false);
        setVisible(true);

        return inputPort;
    }


    /**
     * Adds an output port to this boxcomponent.
     *
     * @return The BoxNodeOutputPort created by this method.
     */
    public BoxNodeOutputPort addOutputPort() {
        if(outputPorts.size() == 0) {
            eastPanel.add(new JPanel());
        }
        BoxNodeOutputPort outputPort = new BoxNodeOutputPort(PetalPort.NORTH_WEST, 
                                                             eastPanel, 
                                                             outputPorts.size(),
                                                             null);
        outputPort.setGenericNode(boxNode);
        outputPort.setBackground(Constants.PORTNODE_COLOR);
        outputPorts.addElement(outputPort);

        eastPanel.add(outputPort);
        eastPanel.add (new JPanel());   

        setVisible(false);
        setVisible(true);

        return outputPort;
    }

    /**
     * Adds an input port to this boxcomponent.
     *
     * @return The BoxNodeInputPort created by this method.
     */
    public BoxNodeInputPort addInputPort() {
        if(inputPorts.size() == 0) {
            westPanel.add(new JPanel());
        }

        BoxNodeInputPort inputPort = new BoxNodeInputPort(PetalPort.NORTH_WEST,
                                                          inputPorts.size(),
                                                          null);

        inputPort.setGenericNode(boxNode);
        inputPort.setBackground(Constants.PORTNODE_COLOR);
        inputPorts.addElement(inputPort);
        
        westPanel.add (inputPort);
        westPanel.add (new JPanel());          
        setVisible(false);
        setVisible(true);
        return inputPort;
    }
    
} 
