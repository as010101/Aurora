#include <sm/include/TimevalAvgTracker.H>
#include <iostream>
#include <sstream>
#include <StorageMgr_Exceptions.H>
#include <util.H>
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
  cout << "Usage: testTimevalAvgTracker <xml-filename>" << endl
       << endl
       << "   (xml-filename can exist or not exist)" << endl;
}

//===============================================================================

void test1()
  throw (exception)
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #1                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  vector<timeval> tvals;
  tvals.push_back(makeTimeval(0, 1));
  tvals.push_back(makeTimeval(1, 2));
  tvals.push_back(makeTimeval(2, 3));
  tvals.push_back(makeTimeval(3, 5));
  tvals.push_back(makeTimeval(4, 8));
  tvals.push_back(makeTimeval(5, 13));
  tvals.push_back(makeTimeval(6, 21));
  tvals.push_back(makeTimeval(7, 34));

  TimevalAvgTracker tracker;
  unsigned long long secSum, usecSum;

  // Get the first 3 timeval's into the tracker, and verify the result...
  secSum = 0;
  usecSum = 0;
  for (size_t i = 0; i < 3; ++i)
    {
      secSum += tvals[i].tv_sec;
      usecSum += tvals[i].tv_usec;
    }
  tracker.addTimevalSums(3, secSum, usecSum);

  timeval tv;
  tracker.getAvgTimeval(tv);
  if ((tv.tv_sec != 1) && (tv.tv_usec != 2))
    {
      ostringstream os;
      os << "Unexpected avg. timestamp reported: " 
	 << timevalToSimpleString(tv) << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // Get the remainting timeval's into the tracker, and verify the result...
  secSum = 0;
  usecSum = 0;
  for (size_t i = 3; i < 8; ++i)
    {
      secSum += tvals[i].tv_sec;
      usecSum += tvals[i].tv_usec;
    }
  tracker.addTimevalSums(5, secSum, usecSum);

  tracker.getAvgTimeval(tv);
  if ((tv.tv_sec != 3) && (tv.tv_usec != 500010))
    {
      ostringstream os;
      os << "Unexpected avg. timestamp reported: " 
	 << timevalToSimpleString(tv) << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  // Ensure that subtraction works...
  secSum = 0;
  usecSum = 0;
  for (size_t i = 1; i < 8; ++i)
    {
      secSum += tvals[i].tv_sec;
      usecSum += tvals[i].tv_usec;
    }
  tracker.subtractTimevalSums(7, secSum, usecSum);

  tracker.getAvgTimeval(tv);
  if ((tv.tv_sec != tvals[0].tv_sec) && (tv.tv_usec != tvals[0].tv_usec))
    {
      ostringstream os;
      os << "Unexpected avg. timestamp reported: " 
	 << timevalToSimpleString(tv) << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }
  
  cout << "************************************************************" << endl;
  cout << "*                       TEST #1 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

void test2(string xmlFilename)
  throw (exception)
{
  cout << "************************************************************" << endl;
  cout << "*                      ENTERING TEST #2                    *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;

  // Setup the baseline TimevalAvgTracker. Ensure we push it into numbers too
  // big to be stored by C++'s intrinsic types (as of this writing ;) ...
  TimevalAvgTracker tracker1;
  tracker1.addTimevalSums(10, numeric_limits<unsigned long long>::max(), 2010);
  tracker1.addTimevalSums(10, numeric_limits<unsigned long long>::max(), 2010);
  tracker1.addTimevalSums(10, numeric_limits<unsigned long long>::max(), 2010);
  
  // Save the baseline TimevalAvgTracker...
  XMLPlatformUtils::Initialize();
  XmlTempString xmlTimevalAvgTrackerString("TimevalAvgTracker");
  DOMDocument * pDoc1 = getDomImplementation()->createDocument(NULL, xmlTimevalAvgTrackerString.toXml(), NULL);
  DOMElement * pDocRootElem = pDoc1->getDocumentElement();

  tracker1.saveToXml(*pDoc1, *pDocRootElem);
  
  writeDomDocToFile(*pDoc1, xmlFilename);
  pDoc1->release();

  // Reinstantiated the TimevalAvgTracker...
  DOMDocument * pDoc2 = readDomDocFromFile(*getDomImplementationLs(), xmlFilename);

  TimevalAvgTracker tracker2;
  tracker2.loadFromXml(*(pDoc2->getDocumentElement()));
  pDoc2->release();
  XMLPlatformUtils::Terminate();

  // Ensure the two TimevalAvgTracker objects are equivalent...
  if (tracker1 != tracker2)
    {
      throw SmException(__FILE__, __LINE__, "XML save, XML load, and/or operator !=(...) have a problem");
    }

  cout << "************************************************************" << endl;
  cout << "*                       TEST #2 PASSED                     *" << endl;
  cout << "************************************************************" << endl;
  cout << endl;
}

//===============================================================================

int main(int argc, char* argv[])
{
  if (argc != 2)
    {
      printUsage();
      return 1;
    }

  string xmlFilename = argv[1];

  try
    {
      test1();
      test2(xmlFilename);
      cout << endl << endl;
    }
  catch (exception & e)
    {
      cout << "Exception caught by main(): " << e.what() << endl;
      return 1;
    }
}

//===============================================================================
