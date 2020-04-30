package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;

import java.io.*;

public class ArcRecord extends RecordDbt
{
    public static String databaseFileName = "ArcTable.db";
    
    protected int id;
    protected float rate;
    protected int typeId;
    protected int sourceNodeId;
    protected int targetNodeId;
    protected int sourcePortIndex;
    protected int targetPortIndex;
    protected int cpFlag;
    protected int parentId;
    
    public ArcRecord()
    {
        super(); // tell Db to allocate on retrieval
    }
    
    public ArcRecord(int id, float rate, int typeId, 
                     int sourceNodeId, int targetNodeId, 
                     int sourcePortIndex, int targetPortIndex, 
                     int cpFlag, int parentId)
    {
        super();
        this.id = id;
        this.rate = rate;
        this.typeId = typeId;
        this.sourceNodeId = sourceNodeId;
        this.targetNodeId = targetNodeId;
        this.sourcePortIndex = sourcePortIndex;
        this.targetPortIndex = targetPortIndex;
        this.cpFlag = cpFlag;
        this.parentId = parentId;
        setRecord();
    }
    
    public void setRecord()
    {
        try {
            ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
            DataOutputStream outStream = new DataOutputStream(byteStream);

            writeFields(outStream);

            byte[] data = byteStream.toByteArray();
            set_size(data.length);
            set_data(data);
        } catch(Exception e) {
            System.out.println(e);
            set_size(0);
	    e.printStackTrace();
            return;
        }
    }

    protected void writeFields(DataOutputStream outStream) throws Exception
    {
        outStream.writeInt(id);
        outStream.writeFloat(rate);
        outStream.writeInt(typeId);
        outStream.writeInt(sourceNodeId);
        outStream.writeInt(targetNodeId);
        outStream.writeInt(sourcePortIndex);
        outStream.writeInt(targetPortIndex);
        outStream.writeInt(cpFlag);
        outStream.writeInt(parentId);
	// System.out.println("Saving: " + toString());
    }
    
    public void parse()
    {
        byte[] data = get_data();
        int size = get_size();
        if (size == 0) return;
        try {
            DataInputStream  inputStream = new DataInputStream (new ByteArrayInputStream(data));

             readFields(inputStream);
        } catch(Exception e) {
            System.out.println(e);
	    e.printStackTrace();
        }
    }

    protected void readFields(DataInputStream inputStream) throws Exception
    {
        id = inputStream.readInt();
        rate = inputStream.readFloat();
        typeId = inputStream.readInt();
        sourceNodeId = inputStream.readInt();
        targetNodeId = inputStream.readInt();
        sourcePortIndex = inputStream.readInt();
        targetPortIndex = inputStream.readInt();
        cpFlag = inputStream.readInt();
        parentId = inputStream.readInt();
	// System.out.println("Loading: " + toString());
    }

    
    public String toString()
    {
        return 
            "Arc: ["+sourceNodeId + ", " +sourcePortIndex + "] --> [" +
            targetNodeId + ", " + targetPortIndex + "] parentId = " + parentId +
	    ", typeId = " + typeId;
    }
    
    public int getId()
    {
        return id;
    }
    
    public float getRate()
    {
        return rate;
    }
    
    public int getTypeId()
    {
        return typeId;
    }
    
    public int getSourceNodeId()
    {
        return sourceNodeId;
    }
    
    public void setSourceNodeId(int boxId)
    {
        sourceNodeId = boxId;
    }

    public int getTargetNodeId()
    {
        return targetNodeId;
    }

    public void setTargetNodeId(int boxId)
    {
        targetNodeId = boxId;
    }
    
    public int getSourcePortIndex()
    {
        return sourcePortIndex;
    }
     
    public int getTargetPortIndex()
    {
        return targetPortIndex;
    }
    
    public int getCpFlag()
    {
        return cpFlag;
    }

    public int getParentId()
    {
        return parentId;
    }

    public void setParentId(int parentId)
    {
        this.parentId = parentId;
    }
}
