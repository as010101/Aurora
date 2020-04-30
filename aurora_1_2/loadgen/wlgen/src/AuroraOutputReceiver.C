#include "AuroraOutputReceiver.H"

// C++ headers
#include <sstream>			// for istringstream
#include <iostream>			// for cin, cout, cerr
#include <iomanip>			// for setw, left
#include <cstdio>			// for perror
#include <cerrno>			// for perror

// Unix headers
#include <unistd.h>			// for read 

// Local headers
#include "SocketClient.H"		// for ::socket_tools::SocketClient
#include "Types.H"			// for source_stream_id_t,
#include "DataType.H"			// for DataType*
#include "AggStreamSource.H"		// for AggStreamSource
#include "AggSourceFileReader.H"	// for AggSourceFileReader

using namespace std;
namespace workload_generator{

AuroraOutputReceiver::AuroraOutputReceiver(
      const char*	auroraOutputServerName, 
      int		auroraOutputServerPort):
   _auroraOutputServerName(auroraOutputServerName),
   _auroraOutputServerPort(auroraOutputServerPort),
   _display_data(true),
   _output_to_file(false)
{
}
  
AuroraOutputReceiver::AuroraOutputReceiver():
   _auroraOutputServerName(DEFALT_AURORA_OUTPUT_SERVER_NAME),
   _auroraOutputServerPort(DEFALT_AURORA_OUTPUT_SERVER_PORT),
   _display_data(true),
   _output_to_file(false)
{
}


AuroraOutputReceiver::~AuroraOutputReceiver() 
{
   for (cinfo_list_t::iterator it = _client_info_list.begin();
        it != _client_info_list.end();
        ++it) {
      close(it->sock);
   }
   _output_file.close();

   for (vector<pthread_t*>::iterator it = _client_threads.begin();
        it != _client_threads.end();
        ++it) {
      delete *it;
   }

}

void AuroraOutputReceiver::startClient() 
{
   pthread_t* receiver_thread = new pthread_t;
   if (pthread_create(receiver_thread,
                      0,
                      (void*(*)(void*))receiverHandler, 
                      (void*)&_new_client_info_list) < 0) 
      perror("AuroraOutputReceiver: creating thread");
   else
      _client_threads.push_back(receiver_thread);
}

void AuroraOutputReceiver::addPort(int port, int len)
{
   if (port == -1)
      return;
   
   ConnectionInfo ci;

   // set port
   ci.port = port;

   // set len
   schema_t::iterator pos = _schemas.find(port);
   if (pos != _schemas.end()) {
      datatype_list_t& type_list = pos->second;
      ci.len = 0;
      for (datatype_list_t::iterator it = type_list.begin();
           it != type_list.end();
           ++it) {
         ci.len += it->length;
      }
   }
   else {
      ci.len = len;
   }

   // set reciever address
   ci.receiver = this;

   // add to new client list
   _new_client_info_list.push_back(ci);
}

void AuroraOutputReceiver::receiverHandler(cinfo_list_t* c_info_list)
{
   int n = c_info_list->size();
   if (n == 0)
      return;

   AuroraOutputReceiver* me = c_info_list->back().receiver;
   ::socket_tools::SocketClient client(
         me->_auroraOutputServerName.c_str(),
         me->_auroraOutputServerPort,
         DEFALT_AURORA_OUTPUT_SERVER_TYPE);
   int sock = client.connect_server();
   if (sock == -1) {
      cout << endl;
      cout << "Aurora output server address error or the server is not up."
           << endl;
      return;
   }
   cout << endl;
   cout << "Connection with Aurora output server estabilished at socket"
        << sock << endl;

   // send number of ports
   if (!cwrite(sock, (char*)&n, sizeof(int))) {
      close(sock);
      return;
   }

   for (cinfo_list_t::iterator cit = c_info_list->begin();
        cit != c_info_list->end();
        ++cit) {
      cit->sock = sock;
      int port = cit->port;
      int len = cit->len;

      // write port
      if (!cwrite(sock, (char*)&port, sizeof(int))) {
         close(sock);
         return;
      }
 
      // write tuple length
      if (!cwrite(sock, (char*)&len, sizeof(int))) {
         close(sock);
         return;
      }

      me->_client_info_list.push_back(*cit);
   }
   me->_new_client_info_list.clear();

   while (true) {
      char* buf;

      buf = new char[16];

      // receive packet size
      if (!cread(sock, buf, sizeof(int))) {
         me->deleteConnection(sock);
         return;
      }
      int packet_size = *(int*)buf;
      delete [] buf;

      if (packet_size > MAX_TUPLE_SIZE + sizeof(int)) {
         cout << "Tuple size too big: " << packet_size << endl;
         me->deleteConnection(sock);
         return;
      }

      // read packet
      buf =  new char[packet_size];
      //read(sock, packet, packet_size);
      if (!cread(sock, buf, packet_size)) {
         delete [] buf;
         me->deleteConnection(sock);
         return;
      }

      // check the packet
      me->readPacket(buf, packet_size);
      delete [] buf;
   }
   me->deleteConnection(sock);
   return;
}


void AuroraOutputReceiver::readPacket(char* packet, int len)
{
   // read port
   int nid = *(int*)packet;
   int data_len = len - sizeof(int);

   if (_output_to_file) {
      if ( _schemas.find(nid) != _schemas.end()) {
         datatype_list_t& type_list = _schemas[nid];
         //read data
         char* next_data = packet + sizeof(int);
         int schema_len = 0;
         for (datatype_list_t::iterator it = type_list.begin();
              it != type_list.end();
              ++it) {
            schema_len += it->length;
         }

         if (data_len == schema_len) {
            if (_write_output_port)
               _output_file << setw(10) << nid; 
            for (datatype_list_t::iterator it = type_list.begin();
                 it != type_list.end();
                 ++it) {
               _output_file << setw(15);
               DataType::outputData(*it, next_data, _output_file);
               next_data += (*it).length;
            }
            _output_file << endl;
         }
      }
   }

   if (_display_data == true) {
      cout << setw(10) << nid << ":   " ; 
      if ( _schemas.find(nid) == _schemas.end()) {
         cout << "Data with length " << data_len; 
         cout << ", schema unknown." << endl;
      }
      else {
         datatype_list_t& type_list = _schemas[nid];
         //read data
         int schema_len = 0;
         for (datatype_list_t::iterator it = type_list.begin();
              it != type_list.end();
              ++it) {
            schema_len += it->length;
         }

         if (data_len != schema_len)
             cout << "Data lengh doesn't match its schema." 
                  << endl
                  << "Received data length: " << data_len
                  << "  Schema data length: " << schema_len
                  << endl;
         else {
             //read data
             char* next_data = packet + sizeof(int);
             for (datatype_list_t::iterator it = type_list.begin();
                  it != type_list.end();
                  ++it) {
                cout << setw(15);
                DataType::outputData(*it, next_data, cout);
                next_data += (*it).length;
             }
             cout << endl;
         }
      } 
   }
}

bool AuroraOutputReceiver::cwrite(int sock, char* packet, int len)
{
   int rc = write(sock, packet, len); 

   if (rc == -1) {
      if (errno == EPIPE) {
         cout << "AuroraOutputReceiver:: socket " << sock 
              << " closed by server." << endl;
      }
      else {
         cerr << "AuroraOutputReceiver: write socke " << sock 
              << " error." << endl;
         perror(""); 
      }
      close(sock);
      return false;
   }
   if (rc != len) {
      cout << "AuroraOutputReceiver:: socket " << sock 
           << " closed by server." << endl;
      close(sock);
      return false;
   }
   return true;
}


bool AuroraOutputReceiver::cread(int sock, char* buf, int len) 
{
   int rlen = 0;
   while (rlen < len) {
      int rval = read(sock, buf + rlen, len - rlen);
      if (rval == 0) {
         cout << "AuroraOutputReceiver: connection closed by server at socket "  
              << sock
              << endl;
         close(sock);
         return false;
      }
      if (rval == -1) {
         if (errno != EBADF) {
            cout << "AuroraOutputReceiver: read socket " << sock << " error." 
                 << endl;
            perror("");
         }
         close(sock);
         return false;
      }
      rlen += rval;
   }
   return true;
}

void AuroraOutputReceiver::deleteConnection(int sock)
{
   cinfo_list_t::iterator cit = _client_info_list.begin();
   while (cit != _client_info_list.end()) {
      if (cit->sock == sock) {
         cit = _client_info_list.erase(cit);
         close(sock);
      }
      else {
         ++cit;
      }
   }
}

void AuroraOutputReceiver::addSchema(source_stream_id_t id, datatype_list_t types)
{
   _schemas[id] = types;

   int len = 0;
   for (datatype_list_t::iterator it = types.begin();
        it != types.end();
        ++it) {
      len += it->length;
   }

   addPort(id, len);
}

void AuroraOutputReceiver::addSchema()
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

void AuroraOutputReceiver::delSchema(source_stream_id_t id)
{
   _schemas.erase(id);
}

void AuroraOutputReceiver::readSchema(const char* filename)
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

void AuroraOutputReceiver::readSchemas(const char* filename)
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
   file.close();
}

