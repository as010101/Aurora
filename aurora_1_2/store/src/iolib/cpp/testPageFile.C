#include <iolib/int_include/PageFile.H>
#include <util.H>
#include <iostream>
#include <vector>
#include <string.h>
#include <stdlib.h>

void printUsage()
{
  cout << "Usage: testPageFile <filename> <bytes-per-page>" << endl;
}

int main(int argc, char* argv[])
{
  if (argc != 3)
    {
      printUsage();
      return 1;
    }

  try
    {
      cout << "********* TEST 1: Create a PageFile **************" << endl;
      string filename = argv[1];

      int bytesPerPage = atoi(argv[2]);
      if (bytesPerPage < 1)
	{
	  throw SmException(__FILE__, __LINE__, "<bytes-per-page> must be > 0");
	}

      // Get rid of the old one, if it exists...
      if (fileExists(filename))
	{
	  deleteFile(filename);
	}

      // Create the new page file.
      PageFile::createPageFile(filename, bytesPerPage);
      cout << "Success" << endl << endl;

      //=========================================================================

      cout << "******** TEST 2: Open a PageFile ***************" << endl;
      FdMultiPool fmp(10);
      fmp.registerFile(filename, O_RDWR);

      PageFile pf(filename, bytesPerPage, fmp);

      cout << endl << "pf.getDebugInfo() returns: " << endl << pf.getDebugInfo() << endl;

      cout << "Success" << endl << endl;

      //=========================================================================

      cout << "******** TEST 3: Allocate pages until we run out *********" << endl;
      pf.growFile();
      vector<int> pageIds;

      int pageId = pf.allocPageInternal();
      while (pageId >= 0)
	{
	  pageIds.push_back(pageId);
	  pageId = pf.allocPageInternal();
	}

      cout << "Success: Allocated " << pageIds.size() << " pages before we ran out." << endl << endl;

      //=========================================================================

      cout << "******** TEST 4: Make sure freeing pages make them available **********" << endl;
      pf.freePage(pageIds[0]);
      pf.freePage(pageIds[1]);

      if((pageIds[0] = pf.allocPageInternal()) == -1)
	{
	  cout << "Failure: Couldn't reallocate the first page." << endl;
	  return 1;
	}

      if((pageIds[1] = pf.allocPageInternal()) == -1)
	{
	  cout << "Failure: Couldn't reallocate the second page." << endl;
	  return 1;
	}

      cout << "Success" << endl << endl;

      //=========================================================================

      cout << "********* TEST 5: Make sure that growing the file works *********" << endl;
      if (pf.allocPageInternal() != -1)
	{
	  cout << "Failure: We could allocate a page when we were supposed to be all out of pages." << endl;
	}

      unsigned int allocWithNoGrowth;
      bool fileGrowthPossible;
      pf.getCapacityInfo(allocWithNoGrowth, fileGrowthPossible);

      if (! fileGrowthPossible)
	{
	  cout << "Failure: We're not allowed to grow the file, for some reason." << endl;
	  return 1;
	}

      if (allocWithNoGrowth != 0)
	{
	  cout << "Failure: If we have no free pages, then why foes allocsWithNoGrowth have a value of " 
	       << allocWithNoGrowth << "?" << endl;
	  return 1;
	}

      pf.growFile();

      pageId = pf.allocPageInternal();
      if (pageId < 0)
	{
	  cout << "Failure: After growing the file, we still can't allocate a page." << endl;
	  return 1;
	}

      pageIds.push_back(pageId);
      cout << "Success" << endl << endl;

      //=========================================================================

      cout << "********* TEST 6: Make sure that file content is preserved *********" << endl;
      char srcBuffer[bytesPerPage];
      char copyBuffer[bytesPerPage];

      for (unsigned int i = 0; i < pageIds.size(); i++)
	{
	  memset(srcBuffer, i % 255, sizeof(srcBuffer));
	  pf.writePage(i, srcBuffer);
	}

      for (unsigned int i = 0; i < pageIds.size(); i++)
	{
	  pf.readPage(i, copyBuffer);

	  for (unsigned int j = 0; j < sizeof(copyBuffer); j++)
	    {
	      if (copyBuffer[j] != char(i % 255))
		{
		  cout << "Failure: Page " << i << " doesn't have right-seeming content." << endl;
		  return 1;
		}
	    }
	}

      cout << "Success" << endl << endl;

      //=========================================================================

      cout << "********* TEST 7: Make sure that closing works *********" << endl;
      pf.shutdown();
      fmp.close();
      cout << "Success" << endl << endl;

      //=========================================================================

      cout << "********* TEST 8: Make sure we can grow file to exact right size *********" << endl;

      string filename2 = filename + "2";
      PageFile::createPageFile(filename2, bytesPerPage);

      FdMultiPool fmp2(10);
      fmp2.registerFile(filename2, O_RDWR);
      PageFile pf2(filename2, bytesPerPage, fmp2);

      int maxDataPages = pf2.getMaxDataPages();
      cout << "The file should be growable to have " << maxDataPages << " data pages." << endl;
      
      try
	{
	  for (int i = 0; i < maxDataPages; i++)
	    {
	      cout << "TEST 8: i = " << i << "/" << maxDataPages << endl;
	      int pageId2 = pf2.allocPageInternal();
	      if (pageId2 == -1)
		{
		  pf2.growFile();
		  pageId2 = pf2.allocPageInternal();
		}

	      if (pageId2 == -1)
		{
		  cout << "Failed: pageId == -1, even after growing the page file." << endl;
		  exit(1);
		}
	    }
	}
      catch (SmFileFullException & e)
	{
	  cout << "Failed: The file got full too early." << endl
	       << e.what() << endl;
	  exit(1);
	}

      cout << "Success" << endl << endl;

      //=========================================================================

      cout << "********* TEST 9: Make sure we can't grow file too big *********" << endl;

      // Make sure that we can't grow the page file beyond its max size...
      try
	{
	  pf2.growFile();
	  cout << "Failure: We could grow the file, even though it was supposedly full." << endl;
	  exit(1);
	}
      catch (SmFileFullException & e)
	{
	  cout << "Success: The file got too full exactly when it should have." << endl;
	}
      
      cout << "Success" << endl << endl;

      pf2.shutdown();
      fmp2.close();

      //=========================================================================

      cout << "############### TEST FULLY SUCCESSFUL #################" << endl;
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
