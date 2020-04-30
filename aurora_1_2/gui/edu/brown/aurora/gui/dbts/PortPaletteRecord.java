package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import java.io.*;


public class PortPaletteRecord extends PortRecord implements PaletteRecord
{
    public static String databaseFileName = "PortPaletteTable.db";
    
    protected int rootBoxId;

    public PortPaletteRecord()
    {
        super(); // tell Db to allocate on retrieval
    }
    
    public PortPaletteRecord(int boxId, int portIndex, 
                             int typeId, int portType, int rootBoxId)
    {
        this.boxId = boxId;
        this.portIndex = portIndex;
        this.typeId = typeId;
        this.portType = portType;
        this.rootBoxId = rootBoxId;
    }

    public Object clone()
    {
        return new PortPaletteRecord(boxId, portIndex, typeId, portType, rootBoxId);
    }
    
    public int getId()
    {
        return boxId;
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

    public int getParentId()
    {
        return boxId;
    }
}
