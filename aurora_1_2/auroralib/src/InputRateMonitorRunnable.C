#include <InputRateMonitorRunnable.H>
#include <AuroraNodeImpl.H>

//===============================================================================

InputRateMonitorRunnable::InputRateMonitorRunnable(AuroraNodeImpl & ani)
	throw (exception) :
	_ani(ani)
{
}

//===============================================================================

InputRateMonitorRunnable::~InputRateMonitorRunnable()
	throw ()
{
}

//===============================================================================

void InputRateMonitorRunnable::run()
	throw ()
{
	try
		{
			_ani.maintainDdmStats();
		}
	catch (exception & e)
		{
			cloneAndSetRunException(e);

			// For now, a more expedient shutdown is needed, because the user
			// otherwise couldn't discover this problem until he called
			// shutdown().  (And even if that did happen, we don't have the code
			// in shutdown() to test this Runnable's exception.)
			cerr << "AuroraNodeImpl::maintainDdmStats() threw an exception: " << endl
				 << e.what() << endl;
			abort();
		}
}

//===============================================================================


