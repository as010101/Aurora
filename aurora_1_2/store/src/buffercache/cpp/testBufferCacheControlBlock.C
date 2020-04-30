#include <buffercache/include/BufferCacheControlBlock.H>

#include <exception>
#include <iostream>
#include <sstream>

using namespace std;

//===============================================================================

void test1()
  throw (exception)
{
  /*
   * This is broken because I changed the interface to PagesWithFramesMap
   * a little bit. The test should be updated soon. -CJC

  cout << "Test 1: Simple object counting" << endl;

  BufferCacheControlBlock cb(10, 60);

  vector<PageAddr> dummyPages;
  vector<int> dummyPriorities;

  for (int i = 0; i < 20; i++)
    {
      dummyPages.push_back(PageAddr(1, i));
      dummyPriorities.push_back((i % 10) + 1);
    }

  // Make sure exactly those pages are mapped that are supposed to be...
  cb.setPriorities(dummyPages, dummyPriorities);

  bool isMapped;
  bool hasFrame;

  for (int i = 0; i < 20; i++)
    {
      cb.lookupPageAddr(PageAddr(1, i), isMapped, hasFrame);

      if (! isMapped)
	{
	  throw SmException(__FILE__, __LINE__, "! isMapped");
	}
    }

  cb.lookupPageAddr(PageAddr(0, 0), isMapped, hasFrame);
  if (isMapped || hasFrame)
    {
      throw SmException(__FILE__, __LINE__, "isMapped || hasFrame");
    }

  cout << "Test 1: *** SUCCESS ***" << endl << endl;

  */
}

//===============================================================================

// Ensures that every page in the specified map has the specified state and
// priority.
void verifyPageMapContent(const map<PageAddr, PagesWithFramesMap::FramedPageInfo> & pageMap,
			 int startPageNum, 
			 int endPageNum, 
			 int effectivePriority,
			  // FRAME_LOCK_TYPE userLock,
			 FRAME_IO_STATE ioState,
			 FRAME_BUFFER_STATE bufferState)
  throw (exception)
{
    map<PageAddr, PagesWithFramesMap::FramedPageInfo>::const_iterator mapPos = pageMap.begin();
    map<PageAddr, PagesWithFramesMap::FramedPageInfo>::const_iterator mapEnd = pageMap.end();

    while (mapPos != mapEnd)
      {
	const PageAddr & pa = mapPos->first;
	const PagesWithFramesMap::FramedPageInfo & fpi = mapPos->second;

	if ((pa._pageNum < startPageNum) || (pa._pageNum > endPageNum))
	  {
	    throw SmException(__FILE__, __LINE__, "Unacceptable page nubmer found");
	  }
      
	if (//(fpi._userLock    != userLock)  ||
	    (fpi._ioState     != ioState) ||
	    (fpi._bufferState != bufferState))
	  {
	    ostringstream errorMsg;
	    errorMsg << "A pinned page was in the wrong state:" << endl
		     << fpi.getDebugInfo();
	    throw SmException(__FILE__, __LINE__, errorMsg.str());
	  }

	if (fpi.getEffectivePriority() != effectivePriority)
	  {
	    throw SmException(__FILE__, __LINE__,
					"fpi.getEffectivePriority() != effectivePriority");
	  }

	++mapPos;
      }
}


//===============================================================================

