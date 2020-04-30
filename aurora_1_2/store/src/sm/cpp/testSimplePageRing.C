#include <sm/include/SimplePageRing.H>
#include <sm/include/PageSlotAddr.H>
#include <StorageMgr_Exceptions.H>

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
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/dom/DOMErrorHandler.hpp>


//===============================================================================

// Verifies that we can create, save, and load a SimplePageRing without 
// disrupting its structure.
void test1(string xmlFilename)
{
  cout << ">>>>>>>>>>>> Entering test1 >>>>>>>>>>>>>" << endl;

  size_t slotsPerPage = 71;

  vector<PageAddr> baselineVector;
  baselineVector.push_back(PageAddr(0, 0));
  baselineVector.push_back(PageAddr(1, 1));
  baselineVector.push_back(PageAddr(2, 2));
  baselineVector.push_back(PageAddr(3, 3));
  baselineVector.push_back(PageAddr(4, 4));
  baselineVector.push_back(PageAddr(5, 5));
  baselineVector.push_back(PageAddr(6, 6));

  SimplePageRing spr1(slotsPerPage);
  spr1.addPageAnywhere(baselineVector[0]);
  spr1.addPageSuccessor(baselineVector[1], baselineVector[0]);
  spr1.addPageSuccessor(baselineVector[2], baselineVector[1]);
  spr1.addPageSuccessor(baselineVector[3], baselineVector[2]);
  spr1.addPageSuccessor(baselineVector[4], baselineVector[3]);
  spr1.addPageSuccessor(baselineVector[5], baselineVector[4]);
  spr1.addPageSuccessor(baselineVector[6], baselineVector[5]);

  // Make sure these tests pass before we add the complication of 
  // externalization...
  if (spr1.getNumPagesInRing() != baselineVector.size())
    {
      throw SmException(__FILE__, __LINE__, "getNumPagesInRing() produced a bad value");
    }

  if (baselineVector.size() != (spr1.getNumSlotsInRing() / slotsPerPage))
    {
      throw SmException(__FILE__, __LINE__, "slots-per-page problem found");
    }

  for (size_t i = 1; i < baselineVector.size(); ++i)
    {
      if (spr1.getSlotDistance(baselineVector[i-1], 0, baselineVector[i], 0) != slotsPerPage)
	{
	  throw SmException(__FILE__, __LINE__, "Apparent topology problem found");
	}
    }

  // Save it out to an XML file...
  XMLPlatformUtils::Initialize();
  XmlTempString xmlSimplePageRingString("SimplePageRing");
  DOMDocument * pDoc1 = getDomImplementation()->createDocument(NULL, xmlSimplePageRingString.toXml(), NULL);
  DOMElement * pDocRootElem = pDoc1->getDocumentElement();

  spr1.saveToXml(*pDoc1, *pDocRootElem);
  
  writeDomDocToFile(*pDoc1, xmlFilename);
  pDoc1->release();

  // Instantiate a new one from the XML file...
  DOMDocument * pDoc2 = readDomDocFromFile(*getDomImplementationLs(), xmlFilename);

  SimplePageRing spr2(*(pDoc2->getDocumentElement()));
  pDoc2->release();
  XMLPlatformUtils::Terminate();

  // Make sure page topology was preserved...
  if (spr2.getNumPagesInRing() != baselineVector.size())
    {
      throw SmException(__FILE__, __LINE__, "getNumPagesInRing() produced a bad value");
    }

  if (baselineVector.size() != (spr2.getNumSlotsInRing() / slotsPerPage))
    {
      throw SmException(__FILE__, __LINE__, "slots-per-page problem found");
    }

  for (size_t i = 1; i < baselineVector.size(); ++i)
    {
      if (spr2.getSlotDistance(baselineVector[i-1], 0, baselineVector[i], 0) != slotsPerPage)
	{
	  throw SmException(__FILE__, __LINE__, "Apparent topology problem found");
	}
    }

  // Do some additional topology tests. I.e., can this thing do the slot/page
  // math that justifies its existence...
  PageSlotAddr psa1(baselineVector[0], 0, 100);
  PageSlotAddr psa2(psa1);

  // This should put psa2 at the very end of the page that psa1 is at the beginning of.  
  spr2.advancePsaByOffset(psa2, 70);

  if (psa1._pa != psa2._pa)
    {
      throw SmException(__FILE__, __LINE__, "advancePsaByOffset seems to have skipped pages too early");
    }

  if (spr2.getSlotDistance(psa1._pa, psa1._slotNum, psa2._pa, psa2._slotNum) != 70)
    {
      ostringstream os;
      os << "getSlotDistance didn't report 70" << endl
	 << "psa1 = " << endl
	 << psa1.getDebugInfo(3) << endl
	 << "psa2 = " << endl
	 << psa2.getDebugInfo(3) << endl << endl;
    
      throw SmException(__FILE__, __LINE__, os.str());
    }
  
  // Since psa2 was at the end of a page, this should force it onto the next page....
  spr2.advancePsaByOffset(psa2, 1);

  if (spr2.getSuccessorPage(psa1._pa) != psa2._pa)
    {
      ostringstream os;
      os << "advancePsaByOffset seems to have not skipped pages properly" << endl
	 << "psa1 = " << endl
	 << psa1.getDebugInfo(3) << endl
	 << "psa2 = " << endl
	 << psa2.getDebugInfo(3) << endl << endl;

      throw SmException(__FILE__, __LINE__, os.str());

    }

  if (psa2._slotNum != 0)
    {
      throw SmException(__FILE__, __LINE__, "advancePsaByOffset seems to have made a _slotNum mistake");
    }

  cout << ">>>>>>>>>>>>  Passed test1  >>>>>>>>>>>>>" << endl;
}

