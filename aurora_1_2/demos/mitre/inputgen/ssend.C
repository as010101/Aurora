#include <fstream>
#include <stdlib.h>        // for atoi
#include <sys/time.h>      // for gettimeofday timeval
#include <unistd.h>        // for sleep;
#include <sstream>			// for istringstream

#include "SocketClient.H"

#include <iostream>
 
using namespace std;
main(int argc, char** argv)
{
   fstream infile;
   int port; 

   if (argc != 4){
      cout << "usage: ssend hostname port filename" << endl; 
      exit(0);
   }

   port = atoi(argv[2]);
   infile.open(argv[3], ios::in);

   fstream inputports_file;
   inputports_file.open("inputports", ios::in);
   if (!inputports_file)
      cerr << "Can't open file: inputports" << endl;
   int enemy_tank_port = 0;		
   int enemy_aircraft_port = 1;	
   int friendly_aircraft_port = 2;	

   char buf[1024];
   inputports_file >> enemy_tank_port;
   inputports_file.getline(buf, sizeof(buf));
   inputports_file >> enemy_aircraft_port;
   inputports_file.getline(buf, sizeof(buf));
   inputports_file >> friendly_aircraft_port;

   // record initial time
   timeval initime;
   gettimeofday(&initime, 0);
    

   char tmpchar[1024];
   char space = ' ';

   socket_tools::SocketClient client(argv[1], port, SOCK_STREAM);
   // conect to server
   int socket;
   socket = client.connect_server();
   int count = 0;
   while (socket == -1)
   {
      ++count;
      cout << count << endl;
      sleep(1);
      socket = client.connect_server();
   }

   // send data
   while(!infile.eof()){
      // read file
      infile.getline(tmpchar, sizeof(tmpchar));

      istringstream data(tmpchar);
      // compute delay time
      int tm = 0; 
      data >> tm;
      timeval now;
      gettimeofday(&now, 0);
      int delay = tm - (now.tv_sec - initime.tv_sec);
      if (delay > 0)
         sleep(delay);
      
      // send data
      /*
      write(socket, tmpchar, strlen(tmpchar));
      write(socket, &space, 1);
      */

      int id = 0;
      int x = 0;
      int y = 0;
      int color = 0;
      int direction = 0;

      data >> id;
      data >> x;
      data >> y;
      data >> color;
      data >> direction;

      int port = 0;

      
      // enemy tank
      if (id < 1000)
         port = enemy_tank_port;
      // enemy aircraft
      else if (id < 4000)
         port = enemy_aircraft_port;
      // friendly aircraft
      else
         port = friendly_aircraft_port;

      int packet_size = 7 * sizeof(int);
      write(socket, (char*)&packet_size, sizeof(int)); 
      write(socket, (char*)&port, sizeof(int)); 
      write(socket, (char*)&tm, sizeof(int)); 
      write(socket, (char*)&id, sizeof(int)); 
      write(socket, (char*)&x, sizeof(int)); 
      write(socket, (char*)&y, sizeof(int)); 
      write(socket, (char*)&color, sizeof(int)); 
      write(socket, (char*)&direction, sizeof(int)); 
   }
   client.close_connection();
   infile.close();
}

