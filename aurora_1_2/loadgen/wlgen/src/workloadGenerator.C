#include "WorkloadGenerator.H"
#include "WGServer.H"
#include <iostream>			// for cout
#include <sstream>			// for istringstream

using namespace std;
using namespace workload_generator;

int main(int argc, char* argv[])
{
   if (argc == 1) {
      WorkloadGenerator workload_generator;
      WGServer	gui_command_reciever(&workload_generator);
      if (gui_command_reciever.initialize())
         workload_generator.startCommandReceiver();
      else {
        cout << "Initializing GUI command reciver failed." << endl;
        return 1;
      }
   } 
   else if (argc == 3) {
      istringstream buffer1(argv[2]);
      int aurora_input_server_port;
      if (!(buffer1 >> aurora_input_server_port)) {
         cout << "Usage: wlgen "
              << "[aurora_input_server_name aurora_input_server_port] "
              << "[GUI_command_receiver_port number_of_handler_threads]"
              << endl;
         return 1;
      }
      // arguments: aurora_input_server_name, aurora_input_server_port
      WorkloadGenerator workload_generator(
          argv[1], 
          aurora_input_server_port);
      WGServer	gui_command_reciever(&workload_generator);
      if (gui_command_reciever.initialize())
         workload_generator.startCommandReceiver();
      else {
        cout << "Initializing GUI command reciver failed." << endl;
        return 1;
      }
   }
   else if (argc == 5) {
      // arguments: aurora_input_server_name, 
      //            aurora_input_server_port
      //            workload_generator_server_port,
      //            number_of_handler_threads
      istringstream buffer1(argv[2]);
      int aurora_input_server_port;
      if (!(buffer1 >> aurora_input_server_port)) {
         cout << "Usage: wlgen "
              << "[aurora_input_server_name aurora_input_server_port] "
              << "[GUI_command_receiver_port number_of_handler_threads]"
              << endl;
         return 1;
      }

      istringstream buffer2(argv[3]);
      int workload_generator_server_port;
      if (!(buffer2 >> workload_generator_server_port)) {
         cout << "Usage: wlgen "
              << "[aurora_input_server_name aurora_input_server_port] "
              << "[GUI_command_receiver_port number_of_handler_threads]"
              << endl;
         return 1;
      }
      istringstream buffer3(argv[4]);
      int number_of_handler_threads;
      if (!(buffer3 >> number_of_handler_threads)) {
         cout << "Usage: wlgen "
              << "[aurora_input_server_name aurora_input_server_port] "
              << "[GUI_command_receiver_port number_of_handler_threads]"
              << endl;
         return 1;
      }
      WorkloadGenerator workload_generator(
          argv[1], 
          aurora_input_server_port);

      WGServer	gui_command_reciever(
          &workload_generator, 
          workload_generator_server_port,
          number_of_handler_threads);
      if (gui_command_reciever.initialize())
         workload_generator.startCommandReceiver();
      else {
        cout << "Initializing GUI command reciver failed." << endl;
        return 1;
      }
   }
   else {
      cout << "Usage: wlgen "
           << "[aurora_input_server_name aurora_input_server_port] "
           << "[GUI_command_receiver_port number_of_handler_threads]" 
           << endl;
      return 1;
   }

   return 0;
}

