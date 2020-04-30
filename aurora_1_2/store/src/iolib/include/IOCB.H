#ifndef IOCB_H
#define IOCB_H

#include <StorageMgr_Exceptions.H>
#include <iolib/include/PageAddr.H>
#include <sys/timeb.h>

// I/O Control Block - Describes a desired I/O operation, and, when complete, the 
// results of that operation
struct IOCB
{
  IOCB();
  virtual ~IOCB();

  typedef enum 
    {
      PAGE_ALLOC,
      PAGE_READ,
      PAGE_WRITE,
      PAGE_FREE,
      INVALID_OPCODE // A sentinel value to help us detect uninitialized IOCBs.
    } OPCODE;

  OPCODE _opType;
  PageAddr _pageAddr;
  char *_pBuffer;
  SmException *_pException;

  // These are just defined sometimes, when we want to collect stats...

  // When execution of this IOCB began (later than when it's queued). Only defined on 
  // PAGE_READ / PAGE_WRITE ops.
  struct timeb _startTime; 

  // When the execution of this IOCB ended (earlier than when it's placed in the reply queue).
  // Only defined on PAGE_READ / PAGE_WRITE ops.
  struct timeb _endTime;

  // Only defined for R/W ops: The number of other operations (reads, writes, and fil growths)
  // executing when this op began execution.
  unsigned short _opsRunningWhenStarted;

  string debugStr() const;

private:
  IOCB(const IOCB & rhs) {assert(false);}
  IOCB & operator= (const IOCB & rhs) {assert(false); return *this;}
};

//********************************************************************************************
//                                           Use
//********************************************************************************************
// READ/WRITE OPERATIONS:
//    WHEN SUBMITTED:
//       _opType: PAGE_READ or PAGE_WRITE
//       _pageAddr: Address of the page to operate on.
//       _pBuffer: User's buffer for the operation.
//            (Must be at least SM_PAGE_BYTES bytes long).
//       _pException: (Ignored)
//    WHEN COMPLETED:
//       _opType, _pageAddr, _pBuffer: Same as they were when submitted.
//       _pException: If NULL, then operation was successful. Otherwise,
//            points to an Exception describing the problem. The user is
//            responsible for deleting the pointer, using the 'delete' operator.
//
//  PAGE_ALLOC OPERATION:
//    WHEN SUBMITTED:
//       _opType: PAGE_ALLOC
//       _pageAddr, _pBuffer, _pException: (Ignored)
//    WHEN COMPLETED:
//       _opType, _pBuffer: Same as they were when submitted.
//       _pageAddr: If _pException is NULL, then this is the address of
//            the page that was allocated. If _pException is NOT NULL, the meaning of 
//            this field's value is undefined.
//       _pException: If NULL, then operation was successful. Otherwise,
//            points to an Exception describing the problem. The user is
//            responsible for deleting the pointer, using the 'delete' operator.
//
//  PAGE_FREE OPERATION:
//    WHEN SUBMITTED:
//       _opType: PAGE_FREE
//       _pageAddr: The page to free
//       _pBuffer, _pException: (Ignored)
//    WHEN COMPLETED:
//       _opType, _pBuffer, _pageAddr: Same as they were when submitted.
//       _pException: If NULL, then operation was successful. Otherwise,
//            points to an Exception describing the problem. The user is
//            responsible for deleting the pointer, using the 'delete' operator.
//********************************************************************************************
//********************************************************************************************

#endif
