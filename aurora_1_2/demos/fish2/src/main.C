#include <AuroraNode.H>
#include <iostream>
#include <Runnable.H>
#include <RunnableRunner.H>
#include <parseutil.H>
#include <unistd.h>
#include <sys/time.h>
#include <BinarySem.H>
#include <sys/time.h>
#include <time.h>
#include <util.H>
#include <sstream>
#include <TraceLogger.H>
#include <logutil.H>
#include <sstream>
#include <algorithm>

#include <Model.H>
#include <DataFeeder.H>
#include <DeqRunnable.H>


#include "FishObserver.H"
#include "FishWidget.H"
#include <qapplication.h>
#include <math.h>


using namespace std;

//===============================================================================

void printUsage()
{
	cout << "Usage: fish2 <xml-config-file>  <data-file>" << endl 
		 << endl;
}

//===============================================================================

int main(int argc, const char * argv[]) {
	if (argc != 3) {
		printUsage();
		return 1;
	}


	string configFilename   = argv[1];
	string dataFilename     = argv[2];

	try {
		AuroraNode * pNode = AuroraNode::createAuroraNode(configFilename);

		Model * pModel = new Model;

		QApplication app(argc, const_cast<char**>(reinterpret_cast<const char **>(argv)));
		FishWidget fish;
		fish.setModel(pModel);
		app.setMainWidget(&fish);
		fish.show();
		FishObserver fishObserver(&fish);
		//return app.exec();

		pModel->setFeederRunState(Model::FEEDER_RUNSTATE_PAUSED);
		pModel->setAppShutdown(false);
		
		//Add the gui as an observer
		pModel->addObserver(&fishObserver);

		DataFeeder * pFeeder = new DataFeeder(pNode, dataFilename, pModel);
		pModel->addObserver(pFeeder);
			
		DeqRunnable * pDequeuer = new DeqRunnable(pNode, pModel);
		pModel->addObserver(pDequeuer);

		{
			RunnableRunner deqThread(* pDequeuer);
			RunnableRunner feederThread(* pFeeder);

			app.exec();


			feederThread.join();

			cout << "*** DONE ENQUEUEING ***" << endl;

			deqThread.join();
		}


		cout << "main(): About to shutdown 2" << endl;
		delete pNode;
		delete pModel;
		delete pFeeder;
		delete pDequeuer;

		cout << "main(): About to shutdown 3" << endl;
	}
	catch (const exception & e)	{
		cout << "main(): Caught exception: " << e.what() << endl;
		return 1;
	}
	catch (...)	{
		cout << "main(): Caught something not publicly derived from 'exception'" 
			 << endl;
		return 1;
	}

	return 0;
}