void test2()
  throw (exception)
{
  /*
   * This is broken because I changed the interface to PagesWithFramesMap
   * a little bit. The test should be updated soon. -CJC

  cout << "Test 2: Verifying state transitions for page loading" << endl;

  BufferCacheControlBlock cb(10, 60);

  vector<PageAddr> dummyPages;
  vector<int> dummyPriorities;

  for (int i = 0; i < 20; i++)
    {
      dummyPages.push_back(PageAddr(1, i));
      dummyPriorities.push_back((i / 2) + 1);
    }

  cb.setPriorities(dummyPages, dummyPriorities);

  // Pages (1, 14)...(1,19) should be recommended for loading, because they're
  // the ones with the 6 top priorities...
  vector<PageAddr> pagesToLoad;
  vector<char *> frames;
  cb.beginFrameLoadOps(6, pagesToLoad, frames);

  if (pagesToLoad.size() != 6)
    {
      throw SmException(__FILE__, __LINE__, "Wrong # of pages to load.");
    }

  for (int i = 0; i < 6; i++)
    {
      if ((pagesToLoad[i]._pageNum < 14) || (pagesToLoad[i]._pageNum > 19))
	{
	  throw SmException(__FILE__, __LINE__, "A wrong page was suggested.");
	}
    }

  // Verify that all pinned pages are in the proper state...
  const set<PageAddr> & pinnedPages = cb._framedPages.getPagesByEffectivePriority(11);

  bool findByPageAddr(const PageAddr & pa, FramedPageInfo & pfi) const
    throw (exception);


  const map<PageAddr, PagesWithFramesMap::FramedPageInfo> & fpPinnedMap = cb._framedPages.getFrameMap(11);
  if (fpPinnedMap.size() != 6)
    {
      throw SmException(__FILE__, __LINE__, "Wrong # of pinned pages.");
    }
  

  verifyPageMapContent(fpPinnedMap, 14, 19, 11, 
		       FRAMELOCK_NONE, 
		       FRAMEIO_LOADING, 
		       BUFFERSTATE_DIRTIED_BY_WORKERTHREADS);
  
  // Verify that we have the right set of non-pinned pages, that that they're in 
  // the proper state...
  int totalPages = 0;
  
  for (int aPriority = 1; aPriority < 11; aPriority++)
    {
      const map<PageAddr, PagesWithFramesMap::FramedPageInfo> & aPageMap = 
	cb._framedPages.getFrameMap(aPriority);

      verifyPageMapContent(aPageMap, 0, 13, aPriority, 
		       FRAMELOCK_NONE, 
		       FRAMEIO_IDLE, 
		       BUFFERSTATE_INIT);


      totalPages += aPageMap.size();
    }

  // We have 10 frames, and 6 were pinned, but when we called 
  // cb.beginFrameLoadOps, we specified at most 6 frames. That means that the 
  // other 4 frames were never assigned pages.
  if (totalPages != 0)
    {
      ostringstream errorMsg;
      errorMsg << "Wrong # of non-pinned pages found: " << totalPages;
      throw SmException(__FILE__, __LINE__, errorMsg.str());
    }


  // Verify that we can unpin pages...
  {
    for (int i = 14; i < 20; i++)
      {
	cb.onFrameLoadComplete(PageAddr(1, i));
      }

    if (cb._framedPages.getFrameMap(11).size() != 0)
      {
	throw SmException(__FILE__, __LINE__, "We still have pinned pages.");
      }

    int totalPages = 0;
    int totalCleanPages = 0;
    int totalInitPages = 0;
  
    for (int aPriority = 1; aPriority < 11; aPriority++)
      {
	const map<PageAddr, PagesWithFramesMap::FramedPageInfo> & aPageMap = 
	  cb._framedPages.getFrameMap(aPriority);
	
	totalPages += aPageMap.size();
	
	map<PageAddr, PagesWithFramesMap::FramedPageInfo>::const_iterator mapPos = aPageMap.begin();
	map<PageAddr, PagesWithFramesMap::FramedPageInfo>::const_iterator mapEnd = aPageMap.end();
	
	while (mapPos != mapEnd)
	  {
	    switch (mapPos->second._bufferState)
	      {
	      case BUFFERSTATE_CLEAN:
		totalCleanPages++;
		break;
	      case BUFFERSTATE_INIT:
		totalInitPages++;
		break;
	      default:
		throw SmException(__FILE__, __LINE__, "Page with wrong buffer state found");
	      }
	    
	    ++mapPos;
	  }
      }
    
    if (totalPages != 6)
      {
	throw SmException(__FILE__, __LINE__, "We don't have 10 unpinned pages.");
      }
    
    if (totalCleanPages != 6)
      {
	throw SmException(__FILE__, __LINE__, "totalCleanPages != 6");
      }
    
    if (totalInitPages != 0)
      {
	throw SmException(__FILE__, __LINE__, "totalInitPages != 4");
      }
  }


  cout << "Test 2: *** SUCCESS ***" << endl << endl;
  */
}


//===============================================================================

int main()
{
try
{
  test1();
  test2();
}
catch (exception & e)
  {
    cout << "main(): Exception caught: " << e.what() << endl;
    return 1;
  }
}
