#include <aurora.H>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <Runnable.H>
#include <RunnableRunner.H>
#include <fstream>

using namespace std;

//===============================================================================

void printUsage()
{
	cout << "Usage: test_capi1 <xml-config-file> <tuple-output-filename>" << endl 
		 << endl;
}

//===============================================================================

void printMenu()
{
	cout << endl
		 << "Menu: " << endl
		 << "   \"l\" --> list ports" << endl
		 << "   \"e <input-port-num> <stream-id> <int-field-val> <float-field-val>\" --> enq tuple" << endl
		 << "   \"b <input-port-num> <num-tuples> [<stream-id> <int-field-val> <float-field-val>]+"
		 << "   \"g {on|off}\" --> turn load generation on/off" << endl
		 << "   \"s\"  --> shutdown" << endl
		 << ": ";
}

//===============================================================================

void listPorts(void * pInstance)
{
	cout << endl
		 << "Input ports:" << endl;

	int numPorts;
	AURORA_API_RETURN_CODE rc;

	rc = aurora_getNumInputPorts(pInstance, & numPorts);
	if (rc != AURORA_OK)
		{
			cout << "Error: aurora_getNumInputPorts(...) returned " << rc;
			return;
		}

	if (numPorts == 0)
		{
			cout << "   (none)" << endl;
		}
	else
		{
			for (int i = 0; i < numPorts; ++i)
				{
					int portName;
					rc = aurora_getInputPortNum(pInstance, i, & portName);
					if (rc != AURORA_OK)
						{
							cout << "Error: aurora_getInputPortNum(...) returned " << rc;
							return;
						}

					cout << "   " << portName << endl;
				}
		}

	cout << endl
		 << "Output ports:" << endl;

	rc = aurora_getNumOutputPorts(pInstance, & numPorts);
	if (rc != AURORA_OK)
		{
			cout << "Error: aurora_getNumOutputPorts(...) returned " << rc;
			return;
		}

	if (numPorts == 0)
		{
			cout << "   (none)" << endl;
		}
	else
		{
			for (int i = 0; i < numPorts; ++i)
				{
					int portName;
					rc = aurora_getOutputPortNum(pInstance, i, & portName);
					if (rc != AURORA_OK)
						{
							cout << "Error: aurora_getOutputPortNum(...) returned " << rc;
							return;
						}

					cout << "   " << portName << endl;
				}
		}


	cout << endl;
}

//===============================================================================

void enqTuple(void * pInstance)
{
	int portName;
	int streamId;
	int intVal;
	float floatVal;
	cin >> portName >> streamId >> intVal >> floatVal;

	char tupleBuffer[8 + // standard timestamp (timeval)
					 4 + // standard stream id (int)
					 4 + // user int field
					 4]; // user float field

	// API fills in the timestamp for us...
	memcpy(tupleBuffer +  8, & streamId, sizeof(streamId));
	memcpy(tupleBuffer + 12, & intVal,   sizeof(intVal));
	memcpy(tupleBuffer + 16, & floatVal, sizeof(floatVal));

	AURORA_API_RETURN_CODE rc = aurora_enqRecord(pInstance,
												 portName,
												 streamId,  
												 tupleBuffer,
												 sizeof(tupleBuffer));
	if (rc != AURORA_OK)
		{
			cout << "Error: aurora_enqRecord(...) returned " << rc;
			return;
		}
}

//===============================================================================

void enqBatch(void * pInstance)
{
	int portName;
	size_t numTuples;

	cin >> portName >> numTuples;

	size_t tupleSize = 
		8 + // standard timestamp (timeval)
		4 + // standard stream id (int)
		4 + // user int field
		4;  // user float field

	size_t bufferSize = tupleSize * numTuples;

	char * pBuffer = new char[bufferSize];
	assert(pBuffer != NULL);

	for (size_t i = 0; i < numTuples; ++i)
		{
			char * pTuple = pBuffer + (i * tupleSize);

			int streamId;
			int intVal;
			float floatVal;

			cin >> streamId >> intVal >> floatVal;

			memcpy(pTuple +  8, & streamId, sizeof(streamId));
			memcpy(pTuple + 12, & intVal,   sizeof(intVal));
			memcpy(pTuple + 16, & floatVal, sizeof(floatVal));
		}


	AURORA_API_RETURN_CODE rc = aurora_enqBatch(pInstance,
												portName,
												pBuffer,
												bufferSize,
												numTuples);

	delete[] pBuffer;

	if (rc != AURORA_OK)
		{
			cout << "Error: aurora_enqBatch(...) returned " << rc;
			return;
		}
}

//===============================================================================

