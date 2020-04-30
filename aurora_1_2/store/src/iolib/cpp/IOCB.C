#include <iolib/include/IOCB.H>
#include <sstream>

//===============================================================================

IOCB::IOCB()
  : _opType(INVALID_OPCODE),
    _pBuffer(NULL),
    _pException(NULL)
{
}

//===============================================================================

IOCB::~IOCB()
{
}

//===============================================================================

string IOCB::debugStr() const
{
  ostringstream s;
  s << "IOCB(" << this << "):" << endl
    << "\t_opType = ";

  switch(_opType)
    {
    case PAGE_ALLOC:
      s << "PAGE_ALLOC";
      break;
    case PAGE_READ:
      s << "PAGE_READ";
      break;
    case PAGE_WRITE:
      s << "PAGE_WRITE";
      break;
    case PAGE_FREE:
      s << "PAGE_FREE";
      break;
    case INVALID_OPCODE:
      s << "INVALID_OPCODE";
      break;
    default:
      s << "<< Totally invalid _opType value >>";
    }

  s << endl
    << "\t_pageAddr = " << _pageAddr.getDebugInfo() << endl
    << "\t_pBuffer = " << reinterpret_cast<const void *>(_pBuffer) << endl
    << "\t_pException = " << _pException << endl;

  return s.str();
}

//===============================================================================
