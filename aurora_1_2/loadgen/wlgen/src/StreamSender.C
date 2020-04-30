#include "StreamSender.H"

// C++ headers				
#include <iostream>			// for cout
#include <cstdio>			// for perror
#include <cerrno>			// for perror

// Linux headers
#include <unistd.h>			// for write

// Local headers
#include "Types.H"			// for agg_stream_id_t

using namespace std;
namespace workload_generator{

StreamSender::StreamSender():
   _client(0), 
   _sock(-1),
   _connected(false), 
   _aurora_input_server_name(DEFAULT_AURORA_INPUT_SERVER_NAME),
   _aurora_input_server_port(DEFAULT_AURORA_INPUT_SERVER_PORT),
   _aurora_input_server_type(DEFAULT_AURORA_INPUT_SERVER_TYPE)
{
   _client = new ::socket_tools::SocketClient(
         _aurora_input_server_name.c_str(),
         _aurora_input_server_port,
         _aurora_input_server_type);
   connectServer();
}

StreamSender::StreamSender(
      const char* aurora_input_server_name, 
      int aurora_input_server_port):
   _client(0), 
   _sock(-1),
   _connected(false), 
   _aurora_input_server_name(aurora_input_server_name),
   _aurora_input_server_port(aurora_input_server_port),
   _aurora_input_server_type(DEFAULT_AURORA_INPUT_SERVER_TYPE)
{
   _client = new ::socket_tools::SocketClient(
         _aurora_input_server_name.c_str(),
         _aurora_input_server_port,
         _aurora_input_server_type);
   connectServer();
}

StreamSender::~StreamSender()
{
   if (_client != 0) {
      delete _client;
   }
}

void StreamSender::connectServer()
{ 
   if (_connected == true) {
      cout << "Already connected to Aurora." << endl;
      return;
   }

   if (_client == 0) {
      _client = new ::socket_tools::SocketClient(
            _aurora_input_server_name.c_str(),
            _aurora_input_server_port,
            _aurora_input_server_type);
   }

   cout << "Connecting to Aurora ...";
   if( (_sock = _client->connect_server()) == -1) {
      cout << "failed." << endl
           << "Aurora server is not active or "
           << "the host name port is not correct. " 
           << endl;
   }
   else {
      cout << "connection established." << endl;
      _connected = true;
   }
}

void StreamSender::connectServer(
      const char* aurora_input_server_name, 
      int aurora_input_server_port)
{
   if (_connected == true) {
      cout << "Already connected to Aurora." << endl;
      return;
   }

   if (_client != 0) {
      delete _client;
   }
   _aurora_input_server_name = aurora_input_server_name;
   _aurora_input_server_port = aurora_input_server_port;
   _client = new ::socket_tools::SocketClient(
         _aurora_input_server_name.c_str(),
         _aurora_input_server_port,
         _aurora_input_server_type);
   connectServer();
}

void StreamSender::stopConnection()
{
   if (_client != 0) 
      delete _client;
   _connected = false;
   _sock = -1;
   _client = 0;
}

void StreamSender::sendTuple(send_item_t* item)
{
   if (item->sender->_connected == false || 
       item->sender->_sock == -1) {
      delete item;
      return;
   }

   int sock = item->sender->_sock;
   static const int BUFFER_SIZE = 1024 * 1024;
   static char buffer[BUFFER_SIZE];

   int used_buffer_size = 0;
   int header_len = sizeof(int);
   for (tuplelist_t::iterator it = item->tuple_list->begin();
        it != item->tuple_list->end();
        ++it) {

      // comput packet size pur tuple
      int packet_size = header_len +	// for size and port
                        (*it)->getDataSize();


      if (used_buffer_size + packet_size + sizeof(int) > BUFFER_SIZE) {
         if (!item->sender->cwrite(sock, buffer, used_buffer_size)) {
            delete item;
            return;
         }
         used_buffer_size = 0;
      }

      // write tuple size
      memcpy(buffer + used_buffer_size, &packet_size, sizeof(int));
      used_buffer_size += sizeof(int);

      // write port
      int port = (*it)->getAggStreamID().numberID;
      memcpy(buffer + used_buffer_size, &port, sizeof(int));
      used_buffer_size += sizeof(int);

      // copy data
      memcpy(buffer + used_buffer_size, 
             (*it)->getDataHandle(), 
             (*it)->getDataSize());
      used_buffer_size += (*it)->getDataSize();
   }

   item->sender->cwrite(sock, buffer, used_buffer_size); 
   delete item;
}

bool StreamSender::cwrite(int sock, char* packet, int len)
{
   int rc = write(sock, packet, len); 

   if (rc == -1) {
      if (errno == EPIPE) {
         cout << "StreamSender::  socket " << sock 
              << " closed by server." << endl;
      }
      else {
         cerr << "StreamSender: write socke " << sock << " error." << endl;
         perror(""); 
      }
      stopConnection();
      return false;
   }
   if (rc != len) {
      cout << "StreamSender:: socket " << sock 
           << " closed by server." << endl;
      stopConnection();
      return false;
   }
   return true;
}

}