void setLoadGen(void * pInstance)
{
	string strFlag;
	cin >> strFlag;

	if (strFlag == "on")
		{
			AURORA_API_RETURN_CODE rc = aurora_xStartLoadGenerator(pInstance);
			if (rc != AURORA_OK)
				{
					cout << "Error: aurora_xStartLoadGenerator(...) returned " << rc;
					return;
				}
		}
	else if (strFlag == "off")
		{
			AURORA_API_RETURN_CODE rc = aurora_xStopLoadGenerator(pInstance);
			if (rc != AURORA_OK)
				{
					cout << "Error: aurora_xStopLoadGenerator(...) returned " << rc;
					return;
				}
		}
	else
		{
			cout << "The parameter must be either 'on' or 'off'" << endl;
		}  
}

//===============================================================================

void shutdown(void * pInstance)
{
	AURORA_API_RETURN_CODE rc = aurora_shutdown(pInstance);
	if (rc != AURORA_OK)
		{
			cout << "Error: aurora_shutdown(...) returned " << rc;
			return;
		}	
}

//===============================================================================

// Returns 'true' iff the user has indicated that he wants the program to shut
// down.
bool handleMenuCmd(void * pInstance)
{
	char cmd;
	cin >> cmd;
	
	switch (cmd)
		{
		case 'l':
			listPorts(pInstance);
			return false;
			break;
		case 'e':
			enqTuple(pInstance);
			return false;
			break;
		case 'b':
			enqBatch(pInstance);
			return false;
			break;		
		case 'g':
			setLoadGen(pInstance);
			return false;
			break;
		case 's':
			shutdown(pInstance);
			return true;
			break;
		default:
			cout << "Invalid command" << endl;
			return false;
		};
}

//===============================================================================

class OutputRunnable : public Runnable
{
private:
	void * _pInstance;
	string _outputFilename;

public:
	OutputRunnable(void * pInstance,
				   string outputFilename)
	{
		_pInstance = pInstance;
		_outputFilename = outputFilename;
	}
	
	void run() 
		throw()
	{
		ofstream outfile(_outputFilename.c_str(), ios::out | ios::app);
		if (! outfile)
			{
				cerr << " Couldn't open output file \"" << _outputFilename << "\"" << endl;
				exit(1);
			}

		bool moreTuples = true;
		int tupleCount = 0;

		size_t buffSize = 1000000;
		char * pBuffer = new char[buffSize];

		while (moreTuples)
			{
				int wasDequeued;
				int outputPortName;
				int numTuples;

				AURORA_API_RETURN_CODE rc = aurora_deqBatch(_pInstance,
															& wasDequeued, 
															& outputPortName, 
															& numTuples,
															pBuffer,
															buffSize);
				if (rc != AURORA_OK)
					{
						cout << "Error: aurora_deqRecord returned " << rc << endl;
						exit(1);
					}

				if (wasDequeued)
					{
						moreTuples = true;
						
						outfile << "### JUST DEQUEUED A BATCH OF SIZE " << numTuples << endl;

						for (size_t i = 0; i < numTuples; ++i)
							{
								size_t bytesPerTuple = 20;
								const char * pTuple = pBuffer + (i * bytesPerTuple);

								timeval tstamp;
								int streamId;
								int userInt;
								float userFloat;

								memcpy(& tstamp,    pTuple,      sizeof(tstamp   ));
								memcpy(& streamId,  pTuple +  8, sizeof(streamId ));
								memcpy(& userInt,   pTuple + 12, sizeof(userInt  ));
								memcpy(& userFloat, pTuple + 16, sizeof(userFloat));

								++ tupleCount;
								outfile << "Tuple #" << tupleCount << endl
										<< "   timestamp.tv_sec  = " << tstamp.tv_sec << endl
										<< "   timestamp.tv_usec = " << tstamp.tv_usec << endl
										<< "   output port       = " << outputPortName << endl
										<< "   stream id         = " << streamId << endl
										<< "   user int field    = " << userInt << endl
										<< "   user float field  = " << userFloat << endl
										<< endl;

								if (! outfile.good())
									{
										cerr << " Couldn't write to output file \"" 
											 << _outputFilename << "\"" << endl;
										exit(1);
									}


							}
					}
				else
					{
						moreTuples = false;
					}
			}

		delete pBuffer;
		
	}
};

//===============================================================================

int main(int argc, const char* argv[])
{
	if (argc != 3)
		{
			printUsage();
			return 1;
		}
  
	const char * pszConfigFilename = argv[1];
	const char * pszTupleOutputFilename = argv[2];


	AURORA_API_RETURN_CODE rc;
	void * pInstance = NULL;

	rc = aurora_init(& pInstance, pszConfigFilename);
	if (rc != AURORA_OK)
		{
			cerr << "aurora_init(...) returned: " << rc << endl;
			return 1;
		}

	assert(pInstance != NULL);

	// Launch output monitor thread...
	OutputRunnable aRunnable(pInstance, string(pszTupleOutputFilename));
	RunnableRunner aRunner(aRunnable);

	// Begin user interactions...
	bool done = false;
	while (! done)
		{
			printMenu();
			done = handleMenuCmd(pInstance);
		}

	cout << endl
		 << "Waiting for output thread to terminate..." << endl;

	aRunner.join();
	cout << "Done." << endl;
	return 0;
}
