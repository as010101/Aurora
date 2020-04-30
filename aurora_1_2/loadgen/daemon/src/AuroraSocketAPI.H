#ifndef _AURORA_SOCKET_API_
#define _AURORA_SOCKET_API_


// Loacal header
#include "InputReceiver.H"
#include "OutputSender.H"

using namespace std;
namespace aurora_socket_api{

class AuroraSocketAPI
{
public:
   AuroraSocketAPI();
   AuroraSocketAPI(
         int	input_receiver_port, 
         int	input_receiver_num_handler_threads,
         int	output_sender_port, 
         int	output_sender_num_handler_threads);
   ~AuroraSocketAPI();

   // Initialize Aurora Instance
   bool initialize(const char* config_filename);

   // Start a command line interface
   // This interface provide all the functions the user needs
   void startCommandReceiver();


   // For people who want to work directly with AURORA API 
   void* auroraInstance() { return _aurora_instance;}

private:
   InputReceiver	_input_receiver;
   OutputSender		_output_sender;
   void*		_aurora_instance;

   void listPorts();
   void printClients();
   void printDataRates();
   void enalbeInternalLoadGenerator();
   void disalbeInternalLoadGenerator();
   void printUsage();

};

}
#endif
