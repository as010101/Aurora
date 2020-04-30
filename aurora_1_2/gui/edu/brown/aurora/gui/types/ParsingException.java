/*
 * @(#)ParsingException.java	1.0 01/17/2002
 *
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

package edu.brown.aurora.gui.types;

/**
 * The ParsingException class indicates that a compile-time exception of some sort has occurred.
 *
 *
 * @author Jeong-hyon Hwang (jhhwang@cs.brown.edu)
 * @version 1.0 04/10/2002
 */
public class ParsingException extends Exception
{
  /**
   * Constructs a	ParsingException with no specified detail message.
   */
  public ParsingException()
  {
    super();
  }

  /**
   * Constructs a	ParsingException with the specified detail message.
   * @param s the detail message.
   */
  public ParsingException(String s)
  {
    super(s);
  }
}
