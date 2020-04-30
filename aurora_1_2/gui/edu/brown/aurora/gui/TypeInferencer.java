// Copyright (C) 2002 Brown University  All Rights Reserved
package edu.brown.aurora.gui;

import edu.brown.aurora.gui.types.*;
import java.util.Vector;


/************************************************************************************
 *
 * A class containing static methods that walks through the tree of nodes and
 * checks the types if they exist, infer them if they don't.
 *
 * @author  Robin Yan (rly@cs.brown.edu)
 *
 *
 ************************************************************************************/

public class TypeInferencer
{
    protected static boolean allTypesValid = false;
    protected static boolean enabled = true;

    /**
     * Does a global type check.  Entry point into the type checker and inferencer.
     *
     */
    public static void typeCheck()
    {
        Editor rootEditor = RootFrame.editor;
        Model rootModel = rootEditor.getModel();

        boolean allTypesValid = TypeInferencer.walkTrees(rootModel);
        if(!allTypesValid) {
            TypeFailedBoxesWindow.showWindow(rootModel);
        } else {
            Utils.addMessagesText("Successful type check.\n");
        }
    }

    /**
     * Enables or disabled the type inferencing check system.
     */
    public static void setEnabled(boolean setEnabled)
    {
	enabled = setEnabled;
    }

    /**
     * Returns a vector representing all of the local tree roots in the whole
     * diagram of a model.
     */
    public static Vector getLocalTreeRoots(Model model)
    {
	Vector treeRoots = new Vector();
	getLocalTreeRoots(model, treeRoots);
	return treeRoots;
    }

    /**
     * Recursive helper method that returns the local tree roots as a vector.
     */
    private static void getLocalTreeRoots(Model model, Vector treeRoots)
    {
	Vector boxNodes = model.getBoxNodes();

	for(int i=0; i<boxNodes.size(); i++) {
	    if(boxNodes.elementAt(i) instanceof BoxNode) {
		BoxNode boxNode = (BoxNode)boxNodes.elementAt(i);
		BoxComponent c = (BoxComponent) boxNode.getComponent();
		Vector inputPorts = c.getInputPorts();
		boolean isRoot = true;
		for(int j=0; j<inputPorts.size(); j++) {
		    BoxNodeInputPort inputPort = (BoxNodeInputPort) inputPorts.elementAt(j);
		    Vector arcs = inputPort.getArcs();
		    if(arcs.size() != 0) {
			isRoot = false;
		    }
		}
		if(isRoot) {
		    // found a local tree root box node.
		    treeRoots.addElement(boxNode);
		}
	    }
	    if(boxNodes.elementAt(i) instanceof SuperBoxNode) {
		SuperBoxNode superBoxNode = (SuperBoxNode)boxNodes.elementAt(i);
		Model superBoxModel = superBoxNode.getSuperBoxModel();
		getLocalTreeRoots(superBoxModel, treeRoots);
	    }
	}

	Vector inputPortNodes = model.getInputPorts();

	for(int i=0; i<inputPortNodes.size(); i++) {
	    if(inputPortNodes.elementAt(i) instanceof InputPortNode) {
		InputPortNode inputPortNode = (InputPortNode)inputPortNodes.elementAt(i);

		SuperBoxNode superBox = model.getSuperBox();
		if(superBox.isRootBox()) {
		    treeRoots.addElement(inputPortNode);
		} else {
		    int index = inputPortNode.getIndex();
		    BoxNodeInputPort boxNodeInputPort = superBox.getInputPort(index);

		    Vector arcs = boxNodeInputPort.getArcs();
		    if(arcs.size() == 0) {
			treeRoots.addElement(inputPortNode);
		    }
		}
	    }
	}

	Vector outputPortNodes = model.getOutputPorts();
	for(int i=0; i<outputPortNodes.size(); i++) {
	    if(outputPortNodes.elementAt(i) instanceof OutputPortNode) {
		OutputPortNode outputPortNode = (OutputPortNode)outputPortNodes.elementAt(i);
		outputPortNode.setColor();
	    }
	}
    }

    /**
     * Entry point into the tree walking algorithm.
     *
     * @param model The model containing the network to walk through.
     */
    public static boolean walkTrees(Model model)
    {
	if(enabled) {
	    allTypesValid = true;

	    Vector treeRoots = getLocalTreeRoots(model);

	    for(int i=0; i<treeRoots.size(); i++) {
		if(treeRoots.elementAt(i) instanceof PrimitiveBoxNode) {
		    PrimitiveBoxNode boxNode = (PrimitiveBoxNode)treeRoots.elementAt(i);
		    boxNode.setTypeValid(true);

		    Type inferredType = boxNode.inferOutputType();

		    //System.out.println("\n\nAt root : " + boxNode.getLabel());

		    walkTrees(boxNode, inferredType);
		}
		if(treeRoots.elementAt(i) instanceof InputPortNode) {
		    InputPortNode inputPortNode = (InputPortNode)treeRoots.elementAt(i);
		    Type type = inputPortNode.getType();

		    //System.out.println("\n\nAt root : " + inputPortNode.toString());

		    walkTrees(inputPortNode, type);
		}
	    }

	    return allTypesValid;
	} else {
	    return true;
	}
    }

