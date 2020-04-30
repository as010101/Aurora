#ifndef _INPUTRECIEVER_H_
#define _INPUTRECIEVER_H_

// STL headers
#include <set>				// for set

// Local headers
#include "SocketServer.H"		// for ::socket_tools::SocketServer
#include "Timeval.H"			// for ::timeval_tool::Timval

using namespace std;
using namespace timeval_tool;

namespace aurora_socket_api{

const int INPUT_RECEIVER_DEFAULT_PORT = 16000;
const int INPUT_RECEIVER_DEFAULT_TYPE = SOCK_STREAM;
const int INPUT_RECEIVER_DEFAULT_NUM_HANDLERS = 10;

const int MAX_TUPLE_SIZE = 1024 * 1024;

using namespace std;

class InputReceiver
{
typedef set<int> port_list_t;

public:
   InputReceiver();
   InputReceiver(int port, int num_handler_threads);
   ~InputReceiver(); 

   bool initialize(void * aurora_instance);
   double averageDataRate();

private:
   ::socket_tools::SocketServer*	_server;
   int					_port;
   int					_num_handler_threads;
   void*				_aurora_instance; 
   port_list_t				_input_ports;

   double				_average_data_rate;
   int					_count;
   Timeval				_last_time;
   pthread_mutex_t			_counter_mutex;
   pthread_t				_counter_thread;

   static void		receiverHandler(int socket, InputReceiver* me);
   bool			validPort(int port);
   void			readPacket(char* packet, int len);
   static bool		cread(int socket, char* buf, int len);
   static void		counterHandler(InputReceiver* me);
};

}
#endif
