#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_

// C++ headers
#include <iostream>	          // for cout
#include <cstring>                // for memset
#include <cstdio>                 // for perror
#include <cerrno>                 // for perror
#include <csignal>	          // for signal, SIGPIPE, SIG_IGN


// Linux headers
#include <sys/types.h>            // for {socket, setsockopt, accept}
#include <sys/socket.h>           // for {socket, setsockopt, listen,accept}
#include <netinet/in.h>           // for htonl
#include <unistd.h>               // for close
#include <pthread.h>              // for pthread_mutex_*, pthread_cond_*

using namespace std;
namespace socket_tools{
               
////////////////////////////////////////////////////////////////////////
//
class Request
{
public:
   Request(int sock) : socket_num(sock), next(0){}

   int	socket_num;
   Request*	next;
};
   
////////////////////////////////////////////////////////////////////////
//
class RequestList
{
public:
   RequestList();

   void	add_request( int);
   int	get_request();
   int	is_empty();

private:
   Request*		first_request;
   Request*		last_request;
   pthread_mutex_t	m_list;          //mutex for access list
   pthread_cond_t	cond_have_request;     //condition  
};

inline RequestList::RequestList() : first_request(0),last_request(0)
{
   if (pthread_mutex_init(&m_list,0) < 0)
      perror("socket_tools::RequestList: pthread_mutex_init");
   if (pthread_cond_init(&cond_have_request,0) < 0)
      perror("socket_tools::RequestList: pthread_cond_init");
}
   
inline void RequestList::add_request(int sock)
{
   if (pthread_mutex_lock(&m_list) < 0)
      perror("socket_tools::RequestList: pthread_mutex_lock");
   Request* newrequest = new Request(sock);
   if(!last_request){
      first_request = newrequest;
      last_request = newrequest;
   }
   else{
      last_request->next = newrequest;
   }
   if (pthread_cond_signal( &cond_have_request ) < 0)
      perror("socket_tools::RequestList: pthread_cond_signal");
   if (pthread_mutex_unlock(&m_list) < 0)
      perror("socket_tools::RequestList: pthread_mutex_unlock");
}

inline int RequestList::get_request()
{
   if (pthread_mutex_lock(&m_list) < 0)
      perror("socket_tools::RequestList: pthread_mutex_lock");

   while( !first_request ){
     if (pthread_cond_wait(&cond_have_request, &m_list) < 0)
      perror("socket_tools::RequestList: pthread_cond_wait");
   }

   int sock = first_request->socket_num;
   
   Request* next_request = first_request->next;
   delete first_request;
   first_request = next_request;
   if( !first_request )
      last_request = 0;
   if (pthread_mutex_unlock(&m_list) < 0)
      perror("socket_tools::RequestList: pthread_mutex_unlock");

   return sock;
}

////////////////////////////////////////////////////////////////////////
//

class ConnectionThread 
{
public:
   ConnectionThread(
         int			pt_num, 
         RequestList*	request_list, 
         void(*serve)(int, void*),	
         void*		parameter
   );

   bool start();

private:
   pthread_t		_thread;
   int			_thread_id;
   RequestList*		_request_list; 

   void (*_serve)(int, void*);
   void*		_parameter;

   static void 	connection_handler(ConnectionThread*);
};

inline ConnectionThread::ConnectionThread(
      int		pt_num, 
      RequestList*	request_list,
      void(*serve)(int, void*),
      void*		parameter): 
   _thread_id(pt_num),
   _request_list(request_list),
   _serve(serve),
   _parameter(parameter)
{
}

inline bool ConnectionThread::start()
{
   if (pthread_create(&_thread,
                      0,
                      (void*(*)(void*))connection_handler,
                      (void*)this) < 0) {
      perror("socket_tools::ConnectionThread: pthread_create");
      return false;
   }
   return true;
}

inline void ConnectionThread::connection_handler(ConnectionThread* me)
{
   while(1){
      int sock = me->_request_list->get_request();
      me->_serve(sock, me->_parameter);
   }
}


/////////////////////////////////////////////////////////////////////
//
//
class SocketServer 
{

public:
   SocketServer(
         int port, 
         int type, 
         int num_handler_threads,
         void(*server_handler)(int, void*),
         void* parameter
   ); 
   ~SocketServer();

