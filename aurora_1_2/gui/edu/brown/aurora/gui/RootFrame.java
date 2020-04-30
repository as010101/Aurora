/*
 * @(#)  RootFrame.java	1.0 05/15/2002
 *
 * Author Jeong-Hyon Hwang and Robin Yan (jhhwangcs.brown.edu, rly@cs.brown.edu)
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */
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
import java.io.*;

/**
 * The entry-point class for the Aurora GUI.  Entry point occurs in the main
 * method.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */

public class RootFrame 
{
    
    public static String home = "tables";
    
    /**
     * A public, static type manager for all of the gui.  Currently the type manager is
     * passed around in a lot of the classes.  Since we're only using one type manager for
     * all of the editors, it is better and preferable to use this type manager instead
     * and phase out the method of passing around typemanagers everywhere.
     */
    public static TypeManager typeManager;


    /**
     * A public, static catalog manager for all of the gui similar to the type manager.. 
     */ 
    public static CatalogManager catalogManager = null;

    /**
     * A public, static parser for all of the gui to use.
     */
    public static Parser parser;


    public static Editor editor = null;

    /**
     * Returns the command-line arguments of the invocation of this class.
     *
     * @return the command-line arguments of the invocation of this class
     * as a string.
     */
    public static String printArgs()
    {
        String args =
            "RootFrame -r -path <path> \n" +
            "\t -r invokes remote execution\n" +
            "\t -path <path> sets the directory of the db files\n" +
            "\t -debug sets debugging on\n " +
            "\t -verbose sets verbosity on\n";
        return args;

    }

    /**
     * Sets up the color scheme for the GUI editor.
     */
    public static void setupColors()
    {
        //SwingColors colors = new SwingSetup();
    }

    public static void load()
    {
        if (catalogManager == null) {
            catalogManager = new CatalogManager();
            typeManager = new TypeManager();
            parser = new Parser(false); // Constants.VERBOSE);
        }
        catalogManager.setHomeDirectory(home);
        if(Constants.VERBOSE) System.out.println("Home Directory = " + 
                                                 catalogManager.getHomeDirectory());
        catalogManager.setCurrentDirectory();

        // new SwingSetup();
        // I don't like the colors that this results in. -rly
        
        try {
            if(catalogManager.isLoaded()) {
                if(editor != null) {
                    editor.setVisible(false);
                }

                catalogManager = new CatalogManager();
                typeManager = new TypeManager();

                SuperBoxNode mainSuperBox = catalogManager.getMainSuperBox();
                mainSuperBox.setLabel(catalogManager.getName());
                editor = mainSuperBox.createNewWindow("Main Aurora Editor for \"" + mainSuperBox.getLabel() + "\"");
                Model superBoxModel = mainSuperBox.getSuperBoxModel();

                CatalogManager.load(superBoxModel);
                editor.getWorkspace().resetPortNodePositions();
                
                SwingRootMonitor s = new SwingRootMonitor(true) 
                    {
                        public void windowClosing(WindowEvent e) 
                        {
                            e.getWindow().dispose();
                        }
                    };
                
                editor.addWindowListener(s);
                Utils.updateHierarchy(superBoxModel);
                editor.showEditor();   
            }
        } catch (Throwable t) {
            System.out.println("Error loading catalog.");
	    t.printStackTrace();
        }

    }

    public static void main(String[] args) throws Throwable {
        if (args != null) {
            int i = 0;
            while (i < args.length) {
                if (args[i].equals("-r")) {
                    catalogManager = new RemoteCatalogManager();
                    i++;
                } else if (args[i].equals("-path")) {
                    home = args[i+1];
                    i+=2;
                } else if (args[i].equals("-verbose")) {
                    Constants.VERBOSE = true;
                    System.out.println("Verbosity on.");
                    i++;
                } else {
                    System.out.println(printArgs());
                    System.exit(0);
                }
            }
        }

        load();
    }
}
