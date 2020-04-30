#ifndef _OUTPUT_SENDER_H_
#define _OUTPUT_SENDER_H_

// Unix headers
#include <pthread.h>			// for pthread*
#include <set>				// for set

// Local headers
#include "SocketServer.H"		// for ::socket_tools::SocketServer
#include "ClientPool.H"			// for ClientPool
#include "Timeval.H"			// for ::timeval_tool::Timval

using namespace std;
using namespace timeval_tool;

namespace aurora_socket_api{

const int OUTPUT_SENDER_DEFAULT_PORT = 17000;
const int OUTPUT_SENDER_DEFAULT_TYPE = SOCK_STREAM;
const int OUTPUT_SENDER_DEFAULT_NUM_HANDLERS = 10;


class OutputSender
{
typedef set<int> port_list_t;
public:
   OutputSender();
   OutputSender(int port, int num_handler_threads);
   ~OutputSender(); 

   bool initialize(void * aurora_instance);
   void printClientList();
   double averageDataRate();

private:
   ::socket_tools::SocketServer*	_server;
   int					_port;
   int					_num_handler_threads;
   void*				_aurora_instance; 

   pthread_t				_sender_thread;
   ClientPool				_client_pool;
   port_list_t				_output_ports;

   double				_average_data_rate;
   int					_count;
   Timeval				_last_time;
   pthread_mutex_t			_counter_mutex;
   pthread_t				_counter_thread;

   static void		senderHandler(OutputSender* me);
   static void		connectionHandler(int socket, OutputSender* me);
   static void		counterHandler(OutputSender* me);

   bool validPort(int port);
   void sendTuple(const client_info_t& client_info, char* packet); 
   bool	cread(int socket, char* buf, int len);
   bool cwrite(int socket, const char* buf, int len);

};

}
#endif
