 // main.c

/////////////////////////////////////////////////////////////////////////////
// Author: Jeong-hyon Hwang(jhhwang@cs.brown.edu)
// Version 1.0 - 2002/07/15
/////////////////////////////////////////////////////////////////////////////

#include "CatalogManager.H"
#include "CompositeType.H"
#include <stdio.h>
#include <typeinfo>

#include "DataInputStream.H"
#include "DataOutputStream.H"

int main(int argc, char** args)
{
  if (argc != 2)
  {
    cout << "please specify the working directory only" << endl;
    return (0);
  }
  string s(args[1]);

  CatalogManager cm(s);
  cout << "Work Directory: " << cm.getDirectory() << endl << endl;

  QueryNetwork* qNet = cm.load();
  cout << endl << cm.getTypeManager().toString() << endl;
  cout << endl << qNet->toString() << endl;

  Box* b = qNet->getBox(1);
    if (b != NULL) {
      cout << "The type of box #1 is " << b->getBoxType() << "." << endl;
      // calling b->getPredicate();
      Predicate *pr = b->getPredicate();
      const char * s = "foo";
      pr->evaluate(const_cast<char *>(s));
    }
  delete qNet;
  return(0);
}

