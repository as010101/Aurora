#include "AggSourceFileReader.H"

// C++ headers
#include <iostream>			// for cout, cerr
#include <sstream>			// for istringstream
#include <cstring>			// for strcpy

// Unix headers

// Local headers
#include "AggStreamSource.H"		// for AggStreamSource 
#include "Types.H"			// for distributiontype_t,
#include "RandomNumGenerator.H"		// for RandomNumGenerator*


using namespace std;

namespace workload_generator
{

bool AggSourceFileReader::readFile(
      const char*	filename, 
      AggStreamSource&	agg_source,
      bool		show_process)
{
   fstream file;
   file.open(filename, ios::in);
   if (! file) {
      cerr << "Can not open file: " << filename << endl;
      return false;
   }

   char s[BUFFERSIZE];

   // read number ID
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading Aggregage stream source number ID failed."
              << endl;
         cerr << "Reading file " << filename << " failed."
              << endl;
         cerr << "Data read in: "
              << endl;
         agg_source.print();
         file.close();
         return false;
      }

      istringstream dataline(s);
      dataline >> agg_source.id.numberID; 

      if (show_process)
         cout << "Number ID " <<  agg_source.id.numberID
              << endl; 

   }

   // read string ID
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading Aggregage stream source string ID failed."
              << endl;
         cerr << "Reading file " << filename << " failed."
              << endl;
         cerr << "Data read in: "
              << endl;
         agg_source.print();
         file.close();
         return false;
      }
 
      istringstream dataline(s);
      dataline >> agg_source.id.stringID; 

      if (show_process)
         cout << "String ID " <<  agg_source.id.stringID
              << endl; 
   }

   // read number of steams ID
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading number of streams failed."
              << endl;
         cerr << "Reading file " << filename << " failed."
              << endl;
         cerr << "Data read in: "
              << endl;
         agg_source.print();
         file.close();
         return false;
      }

      istringstream dataline(s);
      dataline >> agg_source.num_streams; 

      if (show_process)
         cout << "Number of streams " <<  agg_source.num_streams
              << endl; 
   }

   // read number of tuples 
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading number of tuples per streams failed."
              << endl;
         cerr << "Reading file " << filename << " failed."
              << endl;
         cerr << "Data read in: "
              << endl;
         agg_source.print();
         file.close();
         return false;
      }

      istringstream dataline(s);
      dataline >> agg_source.num_tuples; 

      if (show_process)
         cout << "Number of tuples " <<  agg_source.num_tuples
              << endl; 
   }

   // read rate type
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading number of rate type failed."
              << endl;
         cerr << "Reading file " << filename << " failed."
              << endl;
         cerr << "Data read in: "
              << endl;
         agg_source.print();
         file.close();
         return false;
      }

      istringstream dataline(s);
      int type;
      dataline >> type;
      if (type > MAX_DISTRIBUTION_TYPE_NUMBER || type < 0){
         cerr << "Invalid distribution type: " 
              << type
              << endl;
         cerr << "Reading rate distribution type failed."
              << endl;
         file.close();
         return false;
      }
      agg_source.rate_type = static_cast<distributiontype_t>(type); 

      if (show_process)
         cout << "Rate type " << type 
              << endl; 
   }

   // read parameter distributions for rate distribution
   if (readDistrVect(file, agg_source.rate_parameters, show_process) == false) {
      cerr << "Reading distributions for rate parameters failed."
           << endl;
      cerr << "Reading file " << filename << " failed."
           << endl;
      cerr << "Data read in: "
           << endl;
      agg_source.print();
      file.close();
      return false;
   }


   // read number of Attributes
   int m;
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading file " << filename << " failed."
              << endl;
         cerr << "Data read in: "
              << endl;
         agg_source.print();
         file.close();
         return false;
      }

      istringstream dataline(s);
      dataline >> m;
      if (m > MAX_ATTRIBUTE_NUMBER) {
         cerr << "Invalid number of attributes: "
              << m
              << endl;
         cerr << "Reading number of attributes failed."
              << endl;
         cerr << "Reading file " << filename << " failed."
              << endl;
         cerr << "Data read in: "
              << endl;
         agg_source.print();
         file.close();
         return false; 
      }

      if (show_process)
         cout << "Number of attributes" << m 
              << endl; 
   }

   // read Attributes
   for(int i = 0; i < m; ++i) {
      AttributeDef* attr_def = new(AttributeDef);
      if (readAttribueDef(file, *attr_def, show_process)) {
         agg_source.attributes.push_back(*attr_def);
         delete attr_def;
      }
      else {
         delete attr_def;
         cerr << "Reading attributes failed."
              << endl;
         cerr << i << " attributes has been read."
              << endl;
               
         cerr << "Reading file \"" << filename << "\" failed."
              << endl;
         file.close();
         return false;
      }
   }

   cout << "Reading file \"" << filename << "\" finished."
        << endl;
   //agg_source.print();
   file.close();
   return true;
}

bool AggSourceFileReader::readLine(fstream& file, char* buf, int len)
{
      if (file.eof()) {
         cerr << "Invalid file: incomplete information." 
              << endl;
         return false;
      }
      file.getline(buf, len);
      while (!file.eof() && (strlen(buf) == 0 || buf[0] == '%')) 
            file.getline(buf, len);

      if (!file.eof())
         return true;
      else {
         cerr << "Invalid file: incomplete information." 
              << endl;
         return false;
      }
}

