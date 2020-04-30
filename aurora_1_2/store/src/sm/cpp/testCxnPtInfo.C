#include <sm/include/CxnPtInfo.H>
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
#include <unistd.h>

//===============================================================================

void printUsage()
{
  cout << "Usage: testCxnPtInfo <xmlfile>" << endl
       << endl
       <<  "   (<xmlfile> doesn't have to preexist)." << endl;
}

//===============================================================================

// This test verifies that a CxnPtInfo appears to properly maintain its internal
// concept of the structure of the pages, and the times.
void test1()
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #1                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  //-----------------------------------------------------------------------------
  // Ensure that limiting by num tuples works...
  //-----------------------------------------------------------------------------
  bool limitTuplesByNum;
  size_t maxNumTuples;
  bool limitTuplesByAge;
  unsigned int maxTupleAgeSeconds;
  const size_t slotsPerPage = 10;

  // Totally bogus values...
  unsigned long long tupleTstampSec = 10;
  unsigned long long tupleTstampMicroSec = 20;

  vector<PageAddr> pages;
  pages.push_back(PageAddr(1, 1));
  pages.push_back(PageAddr(2, 2));
  pages.push_back(PageAddr(3, 3));
  pages.push_back(PageAddr(4, 4));
  pages.push_back(PageAddr(5, 5));

  {
    CxnPtInfo cpi;

    limitTuplesByNum = true;
    maxNumTuples = 10;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;


    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    cpi.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec * 3); // add  3 tuples
    cpi.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); // add 10 tuples
    cpi.onEnqComplete(pages[2], 0, 5, slotsPerPage, tupleTstampSec *  6, tupleTstampMicroSec * 6); // add  6 tuples
    cpi.onEnqComplete(pages[2], 6, 7, slotsPerPage, tupleTstampSec *  2, tupleTstampMicroSec * 2); // add  2 tuples

    size_t numDeleted = cpi.doBoundedTruncation(slotsPerPage);

    if (numDeleted != 11)
      {
	ostringstream os;
	os << "doBoundedTruncation() should have returned 11, but returned " 
	   << numDeleted << " instead";
	throw SmException(__FILE__, __LINE__, os.str());
      }

    // Ensure we can mess with retention bounds after the fact...
    maxNumTuples = 8;

    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    numDeleted = cpi.doBoundedTruncation(slotsPerPage);

    if (numDeleted != 2)
      {
	ostringstream os;
	os << "doBoundedTruncation() should have returned 2, but returned " 
	   << numDeleted << " instead";
	throw SmException(__FILE__, __LINE__, os.str());
      }
  }

  //-----------------------------------------------------------------------------
  // Ensure that limiting by age works...
  //-----------------------------------------------------------------------------

  // ***NOTE*** This test may fail spuriously, because it relies on timings in
  // a non-realtime environment. This should perhaps be cleaned up later, but
  // I'm not sure how to do that well. -cjc

  {
    bool limitTuplesByNum = false;
    size_t maxNumTuples = 0;
    bool limitTuplesByAge = true;
    unsigned int maxTupleAgeSeconds = 5;

    CxnPtInfo cpi;

    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    // Try to put enough time between the two enqueueings that we can hopefully 
    // catch the experiation of the first enqueuing without the second enqueuing 
    // being expired yet...
    cout << endl << "Be patient... this should take about 10 seconds..." << endl;

    cpi.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec * 3); // add  3 tuples
    sleep(4); 
    cpi.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); // add 10 tuples
    sleep(2);

    size_t numDeleted = cpi.doBoundedTruncation(slotsPerPage);
    if (numDeleted != 3)
      {
	ostringstream os;
	os << "doBoundedTruncation() should have returned 3, but returned " 
	   << numDeleted << " instead";
	throw SmException(__FILE__, __LINE__, os.str());
      }

    // Give the second set enough time to expire, hopefully...
    sleep(4);

    numDeleted = cpi.doBoundedTruncation(slotsPerPage);
    if (numDeleted != 10)
      {
	ostringstream os;
	os << "doBoundedTruncation() should have returned 10, but returned " 
	   << numDeleted << " instead";
	throw SmException(__FILE__, __LINE__, os.str());
      }
  }

  //-----------------------------------------------------------------------------
  // Ensure that page splits work (as best we can from this API)...
  //-----------------------------------------------------------------------------

  {
    CxnPtInfo cpi;

    limitTuplesByNum = true;
    maxNumTuples = 10;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;

    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    cpi.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec * 3); 
    cpi.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); 
    cpi.onEnqComplete(pages[0], 0, 3, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec * 3);  //introduce a page-wrap

    // This shouldn't produce an exception, because it should be valid.
    cpi.onPageSplit(pages[2]);
  }

  cout << "************************************************************" << endl;
  cout << "*                       TEST #1 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

