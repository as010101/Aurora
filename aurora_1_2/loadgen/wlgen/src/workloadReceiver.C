#include "StreamReceiver.H"
#include <iostream>			// for cout
#include <sstream>			// for istringstream

using namespace std;
using namespace workload_generator;

int main(int argc, char* argv[])
{
   if (argc == 1) {
      StreamReceiver receiver;
      if (receiver.initialize())
         receiver.startCommandReceiver();
      else {
        cout << "Initializing input receiver failed." << endl;
        return 1;
      }
   }
   else if (argc == 3) {
      istringstream buffer1(argv[1]);
      int port;
      if (!(buffer1 >> port)) {
         cout << "Usage: wlrec "
              << "[receiver_port num_handler_threads] "
              << endl;
         return 1;
      }
      istringstream buffer2(argv[2]);
      int num_hadler_threads;
      if (!(buffer2 >> num_hadler_threads)) {
         cout << "Usage: wlrec "
              << "[receiver_port num_handler_threads] "
              << endl;
         return 1;
      }
      StreamReceiver receiver(atoi(argv[1]), atoi(argv[2]));
      if (receiver.initialize())
         receiver.startCommandReceiver();
      else {
        cout << "Initializing input receiver failed." << endl;
        return 1;
      }
   }
   else {
      cout << "Usage: wlrec "
           << "[receiver_port num_handler_threads] "
           << endl;
      return 1;
   }

   return 0;

}
