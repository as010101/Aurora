#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdio.h>
#include <string.h>

class Expression
{
public:
  virtual char* evaluate(char *tuple) = 0;
  virtual void setExpression(Expression *expr) {};
};

#endif 
