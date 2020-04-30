#include "AuroraNode.H"
#include "parseutil.H"
#include <unistd.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <Runnable.H>
#include <RunnableRunner.H>
#include <fstream>
#include <sstream>

using namespace std;

//===============================================================================

void
write_file_into_aurora(AuroraNode *node, int port, const char *filename)
{
  ifstream file(filename, ios::in);
  const int linelen = 1000;
  char line[linelen];
  int streamId = 0;

    cout << endl
	 << "Enqueuing: (>) " << flush;

  while (file.good() && !file.eof()) {
    file.getline(line, linelen);
    // line will be comma seperated integers.
    vector<string> v = unpackString(string(line), string(","));
    vector<string>::iterator pos = v.begin();
    int count = v.size();
    int i = 0;
    int ints[count];
    while (pos != v.end()) {
      ints[i] = stringToInt(*pos);
      i++;
      pos++;
    }
    // We have an int array.

    char tupleBuffer[8 + // standard timestamp (timeval)
		     4 + // standard stream id (int)
		     (4 * count) // The ints.
      ];
	// API fills in the timestamp for us...
    memcpy(tupleBuffer +  8, & streamId, sizeof(streamId));
    memcpy(tupleBuffer + 12, ints,   sizeof(ints));

    cout << ">" << flush;
    node->enqRecord(0, // Input port ID.
		    streamId,
		    tupleBuffer,
		    sizeof(tupleBuffer));
    file.close();
    file.open(filename);
  }
  
}

//===============================================================================
static string printCharArray (const char *a, int len) {
  ostringstream oss;
  oss.flags(ios::hex);
  int v = *a;
  oss << v;
  for (int i = 1; i < len; i++) {
    v = *(a+i);
    oss << " " << v;
  }
  return oss.str();
}
//===============================================================================

class OutputRunnable : public Runnable
{
private:
  AuroraNode *node;
  ofstream file;

public:
  OutputRunnable(AuroraNode *node, const char* filename) :
    node(node), file(filename, ios::out | ios::app) {
    if (! file) {
      cerr << " Couldn't open output file \"" << filename << "\"" << endl;
      exit(1);
    }
  }
	
  void run() 
    throw() {

    int outputPorts[] = {65, 66, 67, 68, 69, 70};

    // Find maximum buffer size needed
    unsigned int maxBufferSize = 0;
    for (int i = 0; i < 6; ++i) {
      // DISABLED until we figure this out -- for some reason the aurora node doesnt have tuplesizebyoutputport ready?
      //      int tuplesize = node->getTupleSizeByOutputPort(outputPorts[i]);
      int tuplesize = (15 * 4) + 8;
      if (tuplesize > maxBufferSize) maxBufferSize = tuplesize;
    }
      

    // Now just sit around dequeueing..
    bool wasDequeued = false;
    int tupleCount = 0;
 
    cout << endl
	 << "Dequeuing: (<)" << flush;

    while (true) {
      int outputPortName;
      
      char tupleBuffer[maxBufferSize];
      
      wasDequeued = node->deqRecord(outputPortName,
				    (void*)tupleBuffer, sizeof(tupleBuffer));
      if (wasDequeued) {
	+tupleCount;
	cout << "<" << flush;
	file << tupleCount << ": (port " << outputPortName << ") [" << printCharArray(tupleBuffer, sizeof(tupleBuffer)) << "]" << endl;
      }
      
    
      if (! file.good()) {
	cerr << " Couldn't write to output file " << endl;
	exit(1);
      }
    }
    // done done done!
  }
};

//===============================================================================

void usage(int argc, const char* argv[])
{
  cout << "Usage: "<< argv[0]
       << "<input-file> <output-file> [-Dprops-override]+" << endl 
       << endl;
  exit(1);
}

//===============================================================================

int main(int argc, const char* argv[])
{
  if (argc < 4) {
    usage(argc, argv);
  }
  
  char *config_file = "../src/run_a_network/config.xml";

  /*
	Temporarily broken by a change to AuroraNode::createAuroraNode. I owe Richard
	a fix of this. -cjc
  ostringstream oss;
  char *srcdir = getenv("AURORA_SRC_DIR");
  if (srcdir == NULL) {
    cerr << "Environment variable AURORA_SRC_DIR must be set." << endl;
    exit(1);
  }
  oss << srcdir << "/catalogmgr/" << argv[1];
  const char *catalog = oss.str().c_str();
  */

  const char *input_file = argv[2];
  const char *output_file = argv[3];

  try {
    AuroraNode *node = AuroraNode::createAuroraNode(config_file /*, catalog*/);

    cout << "Waiting to let auroranode initialize..." << flush;
    sleep(3);
    cout << "done." << endl;
    // Launch output monitor thread...
    OutputRunnable output_runnable(node, output_file);
    RunnableRunner output_runner(output_runnable);

    // TODO: Consider port ID other than zero.
    write_file_into_aurora(node, 0, input_file);

    node->shutdown();

    cerr << endl
	 << "Waiting for output thread to terminate..." << endl;
  
    output_runner.join();
    cerr << "Done." << endl;
  } catch (exception & e) {
    cerr << argv[0] << ": Caught runtime exception: " << endl
	 << e.what() << endl << endl;
    return 1;
  }

  return 0;
}
