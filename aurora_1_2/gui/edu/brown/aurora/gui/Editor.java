package edu.brown.aurora.gui;

import edu.brown.aurora.gui.types.*;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;

import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.dnd.*;
import java.util.*;


/**
 * The visual Editor Frame of GUI.  It contains the primary spaces of the GUI, including
 * the PetalEditor, Toolbar, Toolbox, and Messages subframe.  It also contains the semantic
 * data structures required for the superbox that the editor represents.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Editor extends JFrame implements PetalConstants
{
    /**
     * The PetalEditor object on which the design occurs.
     */
    transient private PetalEditor petal_editor;

    /**
     * The data structure that contains all of the boxes and arcs.
     */
    private Model petal_model;

    /**
     * The toolbar of this editor frame.
     */
    private Toolbarspace toolbarspace;

    /**
     * The toolbox of this editor frame.
     */
    private Toolboxspace toolboxspace;

    /**
     * The workspace of this editor frame.  This workspace contains the petalEditor object.
     */
    private Workspace workspace;

    /**
     * The message space of this editor frame.
     */
    private Messagespace messagespace;

    /**
     * The catalog manager
     */
    transient CatalogManager cm;

    /**
     * The name of this frame.
     */
    private String name;

    /**
     * Constructs a new Editor.
     *
     * @param cm The CatalogManager to be associated with this Editor.
     * @param str The name of this Editor frame.
     * @param petal_model The Model object that stores box/arc information
     * for this editor.
     */
    public Editor(CatalogManager cm, String str, PetalEditor petalEditor, Model model)
    {
        this(cm, str, model, petalEditor,
             new Toolbarspace(petalEditor, model),
             new Toolboxspace(petalEditor, model),
             new Workspace(petalEditor, model),
             new Messagespace(petalEditor, model));
    }

    /**
     * This constructor is for creating new windows for superboxes by reusing
     * and sharing objects that already exist.
     * <br><br>
     * I don't know if this is a good idea.  Although it saves memory and is faster,
     * it results in pretty strange behavior.
     *
     * @param cm The CatalogManager associated with this Editor.
     * @param str The name of this Editor frame.
     * @param otherEditor Another Editor whose objects will be used for the
     * construction of this editor.
     */
    public Editor(CatalogManager cm, String str, Editor otherEditor)
    {

        this(cm, str, otherEditor.getModel(), new PetalEditor(otherEditor.getModel()),
             otherEditor.getToolbarspace(), otherEditor.getToolboxspace(),
             otherEditor.getWorkspace(), otherEditor.getMessagespace());
    }

    protected Editor(CatalogManager cm, String name, Model model,
                     PetalEditor petalEditor, Toolbarspace toolbarspace,
                     Toolboxspace toolboxspace, Workspace workspace,
                     Messagespace messagespace)
    {
        super(name);

        this.cm = cm;
        this.name = name;
        this.petal_model = model;
        this.petal_editor = petalEditor;
        this.toolbarspace = toolbarspace;
        this.toolboxspace = toolboxspace;
        this.workspace = workspace;
        this.messagespace = messagespace;

        getRootPane().setDoubleBuffered(true);

        if(petal_model.getSuperBox().isRootBox()) {
            setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
        }

        addWindowFocusListener(new FocusListener(this));

        petal_editor.addEditorCallback(new Callback(petal_editor, model));

        messagespace.setSize(100,100);
        petal_model.setWorkspace(workspace);

        JPanel mainPanel = new JPanel();
        mainPanel.setDoubleBuffered(true);

        petal_model.setEditor(petal_editor);
        petal_model.setAuroraEditor(this);

        BorderLayout layout = new BorderLayout();
        mainPanel.setLayout(layout);

        JSplitPane vsplit = new JSplitPane();
        vsplit.setLeftComponent(toolboxspace);
        vsplit.setRightComponent(new JScrollPane(workspace));
        vsplit.setDividerLocation(100);

        JSplitPane hsplit = new JSplitPane(JSplitPane.VERTICAL_SPLIT);
        hsplit.setTopComponent(vsplit);
        hsplit.setBottomComponent(messagespace);
        hsplit.setDividerLocation(450);

        MenuBar mb = new MenuBar(petal_editor, petal_model, this);
        setJMenuBar(mb);

        mainPanel.add(toolbarspace, BorderLayout.NORTH);
        mainPanel.add(hsplit, BorderLayout.CENTER);

        setContentPane(mainPanel);

        setSize(800, 600);
        setLocation(300,300);

        toolbarspace.setEditor(this);
        toolboxspace.setEditor(this);
        toolboxspace.setSize(100,50);
        workspace.setEditor(this);
        messagespace.setEditor(this);

        workspace.update();

    }

    /**
     * Saves the box diagram in this editor into the catalog.
     */
    public void save(boolean close)
    {
        try {
            Editor rootEditor = Utils.getRootEditor(petal_model);
            Model rootModel = rootEditor.getModel();
            CatalogManager.save(rootModel, close);
            Utils.addMessagesText("Network \"" + rootModel.getSuperBox().getLabel() +
                                  "\" successfully saved to the catalog.\n");
        } catch(Throwable e) {
            ((Exception)e).printStackTrace();
            JOptionPane.showConfirmDialog(null,
                                          e,
                                          "Exception" ,
                                          JOptionPane.DEFAULT_OPTION,
                                          JOptionPane.ERROR_MESSAGE);
        }

    }

    public void kill()
    {
        //setDefaultCloseOperation(WindowConstants.HIDE_ON_CLOSE);
        //super.dispose();
        setVisible(false);
    }

    /**
     * Saves the box diagram in this editor into the catalog and disposes this frame.
     */
    public void dispose()
    {
        if(petal_model.getSuperBox().isRootBox()) {
          int option = JOptionPane.showConfirmDialog(null,
                                            "Do you like to save before quitting?",
                                            "Question" ,
                                            JOptionPane.YES_NO_CANCEL_OPTION,
                                            JOptionPane.QUESTION_MESSAGE);
            if (option == JOptionPane.YES_OPTION) {
              save(petal_model.getSuperBox().getParentModel() == null);
              super.dispose();
            }
            else if (option == JOptionPane.NO_OPTION) {
              super.dispose();
            }

        } else {
            setVisible(false);
        }
    }

    /**
     * Returns the workspace object contained in this Editor.
     *
     * @return The workspace object contained in this editor.
     */
    public Workspace getWorkspace() {
        return workspace;
    }


    /**
     * Returns the messagespace object contained in this editor.
     *
     * @return The messagespace object contained in this editor.
     */
    public Messagespace getMessagespace() {
        return messagespace;
    }

    /**
     * Returns the toolboxspace object contained in this editor.
     *
     * @return The toolboxspace object contained in this editor.
     */
    public Toolboxspace getToolboxspace() {
        return toolboxspace;
    }

    /**
     * Returns the toolbarspace object contained in this editor.
     *
     * @return The toolbarspace object contained in this editor.
     */
    public Toolbarspace getToolbarspace() {
        return toolbarspace;
    }

    /**
     * Returns the catalog manager object contained in this editor.
     *
     * @return The CatalogManager object contained in this editor.
     */
    public CatalogManager getCatalogManager() {
        return cm;
    }

    /**
     * Returns the PetalEditor object contained in this editor.
     *
     * @return The PetalEditor object contained in this editor.
     */
    public PetalEditor getEditor() {
        return petal_editor;
    }

    /**
     * Returns the model object contained in this editor.
     *
     * @return The Model object contained in this editor.
     */
    public Model getModel() {
        return petal_model;
    }

    /**
     * Sets the model object for this editor.
     *
     * @param model The model object to set for this editor.
     */
    public void setModel(Model model) {
        petal_model = model;
    }

    /**
     * Returns the name of this editor as a String.
     *
     * @return The name of this editor as a string.
     */
    public String getName() {
        return name;
    }

    /**
     * Visually updates the workspace contained by this editor.
     */
    public void updateWorkspace()
    {
        workspace.update();
    }

    /**
     * Shows the editor frame, sets it visible.
     */
    public void showEditor()
    {

        try {
            UIManager.setLookAndFeel(Constants.LOOK_AND_FEEL);
            SwingUtilities.updateComponentTreeUI(this);
            pack();
        } catch (Exception e) {
	    System.out.println(e.toString());
	    e.printStackTrace();
	}

        setSize(Constants.EDITOR_SIZE);
        setVisible(true);
    }

    protected class FocusListener implements WindowFocusListener
    {
        protected Editor editor;

        public FocusListener(Editor ed)
        {
            editor = ed;
        }

        public void windowGainedFocus(WindowEvent e)
        {
            editor.updateWorkspace();
        }

        public void windowLostFocus(WindowEvent e)
        {
        }
    }
}
