
/*
 * @(#)Field.java    1.0 06/28/2002
 *
 * Copyright 2002 Adam Singer (arsinger@cs.brown.edu). All Rights Reserved. 
 * Box 0398, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

import edu.brown.aurora.gui.*;

/**
 * A Field represents a packaged PrimitiveType to be created by the
 * workflow generator. In addition to the variables inherited from
 * PrimitiveType, Field stores information about its variance. 
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 06/28/2002
 */

public class Field extends PrimitiveType
{

    /**
     * The type of variance for this Field
     */
    protected int variance;
    /**
     * All of the parameters for this Field's variance
     */
    protected DistributionSet[] varParameters;
    /**
     * Specific name of this Field
     */
    protected String id;
    private int numParams;

    /**
     * Constructs a new Field type with a set length. 
     * @param name the name of the Field.
     * @param length the length of the Field.
     */
    public Field(int typeId, String name, int length)
    {
        super(typeId, name, length);
        id = name;
        variance = 1;
        numParams = Constants.VAR_PARAM_NUM[variance];
        varParameters = new DistributionSet[numParams];
        for (int i=0; i<numParams; ++i) {
            varParameters[i] = new DistributionSet();
            varParameters[i].setDistributionType(0);
            varParameters[i].setParameter(0,1+(9*i));
        }
    }

    /**
     * Sets the ID of this Field. This is the "instance" of the
     * primitive type, such as "Age" instead of "int", or "Name"
     * instead of "string".
     * @param id the specific ID of this Field
     */
    public void setID(String id)
    {
        this.id = id;
    }

    /**
     * Returns the ID of this Field.
     */
    public String getID()
    {
        return id;
    }

    /**
     * Set the type of variance for this Field.
     * @param type the type of variance
     */
    public void setVariance(int type)
    {
        if (variance != type) {
            variance = type;
            numParams = Constants.VAR_PARAM_NUM[type];
            varParameters = new DistributionSet[numParams];
            for (int i=0; i<numParams; ++i) {
                varParameters[i] = new DistributionSet();
                varParameters[i].setDistributionType(0);
                varParameters[i].setParameter(0,10);
            }
        }
    }

    /**
     * Returns the variance of this Field
     * @return variance
     */
    public int getVariance()
    {
        return variance;
    }

    /**
     * Set the value of a given parameter for the variance.
     * @param num the number of the parameter to be set
     * @param value the value to be set into that parameter
     * @return whether the parameter num was valid
     */
    public boolean setParameter(int num, DistributionSet value)
    {
        if (num>=0 && num<numParams) {
            varParameters[num] = value;
            return true;
        }
        return false;
    }

    /**
     * Returns the specified variance parameter
     * @param num the number of the parameter to get
     * @return value of requested param, or null if invalid
     */
    public DistributionSet getParameter(int num)
    {
        if (num>=0 && num<numParams) {
            return varParameters[num];
        }
        System.err.println("Index out of bounds");
        return null;
    }

}
