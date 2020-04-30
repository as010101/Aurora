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
 * The panel implementing the palette.  Not yet implemented.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class HierarchyPanel extends JPanel
{
    private Model model;
    private Editor editor;
    private DefaultTreeModel treeModel;
    private HierarchyNode currentNode;
    private JTree hierarchyTree;

    /**
     * Constructs a new Hierarchy Panel.
     *
     * @param petal_model The PetalModel that this PalettePanel semantically adds 
     * nodes/arcs to.
     * @param editor The editor frame that this BoxesPanel is contained within.
     */
    public HierarchyPanel(Model model, Editor editor) 
    {
        this(model, editor, null);
    }

    /**
     * Constructs a new Hierarchy Panel.
     *
     * @param petal_editor The PetalEditor that this PalettePanel visually adds 
     * nodes/arcs to.
     * @param petal_model The PetalModel that this PalettePanel semantically adds 
     * nodes/arcs to.
     * @param editor The editor frame that this BoxesPanel is contained within.
     * @param palette_list A palette list to put into this palette panel.
     */
    public HierarchyPanel(Model model, Editor editor, DefaultTreeModel treeModel)
    {
        super();
        setDoubleBuffered(true);

        this.model = model;
        this.editor = editor;
        currentNode = null;

        if(treeModel == null) {            
            this.treeModel = new DefaultTreeModel(new DefaultMutableTreeNode(""));
        } else {
            this.treeModel = treeModel;
        }
        hierarchyTree = new JTree(this.treeModel);
        updateTree();
        
        setLayout(new GridLayout(1,1));
        
        hierarchyTree.setFont(Constants.PALETTE_LIST_FONT);        

        add(new JScrollPane(hierarchyTree));

        HierarchyTreeListener htl = new HierarchyTreeListener();
        hierarchyTree.addMouseListener(htl);
        hierarchyTree.addTreeSelectionListener(htl);
    }

    public DefaultTreeModel getTreeModel()
    {
        return treeModel;
    }

    public void updateTree()
    {
        TreeNode root = fillTree(null);
        treeModel.setRoot(root);
        
        for(int i=0; i<hierarchyTree.getRowCount(); i++) {
            hierarchyTree.expandRow(i);
        }
    }

    protected TreeNode fillTree(SuperBoxNode superbox)
    {
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
        
        HierarchyNode root = new HierarchyNode(superbox);
        Model model = superbox.getSuperBoxModel();
        if(model != null) {
            Vector boxes = model.getBoxNodes();
            for(int i=0; i<boxes.size(); i++) {
                BoxNode box = (BoxNode) boxes.elementAt(i);
                if(box instanceof SuperBoxNode) {
                    TreeNode newNode = fillTree((SuperBoxNode)box);
                    root.add((DefaultMutableTreeNode) newNode);
                } else {
                    root.add(new HierarchyNode(box));
                }
            }
        }

        return root;
    }


    private class HierarchyTreeListener extends MouseAdapter implements TreeSelectionListener
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

            updateUI();
        }

        public void valueChanged(TreeSelectionEvent e)
        {
            DefaultMutableTreeNode target= 
                (DefaultMutableTreeNode)(e.getPath().getLastPathComponent());
            Object userObject = target.getUserObject();
            if (target instanceof HierarchyNode) {
                currentNode = (HierarchyNode)target;
            } else { 
                currentNode = null;
            }
        }

    }

}
