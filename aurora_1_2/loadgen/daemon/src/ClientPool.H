#ifndef _CLIENT_POOL_
#define _CLIENT_POOL_

// STL headers
#include <map>				// for map

// Linux headers
#include <pthread.h>			// for pthread_*

using namespace std;
namespace aurora_socket_api{

struct ClientInfo {
   int sock;
   int port;
   int length;
};
typedef ClientInfo client_info_t;

typedef map<int, client_info_t> client_list_t; // map from socket to client_info
typedef map<int, client_list_t> client_map_t;  // map from port to client_list

class ClientPool
{
public:
   ClientPool();
   ~ClientPool(){}
   
   void	addClient(const client_info_t& client_info);
   void deleteClient(const client_info_t& client_info);
   void clearPool();
   void findClients(int port, client_list_t& client_list); 
   void print();

private:
   client_map_t		_client_pool;
   pthread_mutex_t	_mutex;
};

}

#endif
