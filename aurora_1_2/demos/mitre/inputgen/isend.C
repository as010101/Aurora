#include <fstream>         // fro fstream
#include <stdlib.h>        // for atoi
#include <sys/time.h>      // for gettimeofday timeval
#include <unistd.h>        // for sleep;
#include <vector>          // for vector
#include <string>          // for string

#include "SocketClient.H"
#include "Timeval.H"       // for Timeval
#include <iostream>

using namespace workload_generator;

main(int argc, char** argv)
{

   if (argc != 3){
      cout << "usage: ssend hostname port" << endl; 
      exit(0);
   }

   fstream configfile;
   configfile.open("isendconfig", ios::in);
   if (!configfile)
      cerr << "Can't open isendconfig" << endl;
   int num_row;		// number of rows in one image
   int num_column;	// number of columns in one image
   int pixel_bytes;	// bytes/pixel
   int rows_per_tuple;	// rows per tuple
   int images_per_minute;  // images per minute
   int delay;              // initial delay
   int input_port;	   // input portname
   vector<string> img_files; // img filenames

   char buf[1024];
   configfile >> num_row;
   configfile.getline(buf, sizeof(buf));
   configfile >> num_column;
   configfile.getline(buf, sizeof(buf));
   configfile >> pixel_bytes;
   configfile.getline(buf, sizeof(buf));
   configfile >> rows_per_tuple;
   configfile.getline(buf, sizeof(buf));
   configfile >> images_per_minute;
   configfile.getline(buf, sizeof(buf));
   configfile >> delay;
   configfile.getline(buf, sizeof(buf));
   configfile >> input_port;
   configfile.getline(buf, sizeof(buf));
   char image_name[1024];
   while (!configfile.eof()){
      if(configfile >> image_name) {
         configfile.getline(buf, sizeof(buf));
         string imgfile_name = image_name;
         img_files.push_back(imgfile_name);
      }
   }
   configfile.close();

   
   int tuple_per_sec = (int)ceil((double)images_per_minute / 60.0 * 
                                 (double)num_row / 
                                 (double)rows_per_tuple);	
   int rowlen = num_column * pixel_bytes;
   int img_tuple_size = rowlen * rows_per_tuple;
   int tuple_size = sizeof(int) * 5 + img_tuple_size;


   // compute send interval
   int u_interval = 1000000/tuple_per_sec;
   Timeval interval(0, u_interval);
   // record initial time
   Timeval initime;
   gettimeofday(&initime, 0);
   Timeval sendtime(initime);

   socket_tools::SocketClient client(argv[1], atoi(argv[2]), SOCK_STREAM);
   // conect to server
   int socket;
   socket = client.connect_server();
   int count = 0;
   while (socket == -1)
   {
      ++count;
      cout << count << endl;
      sleep(2);
      socket = client.connect_server();
   }

   unsigned long tuple_count = 0;

   while(1) {
      int j = 0;
      for (vector<string>::iterator it = img_files.begin();
           it != img_files.end();
           ++it) {

         fstream imagefile;
         imagefile.open((*it).c_str(), ios::in|ios::binary);

         if (!imagefile) {
            cerr << "Can't open imagefile" << (*it).c_str()
                 << endl
                 << "Pleae check configuration file." 
                 << endl;
            continue;
         }

         imagefile.seekg(0, ios::end);
         int filelen = imagefile.tellg(); 
         imagefile.seekg(0, ios::beg);
  
         Timeval now;
         gettimeofday(&now, 0);

         int img_index = j;
         int timestamp = now.tv_sec - initime.tv_sec;
 
/* 
         char* imgpack = new char[rowlen];
         for (int i = 0; i < rowlen; ++i) {
            imgpack[i] = '0';
         }
 
         // write the first frame
         write(socket, (char*)&tuple_size, sizeof(int));
         write(socket, (char*)&input_port, sizeof(int));
         write(socket, (char*)&timestamp, sizeof(int));
         write(socket, (char*)&img_index, sizeof(int));
         write(socket, (char*)&num_column, sizeof(int));
         write(socket, (char*)&num_row, sizeof(int));
         
         for (int i = 0; i < rows_per_tuple; ++i) {
            write(socket, imgpack, rowlen);
         }
         delete [] imgpack;
*/
         
         int row_index = 1; 
         while( imagefile.tellg() < filelen) {
            int row_index_end = row_index + rows_per_tuple - 1;
 
            write(socket, (char*)&tuple_size, sizeof(int));
            write(socket, (char*)&input_port, sizeof(int));
            write(socket, (char*)&timestamp, sizeof(int));
            write(socket, (char*)&j, sizeof(int));
            write(socket, (char*)&row_index, sizeof(int));
            write(socket, (char*)&row_index_end, sizeof(int));

            ++tuple_count;
            cout << "tuples sent: " << tuple_count << endl;

            row_index += rows_per_tuple;
  
            char* rowbuf = new char[rowlen];
            for (int i = 0; i < rows_per_tuple; ++i) {
               imagefile.read(rowbuf, rowlen);
               write(socket, rowbuf, rowlen);
               if (imagefile.eof())
                  break;
            }
            delete [] rowbuf;
         }
         imagefile.close();
         sleep(delay);
         ++j;
      }  
   }
   client.close_connection();
}

