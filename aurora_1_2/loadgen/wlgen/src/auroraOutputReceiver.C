#include "AuroraOutputReceiver.H"
#include <iostream>			// for cout
#include <sstream>			// for atoi

using namespace std;
using namespace workload_generator;

int main(int argc, char* argv[])
{
   if (argc == 1) {
      AuroraOutputReceiver reciever;
      reciever.startCommandReceiver();
   }
   else if (argc == 3) {
      istringstream buffer(argv[2]);
      int server_output_port;
      if (buffer >> server_output_port) {
         AuroraOutputReceiver reciever(argv[1], server_output_port);
         reciever.startCommandReceiver();
      }
   }
   else {
      cout << "Usage: arec "
           << "[aurora_output_server_name aurora_output_server_port] "
           << endl;
      return 1;
   }

   return 0;
}
