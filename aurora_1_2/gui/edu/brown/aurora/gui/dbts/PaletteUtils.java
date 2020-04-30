package edu.brown.aurora.gui.dbts;

import edu.brown.aurora.gui.*;
import edu.brown.aurora.gui.types.*;

import java.util.*;

import com.sleepycat.db.*;

/**
 * A class of static methods for utility methods interacting with the database and
 * the records.
 */
public class PaletteUtils
{

    public static void loadPalette(PalettePanel palette) throws Throwable
    {
        Vector paletteRecords = new Vector();
        
        Db boxPaletteTable = CatalogManager.openDb(BoxPaletteRecord.databaseFileName, false);
        Db arcPaletteTable = CatalogManager.openDb(ArcPaletteRecord.databaseFileName, false);
        Db portPaletteTable = CatalogManager.openDb(PortPaletteRecord.databaseFileName, false);
        
        IntegerDbt key = new IntegerDbt();        
        
        Dbc iterator = boxPaletteTable.cursor(null, 0);
        BoxPaletteRecord boxPaletteRecord = new BoxPaletteRecord();
        while(iterator.get(key, boxPaletteRecord, Db.DB_NEXT) == 0) {
            boxPaletteRecord.parse();
            if (Constants.VERBOSE) System.out.println("Loading palette box: " + 
                                                      boxPaletteRecord.toString());
            paletteRecords.addElement(boxPaletteRecord.clone());
        }
        
        iterator = arcPaletteTable.cursor(null, 0);
        ArcPaletteRecord arcPaletteRecord = new ArcPaletteRecord();
        while(iterator.get(key, arcPaletteRecord, Db.DB_NEXT) == 0) {
            arcPaletteRecord.parse();
            if (Constants.VERBOSE) System.out.println("Loading palette arc: " + 
                                                      arcPaletteRecord.toString());
            paletteRecords.addElement(arcPaletteRecord.clone());
        }
        
        iterator = portPaletteTable.cursor(null, 0);
        PortPaletteRecord portPaletteRecord = new PortPaletteRecord();
        IntegerArrayDbt portKey = new IntegerArrayDbt(new int[] {0, 0, 0});
        while(iterator.get(portKey, portPaletteRecord, Db.DB_NEXT) == 0) {
            portPaletteRecord.parse();
            if (Constants.VERBOSE) System.out.println("Loading palette port: " + 
                                                      portPaletteRecord.toString());
            paletteRecords.addElement(portPaletteRecord.clone());
        }
        // Move all port and arc palette records into the box palette records where they belong.
        PaletteUtils.loadPaletteRecords(palette, paletteRecords);
    }


    private static PaletteRecord removeLeafPaletteRecord(Vector paletteRecords)
    {
        // Look for a box whose id is not pointed at by a parentId of any other box.
        for(int i=0; i<paletteRecords.size(); i++) {
            
            PaletteRecord paletteRecord = (PaletteRecord) paletteRecords.elementAt(i);



            // Arcs and ports are all leaves.
            if(paletteRecord instanceof ArcPaletteRecord || 
               paletteRecord instanceof PortPaletteRecord) {
                paletteRecords.removeElementAt(i);

                return paletteRecord;
            }

            boolean isLeafRecord = true;
            int boxId = paletteRecord.getId();
            int rootBoxId = paletteRecord.getRootBoxId();

            for(int j=0; j<paletteRecords.size(); j++) {
                PaletteRecord innerPaletteRecord = (PaletteRecord) paletteRecords.elementAt(j);
                int innerParentId = innerPaletteRecord.getParentId();
                int innerBoxId = innerPaletteRecord.getId();

                if(innerParentId == boxId) {
                    isLeafRecord = false;
                }
            }

            // Did not find a nonleaf from the above iteration
            if(isLeafRecord && boxId != rootBoxId) {
                paletteRecords.removeElementAt(i);
                return paletteRecord;
            }                        
        }
        
        return null;
    }

    private static void loadPaletteRecords(PalettePanel palette, Vector paletteRecords)
    {
        PaletteRecord leaf = removeLeafPaletteRecord(paletteRecords);


        if(leaf == null) {
            for(int i=0; i<paletteRecords.size(); i++) {
                PaletteRecord paletteRecord = (PaletteRecord) paletteRecords.elementAt(i);
                palette.addPaletteRecord(paletteRecord);
            }
            
        } else {
            int leafParentId = leaf.getParentId();

            for(int i=0; i<paletteRecords.size(); i++) {
                PaletteRecord paletteRecord = (PaletteRecord) paletteRecords.elementAt(i);
                int id = paletteRecord.getId();                
                if(paletteRecord instanceof BoxPaletteRecord && id == leafParentId) {
                    BoxPaletteRecord boxRecord = (BoxPaletteRecord) paletteRecord;
                    boxRecord.addPaletteRecord(leaf);
                }
            }
            loadPaletteRecords(palette, paletteRecords);
        }
    }

    public static void savePaletteItems(PaletteRecord record,
                                        Db boxPaletteTableSave,
                                        Db arcPaletteTableSave,
                                        Db portPaletteTableSave) 
        throws Throwable
    {
        record.setRecord();
        int id = record.getId();
        IntegerDbt key = new IntegerDbt(id);
        RecordDbt recordItem = (RecordDbt) record;

        if(Constants.VERBOSE) System.out.println("Saving palette record:  " + record.toString());

        if(record instanceof BoxPaletteRecord) {
            if(boxPaletteTableSave.put(null, key, recordItem, 0) == Db.DB_KEYEXIST) {
                System.out.println("PaletteUtils.savePaletteItems : key already exists.");
            }
            BoxPaletteRecord boxRecord = (BoxPaletteRecord) record;
            Vector paletteRecords = boxRecord.getPaletteRecords();
            for(int i=0; i<paletteRecords.size(); i++) {
                PaletteRecord paletteRecord = (PaletteRecord) paletteRecords.elementAt(i);
                savePaletteItems(paletteRecord, 
                                 boxPaletteTableSave, 
                                 arcPaletteTableSave, 
                                 portPaletteTableSave);
            }
        }
        if(record instanceof ArcPaletteRecord) {
            if(arcPaletteTableSave.put(null, key, recordItem, 0) == Db.DB_KEYEXIST) {
                System.out.println("PaletteUtils.savePaletteItems : key already exists.");
            }
        }

        if(record instanceof PortPaletteRecord) {
            PortPaletteRecord portRecord = (PortPaletteRecord) record;
            IntegerArrayDbt portKey = new IntegerArrayDbt(new int[] {id, 
                                                                     portRecord.getPortIndex(), 
                                                                     portRecord.getPortType()});
            if(portPaletteTableSave.put(null, portKey, recordItem, 0) == Db.DB_KEYEXIST) {
                System.out.println("PaletteUtils.savePaletteItems : key already exists.");
            }
        }        
    }
    
    public static PalettePanel getPalettePanel(Model model)
    {
        Editor editor = Utils.getRootEditor(model);
        Toolboxspace toolbox = editor.getToolboxspace();
        PalettePanel palette = toolbox.getPalettePanel();
        return palette;
    }



}
