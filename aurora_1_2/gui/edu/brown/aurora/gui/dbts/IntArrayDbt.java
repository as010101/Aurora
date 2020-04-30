package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;

public class IntArrayDbt extends Dbt
{
	int size;

	IntArrayDbt(int size)
	{
		set_flags(Db.DB_DBT_MALLOC); // tell Db to allocate on retrieval
		this.size = size;
	}

/*	void setString(String value)
	{
		set_size(value.length());
		set_data(value.getBytes());
	}

	String getString()
	{
		return new String(get_data(), 0, get_size());
	}
*/
}
