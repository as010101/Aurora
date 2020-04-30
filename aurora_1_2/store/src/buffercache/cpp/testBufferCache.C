#include <buffercache/include/BufferCache.H>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sstream>
#include <set>

//===============================================================================

void printUsage()
{
  cout << "Usage: testBufferCache <filedir> <maxConcurrency> <bytesPerPage> <numpages> <numFrames>" << endl;
}

//===============================================================================

// Expects the allocations to be immediately satisfiable...
void allocAndWritePages(BufferCache & bc, 
			BinarySem &attendBc,
			vector<PageAddr> & pages, 
			size_t numPages, 
			size_t bytesPerPage,
			char fillChar)
  throw (exception)
{
  bc._cb._availPages.setDesiredPageCount(numPages);

  vector<int> priorities;
  size_t pagesAllocated = 0;
  while (pagesAllocated < numPages)
    {
      PageAddr pa;
      bool success = bc._cb._availPages.getPageNonBlocking(pa, true);
      if (success)
	{
	  pages.push_back(pa);
	  priorities.push_back((pagesAllocated % 10)+1);
	  ++pagesAllocated;
	}
      else
	{
	  bc.awakenWorkerThreads();
	  attendBc.awaitPostThenClear();
	}
    }

  {
    LockHolder lh(bc._cb);
    bc._cb.setPriorities(pages, priorities);
    bc. awakenWorkerThreads();
  }

  set<PageAddr> unprocessedPages;
  for (size_t i = 0; i < numPages; ++i)
    {
      unprocessedPages.insert(pages[i]);
    }

  while (! unprocessedPages.empty())
    {
      {
	LockHolder lh(bc._cb);

	for (set<PageAddr>::iterator pos = unprocessedPages.begin();
	     pos != unprocessedPages.end();)
	  {
	    bool isMapped;
	    bool hasFrame;
	    bc._cb.lookupPageAddr(*pos, isMapped, hasFrame);
		
	    if (hasFrame && 
		(bc._cb._framedPages.getHighestAllowableUserLock(*pos) == FRAMELOCK_READWRITE))
	      {
		// A little bit of read->modify->write action...
		char * pBuffer = bc._cb._framedPages.setPageLockLevelReadWrite(*pos);
		memset(pBuffer, fillChar, bytesPerPage);
		bc._cb._framedPages.markPageDirty(*pos);
		bc._cb._framedPages.setPageLockLevelNone(*pos);
		bc._cb._framedPages.clearUserPriority(*pos);
		bc.awakenWorkerThreads();
		unprocessedPages.erase(pos++);
	      }
	    else
	      {
		++pos;
	      }
	  }
      }

      if (! unprocessedPages.empty())
	{
	attendBc.awaitPostThenClear();
	}
    }
}

//===============================================================================

void verifyPagesContent(BufferCache & bc, 
			BinarySem &attendBc,
			const vector<PageAddr> & pages, 
			size_t bytesPerPage,
			char fillChar)
  throw (exception)
{
  set<PageAddr> unprocessedPages(pages.begin(), pages.end());
  assert(unprocessedPages.size() == pages.size());

  // Cause the pages to be in memory. We may as well do this randomly...
  vector<int> priorities;
  priorities.reserve(pages.size());

  for (size_t i = 0; i < pages.size(); ++i)
    {
      priorities.push_back(10);
    }

  {
    LockHolder lh(bc._cb);
    bc._cb.setPriorities(pages, priorities);
  }

  while (! unprocessedPages.empty())
    {
      {
	LockHolder lh(bc._cb);

	for (set<PageAddr>::iterator pos = unprocessedPages.begin();
	     pos != unprocessedPages.end();)
	  {
	    bool isMapped;
	    bool hasFrame;
	    bc._cb.lookupPageAddr(*pos, isMapped, hasFrame);
		
	    if (hasFrame && 
		(bc._cb._framedPages.getHighestAllowableUserLock(*pos) >= FRAMELOCK_READONLY))
	      {
		const char * pBuffer = bc._cb._framedPages.setPageLockLevelReadOnly(*pos);

		for (size_t j = 0; j < bytesPerPage; ++j)
		  {
		    if (pBuffer[j] != fillChar)
		      {
			throw SmException(__FILE__, __LINE__, "Incorrect character found");
		      }
		  }

		bc._cb._framedPages.setPageLockLevelNone(*pos);
	 	bc._cb._framedPages.clearUserPriority(*pos);
		bc.awakenWorkerThreads();
		unprocessedPages.erase(pos++);
	      }
	    else
	      {
		++pos;
	      }
	  }
      }

      if (! unprocessedPages.empty())
	{
	attendBc.awaitPostThenClear();
	}
    }
}


//===============================================================================

void test1(string fileDir, size_t maxConcurrency, size_t bytesPerPage, 
	   size_t numPages, size_t numFrames)
  throw (exception)
{
  BinarySem attendBc, lazyWritesComplete;
  BufferCache bc(fileDir, bytesPerPage, maxConcurrency, numFrames, 
		 attendBc, lazyWritesComplete);

  {
    {
      LockHolder lh(bc._cb);
      bc._cb._availPages.setDesiredPageCount(5);
      bc.awakenWorkerThreads();
    }

    vector<PageAddr> newPages;
    allocAndWritePages(bc, attendBc, newPages, numPages, bytesPerPage, '$');
    verifyPagesContent(bc, attendBc, newPages,           bytesPerPage, '$');
    bc.beginShutdown();
    bc.awaitShutdown();
  }
} 
//===============================================================================

int main(int argc, const char * argv[])
{
  if (argc != 6)
    {
      printUsage();
      return 1;
    }

  string fileDir        = argv[1];
  size_t maxConcurrency = atoi(argv[2]);
  size_t bytesPerPage   = atoi(argv[3]);
  size_t numPages       = atoi(argv[4]);
  size_t numFrames      = atoi(argv[5]);

  try
    {
      test1(fileDir, maxConcurrency, bytesPerPage, numPages, numFrames);      
    }
  catch (exception & e)
    {
      cout << "main(): Caught exception: " << e.what() << endl;
      return 1;
    }

  cout << "***********************************************************" << endl
       << "*                       TESTS PASSED                      *" << endl
       << "***********************************************************" << endl;
}
