#include <AuroraNode.H>
#include <iostream>
#include <Runnable.H>
#include <RunnableRunner.H>
#include <parseutil.H>
#include <unistd.h>
#include <sys/time.h>
#include <BinarySem.H>

using namespace std;

//===============================================================================

void printUsage()
{
	cout << "Usage: testloadgen <xml-config-file> <seconds-to-run-for> [-prop=value]+" << endl 
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

	// If this becomes posted, this Runnable will simply exit the next time it
	// wakes up, and not shutdown.
	BinarySem _justQuitNow;

	void run() 
		throw()
	{
		try
			{
				for (size_t i = 0; i < _shutdownDelaySeconds; ++i)
					{
						if (_justQuitNow.isPosted())
							{
								return;
							}
						sleep (1);
					}

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

void init_etime(struct itimerval *first);

double get_etime(struct itimerval *first);

int main(int argc, const char * argv[])
{
	if (argc < 3)
		{
			printUsage();
			return 1;
		}

	struct itimerval first;
	init_etime(&first);
	double secs;
	secs = get_etime(&first);

	try
		{
			string configFilename = argv[1];
			size_t secondsToRun  = stringToSize_t(argv[2]);

			AuroraNode * pNode = AuroraNode::createAuroraNode(configFilename, "-", argc, argv);

			pNode->xStartLoadGenerator();

			DelayedShutdownRunnable shutdownRunnable(* pNode, secondsToRun);

			RunnableRunner shutdownThread(shutdownRunnable);

			size_t bufSize = 1000000;
			void *tupleAddr = new char[bufSize];

			static int x;

			int outputPortId;
			int numTuples;

			// dpc: to gain performance, comment out this while loop
			while (pNode->deqBatch(outputPortId, numTuples, tupleAddr, bufSize))
				{
					x += numTuples;
					{
						cout << "++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
						cout << "+++++++++= Got tuples: outputPortId = " << outputPortId << ", x = " << x << endl;
						cout << "++++++++++++++++++++++++++++++++++++++++++++++++++" << endl << endl;
					}
				}

			if (pNode->xHasSchedulerRequestedShutdown())
				{
					cout << "testloadgen shutting down at scheduler's request." << endl;
					pNode->shutdown();
					shutdownRunnable._justQuitNow.post();
				}

			shutdownThread.join();
			delete pNode;

			cout << "***** FINAL: x = " << x << endl;
			printf("Time Spent in testloadgen: %f\n",get_etime(&first)-secs);
		}
	catch (const exception & e)
		{
			cout << "main(): Caught exception: " << e.what() << endl;
			return 1;
		}
	catch (...)
		{
			cout << "main(): Caught something not publicly derived from 'exception'" 
				 << endl;
			return 1;
		}

	return 0;
}

//===============================================================================

void init_etime(struct itimerval *first)
{
    first->it_value.tv_sec = 1000000;
    first->it_value.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL,first,NULL);
}

//===============================================================================

double get_etime(struct itimerval *first)
{
    struct itimerval curr;
    getitimer(ITIMER_VIRTUAL,&curr);
    return (double)(
					(first->it_value.tv_sec + (first->it_value.tv_usec*1e-6)) -
					(curr.it_value.tv_sec + (curr.it_value.tv_usec*1e-6)));
}

//===============================================================================
