package edu.brown.aurora.gui;

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
 * A class implementing static, miscellaneous utility methods relevant to the gui.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Utils
{
    /**
     * Given a model, obtains the root editor.
     *
     * @param model Any model within the superbox tree.
     * @return The root editor for the gui.
     */
    public static Editor getRootEditor(Model model)
    {
        Model nextModel = model;
        SuperBoxNode superBox = model.getSuperBox();
        while(!superBox.isRootBox()) {
            nextModel = superBox.getParentModel();
            superBox = nextModel.getSuperBox();
        }
        return nextModel.getAuroraEditor();
    }

    /**
     * Updates the hierarchy panel.
     *
     * 
     */

    public static void updateHierarchy(Model model)
    {
        Editor editor = getRootEditor(model);
        HierarchyPanel hierarchyPanel = editor.getToolboxspace().getHierarchyPanel();
        hierarchyPanel.updateTree();
    }

    /**
     * Appends a string text to the messages text area.
     *
     * @param text The text to be appended to the messages text area.
     */
    public static void addMessagesText(String text) {
        if(Messagespace.s_messagesTextArea != null) {
            Messagespace.s_messagesTextArea.append(text);
	    scrollMessageToBottom(Messagespace.s_messagesTextArea);
        }
    }


    /**
     * scrolls the parameter text area to the bottom.
     */
    private static void scrollMessageToBottom(JTextArea text)
    {
	try {
	    text.scrollRectToVisible(text.modelToView(text.getDocument().getLength()));
	} catch (Exception e) {}

    }

    /**
     * Appends a string text to the statistics text area.
     *
     * @param text The text to be appended to the statistics text area.
     */
    public static void addStatisticsText(String text) {
        if(Messagespace.s_statisticsTextArea != null) {
            Messagespace.s_statisticsTextArea.append(text);
	    scrollMessageToBottom(Messagespace.s_statisticsTextArea);
        }
    }

    /**
     * Appends a string text to the errors text area.
     *
     * @param text The text to be appended to the errors text area.
     */
    public static void addErrorsText(String text) {
        if(Messagespace.s_errorsTextArea != null) {
            Messagespace.s_errorsTextArea.append(text);
	    scrollMessageToBottom(Messagespace.s_statisticsTextArea);
        }
    }

    /**
     * A generic helper method for adding components in a GridBagLayout layout.
     * gridx, gridy, gridwidth, gridheight, weightx, and weighty are GridBagConstraints
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
     */
    public static void addComponent(GridBagLayout bags,
				    JComponent parent, JComponent child,
				    int gridx, int gridy,
				    int gridwidth, int gridheight,
				    double weightx, double weighty)
    {
        GridBagConstraints c = new GridBagConstraints(gridx, gridy,
                                                      gridwidth, gridheight,
                                                      weightx, weighty,
                                                      GridBagConstraints.CENTER,
                                                      GridBagConstraints.BOTH,
                                                      new Insets(0,0,0,0),
                                                      0,0);
        bags.setConstraints(child, c);
        parent.add(child);
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
    public static void addComponent(GridBagLayout bags,
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


    /**
     * Returns all of the nodes that are connected by an arc to the output port of the
     * given box node.
     *
     * @param boxNode the box node which to get the next box nodes for.
     */
    public static Vector getNextNodes(BoxNode boxNode)
    {
        Vector nextBoxes = new Vector();

        BoxComponent c = (BoxComponent) boxNode.getComponent();
        Vector outputPorts = c.getOutputPorts();

        for(int i=0; i<outputPorts.size(); i++) {
            BoxNodeOutputPort outputPort = (BoxNodeOutputPort) outputPorts.elementAt(i);
            Vector arcs = outputPort.getArcs();
            
            for(int k=0; k<arcs.size(); k++) {
                GenericArc arc = (GenericArc) arcs.elementAt(k);
                if(arc.getTarget() instanceof BoxNode) {
                    BoxNode node = (BoxNode) arc.getTarget();
                    nextBoxes.addElement(node);
                }
            }
        }
        return nextBoxes;
    }

    /**
     * Returns a vector of all the nodes between two selected nodes.  If there are there are
     * no nodes located between the two box nodes, returns an empty vector.
     *
     * @param first The box node from which to start.
     * @param second The box node from which to end up at.
     *
     * @return A vector of box nodes located between the first and second box nodes, exclusively.
     */
    public static Vector getInnerNodes(BoxNode first, BoxNode second)
    {
        Vector stack = new Vector();
        Vector neighbors = getNextNodes(first);
        walkTree(second, stack, neighbors);
        return stack;
    }
    
    protected static boolean walkTree(BoxNode target, Vector stack, Vector neighbors)
    {
        while(neighbors.size() > 0) {
            BoxNode neighbor = (BoxNode) neighbors.elementAt(0);
            neighbors.removeElementAt(0);

            if(neighbor.getBoxId() == target.getBoxId()) {
                return true;
            }

            stack.addElement(neighbor);

            boolean foundNode = walkTree(target, stack, getNextNodes(neighbor));
            if(!foundNode) {
                stack.removeElement(neighbor);
            } else {
                return true;
            }
        }
        return false;
    }

}
