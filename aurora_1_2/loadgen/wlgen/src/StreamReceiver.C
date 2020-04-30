#include "StreamReceiver.H"

// C++ headers
#include <sstream>			// for istringstream
#include <iostream>			// for cin, cout, cerr
#include <iomanip>			// for setw, left
#include <cstdio>			// for perror
#include <cerrno>			// for perror
#include <cmath>			// for ceil

// Unix headers
#include <unistd.h>			// for read 
#include <sys/time.h>			// for gettimeofday

// Local headers
#include "Types.H"			// for source_stream_id_t,
#include "DataType.H"			// for DataType*
#include "AggSourceFileReader.H"	// for AggSourceFileReader
#include "AggStreamSource.H"		// for AggStreamSource

using namespace std;

namespace workload_generator{

StreamReceiver::StreamReceiver():
   _port(DEFAULT_PORT),
   _num_handler_threads(DEFAULT_NUM_HANDLERS),
   _display_data(true),
   _output_to_file(false),
   _server(0),
   _average_data_rate(0),
   _count(0)
{
}

StreamReceiver::StreamReceiver(int port, int num_handler_threads):
   _port(port),
   _num_handler_threads(num_handler_threads),
   _display_data(true),
   _output_to_file(false),
   _server(0),
   _average_data_rate(0),
   _count(0)
{
}

StreamReceiver::~StreamReceiver() 
{
   if (_server != 0)
      delete _server;
}

bool StreamReceiver::initialize()
{
   // initialize mutex, time
   gettimeofday(&_last_time, 0);
   if (pthread_mutex_init(&_counter_mutex, 0) < 0) {
      perror("StreamReceiver: counter mutex init failed. ");
      return false;
   }
   // start counter thread
   if (pthread_create(&_counter_thread,
                      0,
                      (void*(*)(void*))counterHandler, 
                      (void*)this) < 0) {
      perror("StreamReceiver: creating counter thread failed. ");
      return false;
   }

   cout << "Starting input receiver... " << endl;
   _server = new ::socket_tools::SocketServer(
         _port, 
         DEFAULT_TYPE, 
         _num_handler_threads,
         (void(*)(int, void*))receiverHandler,
         this);
   if (_server->start()) {
      cout << "input receiver started at port "
           << _port
           << endl;
      return true;
   }
   else {
      return false;
   }
}

void StreamReceiver::receiverHandler(int sock, StreamReceiver* me)
{
   cout << endl;
   cout << "StreamReceiver: connection request recieved at socket " << sock;
   cout << endl;
   while (1) {
      char buffer[16];
      // receive packet size
      if (!me->cread(sock, buffer, sizeof(int))) {
         return;
      }
      int packet_size = *(int*)buffer;

      if (packet_size > MAX_TUPLE_SIZE + sizeof(int)) {
         cout << "Tuple size too big" << endl;
         return;
      }

      // read packet
      char* buf =  new char[packet_size];
      //read(sock, packet, packet_size);
      if (!me->cread(sock, buf, packet_size)) {
         delete [] buf;
         return;
      }

      // check the packet
      me->readPacket(buf, packet_size);
      delete [] buf;

      // record data rate
      if (pthread_mutex_lock(&(me->_counter_mutex)) < 0)
         perror("StreamReceiver: pthread_mutex_lock");
      ++(me->_count);
      if (pthread_mutex_unlock(&(me->_counter_mutex)) < 0)
         perror("StreamReceiver: pthread_mutex_unlock");
   }
   close (sock);
   return;
}

bool StreamReceiver::cread(int sock, char* buf, int len) 
{
   int rlen = 0;
   while (rlen < len) {
      int rval = read(sock, buf + rlen, len - rlen);
      if (rval == 0) {
         cout << "StreamReceiver: connection closed by client at socket "  
              << sock
              << endl;
         close(sock);
         return false;
      }
      if (rval == -1) {
         cout << "StreamReceiver: read socket " << sock << " error." << endl;
         perror("");
         close(sock);
         return false;
      }
      rlen += rval;
   }
   return true;
}

void StreamReceiver::readPacket(char* packet, int len)
{
   // read port
   int nid = *(int*)packet;
   int data_len = len - sizeof(int);


   if (_display_data)
      cout << setw(10) << nid << ":  " ; 


   if ( _schemas.find(nid) == _schemas.end()) {
      if (_display_data) {
         cout << "Data with length " << data_len; 
         cout << ", schema unknown." << endl;
      }
      return;
   }

   datatype_list_t& type_list = _schemas[nid];
   int schema_len = 0;
   for (datatype_list_t::iterator it = type_list.begin();
        it != type_list.end();
        ++it) {
      schema_len += it->length;
   }

   if (data_len != schema_len) {
      if (_display_data)
         cout << "Data lengh doesn't match its schema." 
              << endl
              << "Received data length: " << data_len
              << "  Schema data length: " << schema_len
              << endl;
      return;
   }

   // write port
   if (_output_to_file)
      _output_file << setw(10) << nid; 

   // write data
   char* next_data = packet + sizeof(int);
   for (datatype_list_t::iterator it = type_list.begin();
        it != type_list.end();
        ++it) {
      if (_output_to_file) {
         _output_file << setw(15);
         DataType::outputData(*it, next_data, _output_file);
      }
      
      if (_display_data)
         DataType::printData(*it, next_data);

      next_data += (*it).length;
   }

   if (_output_to_file) 
      _output_file << endl;

   if (_display_data)
      cout << endl;
}


void StreamReceiver::addSchema(source_stream_id_t id, datatype_list_t types)
{
   _schemas[id] = types;
}

void StreamReceiver::addSchema()
{
   source_stream_id_t id = 0;
   cout << "Please input aggregate stram ID: ";
   while (!(cin >> id)) {
      cout << "Please input aggregate stram ID: ";
   }

   int num = 0;
   cout << "Please input number of attributes: "; 
   while (!(cin >> num)) {
      cout << "Please input number of attributes: "; 
   }

   datatype_list_t type_list;
   if (num <= 0 || num > MAX_ATTRIBUTE_NUMBER) {
      cout << "Number of attributes is not valid." << endl;
      cout << "Please try again." << endl;
      return;
   }

   cout << "Types: 0(INTEGER), 1(FLOAT), 2(DOUBLE), 3(STRING)" << endl;
   for (int i = 0; i < num; ++i) {
      bool correct_type = false;
      datatype_t data_type;
      while(!correct_type) {
         cout << "Please input type for Attribute " << i + 1 << ": ";
         int type = 0;
         while(!(cin >> type)) {
            cout << "Please input type for Attribute " << i + 1 << ": ";
         }
         data_type.type = static_cast<attr_type_t>(type);
         switch (data_type.type) {
            case INTEGER:
               data_type.length = 4;
               correct_type = true;
               break;
            case FLOAT:
               data_type.length = 4;
               correct_type = true;
               break;
            case DOUBLE:
               data_type.length = 8;
               correct_type = true;
               break;
            case STRING:
               cout << "Please input string length: ";
               cin >> data_type.length;
               correct_type = true;
               break;
            default:
               break;
         }
      }
      type_list.push_back(data_type);
   }
   addSchema(id, type_list);
   cout << "New schema added:" << endl; 
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   printSchema(id, type_list); 
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   char tmp[16];
   cin.getline(tmp, 16);
}

void StreamReceiver::delSchema(source_stream_id_t id)
{
   _schemas.erase(id);
}

void StreamReceiver::readSchema(const char* filename)
{
   
   source_stream_id_t id;
   datatype_list_t type_list;
   AggStreamSource new_agg_source;
   if (AggSourceFileReader::readFile(filename, new_agg_source, false)
       == true) {
      id = new_agg_source.id.numberID;
      for (vector<AttributeDef>::iterator 
            it = new_agg_source.attributes.begin();
           it != new_agg_source.attributes.end();
           ++it) {
         type_list.push_back(it->attribute_data_type);
      }
      addSchema(id, type_list);
   }
}

void StreamReceiver::readSchemas(const char* filename)
{
   fstream file;
   file.open(filename, ios::in);
   if (! file) {
      cerr << "Can not open file: " << filename << endl;
      return;
   }

   source_stream_id_t id;
   datatype_list_t type_list;
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
              id = new_agg_source.id.numberID;
              type_list.clear();
              for (vector<AttributeDef>::iterator 
                    it = new_agg_source.attributes.begin();
                   it != new_agg_source.attributes.end();
                   ++it) {
                 type_list.push_back(it->attribute_data_type);
              }
              addSchema(id, type_list);
          }
      }
   }
}