// This test verifies that a CxnPtInfo seems to properly save to/load from an XML
// file.
void test2(string xmlFilename)
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #2                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;


  // Setup reference object in a complicated manner
  bool limitTuplesByNum;
  size_t maxNumTuples;
  bool limitTuplesByAge;
  unsigned int maxTupleAgeSeconds;
  const size_t slotsPerPage = 10;

  // Totally bogus values...
  unsigned long long tupleTstampSec = 10;
  unsigned long long tupleTstampMicroSec = 20;

  vector<PageAddr> pages;
  pages.push_back(PageAddr(1, 1));
  pages.push_back(PageAddr(2, 2));
  pages.push_back(PageAddr(3, 3));
  pages.push_back(PageAddr(4, 4));
  pages.push_back(PageAddr(5, 5));

  CxnPtInfo cpi1;

  limitTuplesByNum = true;
  maxNumTuples = 10;
  limitTuplesByAge = false;
  maxTupleAgeSeconds = 0;
  
  cpi1.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			  limitTuplesByAge, maxTupleAgeSeconds);

  cpi1.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec *  3); // add  3 tuples
  cpi1.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); // add 10 tuples
  cpi1.onEnqComplete(pages[2], 0, 5, slotsPerPage, tupleTstampSec *  6, tupleTstampMicroSec *  6); // add  6 tuples
  cpi1.onEnqComplete(pages[2], 6, 7, slotsPerPage, tupleTstampSec *  2, tupleTstampMicroSec *  2); // add  2 tuples

  // Save it out to XML...
  XMLPlatformUtils::Initialize();
  XmlTempString xmlQTopologyString("CxnPtInfo");
  DOMDocument * pDoc1 = getDomImplementation()->createDocument(NULL, xmlQTopologyString.toXml(), NULL);
  DOMElement * pDocRootElem = pDoc1->getDocumentElement();

  cpi1.saveToXml(* pDoc1, * pDocRootElem);

  writeDomDocToFile(*pDoc1, xmlFilename);
  pDoc1->release();

  // Reinstantiated the object...
  DOMDocument * pDoc2 = readDomDocFromFile(*getDomImplementationLs(), xmlFilename);
  CxnPtInfo cpi2;
  cpi2.loadFromXml(*(pDoc2->getDocumentElement()));
  pDoc2->release();
  XMLPlatformUtils::Terminate();

  // Compare old vs. new objects
  if (cpi1 != cpi2)
    {
      throw SmException(__FILE__, __LINE__, "XML saving/loading didn't work");     
    }

  cout << "************************************************************" << endl;
  cout << "*                       TEST #2 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

