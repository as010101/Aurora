package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;
import edu.brown.aurora.gui.types.*;

import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;

import java.util.*;

/**
 * Configures and displays a menubar on the frame for the GUI.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
class MenuBar extends SwingMenuBar implements ActionListener {
    transient private PetalEditor petal_editor;

    private Model petal_model;
    private Editor editor;
    private static WorkloadDataPackage workloadCommunicator = null;

    private static final String COMMAND_REMOVE_ALL_TYPES = "Remove All Types";
    private static final String FILE_LOAD = "Load Network";
    private static final String FILE_NEW_WORKSPACE = "Clear Workspace";
    private static final String FILE_SAVE_WORKSPACE = "Save Workspace";
    private static final String FILE_QUIT = "Quit";
    private static final String FILE_CLOSE = "Close Window";
    private static final String EDIT_UNDO = "Undo";
    private static final String EDIT_REDO = "Redo";
    private static final String EDIT_SELECTALL = "Select All";
    private static final String EDIT_CLEARSELECTIONS = "Clear Selections";
    private static final String COMMAND_ADD_TYPE = "Add Type";
    private static final String COMMAND_DELETE_TYPE = "Delete Type";
    private static final String COMMAND_DELETE_PALETTE_ITEM = "Delete Selected Palette Item";
    private static final String COMMAND_ADD_INPUTPORT = "Add Input Port";
    private static final String COMMAND_ADD_OUTPUTPORT = "Add Output Port";
    private static final String COMMAND_CREATE_SUPERBOX = "Create SuperBox";
    private static final String COMMAND_DECOMPOSE_SUPERBOX = "Decompose SuperBox";
    private static final String COMMAND_SET_WORKLOAD = "Set Workload Generator";
    private static final String COMMAND_START_WORKLOAD = "Start Workload Generation";
    private static final String COMMAND_UPDATE_WORKLOAD = "Update Workload Rates";
    private static final String COMMAND_STOP_WORKLOAD = "Stop Workload Generation";
    private static final String VIEW_PRETTIFY = "Prettify";
 
    /**
     * Constructs a new MenuBar object.
     *
     * @param petal_editor The PetalEditor object that this MenuBar object manipulates.
     * @param model The model that encapsulates the node/arc information.
     * @param editor The editor frame that this MenuBar is contained within.
     */
    MenuBar(PetalEditor petal_editor, Model model, Editor editor) {
        this.petal_editor=petal_editor;
        this.petal_model = model;
        this.editor = editor;

        SuperBoxNode superBox = petal_model.getSuperBox();        
        setDoubleBuffered(true);
        JMenuItem btn;

        JMenu filemenu = new JMenu("File");
        btn = addButton(filemenu, FILE_NEW_WORKSPACE, "Remove all the nodes and arcs");
        btn = addButton(filemenu, FILE_SAVE_WORKSPACE, "Save the catalog");
        btn = addButton(filemenu, FILE_LOAD, "Load a saved network.");
        filemenu.addSeparator();

        if(superBox == null || superBox.getParentModel() == null) {
            btn = addButton(filemenu, FILE_QUIT, "Quit the project manager");
        } else {
            btn = addButton(filemenu, FILE_CLOSE, "Close the Superbox Window");
        }
        super.add(filemenu);

        JMenu editmenu = new JMenu("Edit");
        //btn = addButton(editmenu, EDIT_UNDO, "Undo previous command(s)");
        //btn = addButton(editmenu, EDIT_REDO, "Redo undone command(s)");
        btn = addButton(editmenu, EDIT_SELECTALL, "Select all nodes and arcs");
        btn = addButton(editmenu, EDIT_CLEARSELECTIONS, "Clear all selections");
        super.add(editmenu);

        JMenu commandmenu = new JMenu("Command");
        btn = addButton(commandmenu, COMMAND_ADD_TYPE, "Add Type");
        btn = addButton(commandmenu, COMMAND_DELETE_TYPE, "Delete Type");
        btn = addButton(commandmenu, COMMAND_REMOVE_ALL_TYPES, "Remove all composite types");
        commandmenu.addSeparator();
        btn = addButton(commandmenu, COMMAND_DELETE_PALETTE_ITEM, "Delete Selected Palette Item");
        commandmenu.addSeparator();
        btn = addButton(commandmenu, COMMAND_ADD_INPUTPORT, "Add Input Port");
        btn = addButton(commandmenu, COMMAND_ADD_OUTPUTPORT, "Add Output Port");
        commandmenu.addSeparator();
        btn = addButton(commandmenu, COMMAND_CREATE_SUPERBOX, "Create SuperBox");

        if(petal_model.getSuperBox() != null && 
           petal_model.getSuperBox().getParentModel() != null) {
            btn = addButton(commandmenu, COMMAND_DECOMPOSE_SUPERBOX, "Decompose SuperBox");
        }
        commandmenu.addSeparator();
        btn = addButton(commandmenu, COMMAND_SET_WORKLOAD, "Set Workload");
        btn = addButton(commandmenu, COMMAND_START_WORKLOAD, "Start Workload");
        btn = addButton(commandmenu, COMMAND_UPDATE_WORKLOAD, "Update Workload");
        btn = addButton(commandmenu, COMMAND_STOP_WORKLOAD, "Stop Workload");

        super.add(commandmenu);
        
        JMenu viewmenu = new JMenu("View");
        btn = addButton(viewmenu, VIEW_PRETTIFY, "Prettify the graph");
        super.add(viewmenu);                

    }
    
    /**
     * Handles all events on the menubar.
     *
     * @param e The event that triggered this method.
     */
    public void actionPerformed(ActionEvent e) {
        String btn = e.getActionCommand();
        if (btn.equals(COMMAND_REMOVE_ALL_TYPES)) {
            if (JOptionPane.showConfirmDialog(null, 
                                              "Do you really want to remove all of the types?", 
                                              "Question" , 
                                              JOptionPane.YES_NO_OPTION, 
                                              JOptionPane.QUESTION_MESSAGE) == JOptionPane.YES_OPTION) {
                
                RootFrame.typeManager.removeAllCompositeTypes();
            }
        }
        else if (btn.equals(FILE_NEW_WORKSPACE)) {
            if (JOptionPane.showConfirmDialog(null, 
                                              "Do you really want to clear all the nodes and arcs?", 
                                              "Question" , 
                                              JOptionPane.YES_NO_OPTION, 
                                              JOptionPane.QUESTION_MESSAGE) == JOptionPane.YES_OPTION) {
                editor.getModel().clear();
            }

        }
        else if (btn.equals(FILE_SAVE_WORKSPACE)) {
            editor.save(false);
        }
        else if (btn.equals(FILE_LOAD)) {
            RootFrame.load();
        }
        else if (btn.equals(FILE_QUIT) || btn.equals(FILE_CLOSE)) {
            editor.dispose();
        }
        else if (btn.equals(EDIT_UNDO)) {
            petal_editor.commandUndo();
        }
        else if (btn.equals(EDIT_REDO)) {
            petal_editor.commandRedo();
        }
        else if (btn.equals(EDIT_SELECTALL)) {
            petal_editor.commandSelectAll();
        }
        else if (btn.equals(EDIT_CLEARSELECTIONS)) {
            petal_editor.commandDeselectAll();
        }
        else if (btn.equals(COMMAND_ADD_TYPE)) {
            // editor.getToolboxspace().getTypePanel().getTypeTree().addType();
	    editor.getToolboxspace().getTypePanel().getTypeList().addType();
            editor.getToolboxspace().setCurrentTab(Toolboxspace.TYPES_ID);
        }
        else if (btn.equals(COMMAND_DELETE_TYPE)) {
            if (JOptionPane.showConfirmDialog(null, 
                                              "Do you really want to remove the selected type?", 
                                              "Question" , 
                                              JOptionPane.YES_NO_OPTION, 
                                              JOptionPane.QUESTION_MESSAGE) == JOptionPane.YES_OPTION) {
                
                // editor.getToolboxspace().getTypePanel().getTypeTree().removeType();
		editor.getToolboxspace().getTypePanel().getTypeList().removeType();
            }
        }
        else if (btn.equals(COMMAND_DELETE_PALETTE_ITEM)) {
            JList list = editor.getToolboxspace().getPalettePanel().getPaletteList();
            int selectedItem = list.getSelectedIndex();
            if(selectedItem == -1) {
                JOptionPane.showConfirmDialog(null, 
                                              "No palette item selected.",
                                              "Error!", 
                                              JOptionPane.DEFAULT_OPTION, 
                                              JOptionPane.ERROR_MESSAGE);                
            } else {
                DefaultListModel model = (DefaultListModel) list.getModel();
                model.removeElementAt(selectedItem);
            }

        }
        else if (btn.equals(COMMAND_ADD_INPUTPORT)) {
            petal_model.addInputPort();
            SuperBoxNode superbox = petal_model.getSuperBox();
            superbox.synchronizePortsToModel();
            Workspace workspace = editor.getWorkspace();
            workspace.resetPortNodePositions();
        }
        else if (btn.equals(COMMAND_ADD_OUTPUTPORT)) {
            petal_model.addOutputPort();
            SuperBoxNode superbox = petal_model.getSuperBox();
            superbox.synchronizePortsToModel();
            Workspace workspace = editor.getWorkspace();
            workspace.resetPortNodePositions();
        }
        else if (btn.equals(COMMAND_CREATE_SUPERBOX)) {
            commandCreateSuperBox();           
            petal_editor.commandDeselectAll();
        }        
        else if (btn.equals(COMMAND_DECOMPOSE_SUPERBOX)) {
            SuperBoxNode superBox = petal_model.getSuperBox();
            superBox.decompose();
        }
        else if (btn.equals(COMMAND_SET_WORKLOAD)) {
            String init = "localhost";
            int port = 15000;
            if (workloadCommunicator!=null) {
                init = workloadCommunicator.getServer();
                port = workloadCommunicator.getPort();
            }
            boolean noCancel = true;
            String s = "";
            while (noCancel&&((s==null)||s.equals(""))) {
                s = JOptionPane.showInputDialog("Please enter a server",init);
                if (s==null) {
                    noCancel = false;
                }
                else if (s.equals("")) {
                    JOptionPane.showConfirmDialog(null,
                                                  "No value entered for server",
                                                  "Error",
                                                  JOptionPane.DEFAULT_OPTION,
                                                  JOptionPane.ERROR_MESSAGE);
                }
            }
            boolean fail = true;
            Integer objPort = new Integer(port);
            while (noCancel&&fail) {
                String p = "";
                p = JOptionPane.showInputDialog("Please enter a port", objPort);
                if (p==null) {
                    noCancel = false;
                }
                else {
                    try {
                        port = (new Integer(p)).intValue();
                        fail = false;
                    } catch (Exception error) {
                        JOptionPane.showConfirmDialog(null,
                                                    error,
                                                    "Exception" ,
                                                    JOptionPane.DEFAULT_OPTION,
                                                    JOptionPane.ERROR_MESSAGE);
                    }
                }
            }
            if (noCancel) {
                SuperBoxNode box = editor.getModel().getSuperBox();
                while (box.getBoxId()!=Constants.ROOTBOXID) {
                    Editor nextEditor = (Editor)box.getEditor().getParent();
                    box = nextEditor.getModel().getSuperBox();
                }
                workloadCommunicator = new WorkloadDataPackage(box);
                workloadCommunicator.setServer(s);
                workloadCommunicator.setPort(port);
            }
        }
        else if (btn.equals(COMMAND_START_WORKLOAD)) {
            if (workloadCommunicator==null) {
                JOptionPane.showConfirmDialog(null,
                                              "No workload generator specified",
                                              "Error",
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
            else {
                workloadCommunicator.setMode(Constants.WORKLOAD_START);
                workloadCommunicator.sendData();
            }
        }
        else if (btn.equals(COMMAND_UPDATE_WORKLOAD)) {
            if (workloadCommunicator==null) {
                JOptionPane.showConfirmDialog(null,
                                              "No workload generator specified",
                                              "Error",
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
            else {
                workloadCommunicator.setMode(Constants.WORKLOAD_UPDATE);
                workloadCommunicator.sendData();
            }
        }
        else if (btn.equals(COMMAND_STOP_WORKLOAD)) {
            if (workloadCommunicator==null) {
                JOptionPane.showConfirmDialog(null,
                                              "No workload generator specified",
                                              "Error",
                                              JOptionPane.DEFAULT_OPTION,
                                              JOptionPane.ERROR_MESSAGE);
            }
            else {
                workloadCommunicator.setMode(Constants.WORKLOAD_STOP);
                workloadCommunicator.sendData();
            }
        }
        else if (btn.equals(VIEW_PRETTIFY)) {
            PetalLayoutMethod layout = new PetalLevelLayout(petal_editor);
            petal_editor.commandLayout(layout);
            editor.getWorkspace().resetPortNodePositions();
        }

        Utils.updateHierarchy(petal_model);
    }   
    
    protected void commandCreateSuperBox()
    {
        PetalNode[] petalNodes = editor.getModel().getSelectedNodes();
        for(int i=0; i<petalNodes.length; i++) {
            for(int j=0; j<petalNodes.length; j++) {
                Vector innerNodes = Utils.getInnerNodes((BoxNode)petalNodes[i], 
                                                        (BoxNode)petalNodes[j]);
                if(innerNodes.size() > 0) {
                    for(int k=0; k<innerNodes.size(); k++) {
                        petal_model.select(innerNodes.elementAt(k));
                    }
                }
            }
        }



        Model newModel = new Model(null);
        SuperBoxNode superBox = new SuperBoxNode(-1, null, newModel, petal_model);

        superBox.setCatalogManager(editor.getCatalogManager());
        newModel.setSuperBox(superBox);
        superBox.createNewWindow(null);
        
	// The following code block sorts all of the nodes from top to bottom so arcs don't cross
        PetalNode[] unsorted_nodes = petal_model.getSelectedNodes();
	Vector nodesV = new Vector();
	for(int i=0; i<unsorted_nodes.length; i++) {
	    nodesV.addElement(unsorted_nodes[i]);
	}
	GenericNode[] nodes = new GenericNode[unsorted_nodes.length];
	for(int i=0; i<nodes.length; i++) {
	    GenericNode ptrNode = null;
	    for(int j=0; j<nodesV.size(); j++) {
		GenericNode currentNode = (GenericNode) nodesV.elementAt(j); 
		if(ptrNode != null) {
		    double ptrY = ptrNode.getComponent().getLocation(null).getY();
		    double currentY = currentNode.getComponent().getLocation(null).getY();
		    if(ptrY > currentY) {
		       ptrNode = currentNode;
		    }
		} else {
		    ptrNode = currentNode;
		}
	    }
	    nodesV.remove(ptrNode);
	    nodes[i] = ptrNode;
	}
	// end block

        if(nodes.length == 0) {
            JOptionPane.showConfirmDialog(null, 
                                          "No boxes have been selected for this operation.",
                                          "Error!", 
                                          JOptionPane.DEFAULT_OPTION, 
                                          JOptionPane.ERROR_MESSAGE);                
        } else {            
            for(int i=0; i<nodes.length; i++) {
                
                Point point = new Point(0,0);
                if(nodes[i] instanceof GenericNode) {
                    GenericNode genericNode = (GenericNode)nodes[i];
                    Component component = genericNode.getNodeComponent();
                    point = component.getLocation(null);
                }
                newModel.addNode(nodes[i], point);
                ((GenericNode)nodes[i]).setParentId(superBox.getBoxId());
            }
            Workspace workspace = editor.getWorkspace();                        
            
            int avgX = 0;
            int avgY = 0;
            
            for(int i=0; i<nodes.length; i++) {
                PetalArc[] arcsFrom = petal_model.getArcsFromNode(nodes[i]);
                
                Point location = nodes[i].getComponent().getLocation(null);
                avgX += location.getX();
                avgY += location.getY();
                
                for(int j=0; j<arcsFrom.length; j++) {

                    GenericNode sourceNodeFrom = (GenericNode)arcsFrom[j].getSource();
                    GenericNode targetNodeFrom = (GenericNode)arcsFrom[j].getTarget();
                    
                    boolean addArcToSource = false;
                    boolean addArcToTarget = false;
                    for(int k=0; k<nodes.length; k++) {
                        if(sourceNodeFrom.equals(nodes[k])) {
                            addArcToSource = true;
                        }                    
                        if(targetNodeFrom.equals(nodes[k])) {
                            addArcToTarget = true;
                        }
                    }
                    
                    GenericArc arc = (GenericArc) arcsFrom[j];
                    Workspace newWorkspace = newModel.getWorkspace();
                    
                    // Both source and target nodes are in the superbox.
                    if(addArcToSource && addArcToTarget) {
                        newWorkspace.addArc(sourceNodeFrom, arc.getOutputPort(),
					    targetNodeFrom, arc.getInputPort());
                    }                            
                    
                    // The source node is in the superbox but the target node is not.
                    if(addArcToSource && !addArcToTarget) {
                        OutputPortNode outputNode = newModel.addOutputPort();
                        superBox.synchronizePortsToModel();
                        
                        BoxNodeInputPort end_portNode = outputNode.getInputPortNode();
                        newWorkspace.addArc(sourceNodeFrom, arc.getOutputPort(),
					    outputNode, end_portNode);
                        
                        BoxNodeOutputPort superBoxOutputPort = superBox.getLastOutputPort();
                        
                        workspace.addArc(superBox, superBoxOutputPort,
                                         targetNodeFrom, arc.getInputPort());
                    }
                }
                
                PetalArc[] arcsTo = petal_model.getArcsToNode(nodes[i]);
                
                for(int j=0; j<arcsTo.length; j++) {
                    GenericNode sourceNodeTo = (GenericNode)arcsTo[j].getSource();
                    GenericNode targetNodeTo = (GenericNode)arcsTo[j].getTarget();
                    boolean addArcToSource = false;
                    boolean addArcToTarget = false;
                    for(int k=0; k<nodes.length; k++) {
                        if(sourceNodeTo.equals(nodes[k])) {
                            addArcToSource = true;
                        }
                        if(targetNodeTo.equals(nodes[k])) {
                            addArcToTarget = true;
                        }
                    }
                    
                    GenericArc arc = (GenericArc) arcsTo[j];
                    Workspace newWorkspace = newModel.getWorkspace();
                    
                    // The target node is in the superbox but the source node is not.
                    if(!addArcToSource && addArcToTarget) {
                        InputPortNode inputNode = newModel.addInputPort();
                        superBox.synchronizePortsToModel();

                        BoxNodeOutputPort end_portNode = inputNode.getOutputPortNode();
                        newWorkspace.addArc(inputNode, end_portNode,
					    targetNodeTo, arc.getInputPort());
                        
                        BoxNodeInputPort superBoxInputPort = superBox.getLastInputPort();
                        
                        workspace.addArc(sourceNodeTo, arc.getOutputPort(),
                                         superBox, superBoxInputPort);
                    }                
                }
                
            }
            
            workspace.addNode(superBox, new Point(avgX/nodes.length, avgY/nodes.length));

            for(int i=0; i<nodes.length; i++) {
                petal_model.removeNode(nodes[i]);
            }

        }
	editor.updateWorkspace();
    }
}




