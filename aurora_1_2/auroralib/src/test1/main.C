#include <AuroraNode.H>
#include <iostream>
#include <string>
#include <exception>
#include <unistd.h>

using namespace std;

//===============================================================================

void printUsage()
{
  cout << "Usage: test1 <xml-config-file>" << endl 
       << endl;
}

//===============================================================================

void printPorts(const AuroraNode & aNode)
{
  vector<int> in, out;

  aNode.listPorts(in, out);

  cout << endl
       << "Input ports: " << endl;
  for (size_t i = 0; i < in.size(); ++i)
    {
      cout << "   " << in[i] << endl;
    }
 
  cout << endl
       << "Output ports: " << endl;
  for (size_t i = 0; i < out.size(); ++i)
    {
      cout << "   " << out.at(i) << endl;
    }
}

//===============================================================================

int main(int argc, char * argv[])
{
  if (argc != 2)
    {
      printUsage();
      return 1;
    }

  string configFilename = argv[1];

  try
  {
    AuroraNode * pNode = AuroraNode::createAuroraNode(configFilename);
    printPorts(* pNode);
    pNode->shutdown();
    delete pNode;
  }
  catch (exception & e)
    {
      cout << "main.C: Caught runtime exception: " << endl
	   << e.what() << endl << endl;
      return 1;
    }

  cout << "*************************************************************" << endl
       << "*                         TEST1 PASSED                      *" << endl
       << "*************************************************************" << endl
       << endl;
}

//===============================================================================
