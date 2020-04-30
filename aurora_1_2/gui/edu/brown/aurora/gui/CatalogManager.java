/*
 * @(#)CatalogManager.java	1.0 04/22/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

package edu.brown.aurora.gui;

import java.util.*;
import javax.swing.*;
import javax.swing.tree.*;
import edu.brown.aurora.gui.types.*;
import edu.brown.aurora.gui.dbts.*;
import edu.brown.bloom.petal.PetalConstants;

import java.awt.Point;
import java.awt.Rectangle;

import com.sleepycat.db.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintStream;

/**
 * A CatalogManager object has a type manager and an Aurora network
 * specification.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/22/2002
 *
 *  Revised by Matthew Hatoun (mhatoun@cs.brown.edu) on 6/19/03
 */
public class CatalogManager implements java.io.Serializable
{
    protected SuperBoxNode mainSuperBox = null;
    protected transient static String home = "";
    protected transient static String currentDirectory = null;
    protected transient static String name = "";
    protected static boolean loaded;

    /**
     * Constructs a new CatalogManager object.
     */
    public CatalogManager()
    {
        mainSuperBox = new SuperBoxNode(Constants.ROOTBOXID, this, Constants.ROOTBOXID);
        mainSuperBox.setBoxId(Constants.ROOTBOXID);
        mainSuperBox.setParentId(Constants.ROOTBOXID);
        loaded = false;
    }

    /**
     * Constructs a new CatalogManager object.
     */
    public CatalogManager(String homeDirectory)
    {
        this();
        home = homeDirectory;
    }

    /**
     * Constructs a new CatalogManager object.
     */
    public CatalogManager(CatalogManager cm)
    {
        set(cm);
        loaded = false;
    }

    public Vector getCatalogNames()
    {
        Vector v = new Vector();
        try {
            Db catalogListTable = new Db(null, 0);
            catalogListTable.set_error_stream(System.err);
            catalogListTable.set_errpfx("Catalog Retrieval Error");
            String fileName =
                CatalogManager.getHomeDirectory() +
                File.separator +
                CatalogListRecordDbt.databaseFileName;
            catalogListTable.open(fileName, null, Db.DB_BTREE, Db.DB_CREATE, 0644);

            // Acquire an iterator for the table.
            Dbc iterator = catalogListTable.cursor(null, 0);

            StringDbt key = new StringDbt();
            CatalogListRecordDbt record = new CatalogListRecordDbt();
            while (iterator.get(key, record, Db.DB_NEXT) == 0) {
                record.parse();
                v.add(record.getName());
            }
            iterator.close();
            catalogListTable.close(0);
        }
        catch (Throwable e) {
            System.out.println(e);
	    e.printStackTrace();
        }
        return v;
    }

    public void setCurrentDirectory()
    {
        JFrame frame = new JFrame();
        loaded = false;

        CatalogManagerDialog dlg = new CatalogManagerDialog(this, frame);

        dlg.show();
        if (currentDirectory == null) {
            System.exit(0);
	}
    }

    public String getName()
    {
        return name;
    }


