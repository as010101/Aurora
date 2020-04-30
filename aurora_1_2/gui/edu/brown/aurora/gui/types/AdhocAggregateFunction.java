/*
 * @(#)AdhocAggregateFunction.java    1.0 04/10/2002
 *
 * Copyright 2002 Jeong-Hyon Hwang (jhhwang@cs.brown.edu). All Rights Reserved.
 * Box# 1910, Dept. of Computer Science, Brown University, RI 02912, USA.
 * All Rights Reserved.
 */

package edu.brown.aurora.gui.types;

public class AdhocAggregateFunction extends AggregateFunction
{

  public AdhocAggregateFunction(String name, Type[] outputTypes)
  {
    super(name, null, outputTypes);
  }

  public String getName()
  {
    return name;
  }

  public Type[] getOutputTypes()
  {
    return outputTypes;
  }
};
