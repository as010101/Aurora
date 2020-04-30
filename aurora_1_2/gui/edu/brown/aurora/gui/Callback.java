package edu.brown.aurora.gui;

import edu.brown.bloom.petal.*;
import edu.brown.bloom.swing.*;

import javax.swing.*;
import javax.swing.tree.*;
import java.awt.event.*;
import java.awt.*;
import java.awt.dnd.*;
import java.util.*;

/**
 * A class that implements petal callbacks.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class Callback implements PetalEditorCallback
{
    transient protected PetalEditor petal_editor;

    /**
     * A flag that the commandDoneCallback method uses to determine whether a
     * popupmenu should be opened or not.
     */
    static boolean showMenu = false;

    /**
     * A flag that the commandDoneCallback method uses to determine whether a
     * popupmenu should be opened or not.
     */
    static boolean showMenuEdit = false;


    protected Model model;

    /**
     * Constructs a Callback object.
     *
     * @param petal_editor The PetalEditor object that this callback object is
     * to be associated with.
     */
    public Callback(PetalEditor petal_editor, Model model)
    {
        this.petal_editor = petal_editor;
        this.model = model;
    }

    /**
     * Sets the callback showmenu flag equal to true.  I think this is an effective,
     * albeit unusual way of handling the closing of popup menus - when a genericnode
     * wants to open up a window, all it has to do is to call this method and then
     * the ensuing commandDoneCallback method would interpret the the flag and either
     * show the menu or close it, whichever is appropriate.
     */

    public static void showMenu() 
    {
        showMenu = true;
        showMenuEdit = true;
    }

    /**
     * The callback method.  This method gets called when an event occurs on the
     * petal editor.
     */
    public void commandDoneCallback()
    {
        if(showMenu) {
            model.deselectAll();
            GenericNode.jmenu.setVisible(true);
            showMenu = false;
        } else {
            if(!showMenuEdit) {
                GenericNode.disableMenu();
            }
            showMenuEdit = false;                            
        }
        Utils.updateHierarchy(model);

    }
}