void AuroraOutputReceiver::readSchemaFile(const char* filename)
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

void AuroraOutputReceiver::writeSchemaFile(const char* filename)
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

void AuroraOutputReceiver::writeOutputFile(
      const char* filename, 
      bool write_output_port)
{
   if (!_output_to_file) {
      _output_file.open(filename, ios::out);
      if (!_output_file) {
         cerr << "Can not open file: " << filename << endl;
         return;
      }
      _output_file.setf(ios::left);
      _output_to_file = true;
      _write_output_port = write_output_port;
      cout << "Received data will be write to file: " 
           << filename << endl;
   }
   else {
      cout << "Output file has been opened already." << endl;
   }
}

void AuroraOutputReceiver::stopOutputToFile()
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

void AuroraOutputReceiver::printSchema(source_stream_id_t id, datatype_list_t types)
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

void AuroraOutputReceiver::printSchemas()
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

void AuroraOutputReceiver::printPorts()
{
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << setw(20) << "output port";
   cout << setw(20) << "socket";
   cout << setw(20) << "length";
   cout << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   for (cinfo_list_t::iterator it = _client_info_list.begin();
        it != _client_info_list.end();
        ++it) {
      cout << setw(20) << it->port;
      cout << setw(20) << it->sock;
      cout << setw(20) << it->len;
      cout << endl;
   }
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}


