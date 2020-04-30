#include <iolib/int_include/RwOpRunner.H>
#include <iostream>

//===============================================================================

RwOpRunner::RwOpRunner(PageDir & pd)
  throw (std::exception) :
  _pPageDir(& pd),
  _pIOCB(NULL)
{
}

//===============================================================================

RwOpRunner::~RwOpRunner()
{
}

//===============================================================================

void RwOpRunner::setOp(IOCB * pIOCB)
  throw (std::exception)
{
  if ((pIOCB != NULL) && (pIOCB->_opType != IOCB::PAGE_READ) && (pIOCB->_opType != IOCB::PAGE_WRITE))
    {
      throw SmIllegalParameterValueException(__FILE__, __LINE__, 
						       "pIOCB isn't PAGE_READ or PAGE_WRITE");
    }

  _pIOCB = pIOCB;
}

//===============================================================================

IOCB * RwOpRunner::getOp() const
{
  return _pIOCB;
}


//===============================================================================

void RwOpRunner::run() 
  throw ()
{
  try
    {
      if (_pIOCB == NULL)
	{
	  throw SmException(__FILE__, __LINE__, "_pIOCB == NULL");
	}
      
      ftime(& (_pIOCB->_startTime));
      
      if (_pIOCB->_opType == IOCB::PAGE_READ)
	{
	  _pPageDir->readPage(_pIOCB->_pageAddr, _pIOCB->_pBuffer);
	}
      else if (_pIOCB->_opType == IOCB::PAGE_WRITE)
	{
	  _pPageDir->writePage(_pIOCB->_pageAddr, _pIOCB->_pBuffer);
	}
      else
	{
	  throw SmInternalException(__FILE__, __LINE__, "Somehow _opType got an illegal value");
	}

      ftime(& (_pIOCB->_endTime));
    }
  catch (SmException & e)
    {
      _pIOCB->_pException = new SmException(e);
    }
  catch (std::exception & e)
    {
      _pIOCB->_pException = new SmException(__FILE__, __LINE__, e.what());
    }
  catch (...)
    {
      SmException * pException = 
	new SmException(__FILE__, __LINE__, 
				  "RwOpRunner::run(): Caught an exception that wasn't a std::exception");
      setRunException(pException);
    }
}

//===============================================================================

RwOpRunner::RwOpRunner(const RwOpRunner & rhs) 
{
  assert(false);
}

//===============================================================================

RwOpRunner & RwOpRunner::operator= (const RwOpRunner & rhs) 
{
  assert(false); 
  return *this;
}

//===============================================================================
