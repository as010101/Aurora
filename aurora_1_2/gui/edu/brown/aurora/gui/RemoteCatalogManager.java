/*
 * @(#)RemoteCatalogManager.java	1.0 04/22/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved. 
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui;
import brown.edu.cosmos.*;
import java.io.*;

/**
 * A RemoteCatalogManager object has a type manager and an Aurora network specification.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/22/2002
 */
public class RemoteCatalogManager extends CatalogManager implements java.io.Serializable
{
    static transient Environment environment = null; 
    
    /**
     * Constructs a new RemoteCatalogManager object. 
     */
    public RemoteCatalogManager()
    {
        if (environment == null)
            environment = new Environment();
        if (!environment.locateServers())
            System.exit(1);
    }

    /**
     * Returns the names of the catalogs stored in a Vector.
     *
     * @return A vector containing the names of all the catalogs.
     */
    public java.util.Vector getCatalogNames()
    {
        Stub s = null;
        int count  = 0;
        for (;;) {
            count++;
            if (count > 3) return null;
            try {
                s = environment.create("edu.brown.aurora.gui.CatalogManager", 
                                       new Object[] {getHomeDirectory()}, true);
                if (s.isInstantiated()) {
                    MessageHandle h = null;
                    h = s.invoke("getCatalogNames", null, true);
                    if (h.getStatus() == MessageHandle.DONE) {
                        java.util.Vector c = (java.util.Vector)(h.getObject());
                        System.out.println("remote catalog list retrieved.");
                        return c;
                    }
                }
            } catch (Throwable e) { System.out.println(e);
	    e.printStackTrace();
	    }
        }	
    }

    /**
     * Selects a catalog.
     */
    public String selectCatalog(String name)
    {
        Stub s = null;
        int count  = 0;
        for (;;) {
            count++;
            if (count > 3) return null;
            try {
                s = environment.create("edu.brown.aurora.gui.CatalogManager", 
                                       null, true);
                if (s.isInstantiated()) {
                    MessageHandle h = null;
                    System.out.println("hi");
                    h = s.invoke("selectCatalog", 
                                 new Object[] {new String(name)}, true);
                    System.out.println("there");
                    if (h.getStatus() == MessageHandle.DONE) {
                        String c = (String)(h.getObject());
                        System.out.println("remote catalog setup succeeded.");
                        System.out.println(c);
                        setCurrentDirectory(c);
                        return c;
                    }
                }
            } catch (Throwable e) { System.out.println(e);
	    e.printStackTrace();
	    }
        }	
    }

    /**
     * Loads a catalog.
     *
     * 
     */
    public CatalogManager load() throws Throwable 
    {
        Stub s = null;
        int count  = 0;
        for (;;) {
            count++;
            if (count > 3) return null;
            try {
                s = environment.create("edu.brown.aurora.gui.CatalogManager", 
                                       null, true);
                if (s.isInstantiated()) {
                    MessageHandle h = null;
                    h = s.invoke("load", null, true);
                    if (h.getStatus() == MessageHandle.DONE) {
                        CatalogManager c = (CatalogManager)(h.getObject());
                        set(c);
                        mainSuperBox.getSuperBoxModel().setSelectSet();
                        mainSuperBox.getSuperBoxModel().adjust();
                        System.out.println("remote load done");
                        return c;
                    }
                }
            } catch (Throwable e) { System.out.println(e);
	    e.printStackTrace();
	    }
        }	
    }

    /**
     * Saves a catalog.
     */
    public CatalogManager save() throws Throwable 
    {
        Stub s = null;
        int count  = 0;
        for (;;) {
            count++;
            if (count > 3) return null;
            try {
                s = environment.create("edu.brown.aurora.gui.CatalogManager", 
                                       new Object[] 
                    {new edu.brown.aurora.gui.CatalogManager(this)}, true);
                if (s.isInstantiated()) {
                    MessageHandle h = null;
                    h = s.invoke("save", null, true);
                    if (h.getStatus() == MessageHandle.DONE) {
                        CatalogManager c = (CatalogManager)(h.getObject());
                        System.out.println("remote save done");
                        return c;
                    }
                }
            } catch (Throwable e) { System.out.println(e);
	    e.printStackTrace();
	    }
        }	
    }
}