void AuroraOutputReceiver::startCommandReceiver()
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
         startClient(); 
         continue;
      }

      if (!strcmp(command, "a")){
         int port = -1;
         command_line >> port;
         int len = 0;
         command_line >> len;
         addPort(port, len);
         continue;
      }

      if (!strcmp(command, "i")){
         addSchema();
         continue;
      }

      if (!strcmp(command, "d")){
         int sock;
         command_line >> sock;
         deleteConnection(sock);
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
         writeOutputFile(filename, true);
         continue;
      }

      if (!strcmp(command, "u")){
         char filename[MAX_COMMAND_LEN] = "";         
         command_line >> filename;
         writeOutputFile(filename, false);
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


      if (!strcmp(command, "l")){
         printPorts();
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

void AuroraOutputReceiver::printUsage()
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
   cout << setw(10) << "a" 
        << setw(15) << "port len"
        << setw(50) << "Add port and tuple len to receive data."
        << endl;
   cout << setw(10) << "c" 
        << setw(15) << ""
        << setw(50) << "Connect to Aurora output sender server."
        << endl;
   cout << setw(10) << "l" 
        << setw(15) << ""
        << setw(50) << "List all connected output ports."
        << endl;
   cout << setw(10) << "d" 
        << setw(15) << "socket"
        << setw(50) << "delete connection at specified socket."
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
        << setw(15) << "filename"
        << setw(50) << "Write output port and received data to a file."
        << endl;
   cout << setw(10) << "u" 
        << setw(15) << "filename"
        << setw(50) << "Write received data to a file without output port."
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
