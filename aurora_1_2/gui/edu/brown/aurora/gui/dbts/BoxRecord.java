package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import java.io.*;

public class BoxRecord extends RecordDbt
{
    public static String databaseFileName = "BoxTable.db";
    
    protected int boxId;
    protected int boxType;
    protected String label;
    protected String description;
    protected String modifier;
    protected int parentId;

    protected float cost;
    protected float selectivity;
    protected boolean useDefinedTypes;
    protected boolean outputDefinedTypes;

    protected int x;
    protected int y;
    protected int width;
    protected int height;
    
    public BoxRecord()
    {
        super(); // tell Db to allocate on retrieval
    }
    
    public BoxRecord(int boxId, int boxType, String label, 
                     String description, String modifier, int parentId,
                     float cost, float selectivity, boolean useDefinedTypes,
		     boolean outputDefinedTypes, int x, int y, int width, int height)        
    {
        super();
        this.boxId = boxId;
        this.boxType = boxType;
        this.label = label;
        this.description = description;
        this.modifier = modifier;
        this.parentId = parentId;
        this.cost = cost;
        this.selectivity = selectivity;
	this.useDefinedTypes = useDefinedTypes;
	this.outputDefinedTypes = outputDefinedTypes;
        this.x = x;
        this.y = y;
        this.width = width;
        this.height = height;
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

    protected void writeFields(DataOutputStream outStream) 
        throws Exception
    {
        outStream.writeInt(boxId);
        outStream.writeInt(boxType);
        outStream.writeUTF(label);
        outStream.writeUTF(description);
        outStream.writeUTF(modifier);
        outStream.writeInt(parentId); 
        outStream.writeFloat(cost);
        outStream.writeFloat(selectivity);        
	outStream.writeInt(useDefinedTypes?1:0);
	outStream.writeInt(outputDefinedTypes?1:0);
        outStream.writeInt(x);
        outStream.writeInt(y);
        outStream.writeInt(width);
        outStream.writeInt(height);
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

    public void setParentId(int parentId)
    {
        this.parentId = parentId;
    }

    protected void readFields(DataInputStream inputStream) throws Exception
    {
        boxId = inputStream.readInt();
        boxType = inputStream.readInt();
        label = inputStream.readUTF();
        description = inputStream.readUTF();
        modifier = inputStream.readUTF();
        parentId = inputStream.readInt();
        cost = inputStream.readFloat();
        selectivity = inputStream.readFloat();
	useDefinedTypes = inputStream.readInt() == 1;
        outputDefinedTypes = inputStream.readInt() == 1;
	x = inputStream.readInt();
        y = inputStream.readInt();
        width = inputStream.readInt();
        height = inputStream.readInt();
    }


    public String toString()
    {
        return 
            "Box: " + 
            " label=" + label +
            " id=" + boxId +
            " type=" + boxType + 
            " parent=" + parentId +
	    " useDefinedTypes=" + useDefinedTypes +
	    " outputDefinedTypes=" + outputDefinedTypes +
            " modifier=" + modifier;
	
    }
    
    public int getBoxId()
    {
        return boxId;
    }
    
    public int getBoxType()
    {
        return boxType;
    }

    public String getLabel()
    {
        return label;
    }

    public String getDescription()
    {
        return description;
    }

    public int getParentId()
    {
        return parentId;
    }

    public String getModifier()
    {
        return modifier;
    }

    public boolean isUseDefinedTypes()
    {
	return useDefinedTypes;
    }

    public boolean isOutputDefinedTypes()
    {
	return outputDefinedTypes;
    }

    
    public java.awt.Rectangle getRectangle()
    {
        return new java.awt.Rectangle(x, y, width, height);
    }
    
}
