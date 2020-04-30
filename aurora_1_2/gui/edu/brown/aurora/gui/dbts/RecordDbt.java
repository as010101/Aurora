package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;

public class RecordDbt extends Dbt
{
    public RecordDbt()
    {
        set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
    }
    
    public static byte[] toByteArray(String s, int length)
    {
        byte[] buffer = new byte[length];
        for (int i = 0; i < buffer.length; i++) {
            buffer[i] = '\0';
        }
        byte[] temp = s.getBytes();
        int l = 0;
        if (temp != null) {
            l = temp.length;
        }
        if (l > buffer.length) {
            l = buffer.length;
        }
        for (int i = 0; i < l; i++) {
            buffer[i] = temp[i];
        }
        return buffer;
    }
}
