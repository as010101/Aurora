#ifndef _LR_SENDER_H_
#define _LR_SENDER_H_ 

// C++ headers
#include <string>			// for string
#include <fstream>			// for fstream
#include <ctime>			// for timeval

// Local headers
#include "SocketClient.H"		// for socket_tools::SocketClient
#include "CarMap.H"

using namespace std;

namespace linear_road{

class LRSender
{
static const int DEFAULT_AURORA_INPUT_SERVER_TYPE = SOCK_STREAM;
// Timestamp is always the second element.
static const int TUPLE_TIMESTAMP_POSITION = 1;
// Type is the first.
static const int TUPLE_TYPE_POSITION = 0;
// Carid is the third element.
static const int TUPLE_CARID_POSITION = 2;
static const int TUPLE_ELEMENT_COUNT = 8;
static const int INPUT_PORT = 0;

public:
   LRSender(CarMap *car_map,
         const char* data_filename,
         const char* aurora_input_server_name, 
         int aurora_input_server_port);
   ~LRSender();

   bool start(bool wait);


private:
  CarMap *cm;
   string	_aurora_input_server_name;
   int		_aurora_input_server_port;

   ::socket_tools::SocketClient*	_client;
   int					_sock;

   string	_filename;
   fstream	_input_file;
   timeval	_initial_time;

   pthread_t		_writer_thread;
   static void		writerHandler(LRSender* me); 

   bool cwrite(int sock, char* packet, int len);

   bool		connectServer();			
   void		stopConnection();
};

}
#endif
