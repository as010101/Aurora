package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import java.io.*;

public class MetadataRecord extends RecordDbt
{
    public static String databaseFileName = "Metadata.db";

    protected int max_lowerBoxId;
    protected int max_upperBoxId;
    
    public MetadataRecord(int max_lowerBoxId, int max_upperBoxId)
    {
        super();
        setMetadataRecord(max_lowerBoxId, max_upperBoxId);
    }

    public MetadataRecord()
    {
        super(); // tell Db to allocate on retrieval
    }

    
    public void setMetadataRecord(int max_lowerBoxId, int max_upperBoxId)
    {
        try {
            ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
            DataOutputStream outStream = new DataOutputStream(byteStream);
            outStream.writeInt(max_lowerBoxId);
            outStream.writeInt(max_upperBoxId);
            byte[] data = byteStream.toByteArray();
            set_size(data.length);
            set_data(data);
        } catch(Exception e) {
            System.out.println(e);
            set_size(0);
            return;
        }
    }
    
    public void parse()
    {
        byte[] data = get_data();
        int size = get_size();
        if (size == 0) return;
        try {
            DataInputStream  inputStream = new DataInputStream (new ByteArrayInputStream(data));
            max_lowerBoxId = inputStream.readInt();
            max_upperBoxId = inputStream.readInt();
        } catch(Exception e) {
            System.out.println(e);
        }
    }
    
    public String toString()
    {
        return 
            "Metadata: max_lowerBoxId = "+max_lowerBoxId + 
            ", max_upperBoxId = "+max_upperBoxId;
    }
    
    public int getMaxLowerBoxId()
    {
        return max_lowerBoxId;
    }

    public int getMaxUpperBoxId()
    {
        return max_upperBoxId;
    }
}
