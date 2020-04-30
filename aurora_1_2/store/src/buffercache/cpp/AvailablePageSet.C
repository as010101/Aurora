#include <StorageMgr_Exceptions.H>
#include <buffercache/include/AvailablePageSet.H>

#include <iostream> // only for debugging

AvailablePageSet::AvailablePageSet()
  throw (std::exception)
{
  _desiredPageCount = 0;
}

//=============================================================================== 
 
AvailablePageSet::~AvailablePageSet()
{
}

//=============================================================================== 
 
  
void AvailablePageSet::setDesiredPageCount(size_t numPages)
  throw (std::exception)
{
  _desiredPageCount = numPages;
}

//=============================================================================== 

void AvailablePageSet::incrementDesiredPageCount(size_t numPages)
  throw (std::exception)
{
  _desiredPageCount += numPages;
}

//=============================================================================== 
 
int AvailablePageSet::getDesiredPageCount() const
  throw (std::exception)
{
  return _desiredPageCount;
}

//=============================================================================== 

bool AvailablePageSet::requestPages(int superBoxId, size_t numPagesWanted, 
				    BinarySem & readyFlag)
  throw (exception)
{
  if (numPagesWanted == 0)
    {
      throw SmException(__FILE__, __LINE__, "numPagesWanted == 0");
    }

  size_t numPagesGranted = 0;

  map<int, vector<PageAddr> >::iterator mailboxPos = _mailboxes.find(superBoxId);
  if (mailboxPos == _mailboxes.end())
    {
      mailboxPos = _mailboxes.insert(make_pair(superBoxId, vector<PageAddr>())).first;
    }
  
  vector<PageAddr> & mailboxPages = mailboxPos->second;

  for (set<PageAddr>::iterator pos = _pagePool.begin();
       (pos != _pagePool.end()) && (numPagesGranted < numPagesWanted);)
    {
      mailboxPages.push_back(*pos);
      _pagePool.erase(pos++);
      ++numPagesGranted;
    }


  size_t numPagesStillNeeded = numPagesWanted - numPagesGranted;

  /*
  cout << endl
       << "*** mailboxPages.size() = " << mailboxPages.size() << endl
       << "*** numPagesGranted = " << numPagesGranted << endl
       << "*** numPagesWanted = " << numPagesWanted << endl
       << "*** numPagesStillNeeded = " << numPagesStillNeeded << endl
       << endl;
  */

  if (numPagesGranted == 0)
    {
      //      cout << "*** numPagesGranted == 0" << endl;
      _pendingRequests.push(PageRequest(superBoxId, 
					numPagesStillNeeded, 
					readyFlag));

      _desiredPageCount += numPagesStillNeeded;
      //      cout << "*** _desiredPageCount = " << _desiredPageCount << endl;
      return false;
    }
  else if (numPagesGranted < numPagesWanted)
    {
      //      cout << "*** numPagesWanted < numPagesGranted" << endl
      //	   << "*** superBoxId = " << endl;
      _pendingRequests.push(PageRequest(superBoxId, 
					numPagesStillNeeded,
					readyFlag));

      _desiredPageCount += numPagesStillNeeded;
      //      cout << "*** _desiredPageCount = " << _desiredPageCount << endl;
      return true;     
    }
  else
    {
      //      cout << "*** numPagesGranted == numPagesWanted" << endl;
      return true;     
    }
}

//=============================================================================== 

size_t AvailablePageSet::getNumPagesAvailable(int superBoxId)
  throw (exception)
{
  map<int, vector<PageAddr> >::iterator mailboxPos = _mailboxes.find(superBoxId);
  if (mailboxPos == _mailboxes.end())
    {
      throw SmException(__FILE__, __LINE__, "Unknown superBoxId");
    }

  return mailboxPos->second.size();
}

//=============================================================================== 

void AvailablePageSet::getPages(int superBoxId, vector<PageAddr> & pages, 
				size_t numWantedNow)
  throw (exception)
{
  map<int, vector<PageAddr> >::iterator mailboxPos = _mailboxes.find(superBoxId);
  if (mailboxPos == _mailboxes.end())
    {
      throw SmException(__FILE__, __LINE__, "Unknown superBoxId");
    }

  vector<PageAddr> & mailboxPages = mailboxPos->second;

  if (mailboxPages.size() < numWantedNow)
    {
      throw SmException(__FILE__, __LINE__, "Mailbox has fewer pages than requested");
    }

  pages.clear();
  pages.reserve(numWantedNow);

  for (size_t i = 0; i < numWantedNow; ++i)
    {
      pages.push_back(mailboxPages.back());
      mailboxPages.pop_back();
    }
}

//=============================================================================== 
 
  
int AvailablePageSet::getCurrentPageCount() const
  throw (std::exception)
{
  return _pagePool.size();
}

//=============================================================================== 
 

bool AvailablePageSet::getPageNonBlocking(PageAddr & pa, bool decrementDesiredCount)
  throw (std::exception)
{
  // We don't really care which PageAddr we grab from the pool...
  set<PageAddr>::iterator pos = _pagePool.begin();

  if (pos == _pagePool.end())
    {
      return false;
    }

  pa = *pos;
  _pagePool.erase(pos);

  if (decrementDesiredCount && (_desiredPageCount > 0))
    {
      --_desiredPageCount;
    }

  return true;
}

//=============================================================================== 
 
  
void AvailablePageSet::addPage(PageAddr pa)
  throw (std::exception)
{
  // Give priority to specific underfilled mailboxes, rather than automatically
  // dumping new pages into _pagePool...
  if (! _pendingRequests.empty())
    {
      bool doPop = false;

      {
	PageRequest & pr = _pendingRequests.front();

	//	cout << "addPage: giving to the mailbox: " << pr._superBoxId << endl;
      
	map<int, vector<PageAddr> >::iterator mailboxPos = _mailboxes.find(pr._superBoxId);
	if (mailboxPos == _mailboxes.end())
	  {
	    throw SmException(__FILE__, __LINE__, "Unknown superBoxId");
	  }

	mailboxPos->second.push_back(pa);
	pr._readyFlag.post();

	if (pr._numPagesStillNeeded > 1)
	  {
	    --pr._numPagesStillNeeded;
	  }
	else
	  {
	    doPop = true;
	  }
      }

      // Do this outside the scope in which we initialized 'pr', because we're 
      // not supposed to invalidate existing references.
      if (doPop)
	{
	_pendingRequests.pop();
	}
    }
  else if (! _pagePool.insert(pa).second)
    {
      throw SmException(__FILE__, __LINE__, 
			"The PageAddress is already in the pool!");
    }
}

//=============================================================================== 

AvailablePageSet::PageRequest::PageRequest(const PageRequest & rhs) : 
  _superBoxId(rhs._superBoxId),
  _numPagesStillNeeded(rhs._numPagesStillNeeded),
  _readyFlag(rhs._readyFlag)
{
}

//=============================================================================== 
