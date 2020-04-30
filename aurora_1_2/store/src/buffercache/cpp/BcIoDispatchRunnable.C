#include <buffercache/include/BcIoDispatchRunnable.H>
#include <buffercache/include/BufferCache.H>
#include <iostream>

BcIoDispatchRunnable::BcIoDispatchRunnable(BufferCache & bc)
  throw (exception)
  : _bc(bc)
{
}

//===============================================================================

BcIoDispatchRunnable::~BcIoDispatchRunnable()
    throw (exception)
{
}

//===============================================================================

void BcIoDispatchRunnable::run()
  throw ()
{
  try
    {
      _bc.dispatchIo();
    }
  catch (const exception & e)
    {
      cerr << endl 
	   << "BcIoDispatchRunnable::run(): Caught exception: " << endl 
	   << e.what() << endl;
      abort();
    }
}
