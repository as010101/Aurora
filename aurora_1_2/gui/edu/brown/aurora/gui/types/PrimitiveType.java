
/*
 * @(#)PrimitiveType.java    1.0 06/26/2002
 *
 * Copyright 2002 Adam Singer (arsinger@cs.brown.edu). All Rights Reserved. 
 * Box 0398, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import edu.brown.aurora.gui.RootFrame;

/**
 * A PrimitiveType represents a primitive type such as int, float, string, etc.
 * If the type has a specific length, the length is determined in the type's 
 * instantiation time and it can not be changed in the type's life span.
 * A PrimitiveType is compatible with only itself. 
 *
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 06/26/2002
 */

public class PrimitiveType extends Type
{
    protected int typeId;
    protected int length;
    protected boolean isFixedLength;

    /**
     * Constructs a new PrimitiveType type. 
     * @param name the name of the PrimitiveType.
     * @param length the length of the PrimitiveType, if FixedLength.
     */
    public PrimitiveType(int typeId, String name, int length)
    {
        super(name, true);
        this.typeId = typeId;
        this.length = length;

        this.isFixedLength = true;
        if(length == TypeManager.VARIABLE_SIZE) {
            isFixedLength = false;
        }
    }

    public boolean equals(Object other)
    {
	if (other == null || !(other instanceof PrimitiveType)) {
	    return false;
	}
	PrimitiveType otherType = (PrimitiveType) other;
	return 
	    this.typeId == otherType.typeId && 
	    this.length == otherType.length;

    }

    /**
     * Returns the length of this type, -1 if undefined variable length. 
     * @return the length of this type.
     */
    public final int getLength()
    {
        return length;
    }

    /**
     * Sets the length of this type.
     *
     * @param length The length to set.
     */
    public final void setLength(int length)
    {
	this.length = length;	
    }

    /**
     * Returns whether this primitive is fixed length. 
     *
     * @return the length of this type.
     */
    public final boolean isFixedLength()
    {
        // To specify that something is of variable type, add the typeId to the 
        // TypeManager.VARIABLE_TYPES array.

        int[] variableTypes = TypeManager.VARIABLE_TYPES;

        for(int i=0; i<variableTypes.length; i++) {
            if(variableTypes[i] == typeId) {
                return false;
            }
        }
        
        return isFixedLength;
    }

    /**
     * Returns the type id of this primitive type.  
     * @see edu.brown.aurora.gui.types.TypeManager
     * @return the type id of this primtiive type.
     */
    public int getTypeId()
    {
        return typeId;
    }

    /**
     * Returns the number of attributes in this type object.  In this case, primitive types
     * always only store one attribute.
     */
    public int size()
    {
	return 1;
    }

    public Attribute getAttribute()
    {
	return new Attribute(name, this, length);
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
	    Attribute thisAttr = getAttribute();
	    Attribute otherAttr = other.getAttribute(0);
	    return otherAttr.isCompatible(thisAttr);
	}
	if(type instanceof CompositeType) {
	    CompositeType other = (CompositeType) type;
	    if(other.size() != size()) {
		return false;
	    }
	    Attribute thisAttr = getAttribute();
	    Attribute otherAttr = other.getAttribute(0);
	    return otherAttr.isCompatible(thisAttr);
	}
	if(type instanceof PrimitiveType) {
	    PrimitiveType other = (PrimitiveType) type;

	    return 
		other.getTypeId() == getTypeId() &&
		other.getLength() == getLength();
	}
	return false;
    }

}
