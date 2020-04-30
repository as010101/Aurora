#include "LRHarness.H"

// C++ headers
#include <iostream>				// for cout, cin, endl
#include <iomanip>				// for setw
#include <sstream>				// for istringstream

// Loacal header

using namespace std;
namespace linear_road{

LRHarness::LRHarness(
      const char* data_filename,
      CarMap *car_map,
      const char* auora_hostname,
      int	aurora_input_receiver_port, 
      int	aurora_output_sender_port):
   _lr_sender(
	 car_map,
         data_filename,
         auora_hostname, 
         aurora_input_receiver_port),
   _lr_receiver(
	 car_map,
         auora_hostname,
         aurora_output_sender_port),
   cm(car_map) {
}

LRHarness::~LRHarness()
{
}


void LRHarness::startCommandReceiver()
{
   if (!_lr_receiver.start()) {
      cout << "Starting receiver failed" << endl;
      return;
   }
   printUsage();
   while (!cin.eof()) {
      const int MAX_COMMAND_LEN = 4096;
      char line[MAX_COMMAND_LEN];
      strcpy(line, "");
      cout << "--> ";
      cin.getline(line, sizeof(line));
      istringstream command_line(line);
      char command[MAX_COMMAND_LEN];
      strcpy(command, "");
      command_line >> command;

      if (strlen(command) == 0) {
         continue;
      }

      if (!strcmp(command, "exit") || 
          !strcmp(command, "quit") ||
          !strcmp(command, "q")) {
         return;
      }

      if (!strcmp(command, "h") || !strcmp(command, "?") 
                                || !strcmp(command, "help")){
         // print usage 
         printUsage();
         continue;
      }

      if (!strcmp(command, "s")){
         _lr_sender.start(true);
         continue;
      }

      if (!strcmp(command, "t")){
         _lr_sender.start(false);
         continue;
      }

      if (!strcmp(command, "c")){
         _lr_receiver.start();
         continue;
      }

      cout << "Invalid Command" << endl;
      
   }
}

void LRHarness::printUsage()
{
   cout.setf(ios::left);
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << setw(10) << "Command" 
        << setw(15) << "Parameters"
        << setw(40) << "Usage"
        << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << setw(10) << "s" 
        << setw(15) << ""
        << setw(50) << "Start sending data."
        << endl;
   cout << setw(10) << "t" 
        << setw(15) << ""
        << setw(50) << "Start sending data without waiting."
        << endl;
   cout << setw(10) << "c" 
        << setw(15) << ""
        << setw(50) << "Connect to Aurora ouput server."
        << endl;
   cout << setw(10) << "h" 
        << setw(15) << ""
        << setw(50) << "Print menu"
        << endl;
   cout << setw(10) << "q" 
        << setw(15) << ""
        << setw(50) << "Exit"
        << endl;
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}

}