bool AggSourceFileReader::readDistribution(
    fstream&		file, 
    distribution_t&	distr,
    bool		show_process)
{
   char s[BUFFERSIZE];

   //read distribution type
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading distribution type failed."
              << endl;
         return false;
      }

      istringstream dataline(s);
      int type;
      dataline >> type;
      if (type > MAX_DISTRIBUTION_TYPE_NUMBER || type < 0){
         cerr << "Invalid distribution type: " 
              << type
              << endl;
         cerr << "Reading distribution type failed."
              << endl;
         return false;
      }
      distr.first = static_cast<distributiontype_t>(type); 

      if (show_process)
         cout << "Distribution type " << type 
              << endl; 
   }

   // read number of parameters 
   int n;
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading number of distribution parameters failed."
              << endl;
         return false;
      }

      istringstream dataline(s);
      dataline >> n;
      if (n > MAX_PARAMETER_NUMBER) {
         cerr << "Invalid number of distribution parameters: "
              << n
              << endl;
         cerr << "Reading number of distribution parameters failed."
              << endl;
         return false; 
      }

      if (show_process)
         cout << "Number of parameters " << n 
              << endl; 
   }

   // read parameters
   for (int i = 0; i < n; ++i) {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading distribution parameters failed."
              << endl;
         cerr << i << " parameters has been read."
              << endl;
         return false;
      }

      istringstream dataline(s);
      double par;
      if ((distr.first == FILENAME) && (i == 0)) {
         char* filename = new char[BUFFERSIZE];
         strcpy(filename, "");
         dataline >> filename;
         long file_address = reinterpret_cast<long>(filename);
         par = static_cast<double>(file_address);
      }
      else 
         dataline >> par;
      distr.second.push_back(par);

      if (show_process)
         cout << "Parameter " << par 
              << endl; 
   }

   if (show_process)
      return true;

   // Check whether the distribution is valid
   if (RandomNumGenerator::checkDistribution(distr) == true)
      return true;
   else {
      cout << "Invalid distribution" << endl;
      return false;
   }
}

bool AggSourceFileReader::readDistrVect(
    fstream&			file, 
    vector<distribution_t>&	distr_vect, 
    bool			show_process)
{
   char s[BUFFERSIZE];

   // read number of distributions 
   int n;
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading number of distributions failed."
              << endl;
         return false;
      }

      istringstream dataline(s);
      dataline >> n;
      if (n > MAX_PARAMETER_NUMBER) {
         cerr << "Invalid number of distributions: "
              << n
              << endl;
         cerr << "Reading number of distributions failed."
              << endl;
         return false; 
      }

      if (show_process)
         cout << "Number of distributions " << n 
              << endl; 
   }

   // read distributions
   for(int i = 0; i < n; ++i) {
      distribution_t* distr = new(distribution_t);
      if (readDistribution(file, *distr, show_process)) {
         distr_vect.push_back(*distr);
         delete distr;
      }
      else {
         delete distr;
         cerr << "Reading distribution vector failed."
              << endl;
         cerr << i << " distributions has been read."
              << endl;
         return false;
      }
   }

   return true;
}

bool AggSourceFileReader::readAttribueDef(
    fstream&		file, 
    AttributeDef&	attr_def,
    bool		show_process)
{
   char s[BUFFERSIZE];

   // read attribute data type
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading attribute data type failed."
              << endl;
         return false;
      }

      istringstream dataline(s);
      int type;
      dataline >> type;
      if (type > MAX_DATA_TYPES || type < 0){
         cerr << "Invalid attribute data type: " 
              << type
              << endl;
         cerr << "Reading attribute data type failed."
              << endl;
         return false;
      }
      attr_def.attribute_data_type.type = static_cast<attr_type_t>(type); 

      if (show_process)
         cout << "Attribute data type " << type 
              << endl; 
   }

   // read attribute data lengh
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading attribute data length failed."
              << endl;
         return false;
      }

      istringstream dataline(s);
      int length;
      dataline >> length; 
      if (length < 0){
         cerr << "Invalid attribute data length: " 
              << length
              << endl;
         cerr << "Reading attribute data length failed."
              << endl;
         return false;
      }
      attr_def.attribute_data_type.length = length;

      if (show_process)
         cout << "Attribute data length " << length 
              << endl; 
   }

   // read source type
   {
      if (readLine(file, s, sizeof(s)) == false) {
         cerr << "Reading attribute distribution type failed."
              << endl;
         return false;
      }

      istringstream dataline(s);
      int type;
      dataline >> type;
      if (type > MAX_DISTRIBUTION_TYPE_NUMBER || type < 0){
         cerr << "Invalid distribution type: " 
              << type
              << endl;
         cerr << "Reading attribute distribution type failed."
              << endl;
      }
      attr_def.source_type = static_cast<distributiontype_t>(type); 

      if (show_process)
         cout << "Attribute source type " << type 
              << endl; 
   }
   
   // read source distribution parameters
   if (readDistrVect(file, attr_def.source_parameters, show_process) == false) {
      cerr << "Readinding distribution vector for attribute source"
           << " failed."
           << endl;
      return false;
   }
   return true;
}

}
