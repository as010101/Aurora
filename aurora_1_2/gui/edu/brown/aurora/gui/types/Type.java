/*
 * @(#)Type.java    1.1 06/20/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved. 
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

/**
 * A Type represents a specific set of information which will be used in the 
 * Aurora Network. Types may be PrimitiveTypes or CompositeTypes. Composite
 * Types are collections of Primitive and other Composite Types. Composite Types
 * may not include themselves.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.1 06/20/2002
 */
public abstract class Type implements java.io.Serializable
{
    protected String name;
    protected boolean isPrimitive;

    /**
     * Constructs a new type having the specified name. 
     * @param name the name of the Type.
     */
    public Type(String name)
    {
        this.name = name;
        isPrimitive = false;
    }

    /**
     * Internal constructor for subclasses to set the "isPrimitive" boolean.
     * @param name the name of the Type.
     * @param isPrimitive whether the Type is primitive
     */
    protected Type(String name, boolean isPrimitive)
    {
        this.name = name;
        this.isPrimitive = isPrimitive;
    }

    /**
     * Check to see if the current subclass of Type is a primitive type.
     * @return true if the type is primitive, false otherwise.
     */
    public boolean isPrimitive()
    {
        return isPrimitive;
    }
    
    /**
     * Compares this object to the specified object. 
     * The result is true if and only if the argument is not null and is a String or Type object 
     * having the same name.
     * @param obj the object to compare with.
     * @return true if the objects are the same; false otherwise. 
     */
    public boolean equals(Object obj)
    {
        if (obj instanceof String)
            return name.equals((String)obj);
        else if (obj instanceof Type)
            return name.equals(((Type)obj).getName());
        else 
            return false;
    }

    /**
     * Determines if the given type is compatible with this type. 
     * @return true if the types are compatible; false otherwise. 
     */
    public abstract boolean isCompatible(Type type);

    /**
     * Returns a String object representing this Type's name. 
     * @return a string representation of the name of this type.
     */
    public String toString()
    {
        return name;
    }

    /**
     * Sets the name to the specified name. 
     * @param name the new name.
     */
    public void setName(String name)
    {
        this.name = name;
    }

    /**
     * Returns a String object representing this Type's name. 
     * @return a string representation of the name of this type.
     */
    public String getName()
    {
        return name;
    }

    public abstract int size();

}
