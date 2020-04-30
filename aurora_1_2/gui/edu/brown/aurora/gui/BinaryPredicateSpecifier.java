package edu.brown.aurora.gui;

/**
 * An interface that specifies the methods that all binary box nodes should
 * implement.
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu) and Robin Yan (rly@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public interface BinaryPredicateSpecifier
{
    /**
     * Returns the first input arc id.
     *
     * @return The first input arc id as an integer.
     */
    public int getLeftArcId();

    /**
     * Returns the second input arc id.
     *
     * @return The second input arc id as an integer.
     */
    public int getRightArcId();


    /**
     * Returns the index of the tuple for the first input which this binary 
     * box operates on.
     *
     * @return The index of the tuple for the first input which this binary 
     * box operates on.
     */
    public int getLeftAttributeIndex();

    /**
     * Returns the index of the tuple for the second input which this binary 
     * box operates on.
     *
     * @return The index of the tuple for the second input which this binary 
     * box operates on.
     */    
    public int getRightAttributeIndex();
    

    /**
     * Returns the type of the operator for this binary box.
     *
     * @return The type of the operator for this binary box as an integer.
     */
    public int getOperatorTypeId();
    
    /**
     * Sets the index of the tuple for the first input which this binary
     * box operates on.
     *
     * @param leftAttributeIndex Specifies the index for the first input
     * which this binary box operates on.
     */
    public void setLeftAttributeIndex(int leftAttributeIndex);

    /**
     * Sets the index of the tuple for the second input which this binary
     * box operates on.
     *
     * @param rightAttributeIndex Specifies the index for the second input
     * which this binary box operates on.
     */    
    public void setRightAttributeIndex(int rightAttributeIndex);
    
    /**
     * Sets the operator type for this binary box.
     *
     * @param operatorTypeId The operator type for this binary box.
     */
    public void setOperatorTypeId(int operatorTypeId);
	
}