void StreamReceiver::readSchemaFile(const char* filename)
{
   fstream file;
   file.open(filename, ios::in);
   if (! file) {
      cerr << "Can not open file: " << filename << endl;
      return;
   }

   while (!file.eof()) {
      int port = -1;
      source_stream_id_t id;
      datatype_list_t type_list;
      int num = 0;
 
      char buf[1024];
      file >> port;
      if (port == -1) {
         file.close();
         return;
      }
      id = static_cast<source_stream_id_t>(port);
      file.getline(buf, sizeof(buf));
      file >> num;
      file.getline(buf, sizeof(buf));
 
      if (num <= 0 || num > MAX_ATTRIBUTE_NUMBER) {
         cout << "Number of attributes is not valid: " << num << endl;
         cout << "Read file " << filename << " failed." << endl;
         file.close();
         return;
      }
 
      for (int i = 0; i < num; ++i) {
         datatype_t data_type;
         int type = -1;
         file >> type;
         file.getline(buf, sizeof(buf));
         data_type.type = static_cast<attr_type_t>(type);
         file >> data_type.length;
         file.getline(buf, sizeof(buf));

         switch (data_type.type) {
            case INTEGER:
            case FLOAT:
            case DOUBLE:
            case STRING:
               break;
            default:
               cout << "Attributes type is not valid: " 
                    << (int)data_type.type << endl;
               cout << "Read file " << filename << " failed." << endl;
               file.close();
               return;
         }
         type_list.push_back(data_type);
      }
      addSchema(id, type_list);
   }
   file.close();
}

