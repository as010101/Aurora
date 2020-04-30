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
 * The panel implementing the box list in the toolbox.  It implements a
 * source-draggable list of various types of boxes into the workspace.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class BoxesPanel extends JPanel
    implements DragGestureListener, DragSourceListener
{
    private Model petal_model;
    transient private PetalEditor petal_editor;
    private Editor editor;

    private JList tools_list;
    private DragSource drag_source;

    /**
     * The list string specifying the add filter box operation.
     */
    public static final String ADD_FILTER_BOX = "Filter Box";
    /**
     * The list string specifying the add drop box operation.
     */
    public static final String ADD_DROP_BOX = "Drop Box";
    /**
     * The list string specifying the add map box operation.
     */
    public static final String ADD_MAP_BOX = "Map Box";
    /**
     * The list string specifying the add aggregate box operation.
     */
    public static final String ADD_AGGREGATE_BOX = "Aggregate Box";
    /**
     * The list string specifying the add update relation box operation.
     */
    public static final String ADD_UPDATE_RELATION_BOX = "Update Relation Box";
    /**
     * The list string specifying the add read relation box operation.
     */
    public static final String ADD_READ_RELATION_BOX = "Read Relation Box";
    /**
     * The list string specifying the add bsort box operation.
     */
    public static final String ADD_BSORT_BOX = "Bsort Box";
    /**
     * The list string specifying the add restream box operation.
     */
    public static final String ADD_RESTREAM_BOX = "Restream Box";
    /**
     * The list string specifying the add union box operation.
     */
    public static final String ADD_UNION_BOX = "Union Box";
    /**
     * The list string specifying the add join box operation.
     */
    public static final String ADD_JOIN_BOX = "Join Box";
    /**
     * The list string specifying the add resample box operation.
     */
    public static final String ADD_RESAMPLE_BOX = "Resample Box";
    /**
     * The list string specifying the add unknown box operation.
     */
    public static final String ADD_UNKNOWN_BOX = "Unknown Box";
    /**
     * The list string specifying the add super box operation.
     */
    public static final String ADD_SUPER_BOX = "Super Box";


    /**
     * Constructs a new Boxes Panel.
     *
     * @param petal_editor The PetalEditor that this BoxesPanel visually adds
     * nodes/arcs to.
     * @param petal_model The PetalModel that this BoxesPanel semantically adds
     * nodes/arcs to.
     * @param editor The editor frame that this BoxesPanel is contained within.
     */
    public BoxesPanel(PetalEditor petal_editor,
                      Model petal_model,
                      Editor editor) {
        super();
        setDoubleBuffered(true);

        this.petal_model = petal_model;
        this.petal_editor = petal_editor;
        this.editor = editor;

        GridLayout layout = new GridLayout(1,1);
        setLayout(new GridLayout(1,1));

        DefaultListModel list = new DefaultListModel();
        list.addElement(ADD_FILTER_BOX);
        list.addElement(ADD_DROP_BOX);
        list.addElement(ADD_MAP_BOX);
        list.addElement(ADD_AGGREGATE_BOX);
        list.addElement(ADD_UPDATE_RELATION_BOX);
        list.addElement(ADD_READ_RELATION_BOX);
        list.addElement(ADD_BSORT_BOX);
        list.addElement(ADD_RESTREAM_BOX);
        list.addElement(ADD_UNION_BOX);
        list.addElement(ADD_JOIN_BOX);
        list.addElement(ADD_RESAMPLE_BOX);
        list.addElement(ADD_UNKNOWN_BOX);
        list.addElement(ADD_SUPER_BOX);


        tools_list = new JList(list);
        tools_list.setFont(Constants.BOXES_LIST_FONT);


        drag_source = DragSource.getDefaultDragSource();
        drag_source.createDefaultDragGestureRecognizer(tools_list,
                                                       DnDConstants.ACTION_COPY_OR_MOVE,
                                                       this);
        tools_list.addListSelectionListener(new BoxesListListener(this));
        tools_list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);

        add(tools_list);
    }

    /**
     * Returns the list object that is currently being selected.
     *
     * @return The list object that is currently being selected.
     */
    public Object getSelectedValue() {
        return tools_list.getSelectedValue();
    }

    /**
     * Returns the index of the selected object in the list.
     *
     * @return the index of the selected object in the list.
     */
    public int getSelectedIndex() {
        return tools_list.getSelectedIndex();
    }

    /**
     * Handles selection during drag and drop. (not implemented)
     *
     * @param o The object being selected.
     * @param ws The workspace where the object exists in.
     */
    public void handleSelect(Object o, Workspace ws) {
    }

    /**
     * Implements the DnD interface.  Returns the DragSource object contained
     * in this object.  See java.awt.dnd.DragSourceListener.
     *
     * @return the DragSource object contained in this object.
     */
    public DragSource getDragSource() {
        return drag_source;
    }

    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dragDropEnd(DragSourceDropEvent dsde)
    {
    }

    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dragEnter(DragSourceDragEvent dsde)
    {
    }

    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dragExit(DragSourceEvent dse)
    {
    }

    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dragOver(DragSourceDragEvent dsde)
    {
    }

    /**
     * Implements the DnD interface.  Not implemented.
     * See java.awt.dnd.DragSourceListener.
     */
    public void dropActionChanged(DragSourceDragEvent dsde)
    {
    }

    /**
     * Implements the DnD interface.  Detects a platform-dependent drag initiating
     * mouse gesture and notifies the listener in order for it to initiate an
     * action for the user.
     * See java.awt.dnd.DragGestureListener.
     */
    public void dragGestureRecognized(DragGestureEvent dge) {
        Object selected = getSelectedValue();
        if ( selected != null ) {
            StringSelection text = new StringSelection(selected.toString());
            drag_source.startDrag(dge, DragSource.DefaultMoveDrop, text, this);
        } else {
            System.out.println("Nothing was selected");
        }
    }


  /**
    * Returns a Model contained by the workspace that this boxespanel operates on.
    *
    * @return a model contained by the workspace that this boxespanel operates on.
    */
    public Model getModel() {
      return petal_model;
    }

    /**
     * A list selection listener.  Mainly changes the editability of the workspace
     * when a the JList selected item has changed.
     *
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */
    protected class BoxesListListener implements ListSelectionListener {

        /**
         * The BoxesPanel that contains the list items that this class
         * listens to.
         */
        protected BoxesPanel panel;

        /**
         * Constructs a new BoxesListListener.
         *
         * @param panel the panel that contains the list items that this
         * class listens to.
         */
        public BoxesListListener(BoxesPanel panel) {
            this.panel = panel;
        }

        /**
         * Implements the ListSelectionListener interface.  Changes the
         * editability of the workspace when a different item has been selected
         * in the list.
         *
         * @param e The list selection change event that was passed into this
         * class.
         */
        public void valueChanged(ListSelectionEvent e) {
            GenericNode.disableMenu();
            Model model = panel.getModel();
            model.deselectAll();
        }
    }
}
