#include <AuroraNode.H>
#include <AuroraNodeImpl.H>

AuroraNode::~AuroraNode()
{
}

AuroraNode * AuroraNode::createAuroraNode(string propsFilename)
  throw (exception)
{
  AuroraNode * pNode = new AuroraNodeImpl(propsFilename);
  return pNode;
}

AuroraNode * AuroraNode::createAuroraNode(string propsFilename, 
										  string prefix, 
										  int argc, 
										  const char * argv[])
	throw (exception)
{
	AuroraNode * pNode = new AuroraNodeImpl(propsFilename, prefix, argc, argv);
	return pNode;
}
