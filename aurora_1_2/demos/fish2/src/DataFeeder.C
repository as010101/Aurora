#include <DataFeeder.H>
#include <DataFileReader.H>
#include <I0Tuple.H>
#include <iostream>

DataFeeder::DataFeeder(AuroraNode * pNode,
					   string dataFilePath,
					   Model * pModel)
{
	_pNode = pNode;
	_pModel = pModel;

	int rc = pthread_cond_init(& _cond, NULL);
	assert(rc == 0);

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(& attr);

	rc = pthread_mutexattr_settype(& attr, PTHREAD_MUTEX_RECURSIVE_NP);
	assert(rc == 0);

	rc = pthread_mutex_init(& _mtx, & attr);
	assert(rc == 0);

	_pReader = new DataFileReader(dataFilePath);
}

//===============================================================================

DataFeeder::~DataFeeder()
{
	delete _pReader;
}

//===============================================================================

void DataFeeder::onFeederRunStateChange(Model::FEEDER_RUNSTATE newState)
{
	int rc = pthread_cond_broadcast(& _cond);
	assert(rc == 0);
}

//===============================================================================

void DataFeeder::onEnqDelayChange(unsigned long delayMilliseconds)
{
	int rc = pthread_cond_broadcast(& _cond);
	assert(rc == 0);
}

//===============================================================================

void DataFeeder::onAppShutdownChange(bool doShutdown)
{
	int rc = pthread_cond_broadcast(& _cond);
	assert(rc == 0);
}

//===============================================================================

void DataFeeder::run() throw()
{
	// Feed tuples to Aurora until we're out of tuples, or until we're told that
	// the run-state is FEEDER_RUNSTATE_FINISHED...

	DataFileParagraph dhp;

	int lastGoodPi = -1;
	int wsPortNum = 0;
	int numTuplesEnq = 0;

	if (! _pReader->getNextParagraph(dhp)) {
		_pModel->setFeederRunState(Model::FEEDER_RUNSTATE_FINISHED);
	}

	while (_pModel->getFeederRunState() != Model::FEEDER_RUNSTATE_FINISHED) {
		Model::FEEDER_RUNSTATE state = wait();

		if (state == Model::FEEDER_RUNSTATE_ACTIVE)	{

			// Getting the next paragraph doesn't take long, so it's ok to do that
			// even if the user did specify that the enqueuing has 
			if (! _pReader->getNextParagraph(dhp)) {
				_pModel->setFeederRunState(Model::FEEDER_RUNSTATE_FINISHED);
			}

			if (_pModel->getFeederRunState() != Model::FEEDER_RUNSTATE_FINISHED) {
				I0Tuple * pEnqTuple = new I0Tuple;

				pEnqTuple->_timeOfSample                   = dhp._sampleTime;
				pEnqTuple->_printerInterval                = dhp._printInterval;
				pEnqTuple->_temperatureCelcius             = dhp._temperatureCelcius;
				pEnqTuple->_pH                             = dhp._pH;
				pEnqTuple->_conductivity_mS_per_cm         = dhp._conductivity_mS_per_cm;
				pEnqTuple->_disolvedO2_mg_per_l            = dhp._disolvedO2_mg_per_l;
				pEnqTuple->_num_ooc_vent_rates             = dhp._numOocVentilatoryRates;
				pEnqTuple->_num_ooc_voltages               = dhp._numOocVoltages;
				pEnqTuple->_num_ooc_cough_rates            = dhp._numOocCoughRates;
				pEnqTuple->_num_ooc_percent_body_movements = dhp._numOocPercentBodyMovements;
				pEnqTuple->_num_dead                       = dhp._numDeadFish;

				if (dhp._numOocVentilatoryRates + 
					dhp._numOocVoltages + 
					dhp._numOocCoughRates + 
					dhp._numOocPercentBodyMovements + 
					dhp._numDeadFish > 6) {
					pEnqTuple->_is_ooc[0] = 'Y';
				}
				else {
					pEnqTuple->_is_ooc[0] = 'N';
					lastGoodPi = dhp._printInterval;
				}

				pEnqTuple->_last_good_printerInterval = lastGoodPi;

				//if (pEnqTuple->_is_ooc[0] == 'Y') {
				//	cout << "ENQUEUEING: " << pEnqTuple->toString() << endl << endl;
				//}

				_pNode->enqRecord(wsPortNum, 0, pEnqTuple, sizeof(*pEnqTuple));
				_pModel->setNumTuplesEnq(++ numTuplesEnq);
				cout << numTuplesEnq << endl;
			}		
		}
	}
}

//===============================================================================

Model::FEEDER_RUNSTATE DataFeeder::wait() {
	int rc = pthread_mutex_lock(& _mtx);
	assert(rc == 0);

	Model::FEEDER_RUNSTATE prevState = _pModel->getFeederRunState();

	if (prevState == Model::FEEDER_RUNSTATE_FINISHED) {
		rc = pthread_mutex_unlock(& _mtx);
		assert(rc == 0);
		return Model::FEEDER_RUNSTATE_FINISHED;
	}

	// Calculate the latest we want to sleep until...
	timeval timeBeforeSleep;
	gettimeofday(& timeBeforeSleep, NULL);

	bool delayComplete = false;

	while (true) {
		if (prevState == Model::FEEDER_RUNSTATE_PAUSED) {
			rc = pthread_cond_wait(& _cond, & _mtx);
			assert(rc == 0);
		}
		else if (prevState == Model::FEEDER_RUNSTATE_ACTIVE) {
			// Calculate delay inside the loop, so that if the value changes we'll
			// notice...
			unsigned long enqDelayMs = _pModel->getEnqDelay();

			timeval tv =
				makeNormalTimeval(timeBeforeSleep.tv_sec  + (enqDelayMs / 1000),
								  timeBeforeSleep.tv_usec + (enqDelayMs % 1000) * 1000);

			timespec nextEnqTime;
			nextEnqTime.tv_sec = tv.tv_sec;
			nextEnqTime.tv_nsec = tv.tv_usec * 1000;

			rc = pthread_cond_timedwait(& _cond, & _mtx, & nextEnqTime);
			delayComplete = (rc == ETIMEDOUT);
		}

		Model::FEEDER_RUNSTATE newState = _pModel->getFeederRunState();
		if (newState == Model::FEEDER_RUNSTATE_FINISHED) {
			rc = pthread_mutex_unlock(& _mtx);
			assert(rc == 0);
			return Model::FEEDER_RUNSTATE_FINISHED;
		}

		if ((prevState == Model::FEEDER_RUNSTATE_PAUSED) &&
			(newState == Model::FEEDER_RUNSTATE_ACTIVE)) {
			rc = pthread_mutex_unlock(& _mtx);
			assert(rc == 0);
			return Model::FEEDER_RUNSTATE_ACTIVE;
		}

		if ((prevState == Model::FEEDER_RUNSTATE_ACTIVE) &&
			(newState == Model::FEEDER_RUNSTATE_ACTIVE) &&
			delayComplete) {
			rc = pthread_mutex_unlock(& _mtx);
			assert(rc == 0);
			return Model::FEEDER_RUNSTATE_ACTIVE;
		}
            
		prevState = newState;
	}
}
