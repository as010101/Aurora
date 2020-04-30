
/*
 * @(#)FieldSet.java    1.0 07/01/2002
 *
 * Copyright 2002 Adam Singer (arsinger@cs.brown.edu). All Rights Reserved. 
 * Box 0398, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import edu.brown.aurora.gui.*;

/**
 * A FieldSet represents a group of fields, common to all streams in an 
 * aggregate stream. It also contains information about the Rate of stream
 * generation, in addition to the number of streams within the aggregate
 * stream.
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 07/01/2002
 */

public class FieldSet
{
    private static int numFieldSets = 0;
    private int id;
    private java.util.Vector fields;
    private int distributionType;
    private DistributionSet[] distributionParams;

    /**
     * Constructs a FieldSet with an unknown number of fields.
     */
    public FieldSet()
    {
        id = numFieldSets;
        ++numFieldSets;
        fields = new java.util.Vector(0);
        distributionType = -1;
        distributionParams = new DistributionSet[0];
    }

    /**
     * Constructs a FieldSet with a number of fields specified.
     * @param numFields the number of fields in this FieldSet.
     */
    public FieldSet(int numFields)
    {
        id = numFieldSets;
        ++numFieldSets;
        fields = new java.util.Vector(numFields);
        distributionType = -1;
        distributionParams = new DistributionSet[0];
    }

    /**
     * Constructs a FieldSet using around a given CompositeType.
     * @param composite the CompositeType to emulate
     */
    public FieldSet(CompositeType composite)
    {
        id = numFieldSets;
        ++numFieldSets;
        int numfields = composite.getNumberOfAttributes();
        setNumFields(numfields);
        for (int i = 0; i<numfields; ++i) {
            addField(composite.getAttributeName(i), 
                     (PrimitiveType)composite.getAttributeType(i));
        }
        distributionType = 0;
        distributionParams = new DistributionSet[1];
        distributionParams[0] = new DistributionSet();
        distributionParams[0].setDistributionType(0);
        distributionParams[0].setParameter(0,1);
    }

    /**
     * Sets the number of Fields in this FieldSet.
     * @param numFields number of fields in this FieldSet
     */
    public void setNumFields(int numFields)
    {
        fields = new java.util.Vector(numFields);
    }

    /**
     * Returns the number of fields in this FieldSet.
     * @return number of fields
     */
    public int getNumFields()
    {
        return fields.size();
    }

    /**
     * Sets the current type of distribution.
     * @param type the type of distribution to use
     */
    public void setDistributionType(int type) {
        if (distributionType != type) {
            distributionType = type;
            int numParams = Constants.VAR_PARAM_NUM[type];
            distributionParams = new DistributionSet[numParams];
            for (int i=0; i<numParams; ++i) {
                distributionParams[i] = new DistributionSet();
            }
        }
    }

    /**
     * Get the current type of distribution.
     * @return distribution being used
     */
    public int getDistributionType()
    {
        return distributionType;
    }

    /**
     * Sets the current value for the specified parameter of the
     * distribution.
     * @param index the parameter to change
     * @param val the DistributionSet to set that parameter to
     */
    public void setDistributionParam(int index, DistributionSet val) {
        if (index>=0 && index<Constants.VAR_PARAM_NUM[distributionType]) {
            distributionParams[index] = val;
        }
    }

    /**
     * Returns the current value for the specified parameter of the rate's
     * distribution.
     * @param index the parameter to find
     * @return value for the specified parameter
     */
    public DistributionSet getDistributionParam(int index) {
        if (index>=0 && index<Constants.VAR_PARAM_NUM[distributionType]) {
            return distributionParams[index];
        }
        return null;
    }

    /**
     * Returns the names of all of the Fields in this FieldSet.
     * @return array of Strings with the IDs of the Fields
     */
    public String[] getFieldIds()
    {
        String[] fieldIds = new String[fields.size()];
        for (int i = 0; i<fields.size(); ++i) {
            fieldIds[i] = ((Field)fields.elementAt(i)).getID();
        }
        return fieldIds;
    }

    /**
     * Returns the types of all the Fields in this FieldSet.
     * @return array of Strings with the types of the Fields
     */
    public String[] getFieldTypes()
    {
        String[] fieldTypes = new String[fields.size()];
        for (int i = 0; i<fields.size(); ++i) {
            fieldTypes[i] = ((Field)fields.elementAt(i)).getName();
        }
        return fieldTypes;
    }

    /**
     * Returns a field in this FieldSet with the given ID if
     * it exists.
     * @param id the id of the field to find
     * @return field with the given id or null if none exists
     */
    public Field getField(String id)
    {
        for (int i = 0; i<fields.size(); ++i) {
            if (((Field)fields.elementAt(i)).getID().equals(id)) {
                return (Field)fields.elementAt(i);
            }
        }
        return null;
    }

    /**
     * Returns the index of the specified field
     * @param field the field to find
     * @return index where the field is located, or -1
     */
    public int getFieldIndex(Field field)
    {
        for (int i = 0; i<fields.size(); ++i) {
            if (((Field)fields.elementAt(i)).getID().equals(field.getID())) {
                return i;
            }
        }
        return -1;
    }

    /**
     * Returns a field in this FieldSet with the given index, if
     * the index is valid.
     * @param index the index of the field to find
     * @return field with the given index or null if none exists
     */
    public Field getField(int index)
    {
        if (index>=0 && index<fields.size()) {
            return (Field)fields.elementAt(index);
        }
        return null;
    }

    /**
     * Sets the field at the given index to a new Field passed in.
     * @param index the index of the Field to be replaced
     * @param field the Field to insert
     */
    public void setField(int index, Field field)
    {
        if (index>=0 && index<=fields.size()) {
            if (index<fields.size()) {
                fields.removeElementAt(index);
            }
            fields.insertElementAt(field, index);
        }
    }

    /**
     * Inserts a Field at the given index.
     * @param index the index at which to insert
     * @param field the Field to insert
     */
    public void insertField(int index, Field field)
    {
        if (index>=0 && index<fields.size()) {
            fields.insertElementAt(field, index);
        }
    }

    /**
     * Adds a new Field with the given id and type.
     * @param id the id of the Field to be added
     * @param type the PrimitiveType of the Field
     */
    public void addField(String id, PrimitiveType type)
    {
        Field tmp = null;
        tmp = new Field(type.getTypeId(), type.getName(), type.getLength());
        tmp.setID(id);
        fields.addElement(tmp);
    }

    /**
     * Removes the first Field with the given id.
     * @param id the id of the Field to remove
     */
    public void removeField(String id)
    {
        for (int i = 0; i<fields.size(); ++i) {
            if (fields.elementAt(i).equals(id)) {
                fields.remove(i);
            }
        }
    }

    /**
     * Removes the Field passed in from the FieldSet.
     * @param field the field to remove
     */
    public void removeField(Field field)
    {
        fields.removeElement(field);
    }

}
