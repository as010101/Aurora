#ifndef _AURORA_OUTPUT_RECEIVER_H_ 
#define _AURORA_OUTPUT_RECEIVER_H_

// C++ headers
#include <vector>			// for vector
#include <map>				// for map
#include <string>			// for string
#include <fstream>			// for fstream


// Local headers
#include "SocketServer.H"		// for ::socket_tools::SocketServer
#include "Types.H"			// for datatype_t, source_stream_id_t
					//     datatype_list_t

using namespace std;
namespace workload_generator{


class AuroraOutputReceiver;
struct ConnectionInfo {
   int				port;
   int				len;
   int				sock;
   AuroraOutputReceiver*	receiver;
};

typedef vector<ConnectionInfo> cinfo_list_t;

const char	DEFALT_AURORA_OUTPUT_SERVER_NAME[] = "localhost";

class AuroraOutputReceiver
{
static const int	DEFALT_AURORA_OUTPUT_SERVER_PORT = 17000;
static const int	DEFALT_AURORA_OUTPUT_SERVER_TYPE = SOCK_STREAM;

typedef vector<pthread_t*> thread_list_t;

public:
   AuroraOutputReceiver();
   AuroraOutputReceiver(
         const char*	auroraOutputServerName, 
         int		auroraOutputServerPort);
   ~AuroraOutputReceiver(); 

   void startCommandReceiver();
private:
   string	_auroraOutputServerName;
   int		_auroraOutputServerPort;

   schema_t		_schemas;
   cinfo_list_t		_client_info_list;
   bool			_display_data;
   cinfo_list_t		_new_client_info_list;
   fstream		_output_file;
   bool			_output_to_file;
   bool			_write_output_port;

   vector<pthread_t*>	_client_threads;
   

   static void	receiverHandler(cinfo_list_t* c_info);
   static bool	cread(int sock, char* buf, int len);
   static bool	cwrite(int sock, char* buf, int len);
   void		readPacket(char* packet, int len);
   void		deleteConnection(int sock);
   void		addPort(int port, int len);		
   void		startClient();

   void	addSchema(source_stream_id_t id, datatype_list_t types);
   void addSchema(); // accept information fron input
   void readSchema(const char* filename);
   void readSchemas(const char* filename);
   void readSchemaFile(const char* filename);
   void writeSchemaFile(const char* filename);
   void writeOutputFile(const char* filename, bool write_output_port);
   void stopOutputToFile();
   void	delSchema(source_stream_id_t id);		
   void printSchema(source_stream_id_t id, datatype_list_t types);
   void printSchemas();
   void printPorts();
   void printUsage();
};

}
#endif
