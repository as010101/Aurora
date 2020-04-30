#include <DeqRunnable.H>
#include <O0Tuple.H>
#include <iostream>

DeqRunnable::DeqRunnable(AuroraNode * pNode,
						 Model * pModel)
{
	_pNode = pNode;
	_pModel = pModel;
}

//===============================================================================

DeqRunnable::~DeqRunnable()
{
}

//===============================================================================

void DeqRunnable::run() throw ()
{
	try	{
		size_t bufferSize = sizeof(O0Tuple);

		int outputPortId;
		char * pBuffer = new char[bufferSize];

		while (_pNode->deqRecord(outputPortId, pBuffer, bufferSize)) {
			if (outputPortId == 72)
				{
					O0Tuple * pTuple = reinterpret_cast<O0Tuple *>(pBuffer);
					_pModel->addNewOocEvent(pTuple);

					cout << pTuple->toString() << endl << endl;
					pBuffer = new char[bufferSize];
				}
			else
				{
					assert(false);
				}
		}

		if (_pNode->xHasSchedulerRequestedShutdown()) {
			cout << "Scheduler requested a shutdown. Going down now." << endl;
			_pModel->setAppShutdown(true);
		}
	}
	catch (exception & e) {
		cout << "DeqRunnable::run(): Caught exception " << e.what() << endl;
		abort();
	}
}

//===============================================================================

void DeqRunnable::onAppShutdownChange(bool doShutdown)
{
	if (doShutdown) {
		_pNode->shutdown();
	}
}

//===============================================================================
