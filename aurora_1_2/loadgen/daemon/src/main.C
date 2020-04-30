#include "AuroraSocketAPI.H"
#include <iostream>
#include <sstream>			// for istringstream

using namespace std;

int main(int argc, const char* argv[])
{
   if (argc == 2) {
      aurora_socket_api::AuroraSocketAPI aurora;
      if (aurora.initialize(argv[1]) == false)
      {
         return 1;
      }
      aurora.startCommandReceiver();
   } 
   else if (argc == 4) {
      int input_server_port;
      int output_server_port;
      istringstream buffer1(argv[2]);
      istringstream buffer2(argv[3]);
      if (!(buffer1 >> input_server_port)) {
         cout << "Usage: run <xml-config-file> "
              << "[input_server_port output_server_port]" << endl;
         return 1;
      }
      if (!(buffer2 >> output_server_port)) {
         cout << "Usage: run <xml-config-file> "
              << "[input_server_port output_server_port]" << endl;
         return 1;
      }
      aurora_socket_api::AuroraSocketAPI aurora(
            input_server_port,
            10,
            output_server_port,
            10);
      if (aurora.initialize(argv[1]) == false)
      {
         return 1;
      }
      aurora.startCommandReceiver();
   }
   else {
      cout << "Usage: run <xml-config-file> "
           << "[input_server_port output_server_port]" << endl;
      return 1;
   }
   return 0;
}
