#ifndef _AGG_SOURCE_FILE_READER_H_
#define _AGG_SOURCE_FILE_READER_H_

// C++ headers
#include <fstream>			// for fstream
#include <vector>			// for vector
// Unix headers

// Local headers
#include "AggStreamSource.H"		// for AggStreamSource 
#include "Types.H"			// for distributiontype_t,
                                        //     distribution_t
using namespace std;

namespace workload_generator
{

const int BUFFERSIZE = 1024 * 64;

class AggSourceFileReader
{
public:

   static bool readFile(
         const char* 		filename, 
         AggStreamSource&	agg_source,
         bool			show_process);

private:

   static bool	readLine(fstream& file, char* buf, int len);
   static bool	readDistrVect(
         fstream&			file, 
         vector<distribution_t>&	distr_vect,
         bool				show_process);
   static bool	readAttribueDef(
         fstream&			file, 
         AttributeDef&			attr_def,
         bool				show_process);
   static bool	readDistribution(
         fstream& 			file, 
         distribution_t& 		distr,
         bool				show_process);
};


}

#endif 
