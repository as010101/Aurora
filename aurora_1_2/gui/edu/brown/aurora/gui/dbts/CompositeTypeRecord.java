package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import java.io.*;
import edu.brown.aurora.gui.*;

public class CompositeTypeRecord extends RecordDbt
{
    static public String databaseFileName = "CompositeType.db";
    
    protected int typeId;
    protected String typeName;
    protected int numberOfFields;
    protected boolean isInferred;
    
    public CompositeTypeRecord()
    {
        super(); // tell Db to allocate on retrieval
    }
    
    public CompositeTypeRecord(int typeId, String typeName, 
			       int numberOfFields, boolean isInferred)
    {
        super();
        this.typeId = typeId;
        this.typeName = typeName;
        this.numberOfFields = numberOfFields;
	this.isInferred = isInferred;
        setSchemaRecord();

    }
    
    public void setSchemaRecord()
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
        if(Constants.VERBOSE) System.out.println("Saving " + toString());
        outStream.writeInt(typeId);
        outStream.writeUTF(typeName);
        outStream.writeInt(numberOfFields);
	outStream.writeInt(isInferred?1:0);
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
        typeName = inputStream.readUTF();
        numberOfFields = inputStream.readInt();
	isInferred = inputStream.readInt() == 1;
	if(Constants.VERBOSE) System.out.println("Loading " + toString());
    }

    public String toString()
    {
        return 
            "CompositeType: " + 
            "id=" + typeId +
            ", Name=" + typeName + 
            ", #fields=" + numberOfFields +
	    ", isInferred=" + isInferred;
    }
    
    public int getTypeId()
    {
        return typeId;
    }
    
    public String getTypeName()
    {
        return typeName;
    }
    
    public int getNumberOfFields()
    {
        return numberOfFields;
    }

    public boolean getIsInferred()
    {
	return isInferred;
    }
}
