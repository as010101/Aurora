#include "LRSender.H"

// C++ headers				
#include <iostream>			// for cout
#include <cstdio>			// for perror
#include <cerrno>			// for perror
#include <sstream>			// for istringstream
#include <vector>

// Linux headers
#include <unistd.h>			// for write, sleep
#include <sys/time.h> 			// for gettimeofday

// Aurora util headers
#include "parseutil.H"

using namespace std;
namespace linear_road{

extern CarMap *the_car_map;

LRSender::LRSender(CarMap *car_map,
      const char* filename,
      const char* aurora_input_server_name, 
      int aurora_input_server_port):
  _aurora_input_server_name(aurora_input_server_name),
  _aurora_input_server_port(aurora_input_server_port),
  cm(car_map),
  _client(0), 
  _sock(-1),
  _filename(filename)
{
}

LRSender::~LRSender()
{
   if (_client != 0) {
      delete _client;
   }
   _input_file.close();
}

bool LRSender::start(bool wait) 
{
   _input_file.open(_filename.c_str(), ios::in);
   if (! _input_file) {
      cerr << "Can not open file: " << _filename.c_str() << endl;
      return false;
   }

   _client = new ::socket_tools::SocketClient(
         _aurora_input_server_name.c_str(),
         _aurora_input_server_port,
         DEFAULT_AURORA_INPUT_SERVER_TYPE);
   if (!connectServer()) {
      delete _client;
      _client = 0;
      _input_file.close();
      return false;
   }

   gettimeofday(&_initial_time, 0);
   if (!wait) {
      // read in first time stamp
      static const int BUFFERSIZE = 1024;
      char buf[BUFFERSIZE];
      _input_file.getline(buf, BUFFERSIZE);
      istringstream line(buf);
      int tm;
      line >> tm; 
      char tmp;
      line >> tmp;
      line >> tm;
      _initial_time.tv_sec -= tm;
      _input_file.seekg(0, ios::beg);
   }

   if (pthread_create(&_writer_thread,
                      0,
                      (void*(*)(void*))writerHandler, 
                      (void*)this) < 0) {
      perror("LRSender: creating generator thread");
      close(_sock);

      delete _client;
      _client = 0;
      _input_file.close();
      return false;
   }
   return true;
}

bool LRSender::connectServer()
{ 
   cout << "Connecting to Aurora ...";
   if( (_sock = _client->connect_server()) == -1) {
      cout << "failed." << endl
           << "Aurora server is not active or "
           << "the host name port is not correct. " 
           << endl;
      return false;
   }
   else {
      cout << "connection established." << endl;
      return true;
   }
}


void LRSender::stopConnection()
{
   if (_client != 0) 
      delete _client;
   _sock = -1;
   _client = 0;
}

void LRSender::writerHandler(LRSender* me)
{
   while(me->_input_file.peek() != EOF) {
      static const int BUFFERSIZE = 1024 * 64;
      char line[BUFFERSIZE];

      me->_input_file.getline(line, BUFFERSIZE);
      // TODO: Some error handling.

      //cerr << "LRSender::writerHandler: LRSender::writerHandler: Got line: " << line << endl;

      // Split the line into integers.
      vector<string> v = unpackString(string(line), string(" ,"));

      if (v.size() != TUPLE_ELEMENT_COUNT) {
	cerr << "LRSender::writerHandler: Malformed line ``" << line << "''" << endl;
	cerr << "LRSender::writerHandler: \tHas " << v.size() << " elements instead of "
	     << TUPLE_ELEMENT_COUNT << "." << endl;
	continue;
      }

      // Get the some data out of the array.
      int timestamp = stringToInt(v[TUPLE_TIMESTAMP_POSITION]);
      
      timeval now;
      gettimeofday(&now, 0);
      long sleep_time = 
        (((int)(me->_initial_time.tv_sec + timestamp - now.tv_sec) * 1000000)
	 - now.tv_usec);
      while (sleep_time > 0) {
	//cerr << "LRSender::writerHandler: Sleeping for " << sleep_time << " useconds." << endl;
	usleep(sleep_time);
	gettimeofday(&now, 0);
	sleep_time = 
	  (((me->_initial_time.tv_sec + timestamp - now.tv_sec) * 1000000)
	   - now.tv_usec);
      }

      // send size
      int data_size = sizeof(int) * (TUPLE_ELEMENT_COUNT + 1);
      if (!me->cwrite(me->_sock, (char*)&data_size, sizeof(int))) {
	cerr << "LRSender::writerHandler: Failed writing socket." << endl;
	me->_input_file.close();
	return;
      }

      // send port 
      int port = me->INPUT_PORT;
      if (!me->cwrite(me->_sock, (char*)&port, sizeof(int))) {
	cerr << "LRSender::writerHandler: Failed writing socket." << endl;
	me->_input_file.close();
	return;
      }
      for (vector<string>::iterator pos = v.begin();
	   pos != v.end();
	   pos++) {
	int data = stringToInt(*pos);
        if (!me->cwrite(me->_sock, (char*)&data, sizeof(int))) {
           me->_input_file.close();
           return;
        }
      }
      int type = stringToInt(v[TUPLE_TYPE_POSITION]);
      if (type == 0) {
	int carid = stringToInt(v[TUPLE_CARID_POSITION]);
	//me->cm->carSends(carid);
	the_car_map->carSends(carid);
      }

   }
   me->_input_file.close();
   me->stopConnection();
   cout << "LRSender::writerHandler: All data in input file has been sent." << endl;
}

bool LRSender::cwrite(int sock, char* packet, int len)
{
   int rc = write(sock, packet, len); 

   if (rc == -1) {
      if (errno == EPIPE) {
         cout << "LRSender::  socket " << sock 
              << " closed by server." << endl;
      }
      else {
         cerr << "LRSender: write socket " << sock << " error." << endl;
         perror(""); 
      }
      stopConnection();
      return false;
   }
   if (rc != len) {
      cout << "LRSender:: socket " << sock 
           << " closed by server." << endl;
      stopConnection();
      return false;
   }
   return true;
}

}
