#include "ClientPool.H"

#include <iostream>			// for cout
#include <iomanip>			// for setw


using namespace std;
namespace aurora_socket_api{

ClientPool::ClientPool()
{
   if (pthread_mutex_init(&_mutex,0) < 0)
      perror("ClientPool: pthread_mutex_init");
}
void ClientPool::addClient(const client_info_t& client_info)
{
   if (pthread_mutex_lock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");

   _client_pool[client_info.port][client_info.sock] = client_info;

   if (pthread_mutex_unlock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");
}

void ClientPool::deleteClient(const client_info_t& client_info)
{
   if (pthread_mutex_lock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");

   for (client_map_t::iterator pit = _client_pool.begin();
        pit != _client_pool.end();
        ++ pit)
   {
      pit->second.erase(client_info.sock);
   }

   if (pthread_mutex_unlock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");
}

void ClientPool::findClients(int port, client_list_t& client_list)
{
   if (pthread_mutex_lock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");

   client_map_t::iterator pos = _client_pool.find(port);
   if ( pos != _client_pool.end()) {
      client_list = pos->second;
   }

   if (pthread_mutex_unlock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");
}

void ClientPool::clearPool()
{
   if (pthread_mutex_lock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");

   _client_pool.clear();
  
   if (pthread_mutex_unlock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");
}

void ClientPool::print()
{
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << setw(15) << "output port"; 
   cout << setw(15) << "socket"; 
   cout << setw(15) << "tuple length"; 
   cout << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   if (pthread_mutex_lock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");
   for (client_map_t::iterator pit = _client_pool.begin();
        pit != _client_pool.end();
        ++ pit)
   {
      for (client_list_t::iterator cit = pit->second.begin();
           cit != pit->second.end();
           ++ cit) {
         cout << setw(15) << cit->second.port; 
         cout << setw(15) << cit->second.sock; 
         cout << setw(15) << cit->second.length; 
         cout << endl;
      }
   }

   if (pthread_mutex_unlock(&_mutex) < 0)
      perror("ClientPool: pthread_mutex_lock");
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}

}

