package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import java.io.*;


public class PortRecord extends RecordDbt
{
    public static String databaseFileName = "PortTable.db";
    
    protected int boxId;
    protected int portIndex;
    protected int typeId;
    protected int portType; // (0 == input, 1 == output), see Constants.INPUTPORTTYPE

    public PortRecord()
    {
        super(); // tell Db to allocate on retrieval
    }
    
    public PortRecord(int boxId, int portIndex, int typeId, int portType)
    {
        super();
        this.boxId = boxId;
        this.portIndex = portIndex;
        this.typeId = typeId;
        this.portType = portType;

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
            return;
        }
    }

    protected void writeFields(DataOutputStream outStream) throws Exception
    {
        outStream.writeInt(boxId);
        outStream.writeInt(portIndex);
        outStream.writeInt(typeId);
        outStream.writeInt(portType);
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
            e.printStackTrace();
            System.out.println(e);
        }
    }

    protected void readFields(DataInputStream inputStream) throws Exception
    {
        boxId = inputStream.readInt();
        portIndex = inputStream.readInt();
        typeId = inputStream.readInt();
        portType = inputStream.readInt();        
    }
    
    public String toString()
    {
        return "\tPort: "+
            " id="+boxId+
            " portIndex="+portIndex+
            " typeId="+typeId+
            " portType="+portType;
    }
    
    public int getPortIndex()
    {
        return portIndex;
    }

    public int getBoxId()
    {
        return boxId;
    }
    
    public int getTypeId()
    {
        return typeId;
    }
    
    public int getPortType()
    {
        return portType;
    }
}
