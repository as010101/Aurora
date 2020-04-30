#ifndef _LR_HARNESS_
#define _LR_HARNESS_

#include <map>

// Local header
#include "LRSender.H"
#include "LRReceiver.H"
#include "CarMap.H"

using namespace std;
namespace linear_road{

  extern CarMap *the_car_map;


  const char DEFAULT_AURORA_SERVER_NAME[] = "localhost";
  static const int DEFAULT_AURORA_INPUT_SERVER_PORT = 16000; 
  static const int DEFALT_AURORA_OUTPUT_SERVER_PORT = 17000;

  class LRHarness
  {
  public:
    LRHarness(
      const char* data_filename,
      CarMap *cm,
      const char* auora_hostname = DEFAULT_AURORA_SERVER_NAME,
      int aurora_input_receiver_port = DEFAULT_AURORA_INPUT_SERVER_PORT,
      int aurora_output_sender_port = DEFALT_AURORA_OUTPUT_SERVER_PORT);
    ~LRHarness();
    
    // Start a command line interface
    // This interface provide all the functions the user needs
    void startCommandReceiver();
    
  private:
    LRSender		_lr_sender;
    LRReceiver		_lr_receiver;
    CarMap *cm;
    
    void printUsage();
  };
  
}
#endif
