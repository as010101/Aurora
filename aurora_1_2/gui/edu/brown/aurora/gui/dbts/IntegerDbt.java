package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;

import java.io.*;

public class IntegerDbt extends Dbt
{
    
    public IntegerDbt()
    {
        set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
    }
    
    public IntegerDbt(int value)
    {
        setInteger(value);
        set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
    }
    
    public void setInteger(int value)
    {
        set_size(4);
        try {
            ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
            DataOutputStream outStream = new DataOutputStream(byteStream);
            outStream.writeInt(value);
            byte[] data = byteStream.toByteArray();
            set_size(data.length);
            set_data(data);
        } catch(Exception e) {
            System.out.println(e);
            return;
        }
    }
    
    public int getInteger()
    {
        byte[] data = get_data();
        try {
            DataInputStream input = new DataInputStream(new ByteArrayInputStream(data));
            return input.readInt();
        } catch(Exception e) {
            System.out.println(e);
            e.printStackTrace();
            return 0;
        }
    }
    
    public String toString()
    {
        return "IntegerDbt: " + getInteger();
    }
}
