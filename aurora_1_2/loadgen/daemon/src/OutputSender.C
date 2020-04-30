#include "OutputSender.H"

// C++ headers
#include <iostream>			// for cout
#include <iomanip>			// for setw
#include <cmath>			// for ceil

// Unix headers
#include <unistd.h>			// for read 
#include <stdio.h>			// for perror
#include <errno.h>			// for perror
#include <sys/time.h>			// for gettimeofday

// Aurora header
#include <aurora.H>			// for aurora API functions 

using namespace std;
namespace aurora_socket_api{

OutputSender::OutputSender():
   _server(0),
   _port(OUTPUT_SENDER_DEFAULT_PORT),
   _num_handler_threads(OUTPUT_SENDER_DEFAULT_NUM_HANDLERS),
   _aurora_instance(0),
   _average_data_rate(0),
   _count(0)
{
}

OutputSender::OutputSender(int port, int num_handler_threads):
   _server(0),
   _port(port),
   _num_handler_threads(num_handler_threads),
   _aurora_instance(0),
   _average_data_rate(0),
   _count(0)
{
}

OutputSender::~OutputSender() 
{
   if (_server != 0)
      delete _server;
}

bool OutputSender::initialize(void* aurora_instance)
{
   _aurora_instance = aurora_instance;

   // read output ports from Aurora
   AURORA_API_RETURN_CODE rc;
   int numPorts;
   rc = aurora_getNumOutputPorts(_aurora_instance, & numPorts);
   if (rc != AURORA_OK)
   {
      cout << "Error: aurora_getNumOutputPorts(...) returned " << (int)rc;
      return false;
   }
   for (int i = 0; i < numPorts; ++i)
   {
      int portName;
      rc = aurora_getOutputPortNum(_aurora_instance, i, & portName);
      if (rc != AURORA_OK)
      {
         cout << "Error: aurora_getOutputPortNum(...) returned " << (int)rc;
         return false;
      }
      _output_ports.insert(portName);
   }

   // initialize mutex, time
   gettimeofday(&_last_time, 0);
   if (pthread_mutex_init(&_counter_mutex, 0) < 0) {
      perror("OutputSender: counter mutex init failed. ");
      return false;
   }
   // start counter thread
   if (pthread_create(&_counter_thread,
                      0,
                      (void*(*)(void*))counterHandler, 
                      (void*)this) < 0) {
      perror("OutputSender: creating counter thread failed. ");
      return false;
   }

   // start sender threads
   if (pthread_create(&_sender_thread,
                      0,
                      (void*(*)(void*))senderHandler, 
                      (void*)this) < 0) {
      perror("OutputSender: creating sender thread failed. ");
      return false;
   }
   
   // start connection handling threads
   cout << "Starting output sender server... " << endl;
   _server = new ::socket_tools::SocketServer(
         _port, 
         OUTPUT_SENDER_DEFAULT_TYPE,
         _num_handler_threads,
         (void(*)(int, void*))connectionHandler,
         this);
   if (_server->start()) {
      cout << "Output sender server started at port " 
           << _port
           << endl;
      return true;
   }
   else {
      cout << "Output sender initialization failed." << endl;
      return false;
   }
}


void OutputSender::senderHandler(OutputSender* me)
{
   const int BUFFER_SIZE = 1024 * 1024;
   bool moreTuples = true;
   while (moreTuples)
   {
      char tupleBuffer[BUFFER_SIZE]; 

      // get data from aurora
      int wasDequeued;
      int port;
      int num_tuples;
      /*
      AURORA_API_RETURN_CODE rc = aurora_deqRecord(
            me->_aurora_instance,
            &wasDequeued, 
            &port, 
            tupleBuffer, 
            sizeof(tupleBuffer));
      if (rc != AURORA_OK)
      {
         cout << "Error: aurora_deqRecord returned " << (int)rc << endl;
      }
      */

      AURORA_API_RETURN_CODE rc = aurora_deqBatch(
            me->_aurora_instance,
            &wasDequeued, 
            &port, 
            &num_tuples,
            tupleBuffer, 
            sizeof(tupleBuffer));
      if (rc != AURORA_OK)
      {
         cout << "Error: aurora_deqRecord returned " << (int)rc << endl;
      }
            
      if (wasDequeued)
      {
         // set count for rate
         if (pthread_mutex_lock(&(me->_counter_mutex)) < 0)
            perror("OutputSender: pthread_mutex_lock");
         me->_count += num_tuples;
         if (pthread_mutex_unlock(&(me->_counter_mutex)) < 0)
            perror("OutputSender: pthread_mutex_unlock");

         // find all clients requesting data from specified port 
         client_list_t client_list;
         me->_client_pool.findClients(port, client_list);

         if (client_list.size() > 0) {
            // find tuple size
            int tuple_size = ((client_list.begin())->second).length;

            char* next_data = tupleBuffer;
            while (num_tuples > 0) {
               next_data += TUPLE_OFFSET_USER_DATA;
               for (client_list_t::iterator it = client_list.begin();
                    it != client_list.end();
                    ++it) {
                  me->sendTuple(it->second, next_data);
                  
               }
               next_data += tuple_size;
               --num_tuples;
             }
         }
      }
      else
      {
         moreTuples = false;
      }
   }
}

void OutputSender::connectionHandler(int socket, OutputSender* me)
{
   cout << endl;
   cout << "OutputSender: connection request recieved at socket " << socket;
   cout << endl;

   ClientInfo newClient;

   newClient.sock = socket;

   bool valid_ports = false;

   // receive number of ports
   int n;
   if (!(me->cread(socket, (char*)&n, sizeof(int))))
      return;

   if (n > me->_output_ports.size()) {
      cout << "Number of of output ports is too large: "
           << n
           << endl;
      return;
   }

   for (int i = 0; i < n; ++i) {
      // receive port number 
      if (!(me->cread(socket, (char*)&(newClient.port), sizeof(int))))
         return;
 
      // receive tuple size
      if (!(me->cread(socket, (char*)&(newClient.length), sizeof(int))))
         return;
      //newClient.length = *(int*)buf;
 
      if (!(me->validPort(newClient.port))) {
         cout << "OutputSender: port number " << newClient.port << " invalid." 
              << endl;
         continue;
      }
 
      (me->_client_pool).addClient(newClient);
      cout << "Data from output port  ";
      cout << newClient.port; 
      cout << " will be send through socket ";
      cout << socket;
      cout << endl;
      valid_ports = true;
   }

   if (!valid_ports) {
      close(socket);
   }
}

void OutputSender::sendTuple(const client_info_t& client_info, char* packet)
{
   int sock = client_info.sock;

   // check offset
   if (TUPLE_OFFSET_USER_DATA > 2 * sizeof(int)) {
      // write head to the data buffer directly
      packet -= 2 * sizeof(int);
      // copy packet size
      int packet_size = sizeof(int) +	// for port
                        client_info.length;
      memcpy(packet, (char*)&packet_size, sizeof(int));

      // copy port
      memcpy(packet + sizeof(int), 
             (const char*)&(client_info.port), 
             sizeof(int));

      // send data
      if (!cwrite(sock, packet, client_info.length + sizeof(int) * 2)){
         _client_pool.deleteClient(client_info);
         return;
      }
   }
   else {
      // send packet size
      int packet_size = sizeof(int) +	// for port
                        client_info.length;
 
 
      if (!cwrite(sock, (char*)&packet_size, sizeof(int))) {
         _client_pool.deleteClient(client_info);
         return;
      }
       
      // send port
      if (!cwrite(sock, (const char*)&(client_info.port), sizeof(int))) {
         _client_pool.deleteClient(client_info);
         return;
      }
 
      // send data
      if (!cwrite(sock, packet, client_info.length)){
         _client_pool.deleteClient(client_info);
         return;
      }
   }
}

bool OutputSender::validPort(int port)
{
   return (_output_ports.find(port) != _output_ports.end());
}

bool OutputSender::cread(int socket, char* buf, int len) 
{
   int rlen = 0;
   while (rlen < len) {
      int rval = read(socket, buf + rlen, len - rlen);
      if (rval == 0) {
         cout << "OutputSender: connection closed by client at socket "  
              << socket
              << endl;
         close(socket);
         return false;
      }
      if (rval == -1) {
         cout << "OutputSender: read socket " << socket << " error." << endl;
         perror("");
         close(socket);
         return false;
      }
      rlen += rval;
   }
   return true;
}

bool OutputSender::cwrite(int socket, const char* buf, int len)
{
   int rval = write(socket, buf, len);
   if (rval == -1) {
      if (errno == EPIPE) {
         cout << "OutputSender::  socket " << socket 
              << " closed by client." << endl;
      }
      else {
         cerr << "OutputSender: write socke " << socket << " error." << endl;
         perror(""); 
      }
      close(socket);
      return false;
   }
   if (rval < len) {
      cout << "OutputSender: Connection closed by client." << endl;
      close(socket);
      return false;
   }
   return true;
}


void OutputSender::printClientList()
{
   _client_pool.print();
}

double OutputSender::averageDataRate() 
{
   return ceil(_average_data_rate * 10)/10.0;
}

void OutputSender::counterHandler(OutputSender* me)
{
   while(true) {
      sleep(1);
      if (pthread_mutex_lock(&(me->_counter_mutex)) < 0)
         perror("OutputSender: pthread_mutex_lock");

         Timeval now;
         gettimeofday(&now, 0);
         Timeval tmp(now);
         now -= me->_last_time;
         me->_average_data_rate = (double)(me->_count) / now.doubleValue();
         me->_last_time = tmp;
         me->_count = 0;

      if (pthread_mutex_unlock(&(me->_counter_mutex)) < 0)
         perror("OutputSender: pthread_mutex_unlock");
   }
}


}
