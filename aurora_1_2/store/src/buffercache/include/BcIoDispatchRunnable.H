#ifndef BCIODISPATCHRUNNABLE_H
#define BCIODISPATCHRUNNABLE_H

#include <Runnable.H>

class BufferCache;

class BcIoDispatchRunnable : public Runnable
{
public:
  BcIoDispatchRunnable(BufferCache & bc)
    throw (exception);

  virtual ~BcIoDispatchRunnable()
    throw (exception);

  void run()
    throw ();

private:
  BufferCache & _bc;
};

#endif
