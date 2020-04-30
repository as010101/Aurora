#ifndef DATAFILEREADER_H
#define DATAFILEREADER_H

#include <exception>
#include <string>
#include <fstream>
#include <sys/time.h>
#include <time.h>

#include <datafile.H>

using namespace std;

class DataFileReader
{
public:
	// File must exist during the whole time this instantance exists.
	DataFileReader(string filename)
		throw (exception);

	virtual ~DataFileReader();

	// If there are more paragraphs to provide, then this will return true
	// and set the fields in 'dfp'.  Otherwise this returns false.
	//
	// Parse errors, file errors, etc. cause an exception.
	bool getNextParagraph(DataFileParagraph & dfp)
		throw (exception);

private:
	void discardFileLines(size_t numLines)
		throw (exception);

	void discardNumbers(size_t numNumbers)
		throw (exception);

	void discardTokens(size_t numTokens)
		throw (exception);

	timeval tstampStringToTimeval(string tstampString)
		throw (exception);

	ifstream _f;

	// Number of the next print interval.  First paragraph in the data file is
	// said to have pi #0, not pi #1.
	int _nextPrintInterval;
};

#endif
