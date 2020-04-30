/****************************************************************/
/*    NAME: Zachary Kaplowitz                                   */
/*    ACCT:                                                     */
/*    FILE: main_server.C                                       */
/****************************************************************/

#include "server.H"
#include <iostream.h>

//this is used to create the executable that starts server.C (the javaapi_server)
int main(int argc, char** argv) {
  
  if (argc<2) {
    cerr<<"Usage: javaapi_server <PortNumber>"<<endl;
    return 1;
  }
  
    unsigned int p = atoi(argv[1]);
    server* s = new server(p);

    //cerr<<"left server"<<endl;
    
    delete s;
    //cerr<<"deleted server"<<endl;

  return 1;
};

