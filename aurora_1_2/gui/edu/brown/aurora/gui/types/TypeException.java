/*
 * @(#)TypeException.java	1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved. 
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved. 
 */

package edu.brown.aurora.gui.types;

/**
 * The TypeException class indicates that a type error of some sort has occurred. 
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class TypeException extends Exception
{

	/**
	 * A TypeException Message: The same attribute exists in the type already. 
	 */
	public static final String DUPLICATE_ATTRIBUTES = "The same attribute exists in the type already.";

	/**
	 * A TypeException Message: The same type exists in the type manager already.
	 */
	public static final String DUPLICATE_TYPES = "The same type exists in the type manager already.";

	/**
	 * A TypeException Message: The attribute is not yet registered.
	 */
	public static final String NON_REGISTERED_ATTRIBUTE = "The attribute is not yet registered.";

	/**
	 * A TypeException Message: The length of an attribute has to be greater than 0. 
	 */
	public static final String INVALID_LENGTH = "The length of an attribute has to be greater than 0.";

	/**
	 * A TypeException Message: The length of an attribute must be represented as a number. 
	 */
	public static final String NON_NUMERICAL_LENGTH = "The length of an attribute must be represented as a number.";

	/**
	 * Constructs a	TypeException with no specified detail message. 
	 */
	public TypeException()
	{
		super();
	}

	/**
	 * Constructs a	TypeException with the specified detail message.
	 * @param s the detail message.
	 */
	public TypeException(String s)
	{
		super(s);
	}

}
