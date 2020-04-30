/*
 * @(#)AggregateFunction.java    1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

package edu.brown.aurora.gui.types;

public class AggregateFunction
{
  protected String name;
  protected Type[] inputTypes;
  protected Type[] outputTypes;

  public AggregateFunction(String name, Type[] inputTypes, Type[] outputTypes)
  {
    this.name = name;
    this.inputTypes = inputTypes;
    this.outputTypes = outputTypes;
  }

  public String getName()
  {
    return name;
  }

  public Type getOutputType(Type inputType)
  {
    if (inputTypes != null)
      for (int i = 0; i < inputTypes.length; i++)
        if (inputType.equals(inputTypes[i]))
          return outputTypes[i];
    return null;
  }
};
