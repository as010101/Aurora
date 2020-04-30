// $Id: Attribute.java,v 1.2 2002/08/24 17:32:46 rly Exp $
// Copyright (C) 2002 Brown University.  All Rights Reserved

package edu.brown.aurora.gui.types;
import java.io.*;

/************************************************************************************
 *
 * Attribute represents an attribute to a type.
 *
 * @author  Robin Yan (rly@cs.brown.edu
 *
 * @version $Revision: 1.2 $ @ $Date: 2002/08/24 17:32:46 $
 *
 ************************************************************************************/

public class Attribute implements Serializable
{
    String name;
    Type type;
    int length;
    
    Attribute(String name, Type type, int length)
    {
	this.name = name;
	this.type = type;
	//if (type.isPrimitive() && !(((PrimitiveType)type).isFixedLength()))
	//    this.length = length;
	this.length = length;

	if(type instanceof PrimitiveType) {
	    PrimitiveType primType = (PrimitiveType) type;
	    primType.setLength(length);
	}
    }
    public void setName(String name)
    {
	this.name = name;
    }
    
    public void setType(Type type)
    {
	this.type = type;
    }
    
    public Type getType()
    {
	return type;
    }

    public String getName()
    {
	return name;
    }
    
    public void setLength(int length)
    {
	if(type instanceof PrimitiveType) {
	    this.length = length;
	    PrimitiveType primType = (PrimitiveType) type;
	    primType.setLength(length);
	}

    }
    
    public int getLength()
    {
	return length;
    }
    
    public final boolean equals(Object obj)
    {
	if (obj instanceof String)
	    return (name.equals((String)obj));
	else if (obj instanceof Type)
	    return (name.equals(((Attribute)obj).name));
	else
	    return false;
    }
    
    public final boolean isCompatible(Attribute a)
    {
	return (type.isCompatible(a.type) && (length == a.length));
    }

}
