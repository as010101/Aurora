#include <AuroraNode.H>
#include <iostream>
#include <string>
#include <sstream>
#include <Runnable.H>
#include <RunnableRunner.H>
#include <parseutil.H>
#include <exception>
#include <unistd.h>
#include <dirent.h>
#include <util.H>

using namespace std;

//===============================================================================

void printUsage()
{
  cout << "Usage: run_a_network <xml-config-file> <catalogs directory> [-Dprops-override]*" << endl 
       << "Example: run_a_network config.xml /aurora/catalogs -DScheduler.queue_mon_flag=T" << endl
       << endl;
}


//===============================================================================

class DelayedShutdownRunnable : public Runnable
{
public:
  DelayedShutdownRunnable(AuroraNode & node,
			  size_t shutdownDelaySeconds) :
    _node(node),
    _shutdownDelaySeconds(shutdownDelaySeconds)
  {
  }

  void run() 
    throw()
  {
    try
      {
	sleep(_shutdownDelaySeconds * 1000000);
	_node.xStopLoadGenerator();
	_node.shutdown();
      }
    catch (exception & e)
      {
	cloneAndSetRunException(e);
      }
  }

private:
  AuroraNode & _node;
  size_t _shutdownDelaySeconds;
};

//===============================================================================


int main(int argc, char * argv[])
{
  bool useDefaults = false;
  if (argc == 1) {
    // use defaults
    useDefaults = true;
    cout << argv[0] << ": using default arguments (../auroralib/src/run_a_network/config.xml ../catalogmgr)" << endl;
  }
  if (!useDefaults && argc != 3)
    {
      printUsage();
      return 1;
    }

  string configFilename = (useDefaults) ?  "../auroralib/src/run_a_network/config.xml" : argv[1];

  try
  {
    
    // Eddie's enhancement time! Wouldn't life be more fun if you had a MENU to choose the darn network?
    // WELL HERE GOES. Text only, cuz I'm sure ppl would complain if it was graphical.
    // First, we will respect a "run interactive" flag else ppl won't be able to run it in batch style
    string catalogDir;
    //string catalogsDir = _pProps->getString("CatalogMgr.catalogDirectory");
    string catalogsDir = (useDefaults) ? "../catalogmgr" : argv[2];
    
    struct dirent **namelist;
    int n;
    vector<string> theDirs;
    n = scandir(catalogsDir.c_str(), &namelist, 0, alphasort);
    if (n < 0) {
      perror("scandir");
      exit(1);
    } else {
      while(n--) {
	struct stat buf;
	if (strcmp(namelist[n]->d_name,"..") == 0 ||
	    strcmp(namelist[n]->d_name,".") == 0) continue;
	// See if ArcTable.db exists inside (dont use Metadata, netgen doesnt make it), if so, its a catalog
	string checkFile(catalogsDir + "/" + namelist[n]->d_name + "/ArcTable.db");
	if (stat(checkFile.c_str(),&buf) == 0) {
	  // Ok, it was found
	  theDirs.push_back(checkFile);
	}
	free(namelist[n]);
      }
      free(namelist);
      // Ok cool, dirs now holds all "valid" catalogs
      cout << "----------------------------- "<<theDirs.size()<<" CATALOGS AVAILABLE ------------------------------" << endl;
      vector<string>::iterator pos = theDirs.begin();
      int dirIdx = 0;
      while (pos != theDirs.end()) {
	cout << " ["<<dirIdx++<<"] "<< (*pos).substr(catalogsDir.length()+1).substr(0,(*pos).substr(catalogsDir.length()+1).rfind('/')) << endl;
	pos++;
      }
      cout << "--------------------------------------------------------------------------------" << endl;
      cout << endl;
      cout << "                           Please choose network to run now: ";
      int chosen;
      while (true) {
	string input;
	if (!(cin >> input)) exit(1);
	istringstream iss(input);
	if (iss >> chosen) break;
	cout << "Invalid input -- enter the integer index of the network you wish to run:";
      }
      catalogDir = theDirs.at(chosen).substr(0,theDirs.at(chosen).rfind('/'));
    }
    string catalogDirCmd = "-DCatalogMgr.directory:string="+catalogDir;
    const char** fakeArgv = new const char*[argc+1];
    for (int i = 0; i < argc; i++) {
      fakeArgv[i] = argv[i];
    }
    fakeArgv[argc] = catalogDirCmd.c_str();
    AuroraNode * pNode = AuroraNode::createAuroraNode(configFilename, "-D", argc+1,fakeArgv);
    pNode->xStartLoadGenerator();


    DelayedShutdownRunnable shutdownRunnable(* pNode, 600);
    RunnableRunner shutdownThread(shutdownRunnable);
    
    size_t bufSize = 1000;
    void *tupleAddr = new char[bufSize];
    static int x;
    
    int outputPortId;
    /**
    while (pNode->deqRecord(outputPortId, tupleAddr, bufSize))
      {
	cout << "[OUTPUT " << ++x << "] [" << outputPortId << "]" << endl;
	cout << "[OUTPUT " << x << "]  --------------------------------" << endl;
	cout << "[OUTPUT " << x << "]";
	// remember you also give whatever you need to skip over the timestamp and streamid
	// timestamp is a timeval: two longs, sid one int (total of 12 bytes) (the 3 i's)
	printTuple((char*)tupleAddr,argv[2]);
	cout << "[OUTPUT " << x << "]  --------------------------------" << endl;
      }
    */
    shutdownThread.join();
    pNode->xStopLoadGenerator();
    pNode->shutdown();
    delete pNode;
  }
  catch (exception & e)
    {
      cout << "run_a_network: Caught runtime exception: " << endl
	   << e.what() << endl << endl;
      return 1;
    }

  cout << "*************************************************************" << endl
       << "*                         DONE                              *" << endl
       << "*************************************************************" << endl
       << endl;
}

//===============================================================================
