package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import java.io.*;
import edu.brown.aurora.gui.*;

public class TypeFieldRecord extends RecordDbt
{
    public static String databaseFileName = "TypeField.db";
    
    protected int typeId;
    protected int fieldIndex;
    protected String fieldName;
    protected int fieldType;
    protected int offset;
    protected int size;
    
    
    public TypeFieldRecord()
    {
        super(); // tell Db to allocate on retrieval
    }
    
    public TypeFieldRecord(int typeId, int fieldIndex, 
                           String fieldName, 
                           int fieldType, int offset, int size)
    {
        super();
        this.typeId = typeId;
        this.fieldIndex = fieldIndex;
        this.fieldName = fieldName;

	if(fieldName == null) {
	    this.fieldName = "";
	}
        this.fieldType = fieldType;
        this.offset = offset;
        this.size = size;
        setTypeFieldRecord();
    }
    
    public void setTypeFieldRecord()        
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
            e.printStackTrace();
            set_size(0);
            return;
        }
    }

    protected void writeFields(DataOutputStream outStream) throws Exception
    {
        if(Constants.VERBOSE)System.out.println("Saving " + toString());
        outStream.writeInt(typeId);
        outStream.writeInt(fieldIndex);	
        outStream.writeUTF(fieldName);
        outStream.writeInt(fieldType);
        outStream.writeInt(size);
        outStream.writeInt(offset);

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
        typeId = inputStream.readInt();
        fieldIndex = inputStream.readInt();
        fieldName = inputStream.readUTF();
        fieldType = inputStream.readInt();
        size = inputStream.readInt();
        offset = inputStream.readInt();
	if(Constants.VERBOSE)	System.out.println("Loading " + toString());
    }
    
    public String toString()
    {
        return "TypeField: " + 
            "id=" + typeId + 
            ", index=" + fieldIndex + 
            ", name=" + fieldName + 
            ", type=" + fieldType + 
            ", offset=" + offset + 
            ", size="+size;
    }
    
    public int getTypeId()
    {
        return typeId;
    }
    
    public int getFieldIndex()
    {
        return fieldIndex;
    }
    
    public int getFieldType()
    {
        return fieldType;
    }
    
    public String getFieldName()
    {
        return fieldName;
    }
    
    public int getOffset()
    {
        return offset;
    }

    public int getSize()
    {
        return size;
    }
}
