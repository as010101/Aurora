#ifndef _SOCKET_CLIENT_H_
#define _SOCKET_CLIENT_H_

// C++ headers
#include <cstring>		// for memset
#include <string>		// for string
#include <cstdio>		// for perror
#include <cerrno>		// for perror
#include <csignal>		// for signal, SIGPIPE, SIG_IGN

// Linux headers
#include <sys/types.h>		// for connect
#include <sys/socket.h>		// for socket, connect
#include <netdb.h>		// for gethostbyname, endhostent, herror
#include <unistd.h>		// for close

using namespace std;
namespace socket_tools{

class SocketClient{
public:
   SocketClient::SocketClient(const char* serverhost, int port, int type);

   SocketClient::~SocketClient();

   int	connect_server();
   void	close_connection();
private:
   int	_sock;
   string    _hostname;
   int	_port;
   int	_type;
};


inline SocketClient::SocketClient(
            const char* serverhost, 
            int port, 
            int type):
      _sock(-1),
      _hostname(serverhost),
      _port(port),
      _type(type)
{
   // Writing to an unconnected socket will cause a process to receive
   // a SIGPIPE signal.  We don't want to die if this happens, so we
   // ignore SIGPIPE. 
   signal (SIGPIPE, SIG_IGN);
}

inline SocketClient::~SocketClient()
{
   close(_sock);
}

inline void SocketClient::close_connection() 
{
   close(_sock);
   _sock = -1;
}

inline int SocketClient::connect_server()
{
   // Create socket. 
   _sock = socket( AF_INET, _type, 0);
   if (_sock == -1) {
       perror("opening stream socket");
       return _sock;
   }

   // Make socket reusable 
   int on=1;
   if (setsockopt(_sock, SOL_SOCKET,SO_REUSEADDR , &on, sizeof(on)) == -1) {
      perror("set socket option");
      exit(1);
   }

   // Connect socket using name specified by command line. 
   struct sockaddr_in server;
   memset(&server, 0,sizeof(server));
   server.sin_family = AF_INET;
  
   struct hostent* hp = gethostbyname(_hostname.c_str());
   if (hp == (struct hostent *) 0) {
      herror("socket_tools::SocketClient: gethostbyname");
      return -1;
   }
  
   int h_addr_index = 0;
   while (hp->h_addr_list[h_addr_index] != NULL) {
       memcpy(&server.sin_addr, hp->h_addr_list[h_addr_index], hp->h_length);
       server.sin_port = htons(_port);
       if (connect(_sock, (struct sockaddr *) &server,
                   sizeof (server)) == -1) {
           if (hp->h_addr_list[++h_addr_index] != NULL) {
               continue;
           }
           perror("socket_tools::SocketClient: connect");
           endhostent();
           return -1;
       }
       break;
   }
   endhostent(); 

   //Connection established
   return _sock;
}

}
#endif
