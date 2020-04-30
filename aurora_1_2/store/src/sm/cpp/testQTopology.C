#include <sm/include/QTopology.H>
#include <sm/include/IntraPageSlotRange.H>
#include <StorageMgr_Exceptions.H>
#include <util.H>
#include <iostream>
#include <sstream>
#include <xercesDomUtil.H>
#include <XmlTempString.H>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>

//===============================================================================

void printUsage()
{
  cout << "Usage: testQTopology <xmlfile1> <xmlfile2>" << endl
       << endl
       <<  "   (<xmlfile1> and <xmlfile2> don't have to preexist)." << endl;
}

//===============================================================================

// This test ensures that regions are properly occupied by records according to
// our allocation algorithm...
void test1()
  throw (exception)
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #1                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  size_t slotsPerPage = 1000;
  set<pair<int, int> > nonEmptyQviews;
  QTopology qt(slotsPerPage);

  qt.addPageWithoutSplit(PageAddr(1, 1));

  // Verify that the first enq on an empty page gives us the whole page...
  IntraPageSlotRange enqRange = qt.getNextEnqRegion();

  if ((enqRange._pa != PageAddr(1,1)) ||
      (enqRange._lowSlot  != 0) ||
      (enqRange._highSlot != (slotsPerPage - 1)) ||
      (enqRange._lowSlotSerialNum != 0))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // Verify that enqueueing into a queue with no qviews ups the serial number, 
  // but doesn't change where future enqueues can go...
  qt.onEnqComplete(1, 50, 0, 0, nonEmptyQviews);
  enqRange = qt.getNextEnqRegion();

  if ((enqRange._pa != PageAddr(1,1)) ||
      (enqRange._lowSlot  != 0) ||
      (enqRange._highSlot != 999) ||
      (enqRange._lowSlotSerialNum != 50))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // Verify that we can get just a middle part of the page populated with records
  // and that subsequent enq/deq operations will work on the expected ranges...
  qt.createQView(42);

  qt.getNextEnqRegion(); // presumable gives us access to the whole page again
  qt.onEnqComplete(1, 600, 0, 0, nonEmptyQviews);

  IntraPageSlotRange deqRange = qt.getNextDeqRegion(42);
  if ((deqRange._pa != PageAddr(1,1)) ||
      (deqRange._lowSlot  != 0) ||
      (deqRange._highSlot != 599) ||
      (deqRange._lowSlotSerialNum != 50))
    {
      ostringstream os;
      os << "Sequence problem: deqRange = " << deqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // This should cause the following slot occupation in the page:
  // [  0... 99] empty (freed by following operation)
  // [100...599] populated
  // [600...999] empty (never populated)
  qt.onDeqComplete(1, 42, 100, 0, 0, nonEmptyQviews);

  // The next enq needs to fill the [600...999] range, not [0...99], to maintain
  // sequence properly...
  enqRange = qt.getNextEnqRegion();
  if ((enqRange._pa != PageAddr(1,1)) ||
      (enqRange._lowSlot  != 600) ||
      (enqRange._highSlot != 999) ||
      (enqRange._lowSlotSerialNum != 650))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  qt.onEnqComplete(1, 400, 0, 0, nonEmptyQviews);


  // Now it's time to fill [0...99]...
  enqRange = qt.getNextEnqRegion();
  if ((enqRange._pa != PageAddr(1,1)) ||
      (enqRange._lowSlot  != 0) ||
      (enqRange._highSlot != 99) ||
      (enqRange._lowSlotSerialNum != 1050))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  qt.onEnqComplete(1, 100, 0, 0, nonEmptyQviews);

  cout << "************************************************************" << endl;
  cout << "*                       TEST #1 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

// This test ensures that page splits are required at the proper times, and that
// a page split causes the proper updating of qview tail pointers.
void test2()
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #2                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  // Verify that when the enq point, and the oldest tuple, are on different 
  // pages, no page split is indicated by the API...
  size_t slotsPerPage = 1000;
  set<pair<int, int> > nonEmptyQviews;
  QTopology qt(slotsPerPage);
  qt.createQView(42);

  qt.addPageWithoutSplit(PageAddr(1, 1));
  qt.addPageWithoutSplit(PageAddr(2, 2));

  IntraPageSlotRange enqRange = qt.getNextEnqRegion();
  if ((enqRange._pa != PageAddr(1,1)) ||
      (enqRange._lowSlot  != 0) ||
      (enqRange._highSlot != 999) ||
      (enqRange._lowSlotSerialNum != 0))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  qt.onEnqComplete(1, 1000, 0, 0, nonEmptyQviews);

  enqRange = qt.getNextEnqRegion();
  if ((enqRange._pa != PageAddr(2,2)) ||
      (enqRange._lowSlot  != 0) ||
      (enqRange._highSlot != 999) ||
      (enqRange._lowSlotSerialNum != 1000))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  qt.onEnqComplete(1, 500, 0, 0, nonEmptyQviews);


  IntraPageSlotRange deqRange = qt.getNextDeqRegion(42);
  if ((deqRange._pa != PageAddr(1,1)) ||
      (deqRange._lowSlot  != 0) ||
      (deqRange._highSlot != 999) ||
      (deqRange._lowSlotSerialNum != 0))
    {
      ostringstream os;
      os << "Sequence problem: deqRange = " << deqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  if (qt.doesAddPageRequireSplit())
    {
      throw SmException(__FILE__, __LINE__, "API claims need for a page split");
    }


  // Verify that when the enq point shares a page with existing records, and some
  // of those records have a higher slot number, a page split is indicated...
  qt.onDeqComplete(1, 42, 500, 0, 0, nonEmptyQviews);

  deqRange = qt.getNextDeqRegion(42);
  if ((deqRange._pa != PageAddr(1,1)) ||
      (deqRange._lowSlot  != 500) ||
      (deqRange._highSlot != 999) ||
      (deqRange._lowSlotSerialNum != 500))
    {
      ostringstream os;
      os << "Sequence problem: deqRange = " << deqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  enqRange = qt.getNextEnqRegion();
  if ((enqRange._pa != PageAddr(2,2)) ||
      (enqRange._lowSlot  != 500) ||
      (enqRange._highSlot != 999) ||
      (enqRange._lowSlotSerialNum != 1500))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  qt.onEnqComplete(1, 500, 0, 0, nonEmptyQviews);


  enqRange = qt.getNextEnqRegion();
  if ((enqRange._pa != PageAddr(1,1)) ||
      (enqRange._lowSlot  != 0) ||
      (enqRange._highSlot != 499) ||
      (enqRange._lowSlotSerialNum != 2000))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  qt.onEnqComplete(1, 400, 0, 0, nonEmptyQviews);

  if (! qt.doesAddPageRequireSplit())
    {
      ostringstream os;
      os <<  "API claims no need for a page split" << endl
	 << "qt = " << endl << qt.getDebugInfo(3) << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // Verify that a page split causes proper relocation of existing qview tail 
  // pointers....
  qt.createQView(29);

  enqRange = qt.getNextEnqRegion();
  if ((enqRange._pa != PageAddr(1,1)) ||
      (enqRange._lowSlot  != 400) ||
      (enqRange._highSlot != 499) ||
      (enqRange._lowSlotSerialNum != 2400))
    {
      ostringstream os;
      os << "Sequence problem: enqRange = " << enqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  qt.onEnqComplete(1, 50, 0, 0, nonEmptyQviews);

  deqRange = qt.getNextDeqRegion(29);
  if ((deqRange._pa != PageAddr(1,1)) ||
      (deqRange._lowSlot  != 400) ||
      (deqRange._highSlot != 449) ||
      (deqRange._lowSlotSerialNum != 2400))
    {
      ostringstream os;
      os << "Sequence problem: deqRange = " << deqRange.getDebugInfo() << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  size_t firstSlotToCopy, lastSlotToCopy;
  PageAddr oldPage;
  qt.addPageWithSplit(PageAddr(3, 3), oldPage,
		      firstSlotToCopy, lastSlotToCopy);

  if ((firstSlotToCopy != 500) || 
      (lastSlotToCopy != 999) || 
      (oldPage != PageAddr(1,1)))
    {
      ostringstream os;
      os << "Page split details problem:" << endl
	 << "   firstSlotToCopy = " << firstSlotToCopy << endl
	 << "   lastSlotToCopy = " << lastSlotToCopy << endl
	 << "   oldPage = " << oldPage.getDebugInfo();
      throw SmException(__FILE__, __LINE__, os.str());
    }

  cout << "************************************************************" << endl;
  cout << "*                       TEST #2 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

// This test verifies that the xml save/load methods for a QTopology work 
// properly.
void test3(string xmlFilename)
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #3                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  // Setup the baseline QTopology...
  size_t slotsPerPage = 1000;
  set<pair<int, int> > nonEmptyQviews;
  QTopology qtBaseline(slotsPerPage);
  qtBaseline.createQView(42);

  qtBaseline.addPageWithoutSplit(PageAddr(1, 1));
  qtBaseline.addPageWithoutSplit(PageAddr(2, 2));

  IntraPageSlotRange enqRange;

  enqRange = qtBaseline.getNextEnqRegion();
  qtBaseline.onEnqComplete(1, 1000, 0, 0, nonEmptyQviews);

  qtBaseline.createQView(29);

  enqRange = qtBaseline.getNextEnqRegion();
  qtBaseline.onEnqComplete(1, 500, 0, 0, nonEmptyQviews);

  enqRange = qtBaseline.getNextEnqRegion();
  qtBaseline.onEnqComplete(1, 150, 0, 0, nonEmptyQviews);

  qtBaseline.createQView(73);

  // Save the baseline QTopology...
  XMLPlatformUtils::Initialize();
  XmlTempString xmlQTopologyString("QTopology");
  DOMDocument * pDoc1 = getDomImplementation()->createDocument(NULL, xmlQTopologyString.toXml(), NULL);
  DOMElement * pDocRootElem = pDoc1->getDocumentElement();

  qtBaseline.saveToXml(*pDoc1, *pDocRootElem);
  
  writeDomDocToFile(*pDoc1, xmlFilename);
  pDoc1->release();

  // Reinstantiated the QTopology...
  DOMDocument * pDoc2 = readDomDocFromFile(*getDomImplementationLs(), xmlFilename);

  QTopology qtNew(1000);
  qtNew.loadFromXml(*(pDoc2->getDocumentElement()));
  pDoc2->release();
  XMLPlatformUtils::Terminate();

  // Ensure the two QTopology objects are equivalent...
  if (qtBaseline != qtNew)
    {
      throw SmException(__FILE__, __LINE__, 
			"Baseline and new QTopology aren't equivalent");
    }

  cout << "************************************************************" << endl;
  cout << "*                       TEST #3 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

// Returns the average timeval of the supplied ones. There must be at least one
// supplied timeval. Rounds down.
timeval getAvgTimeval(const vector<timeval> & tvals)
{
  unsigned long long secSum = 0;
  unsigned long long microSecSum = 0;

  if (tvals.empty())
    {
      throw SmException(__FILE__, __LINE__, "tvals.empty()");
    }

  for (size_t i = 0; i < tvals.size(); ++i)
    {
      secSum += tvals[i].tv_sec;
      microSecSum += tvals[i].tv_usec;
    }

  timeval tv;
  tv.tv_sec  = secSum / tvals.size();
  tv.tv_usec = microSecSum / tvals.size();
  return tv;
}

//===============================================================================

void getTimevalPartSums(const vector<timeval> & tvals, 
			unsigned long long & secSum,
			unsigned long long & microSecSum)
{
  if (tvals.empty())
    {
      throw SmException(__FILE__, __LINE__, "tvals.empty()");
    }

  secSum = 0;
  microSecSum = 0;

  for (size_t i = 0; i < tvals.size(); ++i)
    {
      secSum += tvals[i].tv_sec;
      microSecSum += tvals[i].tv_usec;
    }
}

//===============================================================================

string tvToStr(const timeval & tv)
{
  ostringstream os;
  os << "{ tv_sec = " << tv.tv_sec << ", tv_usec = " << tv.tv_usec << " }";
  return os.str();
}

//===============================================================================

bool operator == (const timeval & lhs, const timeval & rhs) 
{
  return
    (lhs.tv_sec == rhs.tv_sec) &&
    (lhs.tv_usec == rhs.tv_usec);
}

//===============================================================================

bool operator != (const timeval & lhs, const timeval & rhs) 
{
  return ! (lhs == rhs);
}

//===============================================================================

// Verifies that avgTimestamp is properly maintained.
void test4(string xmlFilename)
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #4                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  timeval tv;
  set<pair<int, int> > nonEmptyQviews;

  vector<timeval> tvalsRound1;
  tvalsRound1.push_back(makeTimeval(11, 13));
  tvalsRound1.push_back(makeTimeval(17, 19));
  tvalsRound1.push_back(makeTimeval(23, 29));
  tvalsRound1.push_back(makeTimeval(31, 37));

  timeval round1AvgTimeval = getAvgTimeval(tvalsRound1);

  unsigned long long round1SecSum;
  unsigned long long round1MicroSecSum;
  getTimevalPartSums(tvalsRound1, round1SecSum, round1MicroSecSum);



  vector<timeval> tvalsRound2;
  tvalsRound2.push_back(makeTimeval(41, 43));
  tvalsRound2.push_back(makeTimeval(47, 53));
  tvalsRound2.push_back(makeTimeval(59, 61));
  tvalsRound2.push_back(makeTimeval(67, 71));

  timeval round2AvgTimeval = getAvgTimeval(tvalsRound2);

  unsigned long long round2SecSum;
  unsigned long long round2MicroSecSum;
  getTimevalPartSums(tvalsRound2, round2SecSum, round2MicroSecSum);
  


  vector<timeval> tvalsRound1and2;
  for (size_t i = 0; i < tvalsRound1.size(); ++i)
    {
      tvalsRound1and2.push_back(tvalsRound1[i]);
    }
  for (size_t i = 0; i < tvalsRound2.size(); ++i)
    {
      tvalsRound1and2.push_back(tvalsRound2[i]);
    }

  timeval round1and2AvgTimeval = getAvgTimeval(tvalsRound1and2);

  unsigned long long round1and2SecSum;
  unsigned long long round1and2MicroSecSum;
  getTimevalPartSums(tvalsRound1and2, round1and2SecSum, round1and2MicroSecSum);
  

  // Setup the baseline QTopology...
  size_t slotsPerPage = 1000;
  QTopology qtBaseline(slotsPerPage);
  qtBaseline.createQView(1);

  qtBaseline.addPageWithoutSplit(PageAddr(1, 1));

  IntraPageSlotRange enqRange;

  enqRange = qtBaseline.getNextEnqRegion();
  qtBaseline.onEnqComplete(1, 4, round1SecSum, round1MicroSecSum, nonEmptyQviews);

  size_t numTuples;
  qtBaseline.getQviewStats(1, numTuples, tv);
  if (tv != round1AvgTimeval)
    {
      ostringstream os;
      os << "Unexpected avg. timeval for the qview." << endl
	 << "   expected :" << tvToStr(round1AvgTimeval) << endl
	 << "   found    :" << tvToStr(tv);
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // Save the baseline QTopology...
  XMLPlatformUtils::Initialize();
  XmlTempString xmlQTopologyString("QTopology");
  DOMDocument * pDoc1 = getDomImplementation()->createDocument(NULL, xmlQTopologyString.toXml(), NULL);
  DOMElement * pDocRootElem = pDoc1->getDocumentElement();

  qtBaseline.saveToXml(*pDoc1, *pDocRootElem);
  
  writeDomDocToFile(*pDoc1, xmlFilename);
  pDoc1->release();

  // Reinstantiated the QTopology...
  DOMDocument * pDoc2 = readDomDocFromFile(*getDomImplementationLs(), xmlFilename);

  QTopology qtNew(1000);
  qtNew.loadFromXml(*(pDoc2->getDocumentElement()));
  pDoc2->release();
  XMLPlatformUtils::Terminate();

  // Ensure the two QTopology objects are equivalent...
  if (qtBaseline != qtNew)
    {
      throw SmException(__FILE__, __LINE__, 
			"Baseline and new QTopology aren't equivalent");
    }

  // Ensure avg. timestamp was preserved...
  qtNew.getQviewStats(1, numTuples, tv);
  if (tv != round1AvgTimeval)
    {
      ostringstream os;
      os << "Unexpected avg. timeval for the qview." << endl
	 << "   expected :" << tvToStr(round1AvgTimeval) << endl
	 << "   found    :" << tvToStr(tv);
      throw SmException(__FILE__, __LINE__, os.str());
    }

  //-----------------------------------------------------------------------------
  // Ensure that the avg. changes as we end/deq more records...
  //-----------------------------------------------------------------------------

  enqRange = qtBaseline.getNextEnqRegion();
  qtBaseline.onEnqComplete(1, 4, round2SecSum, round2MicroSecSum, nonEmptyQviews);

  qtBaseline.getQviewStats(1, numTuples, tv);
  if (tv != round1and2AvgTimeval)
    {
      ostringstream os;
      os << "Unexpected avg. timeval for the qview." << endl
	 << "   expected :" << tvToStr(round1and2AvgTimeval) << endl
	 << "   found    :" << tvToStr(tv);
      throw SmException(__FILE__, __LINE__, os.str());
    }


  IntraPageSlotRange deqRange = qtBaseline.getNextDeqRegion(1);
  qtBaseline.onDeqComplete(1, 1, 4, round1SecSum, round1MicroSecSum, nonEmptyQviews);

  qtBaseline.getQviewStats(1, numTuples, tv);
  if (tv != round2AvgTimeval)
    {
      ostringstream os;
      os << "Unexpected avg. timeval for the qview." << endl
	 << "   expected :" << tvToStr(round2AvgTimeval) << endl
	 << "   found    :" << tvToStr(tv);
      throw SmException(__FILE__, __LINE__, os.str());
    }


  cout << "************************************************************" << endl;
  cout << "*                       TEST #4 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

int main(int argc, char * argv[])
{
  if (argc != 3)
    {
      printUsage();
      return 1;
    }

  string xmlFilename1(argv[1]);
  string xmlFilename2(argv[2]);

  try
    {
      test1();
      cout << endl << endl;
      test2();
      cout << endl << endl;
      test3(xmlFilename1);
      cout << endl << endl;
      test4(xmlFilename2);
      cout << endl << endl;
    }
  catch (exception & e)
    {
      cout << "Exception caught by main(): " << e.what() << endl;
      return 1;
    }
}
