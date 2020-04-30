#ifndef AURORANODE_H
#define AURORANODE_H

#include <string>
#include <exception>
#include <vector>

using namespace std;

class TraceLogger;

/*
  OVERVIEW:
  AuroraNode is the interface to a single, local Aurora flow network. An instance 
  of this class does all of the runtime processing traditionally associated with 
  Aurora, except for puting data in to / out of the local flow network - that's the
  responsibility of the application using this library.

  USE:
  Below is the expected normal use of the AuroraNode:
  1. Instantiate the AuroraNode by calling AuroraNode::createAuroraNode(...)
  2. run for a while, doing enqueueing and dequeueing.
  3. call shutdown()
  4. Wait for any pending invocations of deqRecord(...) to return.
  5. delete the AuroraNode.

  
  MULTITHREADING:
  Allowed concurrency:
  (1) Call any method from any thread.

  (2) You may only have one invocation of 'deqRecord(...)' pending at a time.

  (3) Regardless of whether or not there's a pending invocation of
  'deqRecord(...)', you may have as many pending invocations of 
  'enqRecord(...)' as you want at a given moment. However, those invocations
  might be serialize by this class anyway, so don't assume that you'll definitely
  get performance increases by having concurrent invocations of 'enqRecord(...)'.

  (4) When you call 'shutdown(...)', you may not have any pending 
  invocations of 'enqRecord(...)' or ' listPublicQueues(...)'.

  (5) When you call listPublicQueues(...), you may not have any other method
  invocation pending on this object at the time.

  Note that these limitations are mostly arbitrary and for ease of implementation.
  They can be revisited if there's a real need. -cjc, 10 Feb 2003.


  MULTIPLE INSTANTIATION:
  As far as we know, any multiple instances of the AuroraNode class may be 
  concurrently instantiated within a single program. However, we they must each
  use an entirely different set of files (probably), and we haven't actually
  tested that this works.
*/

class AuroraNode
{
public:

	virtual ~AuroraNode();

	// Give the offsets for the standard fields in the tuple...
	enum 
		{
			TUPLE_OFFSET_TIMESTAMP = 0,
			TUPLE_OFFSET_STREAM_ID = 8,
			TUPLE_OFFSET_USER_DATA = 12
		};


	// Starts up the aurora node with the specified details.
	static AuroraNode * createAuroraNode(string propsFilename)
		throw (exception);

	// Like the other constructor, but uses 'prefix', 'argc', and 'argv'
	// as parameters to the PropsFile.  See the documentation in PropsFile.H
	// for how this affects the system.
	static AuroraNode * createAuroraNode(string propsFilename, 
										 string prefix, 
										 int argc, 
										 const char * argv[])
		throw (exception);

	// You must call this before you call this object's destructor, *** and then
	// wait for any pending invocations of deqRecord(...) to return ***
	// If you don't exercise that patience in destroying this object, the states of
	// all the files used by Aurora are undefined.
	//
	// This method terminates any waiting deqRecord(...) invocations, and makes all
	// future method calls throw exceptions (except invocations of this method or
	// or this object's destructor).  
	virtual void shutdown()
		throw (exception) = 0;

	// Obtain a list of IDs of all input and output ports in this Aurora node.
	// Clears each of the supplied vectors first.
	virtual void listPorts(vector<int> & in, vector<int> & out) const
		throw (exception) = 0;

	// Given a valid output port port#, this will return the total number of
	// bytes needed to store the tuples coming from that port. The size includes
	// Aurora's mandatory header fields.
	virtual size_t getTupleSizeByOutputPort(int portNum) const
		throw (exception) = 0;

	// Returns true iff at least one tuple is available for dequeueing.
	virtual bool isDeqTupleAvailable()
		throw (exception) = 0;

