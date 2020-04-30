package edu.brown.aurora.gui.dbts;

import com.sleepycat.db.*;
import edu.brown.aurora.gui.types.*;

public class CatalogListRecordDbt extends RecordDbt
{
	static public String databaseFileName = "CatalogList.db";

	protected String name; // 20

	public CatalogListRecordDbt()
	{
		super(); // tell Db to allocate on retrieval
	}

	public CatalogListRecordDbt(String name)
	{
		super();
		setCatalogListRecord(name);
	}

	public void setCatalogListRecord(String name)
	{
		try
		{
			java.io.ByteArrayOutputStream byteStream = new java.io.ByteArrayOutputStream();
			java.io.DataOutputStream outStream = new java.io.DataOutputStream(byteStream);
			outStream.write(toByteArray(name, 20), 0, 20);
			byte[] data = byteStream.toByteArray();
			set_size(data.length);
			set_data(data);
		} catch(Exception e)
		{
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
		try
		{
			java.io.DataInputStream  inputStream = new java.io.DataInputStream (new java.io.ByteArrayInputStream(data));
			byte[] buffer = new byte[20];
			inputStream.read(buffer);
			name = new String(buffer);
			name = name.trim();
		} catch(Exception e)
		{
			System.out.println(e);
		}
	}

	public String toString()
	{
		return "CatalogList: "+name;
	}
		
	public String getName()
	{
		return name;
	}
}
