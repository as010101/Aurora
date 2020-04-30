package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;

import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.dnd.*;
import java.util.*;
import java.net.URL;
import java.net.MalformedURLException;


import edu.brown.aurora.gui.types.*;

/**
 * A class that implements the toolbar for the aurora GUI.
 * 
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Toolbarspace extends JToolBar
{
    private JButton[] buttons;

    private JComboBox zoom;
    
    transient private PetalEditor petal_editor;
    private Model petal_model;
    private Editor editor;


    public static final String EDITMODE = "Edit Mode";
    public static final String ARCMODE = "Add Arc Mode";

    protected static final String SAVE = "Save";
    protected static final String LOAD = "Load";

    protected static final String TEST = "Test";

    protected static final String INPUT = "Add Input Port";
    protected static final String OUTPUT = "Add Output Port";


    private boolean addArcMode;

    public JButton editarcBtn;
    
    
    /**
     * Constructs a new Toolbarspace object.
     *
     * @param petal_editor The petal editor that this toolbar space operates on.
     * @param petal_model The model that this toolbar space operates on.
     * @param editor The parent editor frame that contains this toolbarspace.
     */
    public Toolbarspace(PetalEditor petal_editor, 
                        Model petal_model) {
        super();

        this.petal_editor = petal_editor;
        this.petal_model = petal_model;
        this.editor = editor;

        


        //buttons = new JButton[4];
	buttons = new JButton[6];
        buttons[0] = new JButton(Constants.SAVE);
        buttons[0].addActionListener(new ToolBarButtonListener());
        buttons[1] = new JButton(Constants.LOAD);
        buttons[1].addActionListener(new ToolBarButtonListener());
        buttons[2] = new JButton(Constants.EDIT);
        buttons[2].addActionListener(new ToolBarButtonListener());
        editarcBtn = buttons[2];
	buttons[3] = new JButton(TEST);
	buttons[3].addActionListener(new ToolBarButtonListener());
	buttons[4] = new JButton(Constants.INPUT);
	buttons[4].addActionListener(new ToolBarButtonListener());
	buttons[5] = new JButton(Constants.OUTPUT);
	buttons[5].addActionListener(new ToolBarButtonListener());

        JPanel filler = new JPanel();

        String[] zoomStrs = { "200%", "150%", "100%", "90%", "80%", "70%", 
                              "60%", "50%", "10%" };
        zoom = new JComboBox(zoomStrs);
	zoom.setSelectedItem("100%");
        zoom.addItemListener(new ZoomListener(zoom, this));

        add(buttons[0]);
        add(buttons[1]);
	// add(buttons[2]);
        // add(buttons[3]);
	add(buttons[4]);
	add(buttons[5]);
        add(filler);
        add(zoom);

        setFloatable(false);        

        addArcMode = false;
        petal_model.setEdit(false);
    }

    /**
     * Sets the zoom level of the editor.
     *
     * @param zoomVal the zoom factor which to set.
     */
    public void setZoom(double zoomVal)
    {
        petal_editor.setScaleFactor(zoomVal);
	(petal_model.getWorkspace()).update();
    }

    /**
     *  Returns whether the mode is currently set to edit or not.
     */
    public boolean isAddArcMode()
    {
        return addArcMode;
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
     * A listener class for the toolbar.
     * 
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */    
    protected class ToolBarButtonListener implements ActionListener
    {
        /**
         * Constructs a new ToolBarButtonListener object.
         */
        public ToolBarButtonListener() 
        {
        }

        /**
         * Performs the appropriate tasks depending on the event passed in for 
         * the toolbarspace.
         *
         * @param e The event that triggered the call of this method.
         */            
        public void actionPerformed(ActionEvent e) 
        {
            GenericNode.disableMenu();

            Object btn = e.getSource();
            if (btn.equals(buttons[0])) {
                editor.save(false);
            }
            if (btn.equals(buttons[1])) {
                RootFrame.load();
            }
            if (btn.equals(buttons[2])) {
                if(addArcMode) {
                    editarcBtn.setIcon(Constants.EDIT);
                    addArcMode = false;
                    petal_model.setEdit(false);
                } else {
                    editarcBtn.setIcon(Constants.ARC);
                    addArcMode = true;
                    petal_model.setEdit(true);

                }
            }
	    if (btn.equals(buttons[3])) {
                test();
	    }
	    if (btn.equals(buttons[4])) {
		petal_model.addInputPort();
		SuperBoxNode superbox = petal_model.getSuperBox();
		superbox.synchronizePortsToModel();
		Workspace workspace = editor.getWorkspace();
		workspace.resetPortNodePositions();
	    }
	    if (btn.equals(buttons[5])) {
		petal_model.addOutputPort();
		SuperBoxNode superbox = petal_model.getSuperBox();
		superbox.synchronizePortsToModel();
		Workspace workspace = editor.getWorkspace();
		workspace.resetPortNodePositions();
	    }
            petal_model.deselectAll();


        }
    }

    protected void test()
    {
        TypeInferencer.typeCheck();

        /*
        PetalNode[] petalNodes = editor.getModel().getSelectedNodes();
        for(int i=0; i<petalNodes.length; i++) {
            for(int j=0; j<petalNodes.length; j++) {
                Vector innerNodes = Utils.getInnerNodes((BoxNode)petalNodes[i], 
                                                        (BoxNode)petalNodes[j]);
                if(innerNodes.size() > 0) {
                    System.out.println("Stack result: ");
                    for(int k=0; k<innerNodes.size(); k++) {
                        System.out.println("\t" + innerNodes.elementAt(k).toString());
                    }
                }
            }
        }
        */
	/*
	UnnamedType newType1 = new UnnamedType();
	newType1.addAttribute(TypeManager.INTEGER_TYPE, TypeManager.INTEGER_SIZE);
	newType1.addAttribute(TypeManager.BOOL_TYPE, TypeManager.BOOL_SIZE);
	newType1.addAttribute(TypeManager.INTEGER_TYPE, TypeManager.INTEGER_SIZE);
	newType1.addAttribute(TypeManager.INTEGER_TYPE, TypeManager.INTEGER_SIZE);

	UnnamedType newType2 = new UnnamedType();
	newType2.addAttribute(TypeManager.STRING_TYPE, 50);

	UnnamedType newType3 = new UnnamedType();
	newType3.addAttribute(TypeManager.FLOAT_TYPE, TypeManager.FLOAT_SIZE);
	newType3.addAttribute(TypeManager.DOUBLE_TYPE, TypeManager.DOUBLE_SIZE);
	newType3.addAttribute(TypeManager.INTEGER_TYPE, TypeManager.INTEGER_SIZE);

	CompositeType[] ctypes = RootFrame.typeManager.getCompositeTypes();

	System.out.println("number of composite types : " + ctypes.length);

	for(int i=0; i<ctypes.length; i++) {
	    System.out.println("Comparing " + ctypes[i].toStringInDetail() + 
			     " with newType1 : " + newType1.isCompatible(ctypes[i]) +
			     ", " + ctypes[i].isCompatible(newType1));
	    System.out.println("Comparing " + ctypes[i].toStringInDetail() + 
			     " with newType2 : " + newType2.isCompatible(ctypes[i]) +
			     ", " + ctypes[i].isCompatible(newType2));
	    System.out.println("Comparing " + ctypes[i].toStringInDetail() + 
			     " with newType3 : " + newType3.isCompatible(ctypes[i]) +
			     ", " + ctypes[i].isCompatible(newType3));

	}
	*/
    }


    /**
     * A listener for the pull-down zoom menu on the toolbar.
     * 
     * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
     * @version 1.0 04/10/2002
     */
    protected class ZoomListener implements ItemListener
    {
        private JComboBox zoom;
        private Toolbarspace toolbar;

        /**
         * Constructs a new zoomlistener object.
         *
         * @param zoom The jcombobox that this listener is associated with.
         * @param toolbar The toolbar that the zoom combobox is contained within.
         */
        public ZoomListener(JComboBox zoom, Toolbarspace toolbar) 
        {
            this.zoom = zoom;
            this.toolbar = toolbar;
        }
 
        /**
         * Performs the appropriate tasks depending on the event passed in for 
         * the zoom pulldown menu.
         *
         * @param e The event that triggered the call of this method.
         */             
        public void itemStateChanged(ItemEvent e) 
        {
            String valStr = zoom.getSelectedItem().toString();
            valStr = valStr.substring(0, valStr.length()-2);
            double toZoom = 0.1 * Double.parseDouble(valStr);
            toolbar.setZoom(toZoom);
        }
    }

}