	// Enqueue a tuple to an input port. This method blocks until the tuple is enqueued.
	// This will throw an exception of 'tupleSize' doesn't match the specified
	// queue's tuple size (in bytes).
	//
	// NOTE ABOUT TUPLE LAYOUT:
	//    The first 12 bytes of each tuple are populated by this method. The
	//    fields that you specified in the Aurora GUI for a given tuple actually
	//    start at the 13th byte of the tuple.
	//
	//    You should use the enumeration values defined above (TUPLE_OFFSET_...) in 
	//    your code when deciding where to place data.
	//
	//    This means that for a tuple that appears in the GUI to only have one
	//    int field, you must supply a 16 byte buffer to this method:
	//    (12 bytes overhead) + (4 bytes for the int).
	//
	//    Furthermore, when you populate the int's value in the tuple, you must
	//    store it at tupleAddr[12...15].
	//
	//    It doesn't matter what the contents of the first 12 bytes are when you
	//    call this method. They're blindly overwritten by this method.
	virtual void enqRecord(int inputPortId,
						   int streamId,  
						   const void *tupleAddr, 
						   size_t tupleSize)
		throw (exception) = 0;


	// Caller must provide full tuples, including the system-private header fields.
	// (See TUPLE_OFFSET_USER_DATA above.)  Control returns when they've all been
	// enqueued.
	//
	// It must be the case that (numTuples * tuplesize) <= bufferSize.
	// (tupleSize is calculated by the API when this method is called.)
	//
	// The 'streamId' field is preserved if it's been set in the provided tuples,
	// but if so it's the caller's responsible to have the streamId occupy the 
	// correct location with in the tuple header.
	virtual void enqBatch(int inputPortId,
						  const void * pBuffer,
						  size_t bufferSize,
						  size_t numTuples)
		throw (exception) = 0;

	// Dequeue some tuple from any of the AuroraNode's output ports. If bufSize 
	// isn't big enough to handle the tuple being delivered, an exception is 
	// thrown. (Exceptions might also be thrown for other reasons.)
	//
	// If a tuple was dequeued, this returns 'true', and sets the 'queueId' 
	// parameter to the Id of the queue from which the tuple was dequeued.
	//
	// If no tuple was dequeued, this returns false and doesn't modify
	// 'qviewId'. This would only happen if 'shutdown()' was called while this
	// method invocation was pending, or the scheduler requested a shutdown to
	// occur while this method was pending.
	//
	// NOTE ABOUT TUPLE LAYOUT:
	//   After this method returns... 
	//
	//   tupleAddr[0...7] contain a 'timeval' struct, which is the timestamp of the
	//   tuple.
	//
	//   tupleAddr[8...11] contain an 'int' value, which gives the streamId of the
	//   tuple.
	//
	//   tupleAddr[12...] contain the user-specified fields for this tuple.
	virtual bool deqRecord(int & outputPortId, 
						   void *tupleAddr, size_t bufSize)
		throw (exception) = 0;


	virtual bool deqBatch(int & outputPortId, int & numTuples, void *pBuffer, size_t bufSize)
		throw (exception) = 0;


	//-----------------------------------------------------------------------------
	// Research APIs. Not necessarily stable or bug-free...
	//-----------------------------------------------------------------------------

	// Start's a thread that enqueues tuples as prescribed by the config file. You
	// must not call this if the load generator is currently running.
	virtual void xStartLoadGenerator()
		throw (exception) = 0;

	// Stops the load-generator thread. If the load generator is started, then you
	// must call this before calling shutdown().
	virtual void xStopLoadGenerator()
		throw (exception) = 0;

	// True whenn the scheduler would like the user to call shutdown().  Note that
	// the onus still lies on the application to actually call the shutdown()
	// method.  All the scheduler did was to express a desire.
	virtual bool xHasSchedulerRequestedShutdown()
		throw (exception) = 0;

 	// Wait for the Scheduler (Aurora engine) to finish its work
 	virtual void schedulerFinishedWait() = 0;

	// The TraceLogger used by Aurora runtime.  You can use this too.
	virtual TraceLogger & xGetTraceLogger()
		throw (exception) = 0;

};

#endif
