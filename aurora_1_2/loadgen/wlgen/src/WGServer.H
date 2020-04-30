#ifndef _WG_SERVER_H_
#define _WG_SERVER_H_

// C++ headers
#include <map>			// for pair

// Local headers
#include "SocketServer.H"		// for SocketServer
#include "WorkloadGenerator.H"		// for WorkloadGenerator

using namespace std;
namespace workload_generator{


class WGServer
{
static const int DEFAULT_PORT = 15000;
static const int DEFAULT_TYPE = SOCK_STREAM;
static const int DEFAULT_NUM_HANDLERS = 1;

static const int MAX_BUFFER_SIZE = 1024 * 1024;

public:
   WGServer(WorkloadGenerator* workload_generator);
   WGServer(
       WorkloadGenerator* workload_generator, 
       int port,
       int num_hadler_threads);
   ~WGServer(); 

   bool	initialize();

private:
   int					_port;
   int					_num_handler_threads;
   
   ::socket_tools::SocketServer*	_server;
   WorkloadGenerator*			_workload_generator;

   static void		serverHandler(int sock, WGServer* me);
   static bool		cread(int socket, char* buf, int len);
   static bool		readAggStreamInfo(
         int* data, int num_ints, AggStreamSource& stream);
};

}
#endif
