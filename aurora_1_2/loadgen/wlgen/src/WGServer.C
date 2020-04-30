#include "WGServer.H"
;
// C++ headers
#include <map>			// for pair
#include <iostream>		// for cout, cerr, endl
#include <cstdio>		// for sprintf

// Local headers
#include "Types.H"
#include "AggStreamSource.H"

using namespace std;
namespace workload_generator{

WGServer::WGServer(WorkloadGenerator* workload_generator):
   _port(DEFAULT_PORT),
   _num_handler_threads(DEFAULT_NUM_HANDLERS),
   _server(0),
   _workload_generator(workload_generator)
{
}

WGServer::WGServer(
      WorkloadGenerator* workload_generator,
      int port,
      int num_handler_threads):
   _port(port),
   _num_handler_threads(num_handler_threads),
   _server(0),
   _workload_generator(workload_generator)
{
}
  

WGServer::~WGServer() 
{
   if (_server != 0)
      delete _server;
}

bool WGServer::initialize()
{
   cout << "Starting GUI command receiver... " << endl;
   _server = new ::socket_tools::SocketServer(
         _port, 
         DEFAULT_TYPE, 
         _num_handler_threads,
         (void(*)(int, void*))serverHandler,
         this);
   if (_server->start()) {
      cout << "input receiver started at port "
           << _port
           << endl;
      return true;
   }
   else {
      return false;
   }
}

bool WGServer::cread(int socket, char* buf, int len) 
{
   int rlen = 0;
   while (rlen < len) {
      int rval = read(socket, buf + rlen, len - rlen);
      if (rval == 0) {
         cout << "WGServer: connection closed by gui at socket "  
              << socket
              << endl;
         close(socket);
         return false;
      }
      if (rval == -1) {
         cout << "WGServer: read socket " << socket << " error." << endl;
         perror("");
         close(socket);
         return false;
      }
      rlen += rval;
   }
}

void WGServer::serverHandler(int sock, WGServer* me)
{
    // This function is writen by arsinger to receive command packet from
    // GUI

    cout << endl << "Request received from gui" << endl;

    // read in the whole packet
    char buf[MAX_BUFFER_SIZE];
    int rval = 1;
    int rlen = 0;
    while (rval && rlen < MAX_BUFFER_SIZE) {
      rval = read(sock, buf + rlen , MAX_BUFFER_SIZE - rlen);
      rlen += rval;
    }
    if (rval != 0) {
       // packet too long
       cout << "WGServer error: Packet too large" << endl;
       close(sock);
       return;
    }

    int num_of_ints_readed = rlen / sizeof(int);

    //read the mode
    if (num_of_ints_readed == 0)  {
       cout << "The size of the packet is not correct." << endl;
       close(sock);
       return;
    }
    wg_mode_t mode;
    mode = (wg_mode_t)(ntohl(*((int*)buf))+0.5);

    // read in the information for aggegate stream sourc
    AggStreamSource stream;
    if (!readAggStreamInfo(
             (int*)(buf + sizeof(int)),
             num_of_ints_readed - 1, 
             stream)) {
       cout << "Wrong data packet from GUI" << endl;
       stream.print();
       close(sock);
       return;
    }

    ////////////////////////
    switch (mode) {
       case WG_START:
          cout << "Adding aggregate stream source: " << endl;
          stream.print();
          me->_workload_generator->addAggSource(stream);
          break;
       case WG_UPDATE:
          cout << "Change aggregate stream rate: " << endl;
          stream.print();
          me->_workload_generator->changRate(stream);
          break;
       case WG_STOP:
          cout << "Stop generating streams." << endl;
          me->_workload_generator->emptySourses();
          break;
       default:
          cerr << "Wrong command received." << endl;
    }

    close(sock);
}

bool WGServer::readAggStreamInfo(
      int* data, 
      int num_ints, 
      AggStreamSource& stream)
{
   int curpos = 0;
   int sign;

   //read the stream id
   if (curpos >= num_ints)  {
      cout << "The size of the packet is not correct." << endl;
      return false;
   }
   int id = (int)(ntohl(data[curpos])+0.5);
   stream.id.numberID = id;
   sprintf(stream.id.stringID, "input%d", id);
   ++curpos;


   // read number of streams
   if (curpos >= num_ints)  {
      cout << "The size of the packet is not correct." << endl;
      return false;
   }
   stream.num_streams = (int)(ntohl(data[curpos])+0.5);
   if (stream.num_streams <= 0) {
      cout << "Number of streams must be larger than zero." << endl;
      return false;
   }
   ++curpos;

   // read number of tuples
   if (curpos >= num_ints)  {
      cout << "The size of the packet is not correct." << endl;
      return false;
   }
   sign = (int)(ntohl(data[curpos])+0.5);
   sign = (sign == 1) ? 1 : -1;
   ++curpos;
   if (curpos >= num_ints)  {
      cout << "The size of the packet is not correct." << endl;
      return false;
   }
   stream.num_tuples = (int)(ntohl(data[curpos])+0.5) * sign;
   ++curpos;
   
   distribution_t dist;
   int maxJ, j, maxK, k, temp1, temp2;

   // read rate information
   
   // read type
   if (curpos >= num_ints)  {
      cout << "The size of the packet is not correct." << endl;
      return false;
   }
   int type = (int)(ntohl(data[curpos])+0.5);
   if (type > MAX_DISTRIBUTION_TYPE_NUMBER || type < 0){
      cerr << "Invalid distribution type: " 
           << type
           << endl;
      cerr << "Reading rate distribution type failed."
           << endl;
      return false;
   }
   stream.rate_type = static_cast<distributiontype_t>(type); 
   ++curpos;

   // read parameter distributions for rate distribution
   // max J is the number of parameters in rate type
   if (curpos >= num_ints)  {
      cout << "The size of the packet is not correct." << endl;
      return false;
   }
   maxJ = (int)(ntohl(data[curpos])+0.5);
   ++curpos;
   
   if (maxJ > MAX_PARAMETER_NUMBER) {
      cerr << "Invalid number of distribution parameters: "
           << maxJ
           << endl;
      return false;
   }
   for (j=0; j<maxJ; ++j) {
       parlist_t params1;

       //distribution type of the single variable
       // read the distribution type
       if (curpos >= num_ints)  {
          cout << "The size of the packet is not correct." << endl;
          return false;
       }
       temp1 = (int)(ntohl(data[curpos])+0.5);
       ++curpos;


       // read number of parameters 
       // maxK is the number of parameters
       if (curpos >= num_ints)  {
          cout << "The size of the packet is not correct." << endl;
          return false;
       }
       maxK = (int)(ntohl(data[curpos])+0.5);
       ++curpos;


       if (maxK > MAX_PARAMETER_NUMBER) {
          cerr << "Invalid number of distribution parameters: "
               << maxK
               << endl;
          return false;
       }
       for (k=0; k<maxK; ++k) {
           // read each of the parameters
           if (curpos >= num_ints)  {
              cout << "The size of the packet is not correct." << endl;
              return false;
           }
           sign = (int)(ntohl(data[curpos])+0.5);
           ++curpos;
           if (curpos >= num_ints)  {
              cout << "The size of the packet is not correct." << endl;
              return false;
           }
           temp2 = (int)(ntohl(data[curpos])+0.5);
           ++curpos;
           if (sign==0) {
               temp2 = -temp2;
           }
           params1.push_back(((double)temp2)/1000000.0);
       }

       //append the new rate distribution parameters
       stream.rate_parameters.push_back(
             pair<distributiontype_t, parlist_t>(
                (distributiontype_t)temp1, 
                (parlist_t)params1) );
   }
  


   // Now read attributes
   // read number of attributes
   if (curpos >= num_ints)  {
      cout << "The size of the packet is not correct." << endl;
      return false;
   }
   int numAttrs = (int)(ntohl(data[curpos])+0.5);
   ++curpos;

   // check number of attributes
   if (numAttrs > MAX_ATTRIBUTE_NUMBER) {
      cout << "Invalid number of attributes: "
           << numAttrs
           << endl;
      return false; 
   }

   for (int a=0; a<numAttrs; ++a) {
       AttributeDef attr;

       // read attribute data type
       if (curpos >= num_ints)  {
          cout << "The size of the packet is not correct." << endl;
          return false;
       }
       attr.attribute_data_type.type = (attr_type_t)(ntohl(data[curpos])+0.5);
       ++curpos;

       // read length of this type 
       if (curpos >= num_ints)  {
          cout << "The size of the packet is not correct." << endl;
          return false;
       }
       attr.attribute_data_type.length = (int)(ntohl(data[curpos])+0.5);
       ++curpos;

       // read attribute distribution type
       if (curpos >= num_ints)  {
          cout << "The size of the packet is not correct." << endl;
          return false;
       }
       attr.source_type = (distributiontype_t)(ntohl(data[curpos])+0.5);
       ++curpos;

       // read number of parameters
       if (curpos >= num_ints)  {
          cout << "The size of the packet is not correct." << endl;
          return false;
       }
       maxJ = (int)(ntohl(data[curpos])+0.5);
       ++curpos;

       if (maxJ > MAX_PARAMETER_NUMBER) {
          cerr << "Invalid number of distribution parameters: "
               << maxJ
               << endl;
          return false;
       }
       // read in the parameters 
       for (j=0; j<maxJ; ++j) {
           parlist_t parameters;

           // distribution type per parameter
           if (curpos >= num_ints)  {
              cout << "The size of the packet is not correct." << endl;
              return false;
           }
           temp1 = (int)(ntohl(data[curpos])+0.5);
           ++curpos;

           // read number of parameters
           if (curpos >= num_ints)  {
              cout << "The size of the packet is not correct." << endl;
              return false;
           }
           maxK = (int)(ntohl(data[curpos])+0.5);
           ++curpos;


           if (maxK > MAX_PARAMETER_NUMBER) {
              cerr << "Invalid number of distribution parameters: "
                   << maxJ
                   << endl;
              return false;
           }
           // read paramters
           for (k=0; k<maxK; ++k) {
               if (curpos >= num_ints)  {
                  cout << "The size of the packet is not correct." << endl;
                  return false;
               }
               sign = (int)(ntohl(data[curpos])+0.5);
               ++curpos;
               if (curpos >= num_ints)  {
                  cout << "The size of the packet is not correct." << endl;
                  return false;
               }
               temp2 = (int)(ntohl(data[curpos])+0.5);
               ++curpos;
               if (sign==0) {
                   temp2 = -temp2;
               }
               parameters.push_back(((double)temp2)/1000.0);
           }

           //append the new rate distribution parameters
           attr.source_parameters.push_back(
                 pair<distributiontype_t, parlist_t>(
                    (distributiontype_t)temp1,
                    (parlist_t)parameters) );
       }
       stream.attributes.push_back(attr);
   }
   return true;
}

}
