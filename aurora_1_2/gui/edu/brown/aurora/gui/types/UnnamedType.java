/*
 * @(#)UnnamedType.java    1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

package edu.brown.aurora.gui.types;

import java.util.*;
import javax.swing.*;
import java.io.Serializable;

/**
 * An unnamed type represents a composite type consisting of a number of attributes, each of 
 * which contains a type and length. 
 *
 * @author Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class UnnamedType extends Type
{
    protected Vector attributes;
    final static String UNNAMEDTYPENAME = "Inferred Type";
    final static String UNNAMEDATTRNAME = "";

    /**
     * Constructs a new UnnamedType.
     * @param name the name of the UnnamedType
     */
    public UnnamedType()
    {
        super(UNNAMEDTYPENAME, false);
	attributes = new Vector();
    }

    /**
     * Returns a String object representing this UnnamedType's name and attributes.
     * @return a string representation of this UnnamedType's name and attributes
     */
    public String toString()
    {
        String s = super.toString() + " = {";
	for(int i=0; i<attributes.size(); i++) {
	    Attribute attr = (Attribute)attributes.elementAt(i);
	    if(i == 0) {
		s += attr.toString();
	    } else {
		s += ", " + attr.toString();
	    }
	}
	return s;
    }

    
    /**
     * Adds an attribute to this Type.
     * @param type the type of the new attribute
     * @param length the length of the new attribute
     */
    public  void addAttribute(Type type, int length)
    {
        attributes.addElement(new Attribute(UNNAMEDATTRNAME, type, length));
    }

    /**
     * Finds the index of the attribute having the specified name.
     * @param name the name of the attribut
     * @return the index of the Attribute object having the specified name
     */
    public void clearAttributes()
    {
	attributes.removeAllElements();
    }

    /**
     * Clones this unnamed type object.
     */
    public Object clone()
    {
	UnnamedType newType = new UnnamedType();
	for(int i=0; i<attributes.size(); i++) {
	    Attribute attr = (Attribute)attributes.elementAt(i);
	    newType.addAttribute(attr.getType(),
				 attr.getLength());
	}
	return newType;
    }

    public int size()
    {
	return attributes.size();
    }

    public Attribute getAttribute(int index)
    {
	return (Attribute)attributes.elementAt(index);
    }


    public boolean isCompatible(Type type)
    {
	if(type == null) {
	    return false;
	}
	if(type instanceof UnnamedType) {
	    UnnamedType other = (UnnamedType) type;
	    if(other.size() != size()) {
		return false;
	    }
	    for(int i=0; i<size(); i++) {
		Attribute otherAttr = other.getAttribute(i);
		Attribute thisAttr = getAttribute(i);

		if(!otherAttr.isCompatible(thisAttr)) {
		    return false;
		}
	    }
	    return true;	    
	}
	if(type instanceof CompositeType) {
	    CompositeType other = (CompositeType) type;
	    if(other.size() != size()) {
		return false;
	    }
	    for(int i=0; i<size(); i++) {
		Attribute otherAttr = other.getAttribute(i);
		Attribute thisAttr = getAttribute(i);

		if(!otherAttr.isCompatible(thisAttr)) {
		    return false;
		}
	    }
	    return true;
	}
	if(type instanceof PrimitiveType) {
	    PrimitiveType other = (PrimitiveType) type;
	    if(other.size() != size()) {
		return false;
	    }

	    Attribute otherAttr = other.getAttribute();
	    Attribute thisAttr = getAttribute(0);
	    return otherAttr.isCompatible(thisAttr);
	}

	return false;
    }
}
