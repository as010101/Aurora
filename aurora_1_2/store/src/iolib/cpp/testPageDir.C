#include <iolib/int_include/PageDir.H>
#include <iostream>
#include <string.h>
#include <vector>

void printUsage()
{
  cout << "Usage: testPageDir <dirname> <bytes-per-page> <total-pages-to-write>" << endl;
}

int main(int argc, char* argv[])
{
  if (argc != 4)
    {
      printUsage();
      return 1;
    }

  string dirname = argv[1];
  int bytesPerPage = atoi(argv[2]);
  unsigned long pagesToWrite = atoi(argv[3]);

  try
    {
      //=========================================================================
      cout << "********* TEST 1: Create a PageDir **************" << endl;
      PageDir pd(dirname, bytesPerPage, 100, NULL);
      cout << "Success" << endl << endl;

      //=========================================================================
      cout << "********* TEST 2: Populate a PageDir w/mucho data **************" << endl;
      char * pSrcData = new char[bytesPerPage];
      memset(pSrcData, 'x', bytesPerPage);

      vector<PageAddr> pages;
      pages.reserve(pagesToWrite);

      for (unsigned int i = 0; i < pagesToWrite; i++)
	{
	  if (((i % 10000) == 0) || (i == (pagesToWrite - 1)))
	    {
	      cout << "\tWriting page " << i << "/" << pagesToWrite << endl;
	    }

	  unsigned int numFreePages = pd.getNumFreePages();
	  if (numFreePages == 0)
	    {
	      pd.growStorage();     
	      numFreePages = pd.getNumFreePages();
	    }

	  PageAddr pa;
	  bool success = pd.allocPageInternal(pa);
	  if (! success)
	    {
	      cout << "Failed: An internal page allocation should have been possible." << endl;
	      return 1;
	    }

	  pages.push_back(pa);
	  pd.writePage(pa, pSrcData);
	}

      cout << "\tDone writing " << pagesToWrite << " pages" << endl << endl;
      cout << "Success" << endl << endl;


      //=========================================================================
      cout << "********* TEST 3: Verify that the data is retained **************" << endl;
      pd.shutdown();

      BinarySem gotFreePagesSem(false);
      PageDir pd2(dirname, 65536, 100, & gotFreePagesSem);

      try
	{
      
	  char * pReadBuff = new char[bytesPerPage];
	  unsigned int numPages = pages.size();
	  for (unsigned int i = 0; i <  numPages; i++)
	    {
	      if ((i % 1000) == 0)
		{
		  cout << "Progress: About to verify pages[" << i << "]" << endl;
		}
	      
	      pd2.readPage(pages[i], pReadBuff);
	      if (memcmp(pReadBuff, pSrcData, bytesPerPage) != 0)
		{
		  cout << "Failure: Mismatch on page: " << pages[i].getDebugInfo();
		  return 1;
		}
	    }
	  
	  cout << "Success" << endl << endl;
	  
	  //=========================================================================
	  cout << "********* TEST 4: Verify that internal frees / reallocs work **************" << endl;
	  
	  for (unsigned int i = 0; i < numPages; i++)
	    {
	      if ((i % 1000) == 0)
		{
		  cout << "i = " << i << endl;
		}
	      
	      pd2.freePage(pages[i]);
	      bool success = pd2.allocPageInternal(pages[i]);
	      if (! success)
		{
		  cout << "Failed: Couldn't allocate a page internally, even though "
		       << "the PageDir should have the capacity." << endl;
		  return 1;
		}
	    }
	  
	  cout << "Success" << endl << endl;
	  
	  //=========================================================================
	  cout << "********* TEST 5: Verify that the free-binary-sem works properly **************" << endl;

	  if (gotFreePagesSem.isPosted())
	    {
	      // Clear the sem before we free a page, so that our test starts from a known state.
	      // I could do some thinking and become confident, from earlier tests, whether or not
	      // this statement needs to be conditional, but I'm being lazy as I write this. -cjc
	      gotFreePagesSem.awaitPostThenClear();
	    }
	  
	  // Make sure there are no more free pages...
	  while (pd2.getNumFreePages() > 0)
	    {
	      PageAddr pa;
	      pd2.allocPageInternal(pa);
	    }
	  
	  // Free up one page, and the sem should be posted because the PageDir should
	  // transition from having no free pages to having a free page...
	  
	  pd2.freePage(pages[0]);
	  if (! gotFreePagesSem.isPosted())
	    {
	      cout << "Test 5: Failed test 5.1" << endl
		   << "\tgotFreePagesSem.isPosted() = " << gotFreePagesSem.isPosted() << endl
		   << "\tpd2.getNumFreePages() = " << pd2.getNumFreePages() << endl;
	      return 1;
	    }
	  
	  gotFreePagesSem.awaitPostThenClear();
	  cout << "Test 5: Passed test 5.1" << endl;
	  
	  // Make sure that freeing up a second page doesn't cause the sem to be
	  // re-posted.
	  pd2.freePage(pages[1]);
	  if (! gotFreePagesSem.isPosted())
	    {
	      cout << "Test 5: Passed test 5.2" << endl;
	    }
	  else
	    {
	      cout << "Test 5: Failed test 5.2" << endl;
	      return 1;
	    }

	  // Run out of free pages again. Then, make sure that growing the page dir,
	  // which creates more free pages, causes the binary sem to be posted.
	  bool success = pd2.allocPageInternal(pages[0]);
	  if (! success)
	    {
	      cout << "Test 5.3 Failed: Couldn't allocate a page internally, even though "
		   << "the PageDir should have the capacity." << endl;
	      return 1;
	    }
	  
	  success = pd2.allocPageInternal(pages[1]);
	  if (! success)
	    {
	      cout << "Test 5.3 Failed: Couldn't allocate a page internally, even though "
		   << "the PageDir should have the capacity." << endl;
	      return 1;
	    }

	  cout << "Test 5: Passed test 5.3" << endl;
	      
	  pd2.growStorage();     
	  if (! gotFreePagesSem.isPosted())
	    {
	      cout << "Test 5: Failed test 5.4" << endl;
	      return 1;
	    }
	  
	  gotFreePagesSem.awaitPostThenClear();
	  cout << "Test 5: Passed test 5.4" << endl;
	  
	  cout << "Success" << endl << endl;
	}
      catch (std::exception & e)
	{
	  cout << "Caught an exception: " << e.what() << endl;
	}
      catch (...)
	{
	  cout << "Caught some exception other than a std::exception!" << endl;
	}

      //=========================================================================

      pd2.shutdown();
      cout << "############### TEST COMPLETE #################" << endl;
    }
  catch (std::exception & e)
    {
      cout << "Caught an exception: " << e.what() << endl;
      return 1;
    }
  catch (...)
    {
      cout << "Caught some exception other than a std::exception!" << endl;
      return 1;
    }
}
