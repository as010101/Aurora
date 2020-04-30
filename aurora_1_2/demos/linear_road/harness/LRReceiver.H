#ifndef _LR_RECEIVER_H_ 
#define _LR_RECEIVER_H_

// C++ headers
#include <vector>			// for vector
#include <map>				// for map
#include <string>			// for string
#include <fstream>			// for fstream

// Local headers
#include "SocketClient.H"		// for ::socket_tools::SocketClient
                                        // for SOCK_STREAM
#include "CarMap.H"

using namespace std;
namespace linear_road{


class LRReceiver;
struct ConnectionInfo {
   int				port;
   int				len;
   string			filename;
   LRReceiver*			receiver;
};

class LRReceiver
{
static const int	DEFALT_AURORA_OUTPUT_SERVER_TYPE = SOCK_STREAM;

typedef vector<pthread_t*> thread_list_t;

public:
   LRReceiver();
   LRReceiver(
     CarMap *car_map,
     const char* auroraOutputServerName, 
     int auroraOutputServerPort);
   ~LRReceiver(); 

   bool start();

private:
  CarMap *cm;
   string	_auroraOutputServerName;
   int		_auroraOutputServerPort;

   vector<pthread_t*>		_client_threads;
   vector<ConnectionInfo*>	_client_info_list;
   

   static void	receiverHandler(ConnectionInfo* c_info);
   static bool	cread(int sock, char* buf, int len);
   static bool	cwrite(int sock, char* buf, int len);
};

}
#endif
