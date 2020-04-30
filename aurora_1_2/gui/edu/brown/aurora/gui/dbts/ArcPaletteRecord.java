package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import java.io.*;

public class ArcPaletteRecord extends ArcRecord implements PaletteRecord
{
    public static String databaseFileName = "ArcPaletteTable.db";
    
    protected int rootBoxId;

    public ArcPaletteRecord()
    {
        super(); // tell Db to allocate on retrieval
    }
    
    public ArcPaletteRecord(int id, float rate, int typeId, 
                     int sourceNodeId, int targetNodeId, 
                     int sourcePortIndex, int targetPortIndex, 
                     int cpFlag, int parentId, int rootBoxId)
    {
        this.id = id;
        this.rate = rate;
        this.typeId = typeId;
        this.sourceNodeId = sourceNodeId;
        this.targetNodeId = targetNodeId;
        this.sourcePortIndex = sourcePortIndex;
        this.targetPortIndex = targetPortIndex;
        this.cpFlag = cpFlag;
        this.parentId = parentId;
        this.rootBoxId = rootBoxId;
    }

    public Object clone()
    {
        return new ArcPaletteRecord(id, rate, typeId, sourceNodeId, targetNodeId, 
                                    sourcePortIndex, targetPortIndex, cpFlag, 
                                    parentId, rootBoxId);
    }
    
    protected void writeFields(DataOutputStream outStream) throws Exception
    {
        super.writeFields(outStream);
        outStream.writeInt(rootBoxId);
    }
    
    protected void readFields(DataInputStream inputStream) throws Exception
    {
        super.readFields(inputStream);        
        rootBoxId = inputStream.readInt();
    }

    public int getRootBoxId()
    {
        return rootBoxId;
    }
}
