#ifndef _STREAM_RECEIVER_H_
#define _STREAM_RECEIVER_H_

// C++ headers
#include <vector>			// for vector
#include <map>				// for map
#include <fstream>			// for fstream


// Local headers
#include "SocketServer.H"		// for ::socket_tools::SocketServer
#include "Timeval.H"			// for ::timeval_tool::Timval
#include "Types.H"			// for datatype_t, source_stream_id_t
					//     datatype_list_t, schema_t
                                        //     MAX_TUPLE_SIZE

using namespace std;
using namespace timeval_tool;

namespace workload_generator{


class StreamReceiver
{
static const int DEFAULT_PORT = 16000;
static const int DEFAULT_TYPE = SOCK_STREAM;
static const int DEFAULT_NUM_HANDLERS = 1;

public:
   StreamReceiver();
   StreamReceiver(int port, int num_handler_threads);
   ~StreamReceiver(); 

   bool initialize();
   void startCommandReceiver();

private:
   int					_port;
   int					_num_handler_threads;
   schema_t				_schemas;
   bool					_display_data;
   fstream				_output_file;
   bool					_output_to_file;

   ::socket_tools::SocketServer*	_server;

   double				_average_data_rate;
   int					_count;
   Timeval				_last_time;
   pthread_mutex_t			_counter_mutex;
   pthread_t				_counter_thread;

   static void	receiverHandler(int sock, StreamReceiver* me);
   static void	counterHandler(StreamReceiver* me);
   static bool	cread(int sock, char* buf, int len);
   void		readPacket(char* packet, int len);

   void	addSchema(source_stream_id_t id, datatype_list_t types);
   void addSchema(); // accept information fron input
   void readSchema(const char* filename);
   void readSchemas(const char* filename);
   void readSchemaFile(const char* filename);
   void writeSchemaFile(const char* filename);
   void writeOutputFile(const char* filename);
   void stopOutputToFile();
   void	delSchema(source_stream_id_t id);		
   void printSchema(source_stream_id_t id, datatype_list_t types);
   void printSchemas();
   void printDataRate();
   void printUsage();

};

}
#endif
