
package edu.brown.aurora.gui.types;

import edu.brown.bloom.petal.*; 

import javax.swing.*;
import javax.swing.border.*;
import java.awt.event.*; 
import javax.swing.event.*;
import java.awt.*;
import edu.brown.bloom.petal.*;
import edu.brown.aurora.gui.*;

import java.util.*;


/**
 * A class to store information for a given distribution
 *
 * @author Adam Singer (arsinger@cs.brown.edu)
 * @version 1.0 09/04/2002
 */
public class DistributionSet
{

    private int _distType;
    private double[] _distParams;

    /**
     * Constructs an empty DistributionSet
     */
    public DistributionSet()
    {
        _distType = -1;
        _distParams = new double[0];
    }

    /**
     * Sets the type of distribution
     *
     * @param distType the type of distribution to use
     */
    public void setDistributionType(int distType)
    {
        if (distType!=_distType) {
            _distType = distType;
            int numParams = Constants.VAR_PARAM_NUM[_distType];
            _distParams = new double[numParams];
            for (int i=0; i<numParams; ++i) {
                _distParams[i] = 0;
            }
        }
    }

    /**
     * Gets the current distribution type of this set
     *
     * @return distribution type
     */
    public int getDistributionType()
    {
        return _distType;
    }

    /**
     * Sets the parameter at the specified index
     *
     * @param index which parameter to set
     * @param value the value to set into that parameter
     */
    public void setParameter(int index, double value)
    {
        if (index>=0&&index<Constants.VAR_PARAM_NUM[_distType]) {
            _distParams[index] = value;
        }
    }

    /**
     * Returns the requested parameter
     *
     * @param index the parameter to return
     * @return paramter at that index
     */
    public double getParameter(int index)
    {
        if (index>=0&&index<Constants.VAR_PARAM_NUM[_distType]) {
            return _distParams[index];
        }
        return -1;
    }

}