    public void deleteCatalog(String name)
    {
        this.name = name;

        try {
            String dbFilename =
                CatalogManager.getHomeDirectory() +
                File.separator +
                CatalogListRecordDbt.databaseFileName;

            String directory = CatalogManager.getHomeDirectory() + File.separator + name;
            new File(directory).delete();

            Db catalogListTable = new Db(null, 0);
            catalogListTable.set_error_stream(System.err);
            catalogListTable.set_errpfx("Catalog Retrieval Error");
            catalogListTable.open(dbFilename, null, Db.DB_BTREE, Db.DB_CREATE, 0644);

            StringDbt key = new StringDbt(name);
            Vector v = new Vector();

            if (catalogListTable.del(null, key, 0) == Db.DB_NOTFOUND) {
                System.out.println("Catalog " + name + " could not be found in the database.");
            }
	    else {
		File directoryToDelete = new File (directory);
		if (directoryToDelete.isDirectory()) {
		    String[] internalFiles = directoryToDelete.list();
		    for (int i = 0; i < internalFiles.length ; i++ ) {
			(new File (directory + File.separator + internalFiles[i])).delete();
		    }
		    directoryToDelete.delete();
		}
	    }
            catalogListTable.close(0);
        } catch (Throwable e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }



    public String selectCatalog(String name)
    {
        this.name = name;
        try {
            String fileName =
                CatalogManager.getHomeDirectory() +
                File.separator +
                CatalogListRecordDbt.databaseFileName;

            //	new File(CatalogManager.getCurrentDirectory()+File.separator+ArcRecord.databaseFileName).delete();

            Db catalogListTable = new Db(null, 0);
            catalogListTable.set_error_stream(System.err);
            catalogListTable.set_errpfx("Catalog Retrieval Error");
            catalogListTable.open(fileName, null, Db.DB_BTREE, Db.DB_CREATE, 0644);

            // Acquire an iterator for the table.
            Dbc iterator = catalogListTable.cursor(null, 0);

            StringDbt key = new StringDbt(name);
            CatalogListRecordDbt record = new CatalogListRecordDbt();
            Vector v = new Vector();
            if (iterator.get(key, record, Db.DB_SET) != 0) {
                if (catalogListTable.put(null, key, new CatalogListRecordDbt(name), 0) == Db.DB_KEYEXIST)
                    System.out.println("The key already exists.");
            }
            currentDirectory = home + File.separator + name;
            File f = new File(currentDirectory);
            if (!(f.exists()))
                f.mkdir();
            if (!(f.exists())) return null;
            iterator.close();
            catalogListTable.close(0);
            loaded = true;
            return currentDirectory;
        } catch (Throwable e) {
            System.out.println(e);
            e.printStackTrace();
            return null;
        }
    }

    /**
     *  creates a new catalog with the given name
     */
    public boolean newCatalog(String name)
    {
        this.name = name;
        try {
            String fileName =
                CatalogManager.getHomeDirectory() +
                File.separator +
                CatalogListRecordDbt.databaseFileName;

    
            Db catalogListTable = new Db(null, 0);
            catalogListTable.set_error_stream(System.err);
            catalogListTable.set_errpfx("Catalog Retrieval Error");
            catalogListTable.open(fileName, null, Db.DB_BTREE, Db.DB_CREATE, 0644);

            // Acquire an iterator for the table.
            Dbc iterator = catalogListTable.cursor(null, 0);

            StringDbt key = new StringDbt(name);
            CatalogListRecordDbt record = new CatalogListRecordDbt();
            Vector v = new Vector();
            if (iterator.get(key, record, Db.DB_SET) != 0) {
                if (catalogListTable.put(null, key, new CatalogListRecordDbt(name), 0) == Db.DB_KEYEXIST)
                    System.out.println("The key already exists.");
            }
            File f = new File(home + File.separator + name);
            if (f.exists()){  
		iterator.close();
		catalogListTable.close(0);
		loaded = false;
		return false;
	    }
	    else {
                f.mkdir();
		iterator.close();
		catalogListTable.close(0);
		loaded = false;
		return true;
	    }
        } catch (Throwable e) {
            System.out.println(e);
            e.printStackTrace();
            return false;
        }
    }

    public boolean isLoaded()
    {
        return loaded;
    }

    public String toString()
    {
        String s = "";
        s += "Type Manager: \r\n";
        if (mainSuperBox != null)  {
            s += "Model: \r\n";
            s += mainSuperBox.getSuperBoxModel()+"\r\n\r\n";
        }
        return s;
    }

    public void set(CatalogManager cm)
    {
        if (cm != null) {
            mainSuperBox = cm.mainSuperBox;
            mainSuperBox.setCatalogManager(this);
        }
    }


    public CatalogManager get()
    {
        return this;
    }

    /**
     * Returns the main super box.
     * @return the main super box
     */

    public SuperBoxNode getMainSuperBox()
    {
        return mainSuperBox;
    }

    public static void setHomeDirectory(String h)
    {
        home = h;
    }

    public static String getHomeDirectory()
    {
        return home;
    }

    public static void setCurrentDirectory(String s)
    {
        currentDirectory = s;
    }

    public static String getCurrentDirectory()
    {
        return currentDirectory;
    }

    /**
     * @param toSave true - save mode, false - load mode.
     */
    public static Db openDb(String filename, boolean toSave) {
        Db newTable = null;
        try {
            String dir = CatalogManager.getCurrentDirectory() + File.separator;

            String errorMsg;
            if(toSave) {
                errorMsg = "Catalog Save error.";
                new File(dir + filename).delete();
            } else {
                errorMsg = "Catalog Retrieval error.";
            }

            newTable = new Db(null, 0);
            newTable.set_error_stream(System.err);
            newTable.set_errpfx(errorMsg);
            newTable.open(dir + filename, null, Db.DB_BTREE, Db.DB_CREATE, 0644);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return newTable;
    }

    /**
     * Loads a model from the database.
     *
     */
    public static void load(Model model) throws Throwable
    {

        String errorMsg = "Catalog Retrieval Error";
        Db boxTable = openDb(BoxRecord.databaseFileName, false);
        Db portTable = openDb(PortRecord.databaseFileName, false);
        Db arcTable = openDb(ArcRecord.databaseFileName, false);
        Db metadataTable = openDb(MetadataRecord.databaseFileName, false);

	TypeInferencer.setEnabled(false);

        loadModel(model, boxTable, portTable, arcTable, metadataTable);

	TypeInferencer.setEnabled(true);
        TypeInferencer.typeCheck();

        boxTable.close(0);
        arcTable.close(0);
        portTable.close(0);
        metadataTable.close(0);
    }

    static void loadModel(Model model,
                          Db boxTable, Db portTable,
                          Db arcTable, Db metadataTable)
        throws Throwable
    {
        if(Constants.VERBOSE) {
            System.out.println("\nLoading data items for SuperBox Id = " + model.getSuperBox().getBoxId());
            System.out.println("=========================================");
        }

        if(model.getSuperBox().isRootBox()) {
            TypeManager tm = RootFrame.typeManager;
            tm.load();
        }
        // Acquire an iterator for the table.
        Dbc iterator = boxTable.cursor(null, 0);

        IntegerArrayDbt key = new IntegerArrayDbt(new int[] {0, model.getSuperBox().getBoxId()});
        BoxRecord record = new BoxRecord();

        CompositeType[] compositeTypes = null;
        try {
            compositeTypes = RootFrame.typeManager.getAllCompositeTypes();
        } catch(Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }

        int inputPortNodes = 0;
        int outputPortNodes = 0;

        while (iterator.get(key, record, Db.DB_NEXT) == 0) {
            if (key.getInteger(1) == model.getSuperBox().getBoxId()) {
                record.parse();
                if (Constants.VERBOSE) System.out.println(record);

                GenericNode node =  null;

                if(record.getBoxType() == Constants.FILTER) {
                    node = FilterBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.MAP) {
                    node = MapBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.AGGREGATE) {
                    node = AggregateBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.UPDATE_RELATION) {
                    node = UpdateRelationBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.READ_RELATION) {
                    node = ReadRelationBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.BSORT) {
                    node = BsortBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.UNION) {
                    node = UnionBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.RESTREAM) {
                    node = RestreamBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.RESAMPLE) {
                    node = ResampleBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.JOIN) {
                    node = JoinBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.DROP) {
                    node = DropBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.SUPERBOX) {
                    node = SuperBoxNode.loadRecord(record, model);
                } else if (record.getBoxType() == Constants.INPUTPORT) {
                    node = InputPortNode.loadRecord(record, model);
                    //check to see if there's a wg_#.wgf file available
                    ((InputPortNode)node).loadWGData(currentDirectory);
                } else if (record.getBoxType() == Constants.OUTPUTPORT) {
                    node = OutputPortNode.loadRecord(record, model);
                } else {
                  node = UnknownBoxNode.loadRecord(record, model);
                }

                if(node != null && model.getSuperBox().getBoxId() == node.getParentId()) {

                    if (node instanceof InputPortNode) {
                        InputPortNode inputPortNode = (InputPortNode) node;
                        model.addInputPort(inputPortNode);
                        model.getSuperBox().synchronizePortsToModel();
                    }
                    if (node instanceof OutputPortNode) {
                        OutputPortNode outputPortNode = (OutputPortNode) node;
                        model.addOutputPort(outputPortNode);
                        model.getSuperBox().synchronizePortsToModel();
                    }

                    // Loads the boxes recursively.
                    if (node instanceof BoxNode) {
                        BoxNode box = (BoxNode) node;
                        box.loadPortRecords(portTable, model);
                    }
                    if(node instanceof SuperBoxNode) {
                        SuperBoxNode superBox = (SuperBoxNode) node;
                        CatalogManager.load(superBox.getSuperBoxModel());
                        superBox.synchronizePortsToModel();
                    }
                }
            }
        }
        iterator.close();

        ArcRecord arcRecord = new ArcRecord();

        iterator = arcTable.cursor(null, 0);

	int maxIdSeen = -1;
        while (iterator.get(key, arcRecord, Db.DB_NEXT) == 0) {
            GenericArc pa = GenericArc.loadRecord(arcRecord, model);
            if (pa != null && pa.getParentId() == model.getSuperBox().getBoxId()) {
                pa.setTargetEnd(new ArcEnd(PetalConstants.PETAL_ARC_END_ARROW));

		int arcId = arcRecord.getId();
                pa.setArcId(arcId);

		if (arcId > maxIdSeen){
		    maxIdSeen = arcId;
		}

                model.addArc(pa);
                if(Constants.VERBOSE) System.out.println("Loading arc : " + pa.toString());
            }
        }

	GenericArc.setCurrentIdCounter(maxIdSeen + 1);
        iterator.close();


        // Stuff that only needs to be loaded once
        if(model.getSuperBox().isRootBox()) {

            // For metadata
            Dbc innerIterator = metadataTable.cursor(null, 0);
            MetadataRecord metadataRecord = new MetadataRecord();
            try {
                if(innerIterator.get(new IntegerDbt(0), metadataRecord, Db.DB_SET) == 0) {
                    metadataRecord.parse();
                    if(Constants.VERBOSE) System.out.println(metadataRecord.toString());

                    if(metadataRecord.getMaxLowerBoxId() > GenericNode.getLowerIdCounter()) {
                        GenericNode.setCurrentLowerIdCounter(metadataRecord.getMaxLowerBoxId());
                    }
                    if(metadataRecord.getMaxUpperBoxId() > GenericNode.getUpperIdCounter()) {
                        GenericNode.setCurrentUpperIdCounter(metadataRecord.getMaxUpperBoxId());
                    }
                }
            } catch (DbException dbe) {
                GenericNode.setCurrentLowerIdCounter(0);
                GenericNode.setCurrentUpperIdCounter(Constants.MAX_STREAM_BOX_ID);
            }

            PalettePanel palette = PaletteUtils.getPalettePanel(model);
            PaletteUtils.loadPalette(palette);
        }
    }


    protected static Db boxTableSave;
    protected static Db portTableSave;
    protected static Db arcTableSave;
    protected static Db metadataTableSave;

    protected static Db boxPaletteTableSave;
    protected static Db arcPaletteTableSave;
    protected static Db portPaletteTableSave;


    /**
     * Saves the model to the database.
     *
     * @param model the model to save to the database.
     * @param close If true, closes the tables.  Otherwise does not close the tables
     * after saving.
     */
    public static void save(Model model, boolean close) throws Throwable
    {
        if(model.getSuperBox().isRootBox()) {
            boxTableSave = openDb(BoxRecord.databaseFileName, true);
            portTableSave = openDb(PortRecord.databaseFileName, true);
            arcTableSave = openDb(ArcRecord.databaseFileName, true);
            metadataTableSave = openDb(MetadataRecord.databaseFileName, true);

            boxPaletteTableSave = openDb(BoxPaletteRecord.databaseFileName, true);
            arcPaletteTableSave = openDb(ArcPaletteRecord.databaseFileName, true);
            portPaletteTableSave = openDb(PortPaletteRecord.databaseFileName, true);
        }

        saveModel(model);

        if(model.getSuperBox().isRootBox()) {
            closeSaveTables(model);
        }
    }

    public static void closeSaveTables(Model model)
        throws Throwable
    {
        int lowerCounter = GenericNode.getLowerIdCounter();
        int upperCounter = GenericNode.getUpperIdCounter();
        MetadataRecord metadataRecord = new MetadataRecord(lowerCounter, upperCounter);
        if(metadataTableSave.put(null,
                                 new IntegerDbt(0),
                                 metadataRecord, 0) == Db.DB_KEYEXIST) {
            System.out.println("CatalogManager.closeSaveTables - key for metadata already exists");
        }

        PalettePanel palette = PaletteUtils.getPalettePanel(model);
        JList paletteList = palette.getPaletteList();
        ListModel listModel = paletteList.getModel();

        for(int i=0; i<listModel.getSize(); i++) {
            Object o = listModel.getElementAt(i);

            if(o instanceof PaletteRecord) {
                PaletteRecord record = (PaletteRecord) o;
                PaletteUtils.savePaletteItems(record,
                                              boxPaletteTableSave,
                                              arcPaletteTableSave,
                                              portPaletteTableSave);
            }
        }

        RootFrame.typeManager.save();

        boxTableSave.close(0);
        portTableSave.close(0);
        arcTableSave.close(0);
        metadataTableSave.close(0);

        boxPaletteTableSave.close(0);
        arcPaletteTableSave.close(0);
        portPaletteTableSave.close(0);
    }


    public static void saveModel(Model model)
        throws Throwable
    {
        // super box handling
        BoxComponent superBoxComponent = (BoxComponent) model.getSuperBox().getComponent();

        Rectangle r = superBoxComponent.getBounds();
        String label = model.getSuperBox().getLabel();

        if(Constants.VERBOSE && model.getSuperBox().isRootBox()) {
            System.out.println("\nSaving Catalog");
            System.out.println("===============");
        }

        // handling contained boxes
        for (int i = 0; i < model.getBoxNodes().size(); i++) {
            Object node = model.getBoxNodes().elementAt(i);

            if(node instanceof BoxNode) {
                BoxNode box = (BoxNode) node;
                box.saveRecord(boxTableSave);
                if(Constants.VERBOSE) System.out.println(box);
		/*
                if(node instanceof PrimitiveBoxNode) {
                    PrimitiveBoxNode primitiveBox = (PrimitiveBoxNode) node;
                    primitiveBox.savePortRecords(portTableSave, model);
		    }
		*/
		box.savePortRecords(portTableSave, model);
	    }
        }
        for(int i=0; i<model.getNumberOfInputPorts(); i++) {
            InputPortNode port = (InputPortNode)model.getInputPorts().elementAt(i);
            if(Constants.VERBOSE) System.out.println(port);
            port.saveRecord(boxTableSave);
            port.saveWGData(currentDirectory);
        }
        for(int i=0; i<model.getNumberOfOutputPorts(); i++) {
            OutputPortNode port = (OutputPortNode)model.getOutputPorts().elementAt(i);
            if(Constants.VERBOSE) System.out.println(port);
            port.saveRecord(boxTableSave);
        }
        for (int i = 0; i < model.getArcsVector().size(); i++) {
            Object arc = model.getArcsVector().elementAt(i);
            if (arc instanceof GenericArc) {
                GenericArc gArc = (GenericArc)arc;
                gArc.saveRecord(arcTableSave);
                if(Constants.VERBOSE) System.out.println(gArc);
            }
        }
    }
}
