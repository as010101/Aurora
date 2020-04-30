#include <iolib/int_include/PdGrowthRunner.H>
#include <iostream>

//===============================================================================

PdGrowthRunner::PdGrowthRunner(PageDir & pd)
  throw (std::exception) :
  _pageDir(pd)
{
}

//===============================================================================

PdGrowthRunner::~PdGrowthRunner()
{
}

//===============================================================================

void PdGrowthRunner::run() 
  throw ()
{
  try
    {
      _pageDir.growStorage();
    }
  catch (std::exception & e)
    {
      cloneAndSetRunException(e);
    }
  catch (...)
    {
      SmException * pException = 
	new SmException(__FILE__, __LINE__, 
				  "PdGrowthRunner::run(): Caught an exception that wasn't a std::exception");
      setRunException(pException);
    }
}

//===============================================================================