   bool start();

private:
   int		_port;		
   int          _type;
   int		_num_handler_threads;
   void		(*_server_handler)(int, void*);
   void*	_parameter;

   int		_sock;

   pthread_t		_monitor_thread;	
   
   RequestList 		_request_list; 
   ConnectionThread** 	_connection_threads;

   static void		monitor(SocketServer* me); 
   bool			bindServer();
};   

inline SocketServer::SocketServer(
      int port, 
      int type, 
      int num_handler_threads, 
      void (*server_handler)(int, void*),
      void* parameter): 
   _port(port),
   _type(type),
   _num_handler_threads(num_handler_threads),
   _server_handler(server_handler),
   _parameter(parameter),
   _sock(0),
   _connection_threads(0)
{
}


inline SocketServer::~SocketServer()
{
   if (_connection_threads != 0) {
       for (int i = 0; i < _num_handler_threads; ++i)
         delete  _connection_threads[i];
       delete _connection_threads;
   }
   if (_sock != 0)
     close(_sock);
}

inline bool SocketServer::start()
{
   // Writing to an unconnected socket will cause a process to receive
   // a SIGPIPE signal.  We don't want to die if this happens, so we
   // ignore SIGPIPE. 
   signal (SIGPIPE, SIG_IGN);

   //Create thread pool
   _connection_threads = new ConnectionThread*[_num_handler_threads]; 
   for (int i=0; i < _num_handler_threads; ++i){
      _connection_threads[i] =  
        new ConnectionThread(
            i+1, 
            &_request_list, 
            _server_handler, 
            _parameter);
      if (_connection_threads[i]->start() == false) {
         cout << "socket_tools::SocketServer: "
              << "creating connection threads failed."
              << endl;
         for (int j = 0; j <= i; ++j)
            delete _connection_threads[i];
         delete _connection_threads;
         _connection_threads = 0;
         return false;
      }
   }

   // Bind Server
   if (!bindServer())
      return false;

   //Create monitor thread
   if (pthread_create(&_monitor_thread,
                      0,
                      (void*(*)(void*))monitor, 
                      (void*)this) < 0) {
      perror("socket_tools::SocketServer: pthread_create");
      cout << "socket_tools::SocketServer: "
           << "creating monitor thread failed."
           << endl;
      return false;
   }

   return true;
}

inline bool SocketServer::bindServer()
{
   // Create socket. 
   _sock = socket(AF_INET, _type, 0);
   if (_sock == -1) {
      perror("socket_tools::SocketServer: opening server socket");
      _sock = 0;
      return false;
   }

   // Make socket reusable 
   int on=1;
   if (setsockopt(_sock, SOL_SOCKET,SO_REUSEADDR , &on, sizeof(on)) == -1) {
      perror("socket_tools::SocketServer: set socket option");
      return false;
   }

   // Bind socket 
   struct sockaddr_in server;
   memset(&server, 0,sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   server.sin_port = htons(_port);
   if (bind(_sock, (struct sockaddr *) &server, sizeof server)
         == -1) {
      perror("socket_tools::SocketServer: binding server socket");
      return false;
   }

   return true;
}

inline void SocketServer::monitor(SocketServer* me)
{
   // Start accepting connections. 
   listen(me->_sock, me->_num_handler_threads);
   while(true) {
      int msgsock = accept(me->_sock,(struct sockaddr *) 0,(socklen_t *) 0);
      if (msgsock == -1)
         perror("socket_tools::SocketServer: server accept") ;
      else {
         me->_request_list.add_request(msgsock);
      }
   }
}

}
   
#endif   
