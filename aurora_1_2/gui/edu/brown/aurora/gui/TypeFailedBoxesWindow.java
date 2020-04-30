package edu.brown.aurora.gui;

import edu.brown.aurora.gui.dbts.PaletteRecord;
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
 * A frame that displays all of the nodes that contained type inference and type checking errors.
 *
 * @author Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class TypeFailedBoxesWindow extends JFrame
{

    protected class TypeFailedBoxesTreeNode extends DefaultMutableTreeNode
    {
	BoxNode node;

	public TypeFailedBoxesTreeNode(BoxNode node)
	{
	    super(node.getLabel());
	    this.node = node;
	}

	public BoxNode getNode()
	{
	    return node;
	}
    }

    protected JTree errorNodesTree;
    protected DefaultTreeModel treeModel;
    protected TypeFailedBoxesTreeNode currentNode;
    protected Model model;

    protected static TypeFailedBoxesWindow tfbWindow = null;
    protected static String CLOSE_WINDOW = "Close";
    protected static String TYPE_CHECK = "Type Check";

    protected JButton closeWindowBtn;

    public static void showWindow(Model model)
    {
	if(tfbWindow == null) {
	    tfbWindow = new TypeFailedBoxesWindow(model);
	} else {
	    tfbWindow.setVisible(true);
	    tfbWindow.updateTree();
	}
    }

    /**
     * Constructs a new TypeFailedBoxesWindow
     */
    protected TypeFailedBoxesWindow(Model model) 
    {
	super("Type Failed Boxes Window");
	
	GridBagLayout bags = new GridBagLayout();

	JPanel mainPanel = new JPanel(bags);
	treeModel = new DefaultTreeModel(new DefaultMutableTreeNode(""));
	errorNodesTree = new JTree(treeModel);
	errorNodesTree.setFont(Constants.PALETTE_LIST_FONT);
	TypeFailedBoxesTreeListener listener = new TypeFailedBoxesTreeListener();
	errorNodesTree.addMouseListener(listener);
	errorNodesTree.addTreeSelectionListener(listener);
	this.model = model;

	closeWindowBtn = new JButton(CLOSE_WINDOW);
        closeWindowBtn.setEnabled(true);
	JButton typeCheckBtn = new JButton(TYPE_CHECK);
	closeWindowBtn.addActionListener(new TFBListener());
	typeCheckBtn.addActionListener(new TFBListener());
	closeWindowBtn.setFont(Constants.TOOLBOX_BUTTON_FONT);
	typeCheckBtn.setFont(Constants.TOOLBOX_BUTTON_FONT);
	Utils.addComponent(bags, mainPanel, new JScrollPane(errorNodesTree), 0,0,3,1,1,20, new Insets(15,15,5,15));
	Utils.addComponent(bags, mainPanel, new JPanel(), 0,1,1,1,4,1, new Insets(10, 50,15,0));
	Utils.addComponent(bags, mainPanel, typeCheckBtn, 1,1,1,1,1,1, new Insets(10,5,15,5));
	Utils.addComponent(bags, mainPanel, closeWindowBtn, 2,1,1,1,1,1, new Insets(10,5,15,15));
	getContentPane().add(mainPanel);

	updateTree();



	setVisible(true);
	setSize(300, 400);
    }

    protected class TFBListener implements ActionListener
    {
	public void actionPerformed(ActionEvent e)
	{
	    if(CLOSE_WINDOW.equals(e.getActionCommand())) {
		setVisible(false);
	    }
	    if(TYPE_CHECK.equals(e.getActionCommand())) {
                TypeInferencer.typeCheck();
                closeWindowBtn.setEnabled(true);
		updateTree();
	    }
	}
    }


    public void updateTree()
    {
	TreeNode root = fillTree(null);
	treeModel.setRoot(root);


	for(int i=0; i<errorNodesTree.getRowCount(); i++) {
	    errorNodesTree.expandRow(i);
	}

    }

    protected TreeNode fillTree(SuperBoxNode superbox)
    {
	// modify this method so that it adds nodes that type fail and superboxes.
	// for now we'll just include all of the boxes.
        if(superbox == null) {
            Editor rootEditor = Utils.getRootEditor(model);
            if(rootEditor == null) {
                return new DefaultMutableTreeNode("");
            }
            model = rootEditor.getModel();
            if(model == null) {
                return new DefaultMutableTreeNode("");
            }
            superbox = rootEditor.getModel().getSuperBox();

        }
        TypeFailedBoxesTreeNode root = new TypeFailedBoxesTreeNode(superbox);
        Model model = superbox.getSuperBoxModel();
        if(model != null) {
            Vector boxes = model.getBoxNodes();
            for(int i=0; i<boxes.size(); i++) {
                BoxNode box = (BoxNode) boxes.elementAt(i);
                if(box instanceof SuperBoxNode) {
                    TreeNode newNode = fillTree((SuperBoxNode)box);
                    root.add((DefaultMutableTreeNode) newNode);
                } else {
                    if(box instanceof PrimitiveBoxNode) {
                        PrimitiveBoxNode primBox = (PrimitiveBoxNode) box;
                        if(!primBox.typeValid()) {
                            root.add(new TypeFailedBoxesTreeNode(box));
                            closeWindowBtn.setEnabled(false);                            
                        }
                    }
                }
            }
        }

        return root;
    }


    private class TypeFailedBoxesTreeListener extends MouseAdapter implements TreeSelectionListener
    {
        public void mouseClicked(MouseEvent event)
        {
            GenericNode.disableMenu();
            if (event.getModifiers() == 4) {	// mouse right button click
                if(currentNode != null) {                     
                    BoxNode boxNode = currentNode.getNode();                    
                    boxNode.showMenu(event);
                    GenericNode.jmenu.setVisible(true);
                    Callback.showMenu = false;
                    Callback.showMenuEdit = false;
                }
            } 
        }

        public void valueChanged(TreeSelectionEvent e)
        {
            DefaultMutableTreeNode target= 
                (DefaultMutableTreeNode)(e.getPath().getLastPathComponent());
            Object userObject = target.getUserObject();
            if (target instanceof TypeFailedBoxesTreeNode) {
                currentNode = (TypeFailedBoxesTreeNode)target;
            } else { 
                currentNode = null;
            }
        }

    }

}