    /**
     * @param node The node to start from in this recursive helper method
     * @param inferredType The type that is output from this node.
     */
    private static void walkTrees(Object node, Type inferredType)
    {
	if(node instanceof PrimitiveBoxNode) {

	    PrimitiveBoxNode boxNode = (PrimitiveBoxNode)node;
	    // System.out.print("Walking the tree, at " + boxNode.getLabel());

            boxNode.inferOutputType();

	    BoxComponent c = (BoxComponent) boxNode.getComponent();
	    Vector outputPorts = c.getOutputPorts();


	    for(int i=0; i<outputPorts.size(); i++) {
		BoxNodeOutputPort outputPort = (BoxNodeOutputPort) outputPorts.elementAt(i);
		Vector arcs = outputPort.getArcs();

		Vector nextBoxes = new Vector();
		walkTreesGetNextBoxes(arcs, nextBoxes, inferredType);

		for(int j=0; j<nextBoxes.size(); j++) {
		    Object nextBox = nextBoxes.elementAt(j);
                    walkNextBox(nextBox, inferredType, j);
		}
	    }
	}
	if(node instanceof InputPortNode) {
	    InputPortNode inputPortNode = (InputPortNode) node;
	    BoxNodeOutputPort outputPort = inputPortNode.getOutputPortNode();
	    // System.out.print("Walking the tree, at " + inputPortNode.getLabel());

	    Vector arcs = outputPort.getArcs();

            Type type = inputPortNode.getType();

	    Vector nextBoxes = new Vector();
	    walkTreesGetNextBoxes(arcs, nextBoxes, type);

	    for(int j=0; j<nextBoxes.size(); j++) {
		Object nextBox = nextBoxes.elementAt(j);
		walkNextBox(nextBox, type, j);
	    }

	}
    }

    /**
     * @param nextBox The box to start from in this recursive helper method
     * @param inferredType The inferred type from the box previous to nextBox
     * @param j The jth input port of nextBox
     */
    private static void walkNextBox(Object nextBox, Type inferredType, int j)
    {
        if(nextBox instanceof BoxNodeInputPort) {
            BoxNodeInputPort port = (BoxNodeInputPort) nextBox;

	    PrimitiveBoxNode primBox = (PrimitiveBoxNode) port.getGenericNode();
            Type nextInferredType = primBox.getOutputType();


	    // System.out.println(" -- nextInferred type is : " + nextInferredType);


            if(inferredType == null) {
                // primBox.setInputType(null, j);
                port.setType(null);
		primBox.setTypeValid(true);
            } else {
                if(!primBox.isUseDefinedTypes()) {
                    //primBox.setInputType(inferredType, j);
                    port.setType((CompositeType)inferredType);
		    primBox.setTypeValid(true);
                } else {
                    Type compareType = primBox.getInputType(j);
                    if(compareType == null || !compareType.isCompatible(inferredType)) {
                        allTypesValid = false;
                        /*
                          System.out.println("Type checking error for box " +
                          primBox.getLabel() + ".");
			*/
                        primBox.setTypeValid(false);
                    } else {
                        primBox.setTypeValid(true);
                    }
                }
            }
            walkTrees(primBox, nextInferredType);
        }
	if(nextBox instanceof GenericPortNode) {
	    GenericPortNode portNode = (GenericPortNode) nextBox;
	    if(!portNode.isUseDefinedTypes() && inferredType instanceof CompositeType) {

		portNode.setType((CompositeType)inferredType);
	    }
	    Type nextInferredType = portNode.getType();
	    walkTrees(nextBox, nextInferredType);
	}

    }

    /**
     * Obtains a list of next boxes from the list of arcs.
     *
     * @param arcs A list of arcs outgoing from a particular output port.
     * @param nextBoxes A vector of next boxes to fill up
     * @param inferredType The type that was inferred from the box containing the
     * outputports for the outgoing arcs.
     */
    private static void walkTreesGetNextBoxes(Vector arcs, Vector nextBoxes, Type inferredType)
    {
	for(int i=0; i<arcs.size(); i++) {
	    GenericArc arc = (GenericArc) arcs.elementAt(i);

	    GenericNode node = (GenericNode) arc.getTarget();

	    if(node instanceof PrimitiveBoxNode) {
		PrimitiveBoxNode nextBoxNode = (PrimitiveBoxNode) node;
		// nextBoxes.addElement(nextBoxNode);
		// System.out.println("adding input port of : " + arc.toString());
		nextBoxes.addElement(arc.getInputPort());

                // Inferencing code.  resets types if the box is using inferred types.
//                nextBoxNode.clearInferredInputTypes();

	    }
	    if(node instanceof SuperBoxNode) {
		SuperBoxNode nextSuperBoxNode = (SuperBoxNode) node;
		BoxNodeInputPort inputPort = arc.getInputPort();

		int index = inputPort.getIndex();
		Model superBoxModel = nextSuperBoxNode.getSuperBoxModel();

		Vector inputPorts = superBoxModel.getInputPorts();
		InputPortNode inputPortNode = (InputPortNode) inputPorts.elementAt(index);

		if(!inputPortNode.isUseDefinedTypes() && inferredType instanceof CompositeType) {
		    inputPortNode.setType((CompositeType)inferredType);
		}

		nextBoxes.addElement(inputPortNode);

	    }
	    if(node instanceof OutputPortNode) {
		OutputPortNode outputPortNode = (OutputPortNode) node;
		outputPortNode.setColor();
		outputPortNode.setType((CompositeType)inferredType);
		int index = outputPortNode.getIndex();
		Model model = outputPortNode.getModel();
		SuperBoxNode superBoxNode = model.getSuperBox();
		if(!superBoxNode.isRootBox()) {
		    BoxNodeOutputPort boxNodeOutputPort = superBoxNode.getOutputPort(index);
		    Vector nextArcs = boxNodeOutputPort.getArcs();
		    walkTreesGetNextBoxes(nextArcs, nextBoxes, inferredType);
		}
	    }
	}
    }
}
