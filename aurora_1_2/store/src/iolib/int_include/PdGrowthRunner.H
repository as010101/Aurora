#ifndef PDGROWTHRUNNER_H
#define PDGROWTHRUNNER_H

#include <Runnable.H>
#include <StorageMgr_Exceptions.H>
#include <iolib/include/IOCB.H>
#include <iolib/int_include/PageDir.H>

/* 
   When this object is run(), it tries to grow the PageDir's storage.The results of the
   execution are specified in this object's run exception.
 */
class PdGrowthRunner : public Runnable
{
public:
  PdGrowthRunner(PageDir & pd)
    throw (std::exception);

  virtual ~PdGrowthRunner();

  void run() 
    throw ();

private:
  PageDir & _pageDir;
};

#endif

