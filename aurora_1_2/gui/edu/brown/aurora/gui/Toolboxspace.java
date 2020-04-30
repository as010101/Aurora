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
 * Implements the toolbox for the editor frame.  It contains a tabbedpane, each
 * tab for each of the important components of the toolbox.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Toolboxspace extends JPanel
{
    transient private PetalEditor petal_editor;
    private Model petal_model;
    private Editor editor;
    private TypePane typePane;

    private JTabbedPane tabbedPane;

    private JPanel typesPanel;
    private BoxesPanel boxes;
    private PalettePanel palette;
    private HierarchyPanel hierarchyPanel;

    /**
     * String identifier for the hierarchy on the tab.
     */
    static final String HIERARCHY = "Hierarchy";
    /**
     * String identifier for the types on the tab.
     */
    static final String TYPES = "Types";
    /**
     * String identifier for the boxes on the tab.
     */
    static final String BOXES = "Boxes";
    /**
     * String identifier for the palette on the tab.
     */
    static final String PALETTE = "Palette";

    /**
     * Unique int identifier for the hierarchy tab.
     */
    public static final int HIERARCHY_ID = 0;
    /**
     * Unique int identifier for the types tab.
     */
    public static final int TYPES_ID = 1;
    /**
     * Unique int identifier for the boxes tab.
     */
    public static final int BOXESPANEL_ID = 2;
    /**
     * Unique int identifier for the palette tab.
     */
    public static final int PALETTEPANEL_ID = 3;

    /**
     * Constructs a new Toolboxspace object.
     *
     * @param petal_editor The PetalEditor that this toolboxspace 
     * operates on.
     * @param petal_model The Model that this toolboxspace operates
     * on.
     * @param editor The parent Editor frame that visually contains
     * this class.  
     */
    public Toolboxspace(PetalEditor petal_editor, Model petal_model) {

        super(new GridLayout(1,1));

        this.petal_editor = petal_editor;
        this.petal_model = petal_model;
        this.editor = editor;

        tabbedPane = new JTabbedPane();

        typesPanel = new JPanel(new GridLayout(1,1));
        JPanel boxesPanel = new JPanel(new GridLayout(1,1));        
        JPanel palettePanel = new JPanel(new GridLayout(1,1));
        
        typePane = new TypePane();
        typesPanel.add(typePane);

        boxes = new BoxesPanel(petal_editor, petal_model, editor);
        boxesPanel = new JPanel(new GridLayout(1,1));
        boxesPanel.add(boxes);

        if(petal_model.getSuperBox().isRootBox()) {
            palette = new PalettePanel(petal_editor, petal_model, editor);
            hierarchyPanel = new HierarchyPanel(petal_model, editor);
        } else {
            Editor rootEditor = Utils.getRootEditor(petal_model);
            palette = rootEditor.getToolboxspace().getPalettePanel();
            ListModel listModel = palette.getPaletteList().getModel();
            palette = new PalettePanel(petal_editor, petal_model, editor, listModel);

            HierarchyPanel rootHierarchy = rootEditor.getToolboxspace().getHierarchyPanel();
            DefaultTreeModel treeModel = rootHierarchy.getTreeModel();
            hierarchyPanel = new HierarchyPanel(petal_model, editor, treeModel);
        }
        palettePanel = new JPanel(new GridLayout(1,1));
        palettePanel.add(palette);


        tabbedPane.addTab(HIERARCHY, hierarchyPanel);
        tabbedPane.addTab(TYPES, typesPanel);
        tabbedPane.addTab(BOXES, boxesPanel);
        tabbedPane.addTab(PALETTE, palettePanel);

        tabbedPane.setSelectedIndex(BOXESPANEL_ID);

        add(tabbedPane);
    }

    /**
     * Returns the type pane.
     *
     * @return the TypePane.
     */
    public TypePane getTypePanel()
    {
        return typePane;
    }

    /**
     * Returns the selected component of the tabbed pane.
     */
    public Component getSelectedTabbedPane()
    {
        return ((Container)tabbedPane.getSelectedComponent()).getComponent(0);
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

    /**
     * Returns the hierarchy panel.
     *
     * @return the hierarchy panel of this toolbox space.
     */
    public HierarchyPanel getHierarchyPanel()
    {
        return this.hierarchyPanel;
    }


    /**
     * Returns the boxes panel.
     *
     * @return The boxes panel.
     */
    public BoxesPanel getBoxesPanel()
    {
        return boxes;
    }

    public PalettePanel getPalettePanel() 
    {
        return palette;

    }

    public void setCurrentTab(int tab)
    {
        tabbedPane.setSelectedIndex(tab);        
    }
    

}