//===============================================================================

// This test exercised the getPageOccupancy(...) method, which is pretty
// complex.
void test2()
{
  cout << ">>>>>>>>>>>> Entering test2 >>>>>>>>>>>>>" << endl;

  size_t slotsPerPage = 10;

  vector<PageAddr> vInput;
  vInput.push_back(PageAddr(0, 0));
  vInput.push_back(PageAddr(1, 1));
  vInput.push_back(PageAddr(2, 2));


  //-----------------------------------------------------------------------------
  // Simple test: A single page, getting all of it...
  //-----------------------------------------------------------------------------
  SimplePageRing spr(slotsPerPage);
  spr.addPageAnywhere(vInput[0]);

  size_t firstPageIdx;
  vector<size_t> vOutput;
  spr.getPageOccupancy(vInput[0], 0, vInput[0], 9, firstPageIdx, vOutput);

  if ((firstPageIdx != 0) || (firstPageIdx >= spr.getPageRing().size()))
    {
      throw SmException(__FILE__, __LINE__, "firstPageIdx problem");
    }

  if (spr.getPageRing()[firstPageIdx] != vInput[0])
    {     
      throw SmException(__FILE__, __LINE__, "page mismatch");
    }

  if ((vOutput.size() != 1) || (vOutput[0] != slotsPerPage))
    {
      ostringstream os;
      os << "vOutput problem:" << endl
	 << "   vOutput.size() = " << vOutput.size() << endl
	 << "   vOutput[0] = " << vOutput[0] << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }


  //-----------------------------------------------------------------------------
  // Simple test: A single page, first idx < second idx...
  //-----------------------------------------------------------------------------

  // Slot sequence implied is: {7, 8, 9, 0, 1, 2, 3, 4}
  spr.getPageOccupancy(vInput[0], 7, vInput[0], 4, firstPageIdx, vOutput);

  if ((firstPageIdx != 0) || (firstPageIdx >= spr.getPageRing().size()))
    {
      throw SmException(__FILE__, __LINE__, "firstPageIdx problem");
    }

  if (spr.getPageRing()[firstPageIdx] != vInput[0])
    {     
      throw SmException(__FILE__, __LINE__, "page mismatch");
    }

  if ((vOutput.size() != 2) || (vOutput[0] != 3) || (vOutput[1] != 5))
    {
      ostringstream os;
      os << "vOutput problem:" << endl
	 << "   vOutput.size() = " << vOutput.size() << endl
	 << "   vOutput[0] = " << vOutput[0] << endl
	 << "   vOutput[1] = " << vOutput[1] << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }


  //-----------------------------------------------------------------------------
  // Hard test: Multiple pages, startpage = endpage
  //-----------------------------------------------------------------------------
  spr.addPageSuccessor(vInput[1], vInput[0]);
  spr.addPageSuccessor(vInput[2], vInput[1]);

  spr.getPageOccupancy(vInput[1], 5, vInput[1], 3, firstPageIdx, vOutput);

  if (firstPageIdx >= spr.getPageRing().size())
    {
      throw SmException(__FILE__, __LINE__, "firstPageIdx problem");
    }

  if ((spr.getPageRing()[firstPageIdx] != vInput[1]) ||
      (spr.getPageRing()[(firstPageIdx+1) % 3] != vInput[2]) ||
      (spr.getPageRing()[(firstPageIdx+2) % 3] != vInput[0]) ||
      (spr.getPageRing()[(firstPageIdx+3) % 3] != vInput[1]))
    {     
      throw SmException(__FILE__, __LINE__, "page mismatch");
    }

  if ((vOutput.size() != 4) || 
      (vOutput[0] != 5)     || 
      (vOutput[1] != 10)    || 
      (vOutput[2] != 10)    || 
      (vOutput[3] != 4))
    {
      ostringstream os;
      os << "vOutput problem:" << endl
	 << "   vOutput.size() = " << vOutput.size() << endl
	 << "   vOutput[0] = " << vOutput[0] << endl
	 << "   vOutput[1] = " << vOutput[1] << endl
	 << "   vOutput[2] = " << vOutput[2] << endl
	 << "   vOutput[3] = " << vOutput[3] << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }


  //-----------------------------------------------------------------------------
  // Hard test: Multiple pages, startpage != endpage
  //-----------------------------------------------------------------------------
  spr.getPageOccupancy(vInput[1], 5, vInput[0], 3, firstPageIdx, vOutput);

  if (firstPageIdx >= spr.getPageRing().size())
    {
      throw SmException(__FILE__, __LINE__, "firstPageIdx problem");
    }

  if ((spr.getPageRing()[firstPageIdx] != vInput[1]) ||
      (spr.getPageRing()[(firstPageIdx+1) % 3] != vInput[2]) ||
      (spr.getPageRing()[(firstPageIdx+2) % 3] != vInput[0]))
    {     
      throw SmException(__FILE__, __LINE__, "page mismatch");
    }

  if ((vOutput.size() != 3) || 
      (vOutput[0] != 5)     || 
      (vOutput[1] != 10)    || 
      (vOutput[2] != 4))
    {
      ostringstream os;
      os << "vOutput problem:" << endl
	 << "   vOutput.size() = " << vOutput.size() << endl
	 << "   vOutput[0] = " << vOutput[0] << endl
	 << "   vOutput[1] = " << vOutput[1] << endl
	 << "   vOutput[2] = " << vOutput[2] << endl;
      throw SmException(__FILE__, __LINE__, os.str());
    }

  cout << ">>>>>>>>>>>>  Passed test2  >>>>>>>>>>>>>" << endl;
}

//===============================================================================

void printUsage()
{
  cout << "Usage: testSimplePageRing <xmlfile>" << endl
       << endl
       <<  "   (<xmlfile> doesn't have to preexist)." << endl;
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
      test1(xmlFilename);
      cout << endl;
      test2();
      cout << endl;
    }
  catch (exception & e)
    {
      cout << "Exception caught by main(): " << e.what() << endl;
      return 1;
    }
}
