#include <aurora.H>
#include <aurora_cxx.H>

#include <iostream>
#include <RuntimeGlobals.H>
#include <logutil.H>

//===============================================================================

AURORA_API_RETURN_CODE aurora_init(void ** ppAuroraInstance,
								   const char * pszPropsFilename)
{
	if ((pszPropsFilename == NULL) || (ppAuroraInstance == NULL))
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_init", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			(* ppAuroraInstance) = 
				AuroraNode::createAuroraNode(string(pszPropsFilename));
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_init", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_init", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}


	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_shutdown(void * pAuroraInstance)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_shutdown", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);
			pNode->shutdown();
			delete pNode;
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_shutdown", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_shutdown", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_getNumInputPorts(void * pAuroraInstance,
											   int * pNumInputPorts)
{
	if ((pAuroraInstance == NULL) || (pNumInputPorts == NULL))
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getNumInputPorts", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);

			vector<int> inPorts;
			vector<int> outPorts;
			pNode->listPorts(inPorts, outPorts);

			(* pNumInputPorts) = inPorts.size();
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getNumInputPorts", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getNumInputPorts", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_getInputPortNum(void * pAuroraInstance,
											  int portIndex, 
											  int * portName)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getInputPortNum", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);

			vector<int> inPorts;
			vector<int> outPorts;
			pNode->listPorts(inPorts, outPorts);

			if ((portIndex < 0) || (portIndex >= inPorts.size()))
				{
					return AURORA_PARAMETER_RANGE_ERROR;
				}

			(* portName) = inPorts.at(portIndex);
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getInputPortNum", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getInputPortNum", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_getNumOutputPorts(void * pAuroraInstance,
												int * pNumOutputPorts)
{
	if ((pAuroraInstance == NULL) || (pNumOutputPorts == NULL))
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getNumOutputPorts", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);

			vector<int> inPorts;
			vector<int> outPorts;
			pNode->listPorts(inPorts, outPorts);

			(* pNumOutputPorts) = outPorts.size();
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getNumOutputPorts", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getNumOutputPorts", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_getOutputPortNum(void * pAuroraInstance,
											   int portIndex, 
											   int * portName)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getOutputPortNum", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);

			vector<int> inPorts;
			vector<int> outPorts;
			pNode->listPorts(inPorts, outPorts);

			if ((portIndex < 0) || (portIndex >= outPorts.size()))
				{
					return AURORA_PARAMETER_RANGE_ERROR;
				}

			(* portName) = outPorts.at(portIndex);
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getOutputPortNum", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_getOutputPortNum", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_enqRecord(void * pAuroraInstance,
										int inputPortId,
										int streamId,  
										const void *tupleAddr, 
										size_t tupleSize)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_enqRecord", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);
			pNode->enqRecord(inputPortId, streamId, tupleAddr, tupleSize);
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_enqRecord", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_enqRecord", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_enqBatch(void       * pAuroraInstance,
									   int          inputPortName,
									   const void * pBuffer,
									   size_t       bufferSize,
									   size_t       numTuples)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_enqBatch", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);
			pNode->enqBatch(inputPortName, pBuffer,	bufferSize,	numTuples);
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_enqBatch", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_enqBatch", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_deqRecord(void * pAuroraInstance,
										int * wasDequeued, 
										int * outputPortId, 
										void *tupleAddr, 
										size_t bufSize)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_deqRecord", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);
			
			if (pNode->deqRecord(* outputPortId, tupleAddr, bufSize))
				{
					(* wasDequeued) = 1;
				}
			else
				{
					(* wasDequeued) = 0;
				}
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_deqRecord", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_deqRecord", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_deqBatch(void   * pAuroraInstance,
									   int    * wasDequeued, 
									   int    * outputPortName, 
									   int    * numTuples,
									   void   * pBuffer, 
									   size_t   bufSize)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_deqBatch", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);

			if (pNode->deqBatch(* outputPortName, * numTuples, pBuffer, bufSize))
				{
					(* wasDequeued) = 1;
				}
			else
				{
					(* wasDequeued) = 0;
				}
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_deqBatch", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_deqBatch", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_xStartLoadGenerator(void * pAuroraInstance)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_xStartLoadGenerator", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);
			pNode->xStartLoadGenerator();
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_xStartLoadGenerator", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_xStartLoadGenerator", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

AURORA_API_RETURN_CODE aurora_xStopLoadGenerator(void * pAuroraInstance)
{
	if (pAuroraInstance == NULL)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_xStopLoadGenerator", "pAuroraInstance is NULL", ""));

			return AURORA_NULL_PARAMETER_ERROR;
		}

	try
		{
			AuroraNode * pNode = reinterpret_cast<AuroraNode *>(pAuroraInstance);
			pNode->xStopLoadGenerator();
		}
	catch (exception & e)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_xStopLoadGenerator", "exception", e.what()));
			return AURORA_ERROR;
		}
	catch (...)
		{
			TraceLogger * pLogger = RuntimeGlobals::getTraceLogger();
			pLogger->log("c_api.errors", makeDebugLogLines("aurora_xStopLoadGenerator", "Caught \"...\"", ""));
			return AURORA_ERROR;
		}

	return AURORA_OK;
}

//===============================================================================