void StreamReceiver::writeSchemaFile(const char* filename)
{
   fstream file;
   file.open(filename, ios::out);
   if (! file) {
      cerr << "Can not open file: " << filename << endl;
      return;
   }

   for (schema_t::iterator it = _schemas.begin();
        it != _schemas.end();
        ++it) {
      file << it->first;
      file << "      // port" << endl;
      file << it->second.size();
      file << "      // number of attributes" << endl;
      for (datatype_list_t::iterator tit = it->second.begin();
           tit != it->second.end();
           ++tit) {
         file << (int)tit->type;
         file << "      // data type" << endl;
         file << (int)tit->length;
         file << "      // data length" << endl;
      }
      file << endl;
   }
}

void StreamReceiver::writeOutputFile(const char* filename)
{
   if (!_output_to_file) {
      _output_file.open(filename, ios::out);
      if (!_output_file) {
         cerr << "Can not open file: " << filename << endl;
         return;
      }
      _output_file.setf(ios::left);
      _output_to_file = true;
      cout << "Received data will be write to file: " 
           << filename << endl;
   }
   else {
      cout << "Output file has been opened already." << endl;
   }
}

void StreamReceiver::stopOutputToFile()
{
   if (_output_to_file) {
      _output_file.close();
      _output_to_file = false;
      cout << "Writing to output file stopped." << endl;
   }
   else {
      cout << "No output file has been opened yet." << endl;
   }

}

void StreamReceiver::printSchema(source_stream_id_t id, datatype_list_t types)
{
   cout << id;
   cout << ":  ";
   for (datatype_list_t::iterator tit = types.begin();
        tit != types.end();
        ++tit) {
      cout << setw(15);
      DataType::printType(*tit);
   }
   cout << endl;
}

