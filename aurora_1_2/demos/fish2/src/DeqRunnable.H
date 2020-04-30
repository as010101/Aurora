#ifndef DEQRUNNABLE_H
#define DEQRUNNABLE_H

#include <Runnable.H>
#include <aurora_cxx.H>
#include <Model.H>
#include <ModelObserver.H>

class DeqRunnable : public Runnable, public ModelObserver
{
public:
	DeqRunnable(AuroraNode * pNode,
				Model * pModel);
	virtual ~DeqRunnable();
	virtual void run() throw ();

	void onAppShutdownChange(bool doChange);

private:
	AuroraNode * _pNode;
	Model * _pModel;
};

#endif
