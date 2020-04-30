#include <iostream>
#include <sstream>			// for istringstream

#include "LRHarness.H"

using namespace std;
using namespace linear_road;

namespace linear_road {
  CarMap *the_car_map;
}

int main(int argc, const char* argv[])
{
  CarMap *cm = new CarMap();
  the_car_map = cm;
  
   if (argc == 2) {
      LRHarness lr_harness(argv[1], cm);
      lr_harness.startCommandReceiver();
   } 
   else if (argc == 5) {
      string host_name(argv[2]);
      int input_server_port;
      int output_server_port;
      istringstream buffer1(argv[3]);
      istringstream buffer2(argv[4]);
      if (!(buffer1 >> input_server_port)) {
         cout << "Usage: lrh <data_file> "
              << "[aurora_hostname aurora_input_server_port"
              << " aurora_output_sever_port]" << endl;
         return 1;
      }
      if (!(buffer2 >> output_server_port)) {
         cout << "Usage: lrh <data_file> "
              << "[aurora_hostname aurora_input_server_port"
              << " aurora_output_sever_port]" << endl;
         return 1;
      }

      LRHarness lr_harness(
            argv[1],
	    cm,
            host_name.c_str(),
            input_server_port,
            output_server_port);
      lr_harness.startCommandReceiver();
   }
   else {
      cout << "Usage: lrh <data_file> "
           << "[aurora_hostname aurora_input_server_port"
           << " aurora_output_sever_port]" << endl;
      return 1;
   }
   return 0;
}
