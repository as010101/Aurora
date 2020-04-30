/*
 * @(#)CompositeType.java    1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

package edu.brown.aurora.gui.types;

import java.util.*;
import javax.swing.*;
import java.io.*;

/**
 * A CompositeType represents a composite type consisting of a number of attributes, each of which has its name and type.
 * When an attribute is of a variable length PrimitiveType such as varchar or binary, the attribute
 * has to have user-defined length. To the contrary, the length of an attribute
 * of a fixed length PrimitiveType (such as int, float, etc.)
 * is determined by the PrimitiveType.
 * Two attributes are compatible
 * only when they are of the same Types and of the same length.
 * Two CompsiteTypes are compatible if all of their corresponding attributes are compatible in the order.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class CompositeType extends Type
{
    protected static int idCounter = 0;
    protected int id = 0;

    protected Vector attributes = new Vector();
    protected boolean isInferred;

    /**
     * Constructs a new CompositeType.
     * @param name the name of the CompositeType
     */
    public CompositeType(String name, boolean isInferred)
    {
        super(name, false);
        this.isInferred = isInferred;
        id = idCounter;
        idCounter++;
    }

    /**
     * Returns this object's identifier.
     * @return this object's identifier.
     */
    public int getId()
    {
        return id;
    }

    /**
     * Returns a String object representing this CompositeType's name and attributes.
     * @return a string representation of this CompositeType's name and attributes
     */
    public final String toStringInDetail()
    {
        String s = super.toString() + " = {";
        Iterator i = attributes.iterator();
        boolean first = true;

        while (i.hasNext())
        {
            if (first)
                first = false;
            else
                s += ", ";
            Attribute a = (Attribute)(i.next());
            s += a.name + ":" + a.type + ":" + a.length;
        }

        s += "}";
        return s;
    }

    /**
     * Returns true if this type is inferred, false otherwise.
     */
    public boolean getIsInferred()
    {
        return isInferred;
    }

    public final void addAttributes(CompositeType c, String prefix) throws TypeException {
      for (int i = 0; i < c.getNumberOfAttributes(); i++) {
        Attribute attribute = c.getAttribute(i);
        addAttribute(prefix+attribute.getName(), attribute.getType(), attribute.getLength());
      }
    }

    /**
     * Adds an attribute to this Type.
     * @param name the name of the new attribute
     * @param type the type of the new attribute
     * @exception TypeException if the given name is already being used
     */
    public final void addAttribute(String name, Type type, int length) throws TypeException
    {
        if(!getIsInferred()) {
            if (findAttribute(name) != null)
                throw new TypeException(TypeException.DUPLICATE_ATTRIBUTES);
        }
        attributes.add(new Attribute(name, type, length));
    }

    /**
     * Inserts an attribute to this Type.
     * @param index the index of the new attribute
     * @param name the name of the new attribute
     * @param type the type of the new attribute
     * @exception TypeException if the given name is already being used
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     */
    public final void insertAttribute(int index, String name, Type type, int length) throws TypeException
    {
        if(!getIsInferred()) {
            if (findAttribute(name) != null)
                throw new TypeException(TypeException.DUPLICATE_ATTRIBUTES);
        }
        attributes.insertElementAt(new Attribute(name, type, length), index);
    }

    /**
     * Finds the attribute having the specified name.
     * @param name the name of the attribut
     * @return the Attribute object having the specified name
     */
    public final Attribute findAttribute(String name)
    {
        for (int i = 0; i < attributes.size(); i++)
        {
            Attribute a = (Attribute)(attributes.elementAt(i));
            if (a.equals(name))
                return a;
        }
        return null;
    }

    /**
     * Finds the attribute at the specified index.
     * @param idx The index of the attribute
     * @return the Attribute object at the specified index.
     */
    public final Attribute getAttribute(int index)
    {
        if(index >= attributes.size()) {
            System.out.println("Error with getting attribute for type : " + getName());
        }
        return (Attribute)attributes.elementAt(index);
    }



    /**
     * Finds the index of the attribute having the specified name.
     * @param name the name of the attribut
     * @return the index of the Attribute object having the specified name
     */
    public final int getAttributeIndex(String name)
    {
        for (int i = 0; i < attributes.size(); i++)
        {
            Attribute a = (Attribute)(attributes.elementAt(i));
            if (a.equals(name))
                return i;
        }
        return -1;
    }

    /**
     * Finds the index of the attribute starting at the specified offset.
     * @param offset the offset of the attribute to find
     * @return the index of the Attribute object starting at the specified offset
     */
    public final int getAttributeIndex(int offset)
    {
        int sum = 0;
        for (int i = 0; i < attributes.size(); i++)
        {
            Attribute a = (Attribute)(attributes.elementAt(i));
            if (sum > offset) return -1;
            else if (sum == offset) return i;
            sum += a.length;
        }
        return -1;
    }

    /**
     * returns a deep copy of the CompositeType
     * @return a deep copy of the CompositeType
     */
    public final CompositeType deepCopy() {
        CompositeType copiedType = new CompositeType ("temporary copy", isInferred);
        try {
            copiedType.deepCopyAttributes (this);
        } catch (TypeException te) {
            JOptionPane.showConfirmDialog(null,
                                          "Type Exception in deep copy",
                                          "Error!",
                                          JOptionPane.DEFAULT_OPTION,
                                          JOptionPane.ERROR_MESSAGE);
        }
        return copiedType;
    }

    /**
     * Copies the attributes from the given CompositeType
     * @param type the CompositeType whose attributes are being copied
     */
    public final void deepCopyAttributes(CompositeType type) throws TypeException {
        attributes.removeAllElements();
            for (int i = 0; i < type.getAttributes().size() ; i++ ) {
                PrimitiveType primitiveType = (PrimitiveType) type.getAttributeType(i);
                /*
                if (primitiveType.isFixedLength()) {
                    addAttribute (new String (type.getAttributeName(i)),
                                  new PrimitiveType (primitiveType.getTypeId(),
                                                     type.getAttributeType(i).getName(),
                                                     type.getAttributeLength(i)));
                }
                else {
                */
                    addAttribute (new String (type.getAttributeName(i)),
                                  new PrimitiveType (primitiveType.getTypeId(),
                                                     type.getAttributeType(i).getName(),
                                                     type.getAttributeLength(i)),
                                  type.getAttributeLength(i));
                    //}
            }
    }

    /**
     * Returns the number of attributes.
     * @return the number of Attribute objects this type has
     */
    public final int getNumberOfAttributes()
    {
        return attributes.size();
    }

    /**
     * Returns the attributes for a composite type
     * @return the attributes for a composite type
     */
    protected final Vector getAttributes ()
    {
        return attributes;
    }

    /**
     * Returns the name of the attribute specified by the given index.
     * @param index the index to the attribute
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     * @return the name of the attribute specified by the index
     */
    public final String getAttributeName(int index)
    {
        return ((Attribute)(attributes.elementAt(index))).name;
    }

    /**
     * Returns the offset of the attribute specified by the given index.
     * @param index the index to the attribute
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     * @return the name of the attribute specified by the index
     */
    public final int getAttributeOffset(int index)
    {
        int sum = 0;
        for (int i = 0; i < index; i++)
            sum += ((Attribute)(attributes.elementAt(i))).length;
        return sum;
    }

    public final String[] getAttributeNames()
    {
        if (attributes.size() == 0) return null;
        String[] names = new String[attributes.size()];
        for (int i = 0; i < attributes.size(); i++)
            names[i] = ((Attribute)(attributes.elementAt(i))).name;
        return names;
    }

    /**
     * Finds the type of the attribute specified by the given index.
     * @param index the index to the attribute
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     * @return the type of the attrbute specified by the given index
     */
    public final Type getAttributeType(int index)
    {
        return ((Attribute)(attributes.elementAt(index))).type;
    }

    /**
     * Finds the length of the attribute specified by the given index.
     * @param index the index to the attribute
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     * @return the length of the attribute specified by the given index
     */
    public final int getAttributeLength(int index)
    {
        return ((Attribute)(attributes.elementAt(index))).length;
    }

    /**
     * Sets the composite type's attributes to the attributes of the given composite type
     * @param type the compsite type whose attributes are being used
     */
    public final void setAttributesTo (CompositeType type)
    {
        attributes = (Vector) type.getAttributes().clone();
    }

    /**
     * Sets the name of the attribute specified by the given index.
     * @param index the index to the attribute
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     */
    public final void setAttributeName(int index, String name) throws TypeException
    {
        Attribute a = findAttribute(name);
        if ((a != null) && (a != attributes.elementAt(index)))
            throw new TypeException(TypeException.DUPLICATE_ATTRIBUTES);
        ((Attribute)(attributes.elementAt(index))).name = name;
    }

    /**
     * Sets the type of the attribute specified by the given index.
     * @param index the index to the attribute
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     */
    public final void setAttributeType(int index, Type type)
    {
        Attribute a = ((Attribute)(attributes.elementAt(index)));
        a.type = type;
        if (type.isPrimitive() && ((PrimitiveType)type).isFixedLength())
            a.length = ((PrimitiveType)type).getLength();
        else a.length = 1;
    }

    /**
     * Sets the length of the attribute specified by the given index.
     * @param index the index to the attribute
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     */
    public final void setAttributeLength(int index, int length) throws TypeException
    {
        if (length <= 0)
            throw new TypeException(TypeException.INVALID_LENGTH);
        ((Attribute)(attributes.elementAt(index))).setLength(length);
    }

    /**
     * Remove one attribute indexed by the specified index.
     * @param index the index of the attribute to remove
     * @exception ArrayIndexOutOfBoundsException if the index is invalid
     */
    public final void removeAttribute(int index)
    {
        attributes.removeElementAt(index);
    }

    /**
     * Returns the number of attributes in this composite type object.
     */
    public int size()
    {
        return attributes.size();
    }

    public boolean equals(Object other)
    {
        if (other == null || !(other instanceof CompositeType)) {
            return false;
        }
        CompositeType otherType = (CompositeType) other;
        if(otherType.size() != size()) {
            return false;
        }
        for(int i=0; i<size(); i++) {
            Attribute otherAttr = otherType.getAttribute(i);
            Attribute thisAttr = getAttribute(i);
            if(otherAttr == null || thisAttr == null ||
               otherAttr.getName() == null || thisAttr.getName() == null ||
               otherAttr.getType() == null || thisAttr.getType() == null) {

                return false;
            }
            if(!otherAttr.getName().equals(thisAttr.getName()) ||
               otherAttr.getLength() != thisAttr.getLength() ||
               !otherAttr.getType().equals(thisAttr.getType())) {
                return false;
            }
        }
        return true;
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

                if(!thisAttr.isCompatible(otherAttr)) {
                    System.out.println("attribute " + otherAttr + " and " + thisAttr + " are incompatible");
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
