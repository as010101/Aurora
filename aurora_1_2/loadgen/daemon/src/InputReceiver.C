#include "InputReceiver.H"

// C++ headers
#include <iostream>			// for cout
#include <iomanip>			// for setw
#include <cmath>			// for ceil

// Unix headers
#include <unistd.h>			// for read, sleep
#include <stdio.h>			// for perror
#include <errno.h>			// for perror
#include <sys/time.h>			// for gettimeofday

// Aurora header
#include <aurora.H>			// for aurora API functions 


using namespace std;
namespace aurora_socket_api{

InputReceiver::InputReceiver():
   _server(0),
   _port(INPUT_RECEIVER_DEFAULT_PORT),
   _num_handler_threads(INPUT_RECEIVER_DEFAULT_NUM_HANDLERS),
   _average_data_rate(0),
   _count(0)
{
}

InputReceiver::InputReceiver(int port, int num_handler_threads):
   _server(0),
   _port(port),
   _num_handler_threads(num_handler_threads),
   _average_data_rate(0),
   _count(0)
{
}

bool InputReceiver::initialize(void* aurora_instance)
{
   _aurora_instance = aurora_instance;
   // read output ports from Aurora
   AURORA_API_RETURN_CODE rc;

   int numPorts;
   rc = aurora_getNumInputPorts(_aurora_instance, & numPorts);
   if (rc != AURORA_OK)
   {
      cout << "Error: aurora_getNumInputPorts(...) returned " << (int)rc;
      return false;
   }

   for (int i = 0; i < numPorts; ++i)
   {
      int portName;
      rc = aurora_getInputPortNum(_aurora_instance, i, & portName);
      if (rc != AURORA_OK)
      {
         cout << "Error: aurora_getInputPortNum(...) returned " << (int)rc;
         return false;
       }
      _input_ports.insert(portName);
   }

   // initialize mutex, time
   gettimeofday(&_last_time, 0);
   if (pthread_mutex_init(&_counter_mutex, 0) < 0) {
      perror("InputReceiver: counter mutex init failed. ");
      return false;
   }
   // start counter thread
   if (pthread_create(&_counter_thread,
                      0,
                      (void*(*)(void*))counterHandler, 
                      (void*)this) < 0) {
      perror("InputReceiver: creating counter thread failed. ");
      return false;
   }

   cout << "Starting input receiver server... " << endl;
   _server = new ::socket_tools::SocketServer(
         _port, 
         INPUT_RECEIVER_DEFAULT_TYPE,
         _num_handler_threads,
         (void(*)(int, void*))receiverHandler,
         this);
   if (_server->start()) {
      cout << "Input receiver server started at port " 
           << _port
           << endl;
      return true;
   }
   else {
      cout << "Input receiver initialization failed." << endl;
      return false;
   }
}

InputReceiver::~InputReceiver() 
{
   if (_server != 0)
      delete _server;
}

void InputReceiver::receiverHandler(int socket, InputReceiver* me)
{
   if (me->_aurora_instance == 0)
   {
      cerr << "No Aurora Instance has been associated with this receiver." 
           << endl;
      return;
   }

   cout << endl;
   cout << "InputReceiver: connection request recieved at socket " << socket;
   cout << endl;

   while (1) {
      char buf[64];
      // receive packet size
      if (!(cread(socket, buf, sizeof(int))))
         return;
      int packet_size = *(int*)buf;
      int tuple_size = packet_size - sizeof(int) + // tuple size
                       TUPLE_OFFSET_USER_DATA;  // for system data

      // allocate buffer 
      char* tuple_buffer = 
        new char[tuple_size];
      if (!(cread(socket, 
                  tuple_buffer + TUPLE_OFFSET_USER_DATA - sizeof(int), 
                  packet_size))){
         delete [] tuple_buffer;
         return;
      }

      // check the port
      int portName = 
        *(int*)(tuple_buffer + TUPLE_OFFSET_USER_DATA - sizeof(int)); 
      if (!me->validPort(portName)) {
         cerr << "InputReceiver: invalid port " << portName << endl;
         delete [] tuple_buffer;
         return;
      }
 
      // set interface parameters for Aurora API
      AURORA_API_RETURN_CODE rc = aurora_enqRecord(
         me->_aurora_instance,
         portName, // input port name
         0,  // stream ID  
         tuple_buffer,
         tuple_size);
      if (rc != AURORA_OK)
      {
         cerr << "Error: aurora_enqRecord(...) returned " << (int)rc
              << endl;
      }

      // delete the packet
      delete [] tuple_buffer; 

      // record data rate
      if (pthread_mutex_lock(&(me->_counter_mutex)) < 0)
         perror("InputReceiver: pthread_mutex_lock");
      ++(me->_count);
      if (pthread_mutex_unlock(&(me->_counter_mutex)) < 0)
         perror("InputReceiver: pthread_mutex_unlock");

   }
}

void InputReceiver::readPacket(char* packet, int len)
{
   if (_aurora_instance == 0)
   {
      cerr << "No Aurora Instance has been associated with this receiver." 
           << endl;
      return;
   }

   // set interface parameters for Aurora API
   char* data = packet + TUPLE_OFFSET_USER_DATA - sizeof(int);
   int portName = *(int*)data; 

   if (!validPort(portName)) {
      cerr << "InputReceiver: invalid port " << portName << endl;
      return;
   }

   // read data
   AURORA_API_RETURN_CODE rc = aurora_enqRecord(
      _aurora_instance,
      portName,
      portName,  
      packet,
      len);
   if (rc != AURORA_OK)
   {
      cerr << "Invalid data received. " << (int)rc;
      cerr << "Error: aurora_enqRecord(...) returned " << (int)rc;
   }
}

bool InputReceiver::validPort(int port)
{
   return (_input_ports.find(port) != _input_ports.end());
}

bool InputReceiver::cread(int socket, char* buf, int len) 
{
   int rlen = 0;
   while (rlen < len) {
      int rval = read(socket, buf + rlen, len - rlen);
      if (rval == 0) {
         cout << "InputReceiver: connection closed by client at socket "  
              << socket
              << endl;
         close(socket);
         return false;
      }
      if (rval == -1) {
         cout << "InputReceiver: read socket " << socket << " error." << endl;
         perror("");
         close(socket);
         return false;
      }
      rlen += rval;
   }
   return true;
}

double InputReceiver::averageDataRate() 
{
   return ceil(_average_data_rate * 10)/10.0;
}

void InputReceiver::counterHandler(InputReceiver* me)
{
   while(true) {
      sleep(1);
      if (pthread_mutex_lock(&(me->_counter_mutex)) < 0)
         perror("InputReceiver: pthread_mutex_lock");

         Timeval now;
         gettimeofday(&now, 0);
         Timeval tmp(now);
         now -= me->_last_time;
         me->_average_data_rate = (double)(me->_count) / now.doubleValue();
         me->_last_time = tmp;
         me->_count = 0;

      if (pthread_mutex_unlock(&(me->_counter_mutex)) < 0)
         perror("InputReceiver: pthread_mutex_unlock");
   }
}


}
