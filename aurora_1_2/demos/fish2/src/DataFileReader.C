#include <DataFileReader.H>
#include <StorageMgr_Exceptions.H>
#include <sstream>

#include <iostream> 

using namespace std;

//===============================================================================

DataFileReader::DataFileReader(string filename)
	throw (exception)
	: _f(filename.c_str())
{
	_nextPrintInterval = 0;
	discardFileLines(4);
}

//===============================================================================

DataFileReader::~DataFileReader()
{
}

//===============================================================================

bool DataFileReader::getNextParagraph(DataFileParagraph & dfp)
	throw (exception)
{
	// Assumes _f is positioned at the very beginning of a new data pararagraph,
	// or at a point in the file where there's only whitespace to the end of the
	// file.


	// We can't just test for _f.eof(), because there's an extra newline after 
	// the final paragraph.  We eat that newline and then test for eof, all in
	// one call...
	if (_f.peek() == EOF)
		{
			return false;
		}

	// Get the timestamp
	discardTokens(4);
	string strMonth, strDayOfMonth, strTime, strYear;
	_f >> strMonth >> strDayOfMonth >> strTime >> strYear;

	string strTimestamp = strMonth + " " + strDayOfMonth + " " + strTime + " " + strYear;
	
	dfp._sampleTime = tstampStringToTimeval(strTimestamp);
	dfp._printInterval = _nextPrintInterval;

	discardFileLines(1); // eat the newline char

	// Either line 2 or 3 of the paragraph contains the most recent water 
	// reading.  The other of the two lines is uninteresting...
	if (((_nextPrintInterval % 4) == 0) || 
		((_nextPrintInterval % 4) == 1))
		{
			_f >> dfp._temperatureCelcius;
			_f >> dfp._pH;
			_f >> dfp._conductivity_mS_per_cm;
			_f >> dfp._disolvedO2_mg_per_l;

			discardTokens(1);
			discardFileLines(2); // eat the newline char, and then the next line
		}
	else
		{
			discardFileLines(1); // eat last token on this line and hte newline char

			_f >> dfp._temperatureCelcius;
			_f >> dfp._pH;
			_f >> dfp._conductivity_mS_per_cm;
			_f >> dfp._disolvedO2_mg_per_l;


			discardFileLines(1); // eat last token on this line and hte newline char
		}

	// Read in data for each of the 8 fish...
	for (int i = 0; i < 8; ++i)
		{
			SingleFishReading & r = dfp._singleFishInfo[i];
			_f >> r._ventilationsPerMinute;
			_f >> r._volts;
			_f >> r._coughsPerMinute;
			_f >> r._percentBodyMovement;
		}

	// Read in summary data for fish...
	discardFileLines(2);
	_f >> dfp._numOocVentilatoryRates;
	_f >> dfp._numOocVoltages;
	_f >> dfp._numOocCoughRates;
	_f >> dfp._numOocPercentBodyMovements;
	_f >> dfp._numDeadFish;
	discardFileLines(4); // newline char and following 4 lines
	
	if (! _f)
		{
			throw SmException(__FILE__, __LINE__, "Some problem parsing the file");
		}

	++ _nextPrintInterval;
	return true;
}

//===============================================================================

void DataFileReader::discardFileLines(size_t numLines)
	throw (exception)
{
	for (int i = 0; i < numLines; ++i)
		{
			_f.ignore(numeric_limits<std::streamsize>::max(), '\n');
		}


	if (! _f)
		{
			ostringstream os;
			os << "Problem eating next " << numLines << " lines from data file";
			throw SmException(__FILE__, __LINE__, os.str());
		}
}

//===============================================================================

void DataFileReader::discardNumbers(size_t numNumbers)
	throw (exception)
{
	for (int i = 0; i < numNumbers; ++i)
		{
			float dummy;
			_f >> dummy;
		}

	if (! _f)
		{
			ostringstream os;
			os << "Problem eating next " << numNumbers << " numbers from data file";
			throw SmException(__FILE__, __LINE__, os.str());
		}
}

//===============================================================================

void DataFileReader::discardTokens(size_t numTokens)
	throw (exception)
{
	for (int i = 0; i < numTokens; ++i)
		{
			string dummy;
			_f >> dummy;

			if (! _f)
				{
					ostringstream os;
					os << "Problem eating next " << numTokens << " tokens from data file";
					throw SmException(__FILE__, __LINE__, os.str());
				}
		}
}


//===============================================================================

timeval DataFileReader::tstampStringToTimeval(string tstampString)
	throw (exception)
{
	timeval returnVal;
	struct tm tempTm;

	strptime(tstampString.c_str(), "%b %d %H:%M:%S %Y", & tempTm);

	returnVal.tv_sec = mktime(& tempTm);
	returnVal.tv_usec = 0;
	
	return returnVal;
}