void StreamReceiver::printSchemas()
{
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   for (schema_t::iterator it = _schemas.begin();
        it != _schemas.end();
        ++it) {
      cout << it->first;
      cout << ":  ";
      for (datatype_list_t::iterator tit = it->second.begin();
           tit != it->second.end();
           ++tit) {
         cout << setw(15);
         DataType::printType(*tit);
         cout << "  ";
      }
      cout << endl;
   }
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}

void StreamReceiver::printDataRate() 
{
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << "Received data rate:      "
        << setw(15) << ceil(_average_data_rate * 10)/10.0
        << " tuples/second." << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}

void StreamReceiver::counterHandler(StreamReceiver* me)
{
   while(true) {
      sleep(2);
      if (pthread_mutex_lock(&(me->_counter_mutex)) < 0)
         perror("StreamReceiver: pthread_mutex_lock");

         Timeval now;
         gettimeofday(&now, 0);
         Timeval tmp(now);
         now -= me->_last_time;
         me->_average_data_rate = (double)(me->_count) / now.doubleValue();
         me->_last_time = tmp;
         me->_count = 0;

      if (pthread_mutex_unlock(&(me->_counter_mutex)) < 0)
         perror("StreamReceiver: pthread_mutex_unlock");
   }
}

void StreamReceiver::startCommandReceiver()
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

      if (!strcmp(command, "i")){
         addSchema();
         continue;
      }

      if (!strcmp(command, "d")){
         source_stream_id_t id;
         command_line >> id;
         delSchema(id);
         continue;
      }

      if (!strcmp(command, "p")){
         printSchemas();
         continue;
      }

      if (!strcmp(command, "r")){
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         readSchema(filename);
         continue;
      }

      if (!strcmp(command, "b")){
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         readSchemas(filename);
         continue;
      }

      if (!strcmp(command, "f")){
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         readSchemaFile(filename);
         continue;
      }

      if (!strcmp(command, "w")){
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         writeSchemaFile(filename);
         continue;
      }

      if (!strcmp(command, "o")){
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         writeOutputFile(filename);
         continue;
      }

      if (!strcmp(command, "t")){
         stopOutputToFile();
         continue;
      }

      if (!strcmp(command, "m")){
         _schemas.clear();
         continue;
      }

      if (!strcmp(command, "s")){
         _display_data = !_display_data;
         continue;
      }

      if (!strcmp(command, "e")){
         printDataRate();
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

void StreamReceiver::printUsage()
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
   cout << setw(10) << "e" 
        << setw(15) << ""
        << setw(50) << "Print received data rate."
        << endl;
   cout << setw(10) << "s" 
        << setw(15) << ""
        << setw(50) << "Switch between displaying received data or not."
        << endl;
   cout << setw(10) << "r" 
        << setw(15) << "filename"
        << setw(50) << "Read schema infomation from input config file."
        << endl;
   cout << setw(10) << "b" 
        << setw(15) << "filename"
        << setw(50) << "Read several input config files from specified file."
        << endl;
   cout << setw(10) << "f" 
        << setw(15) << "filename"
        << setw(50) << "Read several schemas from specified schema file."
        << endl;
   cout << setw(10) << "w" 
        << setw(15) << "filename"
        << setw(50) << "Save current schemas to a schema file."
        << endl;
   cout << setw(10) << "i" 
        << setw(15) << ""
        << setw(50) << "Add schema information from input."
        << endl;
   cout << setw(10) << "o" 
        << setw(15) << ""
        << setw(50) << "Write received data to a file."
        << endl;
   cout << setw(10) << "t" 
        << setw(15) << ""
        << setw(50) << "Stop writing received data to file."
        << endl;
   cout << setw(10) << "p" 
        << setw(15) << ""
        << setw(50) << "Print all schema infomation."
        << endl;
   cout << setw(10) << "m" 
        << setw(15) << ""
        << setw(50) << "Clear all schema infomation."
        << endl;
   cout << setw(10) << "h" 
        << setw(15) << ""
        << setw(50) << "Print menu."
        << endl;
   cout << setw(10) << "q" 
        << setw(15) << ""
        << setw(50) << "Exit."
        << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}

}
