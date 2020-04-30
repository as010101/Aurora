/*
 * @(#)TypeManager.java 1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved. 
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import edu.brown.aurora.gui.*;


import java.util.*;
import javax.swing.tree.*;
import javax.swing.*;
import edu.brown.aurora.gui.dbts.*;
import com.sleepycat.db.*;
import java.io.*;

/**
 * A TypeManager object has a number of pre-defined primitive types and enables 
 * creation, update and deletion of composite types. The TypeManager class extends
 * DefaultTreeModel which uses TreeNodes.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class TypeManager
{
    public static int UNDEFINED_TYPE = 0;
    public static int INTEGER = -1;
    public static int FLOAT = -2;
    public static int DOUBLE = -3;
    public static int STRING = -4;
    public static int BOOL = -5;
    public static int TIMESTAMP = -6;

    public static String INTEGER_NAME = "integer";
    public static String FLOAT_NAME = "float";
    public static String DOUBLE_NAME = "double";
    public static String STRING_NAME = "string";
    public static String BOOL_NAME = "bool";
    public static String TIMESTAMP_NAME = "timestamp";

    public static int VARIABLE_SIZE = -1;

    public static int INTEGER_SIZE = 4;
    public static int FLOAT_SIZE = 4;
    public static int DOUBLE_SIZE = 8;
    public static int STRING_SIZE = VARIABLE_SIZE;
    public static int BOOL_SIZE = 1;
    public static int TIMESTAMP_SIZE = 8;

    public static final PrimitiveType INTEGER_TYPE = new PrimitiveType(INTEGER, 
								       INTEGER_NAME, 
								       INTEGER_SIZE);
    public static final PrimitiveType FLOAT_TYPE = new PrimitiveType(FLOAT, 
								     FLOAT_NAME, 
								     FLOAT_SIZE);
    public static final PrimitiveType DOUBLE_TYPE = new PrimitiveType(DOUBLE, 
								      DOUBLE_NAME, 
								      DOUBLE_SIZE);
    public static final PrimitiveType STRING_TYPE = new PrimitiveType(STRING, 
								      STRING_NAME, 
								      STRING_SIZE);
    public static final PrimitiveType BOOL_TYPE = new PrimitiveType(BOOL, 
								    BOOL_NAME, 
								    BOOL_SIZE);
    public static final PrimitiveType TIMESTAMP_TYPE = new PrimitiveType(TIMESTAMP, 
									 TIMESTAMP_NAME, 
									 TIMESTAMP_SIZE);

    private Vector primitiveTypes = new Vector();

    private Vector allTypes;
    private DefaultListModel listModel;


    public static int[] VARIABLE_TYPES = { STRING };

    public static boolean isNumber(Type otherType)
    {
	if(otherType == null) {
	    return false;
	}
 
	return (otherType.isCompatible(INTEGER_TYPE) ||
		otherType.isCompatible(FLOAT_TYPE) ||
		otherType.isCompatible(DOUBLE_TYPE));	
    }


    /**
     * Constructs a new TypeManager object. This constructor creates the root named 'Types' and 
     * instantiates a number of primitive types.
     */
    public TypeManager()
    {
	listModel = new DefaultListModel();
	allTypes = new Vector();
        setupPrimitiveTypes();
    }
    
    public DefaultListModel getTypeList()
    {
	return listModel;
    }
    
    /**
     * Returns the CompositeType having the specified name. 
     * @param name the name of the Compositype to search
     */
    public final CompositeType findCompositeType(String name)
    {
	for(int i=0; i<allTypes.size(); i++) {
	    if(allTypes.elementAt(i) instanceof CompositeType) {
		CompositeType ctype = (CompositeType) allTypes.elementAt(i);
		if(ctype.getName().equals(name)) {
		    return ctype;
		}
	    }
	}
	return null;
    }
    
    /**
     * Returns the index of the specified CompositeType. 
     * @return the index of the specified CompositeType
     * @param name the CompositeType
     */
    public final int findCompositeTypeIndex(CompositeType type)
    {
	return allTypes.indexOf(type);
    }

    public CompositeType findCompositeType(int index)
    {
	if(index >= allTypes.size()) {
	    return null;
	}
	return (CompositeType) allTypes.elementAt(index);
    }

    /**
     * Adds a CompositeType to this TypeManager. 
     * @param type the type to add.
     * @exception TypeException if the given name is already being used.
     */
    public final void addCompositeType(CompositeType type) throws TypeException
    {
	if(!type.getIsInferred()) {
	    if (findCompositeType(type.getName()) != null) {
		throw new TypeException(TypeException.DUPLICATE_TYPES);
	    }
	    listModel.addElement(type);
	}
	allTypes.addElement(type);
    }

    /**
     * Removes the specified node. 
     * @param target the node to remove.
     */
    public final void removeType(CompositeType type, int index)
    {
	if(!type.getIsInferred()) {
	    listModel.removeElementAt(index);
	    //System.out.println("in TypeManager.removeType, index = " + index);
	}
	
	index = -1;
	for (int i = 0; i < allTypes.size(); i++) {
	    CompositeType otherType = (CompositeType)allTypes.get(i);
	    if ((otherType.getName()).equals(type.getName())) {
		index = i;
	    }
	}
	//System.out.println("in TypeManager.removeType, vector index = " + index);
	
	if (index != -1) {
	    allTypes.removeElementAt(index);
	}
    }
    
    /**
     * Finds the Primitive Type having the specified name. 
     * @param name the name of the PrimitiveType.
     */
    public final PrimitiveType findPrimitiveType(String name)
    {
        for (int i = 0; i < primitiveTypes.size(); i++) {
            PrimitiveType a = (PrimitiveType)(primitiveTypes.elementAt(i));
            if (a.getName().equals(name))
                return a;
        }
        return null;
    }


    public final int findPrimitiveTypeLength(String name)
    {
        for (int i = 0; i < primitiveTypes.size(); i++) {
            PrimitiveType a = (PrimitiveType)(primitiveTypes.elementAt(i));
            if (a.getName().equals(name))
                return a.getLength();
        }
        return -1;
    }


    public final int findPrimitiveTypeIndex(PrimitiveType type)
    {
        for (int i = 0; i < primitiveTypes.size(); i++) {
            if (primitiveTypes.elementAt(i).equals(type))
                return i;
        }
        return -1;
    }

    public String toString()
    {
        String s = "";
        for (int i = 0; i < primitiveTypes.size(); i++)
            s += (PrimitiveType)(primitiveTypes.elementAt(i)) + "\r\n";
        s += "\r\n";
        CompositeType[] types = getCompositeTypes();
        if (types != null)
        {
            for (int i = 0; i < types.length; i++)
                s += types[i].toStringInDetail() + "\r\n";
            s += "\r\n";
        }

        return s;
    }

    /**
     * Finds the Primitive Type having the specified index. 
     * @param index the index of the PrimitiveType.
     */
    public final PrimitiveType findPrimitiveType(int index)
    {
        return (PrimitiveType)(primitiveTypes.elementAt(-index - 1));
    }
    
    /**
     * Returns the names of the primitive types. 
     * @return a string array representing the names of the primitive types.
     */
    public final String[] getPrimitiveTypeNames()
    {
        String[] names = null;
        
        if (primitiveTypes.size() > 0)
        {
            int index = 0;
            names = new String[primitiveTypes.size()];
            Iterator i = primitiveTypes.iterator();
            while (i.hasNext())
                names[index++] = ((PrimitiveType)(i.next())).getName();
        }
        return names;
    }

    /**
     * Returns the composite types that are defined (not inferred). 
     * @return a CompositeType array representing the composite types.
     */
    public final CompositeType[] getCompositeTypes()
    {
        CompositeType[] names = new CompositeType[listModel.size()];
	for(int i=0; i<names.length; i++) {
	    names[i] = (CompositeType) listModel.elementAt(i);
	}
	return names;
    }

    public final CompositeType[] getAllCompositeTypes()
    {
        CompositeType[] names = new CompositeType[allTypes.size()];
	for(int i=0; i<names.length; i++) {
	    names[i] = (CompositeType) allTypes.elementAt(i);
	}
	return names;
    }

    /**
     * Remove all the composite types.
     */
    public final void removeAllCompositeTypes()
    {
        listModel.removeAllElements();
	allTypes.removeAllElements();
    }
    
    private void setupPrimitiveTypes()
    {
        // Addition is order sensitive!
        primitiveTypes.add(INTEGER_TYPE);
        primitiveTypes.add(FLOAT_TYPE);
        primitiveTypes.add(DOUBLE_TYPE);
        primitiveTypes.add(STRING_TYPE);
        primitiveTypes.add(BOOL_TYPE);
        primitiveTypes.add(TIMESTAMP_TYPE);
    }
    
    /**
     * Returns the first primitive type. 
     * @return the first primitive type.
     */
    public PrimitiveType getDefaultPrimitiveType()
    {
        try {
            return (PrimitiveType)(primitiveTypes.elementAt(0));
        } catch(Exception e) {
            return null;
        }
    }

    public String getNewTypeName(String basename)
    {
	
	int counter = 1;
	String name = basename+(counter++);
	while(findCompositeType(name) != null) {
	    name = basename+(counter++);
	}
	return name;
    }

    public CompositeType getInferredType(CompositeType type)
    {
	for(int i=0; i<allTypes.size(); i++) {
	    CompositeType newType = (CompositeType)allTypes.elementAt(i);
	    if(newType.getIsInferred() && type.equals(newType)) {
		return newType;
	    }
	}
	return null;
    }


    private void addCompositeTypes()
    {
        try
        {
            CompositeType t = new CompositeType("Car", false);
//            t.addAttribute("plate_number", findPrimitiveType("string"), 10);
            t.addAttribute("color", findPrimitiveType("int"), findPrimitiveTypeLength("int"));
            t.addAttribute("weight", findPrimitiveType("int"), findPrimitiveTypeLength("int"));
//            t.addAttribute("etc", findPrimitiveType("binary"), 100);
            addCompositeType(t);

            t = new CompositeType("Person", false);
//            t.addAttribute("name", findPrimitiveType("string"), 20);
            t.addAttribute("ssn", findPrimitiveType("int"), findPrimitiveTypeLength("int"));
            t.addAttribute("height", findPrimitiveType("int"), findPrimitiveTypeLength("int"));
            t.addAttribute("weight", findPrimitiveType("int"), findPrimitiveTypeLength("int"));
            addCompositeType(t);

            t = new CompositeType("Location", false);
//            t.addAttribute("name", findPrimitiveType("string"), 20);
            t.addAttribute("latitude", findPrimitiveType("float"), findPrimitiveTypeLength("int"));
            t.addAttribute("longitude", findPrimitiveType("float"), findPrimitiveTypeLength("int"));
            addCompositeType(t);

            t = new CompositeType("Direction", false);
            t.addAttribute("angle", findPrimitiveType("float"), findPrimitiveTypeLength("int"));
            addCompositeType(t);

            t = new CompositeType("Book", false);
//            t.addAttribute("name", findPrimitiveType("string"), 20);
            t.addAttribute("owner_ssn", findPrimitiveType("int"), findPrimitiveTypeLength("int"));
            addCompositeType(t);

        } catch(Exception e){};
    }
    
    /**
     * Loads from the Database. 
     */
    public void load() throws Throwable
    {
        // addCompositeTypes();

        Db typeRecordTable = new Db(null, 0);
        typeRecordTable.set_error_stream(System.err);
        typeRecordTable.set_errpfx("Catalog Retrieval Error");
        typeRecordTable.open(CatalogManager.getCurrentDirectory()+
                             File.separator+
                             CompositeTypeRecord.databaseFileName, 
                             null, Db.DB_BTREE, Db.DB_CREATE, 0644);
        
        Db typeFieldRecordTable = new Db(null, 0);
        typeFieldRecordTable.set_error_stream(System.err);
        typeFieldRecordTable.set_errpfx("Catalog Retrieval Error");
        typeFieldRecordTable.open(CatalogManager.getCurrentDirectory()+
                                  File.separator+
                                  TypeFieldRecord.databaseFileName, 
                                  null, Db.DB_BTREE, Db.DB_CREATE, 0644);
        
        // Acquire an iterator for the table.
        Dbc outerIterator;
        outerIterator = typeRecordTable.cursor(null, 0);
        
        Dbc innerIterator;
        innerIterator = typeFieldRecordTable.cursor(null, 0);
        
        IntegerDbt outerKey = new IntegerDbt();
        CompositeTypeRecord typeRecord = new CompositeTypeRecord();
        
        while (outerIterator.get(outerKey, typeRecord, Db.DB_NEXT) == 0) {
            typeRecord.parse();
            if (Constants.VERBOSE) System.out.println(typeRecord);
	    // if(!typeRecord.getIsInferred()) {
		CompositeType t = new CompositeType(typeRecord.getTypeName(), 
						    typeRecord.getIsInferred());
		
		IntegerArrayDbt innerKey = new IntegerArrayDbt(new int[] {outerKey.getInteger(), 0});
		TypeFieldRecord typeFieldRecord = new TypeFieldRecord();
		
		if (innerIterator.get(innerKey, typeFieldRecord, Db.DB_SET_RANGE) == 0) {
		    int[] indices = innerKey.getIntegerArray();
		    if (indices[0] == outerKey.getInteger()) {
			typeFieldRecord.parse();
			if (Constants.VERBOSE) System.out.println(typeFieldRecord);
			t.addAttribute(typeFieldRecord.getFieldName(), 
				       findPrimitiveType(typeFieldRecord.getFieldType()),
				       typeFieldRecord.getSize());
			
			while (innerIterator.get(innerKey, typeFieldRecord, Db.DB_NEXT) == 0) {
			    indices = innerKey.getIntegerArray();
			    if (indices[0] != outerKey.getInteger()) break;
			    typeFieldRecord.parse();
			    if (Constants.VERBOSE) System.out.println(typeFieldRecord);
			    t.addAttribute(typeFieldRecord.getFieldName(), 
					   findPrimitiveType(typeFieldRecord.getFieldType()),
					   typeFieldRecord.getSize());
			}
		    }
		}
		addCompositeType(t);
		//}
        }

        innerIterator.close();
        outerIterator.close();
        typeRecordTable.close(0);
        typeFieldRecordTable.close(0);
    }

    /**
     * Saves to the Database. 
     */
    public void save() throws Throwable
    {
        String typeFilename = 
            CatalogManager.getCurrentDirectory()+
            File.separator+
            CompositeTypeRecord.databaseFileName;

        String fieldFilename =
            CatalogManager.getCurrentDirectory()+
            File.separator+
            TypeFieldRecord.databaseFileName;

        new File(typeFilename).delete();
        new File(fieldFilename).delete();

        Db typeRecordTable = new Db(null, 0);
        typeRecordTable.set_error_stream(System.err);
        typeRecordTable.set_errpfx("Catalog Save Error");
        typeRecordTable.open(typeFilename, null, Db.DB_BTREE, Db.DB_CREATE, 0644);

        Db typeFieldRecordTable = new Db(null, 0);
        typeFieldRecordTable.set_error_stream(System.err);
        typeFieldRecordTable.set_errpfx("Catalog Save Error");
        typeFieldRecordTable.open(fieldFilename, null, Db.DB_BTREE, Db.DB_CREATE, 0644);

        CompositeType[] compositeTypes = getAllCompositeTypes();

        if (compositeTypes != null) {
            for (int i = 0; i < compositeTypes.length; i++) {
                CompositeType type = compositeTypes[i];
                IntegerDbt key = new IntegerDbt(type.getId());
                CompositeTypeRecord typeRecord = new CompositeTypeRecord(type.getId(), 
                                                                         type.getName(), 
                                                                         type.getNumberOfAttributes(),
									 type.getIsInferred());
                if (typeRecordTable.put(null, key, typeRecord, 0) == Db.DB_KEYEXIST) 
                    System.out.println("The key already exists.");
                int offset = 0;
                
                for (int j = 0; j < type.getNumberOfAttributes(); j++) {
                    Type t = type.getAttributeType(j);
                    if (t.isPrimitive()) {
                        int size = type.getAttributeLength(j);
                        IntegerArrayDbt innerKey = new IntegerArrayDbt(new int[] {type.getId(), j});
                        
                        int fieldType = 0;

                        PrimitiveType primitiveType = (PrimitiveType) t;
                        fieldType = primitiveType.getTypeId();

                        TypeFieldRecord typeFieldRecord = new TypeFieldRecord(type.getId(), 
                                                                              j, 
                                                                              type.getAttributeName(j), 
                                                                              fieldType,
                                                                              offset,
                                                                              size);
                        offset += size;
                        if (typeFieldRecordTable.put(null, innerKey, typeFieldRecord, 0) == Db.DB_KEYEXIST) 
                            System.out.println("The key already exists.");
                    }
                }
            }
            if (Constants.VERBOSE) System.out.println("" + compositeTypes.length + 
						      " types are recorded.");
        }   
        typeRecordTable.close(0);
        typeFieldRecordTable.close(0);
    }
}