// This test verifies that when connecting to a cxn point, the supplied history
// spec actually functions properly.
//
// Like test1(), there's a small chance of this test giving an unfounded 
// complaint, because of realtime requirements it has.
void test3()
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #3                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  bool limitTuplesByNum;
  size_t maxNumTuples;
  bool limitTuplesByAge;
  unsigned int maxTupleAgeSeconds;
  const size_t slotsPerPage = 10;

  // Totally bogus values...
  unsigned long long tupleTstampSec = 10;
  unsigned long long tupleTstampMicroSec = 20;

  vector<PageAddr> pages;
  pages.push_back(PageAddr(1, 1));
  pages.push_back(PageAddr(2, 2));
  pages.push_back(PageAddr(3, 3));
  pages.push_back(PageAddr(4, 4));
  pages.push_back(PageAddr(5, 5));

  //-----------------------------------------------------------------------------
  // Test complete lack of a predicate...
  //-----------------------------------------------------------------------------
  {
    CxnPtInfo cpi;

    limitTuplesByNum = true;
    maxNumTuples = 10;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;

    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    // This should return false, because we have no tuples yet...
    bool hasTuples;
    PageSlotAddr psa;

    limitTuplesByNum = false;
    maxNumTuples = 0;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;

    TimevalAvgTracker timestampStats;

    hasTuples = cpi.getNextDeqPoint(slotsPerPage,
				    psa,
				    timestampStats,
				    limitTuplesByNum,
				    maxNumTuples,
				    limitTuplesByAge,
				    maxTupleAgeSeconds);

    if (hasTuples)
      {
	throw SmException(__FILE__, __LINE__, "Got tuples when there were none to get");
      }

    cpi.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec *  3); // add  3 tuples
    cpi.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); // add 10 tuples

    // We presently have 13 tuples, because we haven't truncated yet...

    // Request all the tuples (apply no predicates)...
    hasTuples = cpi.getNextDeqPoint(slotsPerPage,
				    psa,
				    timestampStats,
				    limitTuplesByNum,
				    maxNumTuples,
				    limitTuplesByAge,
				    maxTupleAgeSeconds);

    if (! hasTuples)
      {
	throw SmException(__FILE__, __LINE__, "Got no tuples, but we should have");
      }

    if ((psa._pa != pages[0]) || (psa._slotNum != 7) || (psa._serialNum != 0))
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }
      
    unsigned long minExpectedTimevals; 
    unsigned long maxExpectedTimevals; 

    minExpectedTimevals = 13;
    maxExpectedTimevals = 13;
    if ((timestampStats.getNumTimevals() < minExpectedTimevals) ||
	(timestampStats.getNumTimevals() > maxExpectedTimevals))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats.getNumTimevals() out of range");
      }

    timeval tv;
    timestampStats.getAvgTimeval(tv);
    if ((tv.tv_sec != tupleTstampSec) || (tv.tv_usec != tupleTstampMicroSec))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats had the wrong average");
      }

    // This should leave us with 10 tuples.
    cpi.doBoundedTruncation(slotsPerPage);

    // Request all the tuples (apply no predicates)...
    hasTuples = cpi.getNextDeqPoint(slotsPerPage,
				    psa,
				    timestampStats,
				    limitTuplesByNum,
				    maxNumTuples,
				    limitTuplesByAge,
				    maxTupleAgeSeconds);

    if (! hasTuples)
      {
	throw SmException(__FILE__, __LINE__, "Got no tuples, but we should have");
      }

    if ((psa._pa != pages[1]) || (psa._slotNum != 0) || (psa._serialNum != 3))
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }

    minExpectedTimevals = 10;
    maxExpectedTimevals = 10;
    if ((timestampStats.getNumTimevals() < minExpectedTimevals) ||
	(timestampStats.getNumTimevals() > maxExpectedTimevals))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats.getNumTimevals() out of range");
      }

    timestampStats.getAvgTimeval(tv);
    if ((tv.tv_sec != tupleTstampSec) || (tv.tv_usec != tupleTstampMicroSec))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats had the wrong average");
      }
  }

  //-----------------------------------------------------------------------------
  // Test just a 'top-n' predicate...
  //-----------------------------------------------------------------------------
  {
    CxnPtInfo cpi;

    limitTuplesByNum = true;
    maxNumTuples = 10;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;

    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    cpi.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec *  3); // add  3 tuples
    cpi.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); // add 10 tuples

    // We presently have 13 tuples, because we haven't truncated yet...
    limitTuplesByNum = true;
    maxNumTuples = 5;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;

    bool hasTuples;
    PageSlotAddr psa;
    TimevalAvgTracker timestampStats;

    hasTuples = cpi.getNextDeqPoint(slotsPerPage,
				    psa,
				    timestampStats,
				    limitTuplesByNum,
				    maxNumTuples,
				    limitTuplesByAge,
				    maxTupleAgeSeconds);

    if (! hasTuples)
      {
	throw SmException(__FILE__, __LINE__, "Got no tuples, but we should have");
      }

    /* We can't test this exactly, because of the freedom we now allow on top-n
       predicates regarding pulling in extra tuples...
    if ((psa._pa != pages[1]) || (psa._slotNum != 5) || (psa._serialNum != 8))
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }
    */
    if (psa._serialNum > 8)
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }

    unsigned long minExpectedTimevals = 5;
    unsigned long maxExpectedTimevals = 13;
    if ((timestampStats.getNumTimevals() < minExpectedTimevals) ||
	(timestampStats.getNumTimevals() > maxExpectedTimevals))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats.getNumTimevals() out of range");
      }

    timeval tv;
    timestampStats.getAvgTimeval(tv);
    if ((tv.tv_sec != tupleTstampSec) || (tv.tv_usec != tupleTstampMicroSec))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats had the wrong average");
      }
  }

  //-----------------------------------------------------------------------------
  // Test just a 'tuple-age' predicate...
  //-----------------------------------------------------------------------------
  {
    limitTuplesByNum = true;
    maxNumTuples = 100;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;

    CxnPtInfo cpi;

    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    // Try to put enough time between the two enqueueings that we can hopefully 
    // catch the experiation of the first enqueuing without the second enqueuing 
    // being expired yet...
    cout << endl << "Be patient... this should take about 10 seconds..." << endl << endl;


    cpi.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec *  3); // add  3 tuples
    sleep(4); 
    cpi.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); // add 10 tuples
    sleep(2);

    limitTuplesByNum = false;
    maxNumTuples = 0;
    limitTuplesByAge = true;
    maxTupleAgeSeconds = 4;

    bool hasTuples;
    PageSlotAddr psa;
    TimevalAvgTracker timestampStats;

    // This should pick up only the most recently enqueued bunch of tuples...
    hasTuples = cpi.getNextDeqPoint(slotsPerPage,
				    psa,
				    timestampStats,
				    limitTuplesByNum,
				    maxNumTuples,
				    limitTuplesByAge,
				    maxTupleAgeSeconds);

    if (! hasTuples)
      {
	throw SmException(__FILE__, __LINE__, "Got no tuples, but we should have");
      }

    /* We can't test this exactly, because of the freedom we now allow on top-n
       predicates regarding pulling in extra tuples...
    if ((psa._pa != pages[1]) || (psa._slotNum != 0) || (psa._serialNum != 3))
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }
    */
    if (psa._serialNum > 3)
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }

    unsigned long minExpectedTimevals = 10;
    unsigned long maxExpectedTimevals = 13;
    if ((timestampStats.getNumTimevals() < minExpectedTimevals) ||
	(timestampStats.getNumTimevals() > maxExpectedTimevals))
      {
	ostringstream os;
	os << "timestampStats.getNumTimevals() out of range: " << timestampStats.getNumTimevals();
	throw SmException(__FILE__, __LINE__, os.str());
      }

    timeval tv;
    timestampStats.getAvgTimeval(tv);
    if ((tv.tv_sec != tupleTstampSec) || (tv.tv_usec != tupleTstampMicroSec))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats had the wrong average");
      }
  }

  //-----------------------------------------------------------------------------
  // Test a combined 'top-n'/'tuple-age' predicate pair...
  //-----------------------------------------------------------------------------
  {
    limitTuplesByNum = true;
    maxNumTuples = 100;
    limitTuplesByAge = false;
    maxTupleAgeSeconds = 0;

    CxnPtInfo cpi;

    cpi.setRetentionBounds(limitTuplesByNum, maxNumTuples,
			   limitTuplesByAge, maxTupleAgeSeconds);

    // Try to put enough time between the two enqueueings that we can hopefully 
    // catch the experiation of the first enqueuing without the second enqueuing 
    // being expired yet...
    cout << "Be patient again... this should take about 7 seconds..." << endl << endl;

    cpi.onEnqComplete(pages[0], 7, 9, slotsPerPage, tupleTstampSec *  3, tupleTstampMicroSec *  3); // add  3 tuples
    sleep(4); 
    cpi.onEnqComplete(pages[1], 0, 9, slotsPerPage, tupleTstampSec * 10, tupleTstampMicroSec * 10); // add 10 tuples

    limitTuplesByNum = true;
    maxNumTuples = 11;
    limitTuplesByAge = true;
    maxTupleAgeSeconds = 6;

    bool hasTuples;
    PageSlotAddr psa;
    TimevalAvgTracker timestampStats;

    // This should pick up all of the tuples (by age), but limit it to 5 tuples
    // because of a latest-n requirement...
    hasTuples = cpi.getNextDeqPoint(slotsPerPage,
				    psa,
				    timestampStats,
				    limitTuplesByNum,
				    maxNumTuples,
				    limitTuplesByAge,
				    maxTupleAgeSeconds);

    if (! hasTuples)
      {
	throw SmException(__FILE__, __LINE__, "Got no tuples, but we should have");
      }

    /* We can't test this exactly, because of the freedom we now allow on top-n
       predicates regarding pulling in extra tuples...
    if ((psa._pa != pages[0]) || (psa._slotNum != 9) || (psa._serialNum != 2))
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }
    */
    if (psa._serialNum > 2)
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }

    unsigned long minExpectedTimevals = 11;
    unsigned long maxExpectedTimevals = 13;
    if ((timestampStats.getNumTimevals() < minExpectedTimevals) ||
	(timestampStats.getNumTimevals() > maxExpectedTimevals))
      {
	ostringstream os;
	os << "timestampStats.getNumTimevals() out of range: " << timestampStats.getNumTimevals();
	throw SmException(__FILE__, __LINE__, os.str());
      }

    timeval tv;
    timestampStats.getAvgTimeval(tv);
    if ((tv.tv_sec != tupleTstampSec) || (tv.tv_usec != tupleTstampMicroSec))
      {
	throw SmException(__FILE__, __LINE__, "timestampStats had the wrong average");
      }


    // Give the first set of enqueued tuples a chance to expire (relative to our
    // cxn parameters, not from the cxn point itself). Now age, not latest-n, 
    // should be the limiting factor in our results.
    sleep(3);

    hasTuples = cpi.getNextDeqPoint(slotsPerPage,
				    psa,
				    timestampStats,
				    limitTuplesByNum,
				    maxNumTuples,
				    limitTuplesByAge,
				    maxTupleAgeSeconds);


    if (! hasTuples)
      {
	throw SmException(__FILE__, __LINE__, "Got no tuples, but we should have");
      }

    /* We can't test this exactly, because of the freedom we now allow on top-n
       predicates regarding pulling in extra tuples...
    if ((psa._pa != pages[1]) || (psa._slotNum != 0) || (psa._serialNum != 3))
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }
    */
    if (psa._serialNum > 3)
      {
	ostringstream os;
	os << "Unexpected data returned frmo cpi.getNextDeqPoint. psa =" << endl
	   << psa.getDebugInfo(3);
	throw SmException(__FILE__, __LINE__, os.str());
      }
  }

  cout << "************************************************************" << endl;
  cout << "*                       TEST #3 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

int main(int argc, char * argv[])
{
  if (argc != 2)
    {
      printUsage();
      return 1;
    }

  string xmlFilename(argv[1]);

  try
    {
      /*
      test1();
      cout << endl << endl;
      test2(xmlFilename);
      cout << endl << endl;
      */
      test3();
      cout << endl << endl;
    }
  catch (exception & e)
    {
      cout << "Exception caught by main(): " << e.what() << endl;
      return 1;
    }
}
