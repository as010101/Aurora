#include "WorkloadGenerator.H"

// C++ headers
#include <sstream>			// for istringstream
#include <iostream>			// for cin, cout, cerr
#include <iomanip>			// for setw, left
#include <ctime>			// for time
#include <cstdlib>			// for srand48
#include <fstream>			// for fstream

// Unix headers
#include <unistd.h>			// for getpid

// Local headers
#include "Types.H"			// for NEVER_USE_FOR_NUMBER_ID
					//     NEVER_USE_FOR_STRING_ID
                                        //     MAX_STRING_ID_LEN
                                        //     agg_stream_id_t


using namespace std;
namespace workload_generator {

WorkloadGenerator::WorkloadGenerator()
{
   initialize();
}

WorkloadGenerator::WorkloadGenerator(
      const char* aurora_receiver_name, 
      int port):
   _stream_sender(aurora_receiver_name, port)
{
   initialize();
}

void WorkloadGenerator::initialize()
{
   _stream_buffer.initialize(&_stream_sender);
   _source_generator.initialize(&_source_pool);
   _controller.initialize(&_source_pool, &_stream_buffer);

   // use random seed
   srand48(time(NULL)^getpid());
}

void WorkloadGenerator::connectAurora()
{
   _stream_sender.connectServer();
}

void WorkloadGenerator::connectAurora(
      const char* aurora_receiver_name, 
      int port)
{
   _stream_sender.connectServer(aurora_receiver_name, port);
}

void WorkloadGenerator::disconnectAurora()
{
   _stream_sender.stopConnection();
}

void WorkloadGenerator::addAggSource(AggStreamSource& agg_source)
{
   _source_generator.newSources(agg_source);
}

void WorkloadGenerator::changeRate(double ratio)
{
   _source_pool.changeRate(ratio);
}

void WorkloadGenerator::changRate(AggStreamSource& agg_source)
{
   _source_generator.changeSourceRate(agg_source);
}

void WorkloadGenerator::emptySourses()
{
   _source_pool.emptyPool();
}

void WorkloadGenerator::readAggInfoFile(char* filename)
{
   AggStreamSource new_agg_source;
   if (AggSourceFileReader::readFile(filename, new_agg_source, false)
       == true) {
      _source_generator.newSources(new_agg_source);
   }
}

void WorkloadGenerator::readAggInfoFiles(char* filename)
{
   fstream file;
   file.open(filename, ios::in);
   if (! file) {
      cerr << "Can not open file: " << filename << endl;
      return;
   }

   while (!file.eof()) {
      char aggfilename[1024];
      strcpy(aggfilename, "");
      file >> aggfilename;
      if (strcmp(aggfilename, "")) {
          AggStreamSource new_agg_source;
          if (AggSourceFileReader::readFile(
                aggfilename, 
                new_agg_source, false)
              == true) {
             _source_generator.newSources(new_agg_source);
          }
      }
   }
}

void WorkloadGenerator::printAggInfoFile(char* filename)
{
   AggStreamSource new_agg_source;
   if (AggSourceFileReader::readFile(filename, new_agg_source, false)
       == true) {
      new_agg_source.print();
   }
}

void WorkloadGenerator::printPartialAggInfoFile(char* filename)
{
   AggStreamSource new_agg_source;
   AggSourceFileReader::readFile(filename, new_agg_source, true);
   new_agg_source.print();
}


void WorkloadGenerator::deleteAggSource(int numberID)
{
   agg_stream_id_t agg_id;
   agg_id.numberID = numberID;
   strcpy(agg_id.stringID, NEVER_USE_FOR_STRING_ID); 
   _source_pool.delAggSources(agg_id);
}

void WorkloadGenerator::deleteAggSource(char* stringID)
{
   agg_stream_id_t agg_id;
   agg_id.numberID = NEVER_USE_FOR_NUMBER_ID;
   strcpy(agg_id.stringID, stringID); 
   _source_pool.delAggSources(agg_id);
}

void WorkloadGenerator::changRageByFile(char* filename)
{
   AggStreamSource new_agg_source;
   if (AggSourceFileReader::readFile(filename, new_agg_source, false)
       == true) {
      _source_generator.changeSourceRate(new_agg_source);
   }
}


void WorkloadGenerator::startCommandReceiver()
{
   printUsage();
   while (1) {
      const int MAX_COMMAND_LEN = 4096;
      char line[MAX_COMMAND_LEN];
      strcpy(line, "");
      cout << "--> ";
      cin.getline(line, sizeof(line));
      istringstream command_line(line);
      char command[MAX_COMMAND_LEN];
      strcpy(command, "");
      command_line >> command;

      if (strlen(command) == 0) {
         continue;
      }

      if (!strcmp(command, "exit") || 
          !strcmp(command, "quit") ||
          !strcmp(command, "q")) {
         return;
      }

      if (!strcmp(command, "c")){
         connectAurora();
         continue;
      }

      if (!strcmp(command, "cn")){
         char serverhost[MAX_COMMAND_LEN] = "";
         int port = -1;
         command_line >> serverhost;
         command_line >> port;
         connectAurora(serverhost, port);
         continue;
      }

      if (!strcmp(command, "dc")){
         disconnectAurora();
         continue;
      }

      if (!strcmp(command, "r")){
         // read file name
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         readAggInfoFile(filename);
         continue;
      }

      if (!strcmp(command, "b")){
         // read file name
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         readAggInfoFiles(filename);
         continue;
      }

      if (!strcmp(command, "p")){
         // read file name
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         printAggInfoFile(filename);
         continue;
      }

      if (!strcmp(command, "pp")){
         // read file name
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         printPartialAggInfoFile(filename);
         continue;
      }
      
      if (!strcmp(command, "m")){
         double ratio;
         if (command_line >> ratio) {
            if (ratio <= 0)
               cout << "Ratio must be larger than zero." << endl;
            else
               changeRate(ratio);
         }
         else 
            cout << "Please specify a ratio." << endl;
         continue;
      }

      if (!strcmp(command, "cr")){
         // read file name
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         changRageByFile(filename); 
         continue;
      }

      if (!strcmp(command, "d")){
         // read file name
         int numberID; 
         if(command_line >> numberID)
            deleteAggSource(numberID);
         else
            cout << "Invalid ID." << endl;
         continue;
      }

      if (!strcmp(command, "ds")){
         // read file name
         char stringID[MAX_COMMAND_LEN];
         if ((command_line >> stringID) &&
              strlen(stringID) < MAX_STRING_ID_LEN) 
           deleteAggSource(stringID);
         else
           cerr << "Invalid String ID." << endl;
         continue;
      }

      if (!strcmp(command, "da")){
         emptySourses();
         continue;
      }

      if (!strcmp(command, "h") || !strcmp(command, "?") 
                                || !strcmp(command, "help")){
         // print usage 
         printUsage();
         continue;
      }

      cout << "Invalid Command" << endl;
      
   }
}

void WorkloadGenerator::printUsage()
{
   cout.setf(ios::left);
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << setw(10) << "Command" 
        << setw(15) << "Parameters"
        << setw(40) << "Usage"
        << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << setw(10) << "r" 
        << setw(15) << "filename"
        << setw(50) << "Generate workload for specified configuration file."
        << endl;
   cout << setw(10) << "b" 
        << setw(15) << "filename"
        << setw(50) << "Generate workload for specified files."
        << endl;
   cout << setw(10) << "m" 
        << setw(15) << "ratio"
        << setw(50) << "Mutiply current data rate by specified ratio."
        << endl;
   cout << setw(10) << "cr" 
        << setw(15) << "filename"
        << setw(50) << "Change rate according to specified configuration file."
        << endl;
   cout << setw(10) << "d" 
        << setw(15) << "number ID"
        << setw(50) << "Stop generating load for specified stream."
        << endl;
   cout << setw(10) << "ds" 
        << setw(15) << "string ID"
        << setw(50) << "Stop generating load for specified stream."
        << endl;
   cout << setw(10) << "da" 
        << setw(15) << ""
        << setw(50) << "Stop all load generation."
        << endl;
   cout << setw(10) << "c" 
        << setw(15) << ""
        << setw(50) << "Connect to Aurora receiver."
        << endl;
   cout << setw(10) << "cn" 
        << setw(15) << "host port"
        << setw(50) << "Connect to Aurora receiver at specified address."
        << endl;
   cout << setw(10) << "dc" 
        << setw(15) << ""
        << setw(50) << "Disconnect to Aurora receiver."
        << endl;
   cout << setw(10) << "p" 
        << setw(15) << "filename"
        << setw(50) << "Print Aggregate Stream Information from specified file."
        << endl;
   cout << setw(10) << "pp" 
        << setw(15) << "filename"
        << setw(50) << "Print what has been readed from specified file."
        << endl;
   cout << setw(10) << "h" 
        << setw(15) << ""
        << setw(50) << "Print menu"
        << endl;
   cout << setw(10) << "q" 
        << setw(15) << ""
        << setw(50) << "Exit"
        << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}

}

