#include "LRReceiver.H"

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

using namespace std;
namespace linear_road{

extern CarMap *the_car_map;

LRReceiver::LRReceiver(CarMap *car_map,
		       const char*	auroraOutputServerName, 
		       int		auroraOutputServerPort):
  cm(car_map),
  _auroraOutputServerName(auroraOutputServerName),
  _auroraOutputServerPort(auroraOutputServerPort)
{
}

LRReceiver::~LRReceiver() 
{

   for (vector<pthread_t*>::iterator it = _client_threads.begin();
        it != _client_threads.end();
        ++it) {
      delete *it;
   }

   for (vector<ConnectionInfo*>::iterator it = _client_info_list.begin();
        it != _client_info_list.end();
        ++it) {
      delete *it;
   }

}

bool LRReceiver::start()
{
  // Main output port for LR.
  ConnectionInfo* c_info = new ConnectionInfo;
  c_info->port = 80;
  c_info->len = 11 * sizeof(int);
  c_info->filename = "LRTollNotifications.dat";
  c_info->receiver = this;

  pthread_t* receiver_thread = new pthread_t;
  if (pthread_create(receiver_thread,
		     0,
		     (void*(*)(void*))receiverHandler, 
		     (void*)c_info) < 0) {
    perror("LRReceiver: creating thread");
  } else {
    _client_threads.push_back(receiver_thread);
    _client_info_list.push_back(c_info);
  }
  return true;
}


void LRReceiver::receiverHandler(ConnectionInfo* c_info)
{
   fstream output_file;
   output_file.open((c_info->filename).c_str(), ios::out);
   if (!output_file) {
      cerr << "Can not open file: " << c_info->filename << endl;
      return;
   }

   LRReceiver* me = c_info->receiver;
   ::socket_tools::SocketClient client(
         me->_auroraOutputServerName.c_str(),
         me->_auroraOutputServerPort,
         DEFALT_AURORA_OUTPUT_SERVER_TYPE);
   int sock = client.connect_server();

   if (sock == -1) {
      cout << endl;
      cout << "Aurora output server address error or the server is not up."
           << endl;
      output_file.close();
      return;
   }
   cout << endl;
   cout << "Connection with Aurora output server estabilished at socket"
        << sock << endl;

   int n = 1;
   // send number of ports
   if (!cwrite(sock, (char*)&n, sizeof(int))) {
      output_file.close();
      close(sock);
      return;
   }

   // write port
   if (!cwrite(sock, (char*)&(c_info->port), sizeof(int))) {
      output_file.close();
      close(sock);
      return;
   }
 
   // write tuple length
   if (!cwrite(sock, (char*)&(c_info->len), sizeof(int))) {
      output_file.close();
      close(sock);
      return;
   }


   while (true) {
      char* buf;

      int packet_size;
      // receive packet size
      if (!cread(sock, (char*)&packet_size, sizeof(int))) {
         output_file.close();
         close(sock);
         return;
      }

      if (packet_size !=  (c_info->len) + sizeof(int)) {
         cout << "Tuple size wrong: " << packet_size << endl;
         output_file.close();
         close(sock);
         return;
      }

      // read packet
      buf =  new char[packet_size];
      //read(sock, packet, packet_size);
      if (!cread(sock, buf, packet_size)) {
         delete [] buf;
         output_file.close();
         close(sock);
         return;
      }

      int* data = (int*)buf + 1;
      // Notice the carid and time.
      int carid = data[0];
      int toll = data[10];
      if (carid < 100) {
	cout << "Got a toll notification for car " << carid << " with toll " << toll << endl;
      }

      // Notice the packet.
      //c_info->receiver->cm->carGets(carid);
      the_car_map->carGets(carid);

      // write the packet to file
      for (int i = 0; i < (packet_size / sizeof(int) - 2); ++i) {
         output_file << *(data + i); 
         output_file << ',';
      }
      output_file << *(data + packet_size / sizeof(int) - 2 );
      output_file << endl;
      
      delete [] buf;
   }
   output_file.close();
   close(sock);
   return;
}


bool LRReceiver::cwrite(int sock, char* packet, int len)
{
   int rc = write(sock, packet, len); 

   if (rc == -1) {
      if (errno == EPIPE) {
         cout << "LRReceiver:: socket " << sock 
              << " closed by server." << endl;
      }
      else {
         cerr << "LRReceiver: write socke " << sock 
              << " error." << endl;
         perror(""); 
      }
      close(sock);
      return false;
   }
   if (rc != len) {
      cout << "LRReceiver:: socket " << sock 
           << " closed by server." << endl;
      close(sock);
      return false;
   }
   return true;
}


bool LRReceiver::cread(int sock, char* buf, int len) 
{
   int rlen = 0;
   while (rlen < len) {
      int rval = read(sock, buf + rlen, len - rlen);
      if (rval == 0) {
         cout << "LRReceiver: connection closed by server at socket "  
              << sock
              << endl;
         close(sock);
         return false;
      }
      if (rval == -1) {
         if (errno != EBADF) {
            cout << "LRReceiver: read socket " << sock << " error." 
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


}
