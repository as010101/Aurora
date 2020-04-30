#include <buffercache/include/AvailablePageSet.H>
#include <StorageMgr_Exceptions.H>
#include <iostream>

//===============================================================================

void test1()
  throw (exception)
{
  cout << "############################################################" << endl;
  cout << "#                    Entering Test 1                       #" << endl;
  cout << "############################################################" << endl;
  cout << endl;

  BinarySem sem1, sem2, sem3;
  vector<PageAddr> pages;
  AvailablePageSet aps;

  //-----------------------------------------------------------------------------
  // Test A: Immediately satisfiable page request
  //-----------------------------------------------------------------------------

  aps.addPage(PageAddr(1, 1));
  aps.addPage(PageAddr(1, 2));

  bool success;
  success =  aps.requestPages(1, 1, sem1);

  if (! success)
    {
      throw SmException(__FILE__, __LINE__, "Page not immediately available");
    }

  if (aps.getNumPagesAvailable(1) != 1)
    {
      cout << aps.getNumPagesAvailable(1) << endl;
      throw SmException(__FILE__, __LINE__, "Wrong number of pages found in mailbox");
    }

  aps.getPages(1, pages, 1);
  if (pages.size() != 1)
    {
      throw SmException(__FILE__, __LINE__, "Wrong number of pages pulled from mailbox");
    }

  if ((pages[0] != PageAddr(1,1)) && (pages[0] != PageAddr(1,2)))
    {
      throw SmException(__FILE__, __LINE__, "Unrecognized page pulled from mailbox");
    }

  //-----------------------------------------------------------------------------
  // Test B: Partially satisfiable page request
  //-----------------------------------------------------------------------------
  
  success = aps.requestPages(2, 2, sem2);

  if (! success)
    {
      throw SmException(__FILE__, __LINE__, "Page not immediately available");
    }
  

  if (aps.getNumPagesAvailable(2) != 1)
    {
      throw SmException(__FILE__, __LINE__, "Wrong number of pages found in mailbox");
    }

  aps.getPages(2, pages, 1);
  if (pages.size() != 1)
    {
      throw SmException(__FILE__, __LINE__, "Wrong number of pages pulled from mailbox");
    }

  if ((pages[0] != PageAddr(1,1)) && (pages[0] != PageAddr(1,2)))
    {
      throw SmException(__FILE__, __LINE__, "Unrecognized page pulled from mailbox");
    }

  if (sem2.isPosted())
    {
      throw SmException(__FILE__, __LINE__, "BinarySem posted too early");
    }

  aps.addPage(PageAddr(1,3));

  if (! sem2.isPosted())
    {
      throw SmException(__FILE__, __LINE__, "BinarySem not posted when it should be");
    }

  if (aps.getNumPagesAvailable(2) != 1)
    {
      throw SmException(__FILE__, __LINE__, "Wrong number of pages found in mailbox");
    }

  aps.getPages(2, pages, 1);
  if (pages.size() != 1)
    {
      throw SmException(__FILE__, __LINE__, "Wrong number of pages pulled from mailbox");
    }

  if (pages[0] != PageAddr(1,3))
    {
      throw SmException(__FILE__, __LINE__, "Unrecognized page pulled from mailbox");
    }

  cout << endl;
  cout << "############################################################" << endl;
  cout << "#                     Passed Test 1                        #" << endl;
  cout << "############################################################" << endl;
}

//===============================================================================

void printUsage()
{
  cout << "Usage: testAvailablePageSet" << endl;
}

//===============================================================================

int main(int argc, char * argv[])
{
  if (argc != 1)
    {
      printUsage();
      return 1;
    }

  try
    {
      test1();
    }
  catch (exception & e)
    {
      cout << "main: exception caught: " << e.what() << endl;
    }
}
