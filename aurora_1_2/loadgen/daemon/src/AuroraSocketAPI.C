#include "AuroraSocketAPI.H"

// C++ headers
#include <iostream>				// for cout, cin, endl
#include <iomanip>				// for setw
#include <sstream>				// for istringstream

// Aurora header
#include <aurora.H>

// Loacal header

using namespace std;
namespace aurora_socket_api{

AuroraSocketAPI::AuroraSocketAPI(): 
   _aurora_instance(0)
{
}

AuroraSocketAPI::AuroraSocketAPI(
      int	input_receiver_port, 
      int	input_receiver_num_handler_threads,
      int	output_sender_port, 
      int	output_sender_num_handler_threads):
   _input_receiver(
         input_receiver_port, 
         input_receiver_num_handler_threads),
   _output_sender(
         output_sender_port, 
         output_sender_num_handler_threads),
   _aurora_instance(0)
{
}

AuroraSocketAPI::~AuroraSocketAPI()
{
   AURORA_API_RETURN_CODE rc = aurora_shutdown(_aurora_instance);
   if (rc != AURORA_OK)
   {
      cout << "Error: aurora_shutdown(...) returned " << (int)rc;
      return;
   }	
}

bool AuroraSocketAPI::initialize(const char* config_filename)
{
   bool result = true;
   AURORA_API_RETURN_CODE rc;

   rc = aurora_init(& _aurora_instance, config_filename);
   if (rc != AURORA_OK)
   {
      cerr << "aurora_init(...) returned: " << (int)rc << endl;
      return false;
   }

   assert(_aurora_instance != 0);

   if (!_input_receiver.initialize(_aurora_instance)) {
      result = false;
   }
   if (!_output_sender.initialize(_aurora_instance)) {
      result = false;
   }

   return result;
}

void AuroraSocketAPI::listPorts()
{
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
   cout << "Input ports:" << endl;

   int numPorts;
   AURORA_API_RETURN_CODE rc;
      
   rc = aurora_getNumInputPorts(_aurora_instance, & numPorts);
   if (rc != AURORA_OK)
   {
      cout << "Error: aurora_getNumInputPorts(...) returned " << (int)rc;
      return;
   }

   if (numPorts == 0)
   {
      cout << "   (none)" << endl;
   }
   else
   {
      for (int i = 0; i < numPorts; ++i)
      {
         int portName;
         rc = aurora_getInputPortNum(_aurora_instance, i, & portName);
         if (rc != AURORA_OK)
         {
            cout << "Error: aurora_getInputPortNum(...) returned " << (int)rc;
            return;
          }
         cout << "   " << portName << endl;
      }
   }
      
   cout << endl << "Output ports:" << endl;
      
   rc = aurora_getNumOutputPorts(_aurora_instance, & numPorts);
   if (rc != AURORA_OK)
   {
   cout << "Error: aurora_getNumOutputPorts(...) returned " << (int)rc;
   return;
   }
      
   if (numPorts == 0)
   {
   cout << "   (none)" << endl;
   }
   else
   {
      for (int i = 0; i < numPorts; ++i)
      {
         int portName;
         rc = aurora_getOutputPortNum(_aurora_instance, i, & portName);
         if (rc != AURORA_OK)
         {
            cout << "Error: aurora_getOutputPortNum(...) returned " << (int)rc;
            return;
         }
            
         cout << "   " << portName << endl;
      }
   }

   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}


void AuroraSocketAPI::enalbeInternalLoadGenerator()
{
   AURORA_API_RETURN_CODE rc = aurora_xStartLoadGenerator(_aurora_instance);
   if (rc != AURORA_OK)
   {
      cout << "Error: aurora_xStartLoadGenerator(...) returned " << (int)rc;
   }
   cout << "Using internal workload generator." << endl;
}

void AuroraSocketAPI::disalbeInternalLoadGenerator()
{
   AURORA_API_RETURN_CODE rc = aurora_xStopLoadGenerator(_aurora_instance);
   if (rc != AURORA_OK)
   {
      cout << "Error: aurora_xStopLoadGenerator(...) returned " << (int)rc;
   }
   cout << "Internal workload generator stopped." << endl;
}

void AuroraSocketAPI::printClients()
{
   _output_sender.printClientList();
}

void AuroraSocketAPI::printDataRates()
{
   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;

   cout << "Enqueue data rate:        " 
        << setw(15) << _input_receiver.averageDataRate()
        << " tuples/second." << endl;
   cout << "Dequeue data rate:        " 
        << setw(15) << _output_sender.averageDataRate()
        << " tuples/second." << endl;

   cout << "----------------------------------------";
   cout << "----------------------------------------" << endl;
}

void AuroraSocketAPI::startCommandReceiver()
{
   printUsage();
   while (1) {
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

      if (!strcmp(command, "l")){
         listPorts();
         continue;
      }

      if (!strcmp(command, "r")){
         printDataRates();
         continue;
      }

      if (!strcmp(command, "e")){
         enalbeInternalLoadGenerator();
         continue;
      }

      if (!strcmp(command, "d")){
         disalbeInternalLoadGenerator();
         continue;
      }

      if (!strcmp(command, "p")){
         printClients();
         continue;
      }

      if (!strcmp(command, "h") || !strcmp(command, "?") 
                                || !strcmp(command, "help")){
         // print usage 
         printUsage();
         continue;
      }

      cout << "Invalid Command" << endl;
      
   }
}

void AuroraSocketAPI::printUsage()
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
   cout << setw(10) << "l" 
        << setw(15) << ""
        << setw(50) << "List ports."
        << endl;
   cout << setw(10) << "r" 
        << setw(15) << ""
        << setw(50) << "Print average enqueue/dequeue data rates."
        << endl;
   cout << setw(10) << "e" 
        << setw(15) << ""
        << setw(50) << "Enable internal workload generator."
        << endl;
   cout << setw(10) << "d" 
        << setw(15) << ""
        << setw(50) << "Disable internal workload generator."
        << endl;
   cout << setw(10) << "p" 
        << setw(15) << ""
        << setw(50) << "Print output client list."
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

