#ifndef _STREAM_SENDER_H_
#define _STREAM_SENDER_H_ 

// C++ headers
#include <string>			// for string

// Local headers
#include "StreamTuple.H"		// for StreamTuple, tuplelist_t
#include "SocketClient.H"		// for socket_tools::SocketClient

using namespace std;

namespace workload_generator{

class StreamSender;

struct SendItem
{
   StreamSender*	sender;
   tuplelist_t*	tuple_list;

   SendItem(StreamSender* sender_p, tuplelist_t* tuple_list_p):
      sender(sender_p), tuple_list(tuple_list_p){}

   ~SendItem() {
      for (tuplelist_t::iterator it = tuple_list->begin();
           it != tuple_list->end();
           ++it) {
         delete (*it);
      }
      delete tuple_list;
   }
};

typedef struct SendItem send_item_t;


const char DEFAULT_AURORA_INPUT_SERVER_NAME[] = "localhost";

class StreamSender
{
static const int DEFAULT_AURORA_INPUT_SERVER_PORT = 16000; 
static const int DEFAULT_AURORA_INPUT_SERVER_TYPE = SOCK_STREAM;

public:
   StreamSender();
   StreamSender(
         const char* aurora_input_server_name, 
         int aurora_input_server_port);
   ~StreamSender();

   static void	sendTuple(send_item_t* item); 
   void		connectServer();			
   void		connectServer(
         const char* aurora_input_server_name, 
         int aurora_input_server_port);
   void		stopConnection();

private:
   ::socket_tools::SocketClient*	_client;

   int		_sock;
   bool		_connected;
   string	_aurora_input_server_name;
   int		_aurora_input_server_port;
   int		_aurora_input_server_type;

   bool cwrite(int sock, char* packet, int len);
};

}
#endif
