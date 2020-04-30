#ifndef RWOPRUNNER_H
#define RWOPRUNNER_H

#include <Runnable.H>
#include <StorageMgr_Exceptions.H>
#include <iolib/include/IOCB.H>
#include <iolib/int_include/PageDir.H>

/* 
   When this object is run(), it executes the specified IOCB. The results of the
   execution are specified in the appropriate IOCB fields.
 */
class RwOpRunner : public Runnable
{
public:
  RwOpRunner(PageDir & pd)
    throw (std::exception);

  virtual ~RwOpRunner();

  // Sets the IOCB that's to be executed by this object the next time its run()
  // method is called.
  //
  // The specified IOCB must have an _opType value of PAGE_READ or PAGE_WRITE.
  //
  // The specified IOCB object may not be accessed by any other thread while
  // this RwOpRunner is being run. Once this RwOpRunner no longer being run, 
  // it will no longer acccess the specified IOCB object.
  void setOp(IOCB * pIOCB)
    throw (std::exception);

  IOCB * getOp() const;

  void run() 
    throw ();

private:
  RwOpRunner(const RwOpRunner & rhs);
  RwOpRunner & operator= (const RwOpRunner & rhs);

  PageDir * _pPageDir;
  IOCB * _pIOCB;
};

#endif

