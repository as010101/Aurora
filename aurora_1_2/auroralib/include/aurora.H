#ifndef AURORA_H
#define AURORA_H

#include <sys/types.h>

/*
  This is the only file you should need to include to access Aurora's C API.

  Unless otherwise stated, these functions have the exact same meaning as the
  corresponding methods in the AuroraNode class.
*/

#ifdef __cplusplus
extern "C" {
#endif
	
	/* These error numbers are arbitrarily assigned, but we give exlicit values
       to help people look them up quickly.	*/
	typedef enum
		{
			AURORA_OK = 0,
			AURORA_ERROR = 1,
			AURORA_INVALID_PORT_INDEX = 2,
			AURORA_NULL_PARAMETER_ERROR = 3,
			AURORA_PARAMETER_RANGE_ERROR = 4
		} AURORA_API_RETURN_CODE;

	// Give the offsets for the standard fields in the tuple...
	enum 
		{
			TUPLE_OFFSET_TIMESTAMP = 0,
			TUPLE_OFFSET_STREAM_ID = 8,
			TUPLE_OFFSET_USER_DATA = 12
		};
	
	/* Creates a new AuroraNode, referenced by pAuroraInstance. This function
	   sets (* ppAuroraInstance) to have the instance pointer. */
	AURORA_API_RETURN_CODE aurora_init(void ** ppAuroraInstance,
									   const char * pszPropsFilename);

	/* Like the C++ equivalent, but also destroys the Aurora instance. */
	AURORA_API_RETURN_CODE aurora_shutdown(void * pAuroraInstance);




	/* --------------------------------------------------------------------------
	   These functions replace the C++ AuroraNode::listPorts(...) method. We go
	   this style of interface to make it easy to build a Java Native Interface
	   wrapper on top of this code. */

	/* Sets (* pNumInputPorts) to the count of input ports this Aurora instance 
	   has */
	AURORA_API_RETURN_CODE aurora_getNumInputPorts(void * pAuroraInstance,
												   int * pNumInputPorts);

	/* Sets (* portName) to the (numeric) name of the input port whose index is
	   portIndex. portIndex is just an artifact of this API. It must be a number
	   where 0 <= portIndex < (* pNumInputPorts). */
	AURORA_API_RETURN_CODE aurora_getInputPortNum(void * pAuroraInstance,
												  int portIndex, 
												  int * portName);

	/* Sets (* pNumOutputPorts) to the count of output ports this Aurora instance 
	   has */
	AURORA_API_RETURN_CODE aurora_getNumOutputPorts(void * pAuroraInstance,
													int * pNumOutputPorts);

	/* Sets (* portName) to the (numeric) name of the output port whose index is
	   portIndex. portIndex is just an artifact of this API. It must be a number
	   where 0 <= portIndex < (* pNumOutputPorts). */
	AURORA_API_RETURN_CODE aurora_getOutputPortNum(void * pAuroraInstance,
												   int portIndex, 
												   int * portName);

	/*-------------------------------------------------------------------------*/

	AURORA_API_RETURN_CODE aurora_enqRecord(void * pAuroraInstance,
											int inputPortName,
											int streamId,  
											const void *tupleAddr, 
											size_t tupleSize);

	AURORA_API_RETURN_CODE aurora_enqBatch(void       * pAuroraInstance,
										   int          inputPortName,
										   const void * pBuffer,
										   size_t       bufferSize,
										   size_t       numTuples);


	/* The C++ method version of this function had a bool return code. However,
	   we added a first parameter to this function to return that value, so that
	   the actual return code of this function could report any errors.

	   (wasDequeued == 1) is like the C++ method returning true
	   (wasDequeued == 0) is like the C++ method returning false
	*/
	AURORA_API_RETURN_CODE aurora_deqRecord(void   * pAuroraInstance,
											int    * wasDequeued, 
											int    * outputPortName, 
											void   * tupleAddr, 
											size_t   bufSize);

	/*
	  Caller sets: 
	     pAuroraInstance
		 pBuffer
		 bufSize

	  Function sets:
	     *wasDequeued
		 *outputPortName
		 *numTuples
	 */
	AURORA_API_RETURN_CODE aurora_deqBatch(void   * pAuroraInstance,
										   int    * wasDequeued, 
										   int    * outputPortName, 
										   int    * numTuples,
										   void   * pBuffer, 
										   size_t   bufSize);

	AURORA_API_RETURN_CODE aurora_xStartLoadGenerator(void * pAuroraInstance);

	AURORA_API_RETURN_CODE aurora_xStopLoadGenerator(void * pAuroraInstance);

#ifdef __cplusplus
}
#endif

#endif
