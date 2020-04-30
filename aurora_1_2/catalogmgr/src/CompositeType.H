/*
 * @(#)CompositeType.H    1.0 07/15/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

#ifndef __CompositeType_H
#define __CompositeType_H

#include <vector>
#include <string>

using namespace std;

/**
 * An Attribute object represents an attribute in a user-defined composite type.
 * When an attribute is of a variable length PrimitiveType such as string, the attribute
 * has to have user-defined length. To the contrary, the length of an attribute
 * of a fixed length primitive type (such as int, float, etc.) is determined by the primitive type.
 * Two attributes are compatible only when they are of the same type and of the same length.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class Attribute
{
public:
    string m_fieldName;
    int m_fieldType; // < 0
    int m_size;

    /**
     * Constructs a new composite type having the specified field name, field type, and field size.
     * @param fieldName the name of the Attribute.
     * @param fieldType the type of the Attribute.
     * @param size the size of the Attribute.
     */
    Attribute(string fieldName, int fieldType, int size);

    /**
     * Returns a String object representing this Attribute.
     * @return a string representation of this Attribute.
     */
    string toString();
};

typedef vector<Attribute*> AttributeVector;

/**
 * A CompositeType object represents a user-defined composite type.
 * A CompositeType has a name and a number of attributes, each of which has its name and type.
 * When an attribute is of a variable length PrimitiveType such as string, the attribute
 * has to have user-defined length. To the contrary, the length of an attribute
 * of a fixed length PrimitiveType (such as int, float, etc.) is determined by the PrimitiveType.
 * Two attributes are compatible only when they are of the same Types and of the same length.
 * Two CompositeTypes are compatible if all of their corresponding attributes are compatible in the order.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 07/15/2002
 */
class CompositeType
{
  // Attributes
  protected:
    string m_name;
    AttributeVector m_attributes;

  // Methods
  public:
    /**
     * Constructs a new composite type having the specified name.
     * @param name the name of the Type.
     */
    CompositeType(string name);

    /**
     * Returns a String object representing this Type's name.
     * @return a string representation of the name of this type.
     */
    string getName();

    /**
     * Returns the number of Attributes this Type has.
     * @return the number of Attributes this Type has.
     */
    int getNumberOfAttributes();

    /**
     * Returns the component at the specified index.
     * @param index an index into this vector.
     * @return the component at the specified index.
     */
    Attribute& getAttribute(int index);

    /**
     * Adds the specified attribute to this composite type.
     * @param attribute an attribute to add.
     */
    void addAttribute(Attribute* attribute);

    /**
     * Returns a String object representing this CompositeType's name and attributes.
     * @return a string representation of this CompositeType's name and attributes
     */
    string toString();

};
#endif // __CompositeType_H
