package edu.brown.aurora.gui;


/**
 * An interface that all box predicates implement.
 * 
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public interface PredicateSpecifier
{
    /**
     * Specifies the "greater than" predicate.
     */    
    public static int GT = 0;

    /**
     * Specifies the "less than" predicate.
     */    
    public static int LT = 1;

    /**
     * Specifies the "equals to" predicate.
     */
    public static int EQ = 2;

    /**
     * Specifies the "greater than or equal to" predicate.
     */
    public static int GEQ = 3;

    /**
     * Specifies the "less than or equal to" predicate.
     */
    public static int LEQ = 4;

    /**
     * Specifies the "not equal to" predicate.
     */
    public static int NEQ = 5;
    
    /**
     * Returns the first index tuple.
     *
     * @return an int representing the first index tuple.
     */
    public int getLeftIndex();
    
    /**
     * Returns the operator type id.
     *
     * @return the operator type id.
     */
    public int getOperatorTypeId();
    
    /**
     * Returns the id of the second type.
     *
     * @return The id of the second type.
     */
    public int getRightTypeId();
    
    /**
     * Returns the value of this predicate object.
     *
     * @return the value of the predicate as a string.
     */
    public String getValue();
    
    /**
     * Sets the value of the left index tuple.
     *
     * @param leftIndex the value to set.
     */
    public void setLeftIndex(int leftIndex);
    
    /**
     * Sets the operator type id.
     *
     * @param operatorTypeId The operator type id to set.
     */
    public void setOperatorTypeId(int operatorTypeId);
    
    /**
     * Sets the value of the right index tuple.
     *
     * @param rightTypeId The value to set.
     */
    public void setRightTypeId(int rightTypeId);
    
    /**
     * Sets the value of this object as a string.
     *
     * @param value The value to set.
     */
    public void setValue(String value);
}
