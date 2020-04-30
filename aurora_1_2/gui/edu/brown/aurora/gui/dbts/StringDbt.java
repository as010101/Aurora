package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;

public class StringDbt extends Dbt
{
    public StringDbt()
    {
        set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
    }
    
    public StringDbt(String value)
    {
        setString(value);
        set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
    }
    
    public void setString(String value)
    {
        set_data(value.getBytes());
        set_size(value.length());
    }
    
    public String getString()
    {
        return new String(get_data(), 0, get_size());
    }
    
    public String toString()
    {
        return "StringDbt: "+getString();
    }
}
