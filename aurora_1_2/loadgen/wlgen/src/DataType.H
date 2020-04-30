#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <iostream>			// for ofstream

#include "Types.H"			// for datatype_t


using namespace std;
namespace workload_generator{

class DataType
{
public:
   // Return the size of data with specified type
   static int Size(const datatype_t& type); 

   // Typecast a double to the specified type
   static void TypeCast(const datatype_t& type, double data, char* result);

   // Print the data according to the specified type
   static void printData(const datatype_t& type, char* data);

   // Print the data according to the specified type
   static void outputData(const datatype_t& type, char* data, ostream& ostrm);

   // Print the type 
   static void printType(const datatype_t& type);
};

}
#endif
