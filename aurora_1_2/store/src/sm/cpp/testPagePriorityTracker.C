#include <sm/include/PagePriorityTracker.H>
#include <StorageMgr_Exceptions.H>
#include <iostream>

//===============================================================================

void printUsage()
{
  cout << "Usage: testPagePriorityTracker" << endl;
}

//===============================================================================

void test1()
  throw (exception)
{
  cout << ">>>>>>>>>>>>>>>>>>  Entering test #1  >>>>>>>>>>>>>>>>>>>>>>" << endl
       << endl;

  PagePriorityTracker t;

  vector<PageAddr> vInTop, vInHigh, vInMedium, vInLow;

  vInTop   .push_back(PageAddr(3, 1));
  vInTop   .push_back(PageAddr(3, 2));
  vInHigh  .push_back(PageAddr(2, 1));
  vInHigh  .push_back(PageAddr(2, 2));
  vInMedium.push_back(PageAddr(3, 1)); // Note that this page also has a top priority
  vInMedium.push_back(PageAddr(1, 2));
  vInLow   .push_back(PageAddr(0, 1));
  vInLow   .push_back(PageAddr(0, 2));

  t.increaseUseCounts(vInTop, vInHigh, vInMedium, vInLow);

  const set<PageAddr> & vOutTop = 
    t.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_TOP);

  if ((vOutTop.size() != 2) ||
      (vOutTop.find(PageAddr(3, 1)) == vOutTop.end()) ||
      (vOutTop.find(PageAddr(3, 2)) == vOutTop.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutTop has wrong content");
    }

  const set<PageAddr> & vOutHigh = 
    t.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_HIGH);

  if ((vOutHigh.size() != 2) ||
      (vOutHigh.find(PageAddr(2, 1)) == vOutHigh.end()) ||
      (vOutHigh.find(PageAddr(2, 2)) == vOutHigh.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutHigh has wrong content");
    }

  const set<PageAddr> & vOutMedium = 
    t.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_MEDIUM);

  if ((vOutMedium.size() != 1) ||
      (vOutMedium.find(PageAddr(1, 2)) == vOutMedium.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutMedium has wrong content");
    }

  const set<PageAddr> & vOutLow = 
    t.getPageSetByEffectivePriority(PagePriorityTracker::EFFECTIVE_PRIORITY_LOW);

  if ((vOutLow.size() != 2) ||
      (vOutLow.find(PageAddr(0, 1)) == vOutLow.end()) ||
      (vOutLow.find(PageAddr(0, 2)) == vOutLow.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutLow has wrong content");
    }

  //-----------------------------------------------------------------------------
  // Test that elevating priorities is effective...
  //-----------------------------------------------------------------------------

  vInTop.clear();
  vInHigh.clear();
  vInMedium.clear();
  vInLow.clear();

  // Elevate priority from "low" to "top" of this page.
  vInTop.push_back(PageAddr(0, 1));
  t.increaseUseCounts(vInTop, vInHigh, vInMedium, vInLow);

  if ((vOutTop.size() != 3) ||
      (vOutTop.find(PageAddr(3, 1)) == vOutTop.end()) ||
      (vOutTop.find(PageAddr(3, 2)) == vOutTop.end()) ||
      (vOutTop.find(PageAddr(0, 1)) == vOutTop.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutTop has wrong content");
    }

  if ((vOutLow.size() != 1) ||
      (vOutLow.find(PageAddr(0, 2)) == vOutLow.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutLow has wrong content");
    }

  //-----------------------------------------------------------------------------
  // Test that reducing priorities can unmap a PageAddr...
  //-----------------------------------------------------------------------------

  vInTop.clear();
  vInHigh.clear();
  vInMedium.clear();
  vInLow.clear();

  // Eliminate all priorities for this page...
  vInTop.push_back(PageAddr(0, 1));
  vInLow.push_back(PageAddr(0, 1));
  t.decreaseUseCounts(vInTop, vInHigh, vInMedium, vInLow);

  // Make sure we kicked it out of its most recent priority level...
  if ((vOutTop.size() != 2) ||
      (vOutTop.find(PageAddr(3, 1)) == vOutTop.end()) ||
      (vOutTop.find(PageAddr(3, 2)) == vOutTop.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutTop has wrong content");
    }
  
  // Make sure it doesn't regress to its previous priority level...
  if ((vOutLow.size() != 1) ||
      (vOutLow.find(PageAddr(0, 2)) == vOutLow.end()))
    {
      throw SmException(__FILE__, __LINE__, "vOutLow has wrong content");
    }

  cout << "<<<<<<<<<<<<<<<<<<<  Passed test #1  <<<<<<<<<<<<<<<<<<<<<<<" << endl
       << endl;
}

//==============================================================================

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
      cout << "main: Caught exception: " << e.what() << endl;
      return 1;
    }
}

//===============================================================================
