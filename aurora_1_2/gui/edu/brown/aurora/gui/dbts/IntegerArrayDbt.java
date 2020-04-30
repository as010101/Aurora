package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;

import java.io.*;

public class IntegerArrayDbt extends Dbt
{
    
    public IntegerArrayDbt()
    {
        set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
    }
    
    public IntegerArrayDbt(int[] value)
    {
        setIntegerArray(value);
        set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
    }
    
    public void setIntegerArray(int[] value)
    {
        if (value == null) {
            set_size(0);
            return;
        }
        try {
            ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
            DataOutputStream outStream = new DataOutputStream(byteStream);
            for (int i = 0; i < value.length; i++)
                outStream.writeInt(value[i]);
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
    
    public int[] getIntegerArray()
    {
        byte[] data = get_data();
        int size = get_size()/4;
        if (size == 0) return null;
        try {
            DataInputStream  inputStream = new DataInputStream (new ByteArrayInputStream(data));
            int[] ints = new int[size];
            for (int i = 0; i < size; i++)
                ints[i] = inputStream.readInt();
            return ints;
        } catch(Exception e) {
            System.out.println(e);
            e.printStackTrace();
            return null;
        }
    }
    
    public int getInteger(int index)
    {
        byte[] data = get_data();
        int size = get_size()/4;
        if (size == 0) return -1;
        try {
            DataInputStream  inputStream = new DataInputStream (new ByteArrayInputStream(data));
            int[] ints = new int[size];
            for (int i = 0; i < size && i <= index; i++)
                ints[i] = inputStream.readInt();
            return ints[index];
        } catch(Exception e) {
            System.out.println(e);
            return -1;
        }
    }
    
    public String toString()
    {
        String str = "";
        int[] ints = getIntegerArray();
        if (ints == null) return "";
        for (int i = 0; i < ints.length; i++)
            if (i == 0)
                str += "IntegerArrayDbt: "+ints[0];
            else 
                str += ", "+ints[i];
        return str;
    }    
}
