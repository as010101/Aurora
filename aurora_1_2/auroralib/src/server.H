/**************************************************************/
/*	Name: Zachary Aaron Kaplowitz (zkaplowi)
/*	File: server.H
/*	Asgn: server
/*	Date: 
/**************************************************************/

#ifndef server_Header
#define server_Header

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <iostream.h>
#include <sstream>


#include <aurora_cxx.H>

class server {

public:

  server(unsigned int hP);
  virtual ~server();

private:
  void writeData(int conn,char* rt,int size);
  void a_shutdown(AuroraNode* pAI, string inpt, int conn);
  void a_getNumInputPorts(AuroraNode* pAI, string inpt, int conn);
  void a_getInputPortNum(AuroraNode* pAI, string inpt, int conn);
  void a_getNumOutputPorts(AuroraNode* pAI, string inpt, int conn);
  void a_getOutputPortNum(AuroraNode* pAI, string inpt, int conn);
  void a_enqRecord(AuroraNode* pAI, string inpt, int conn, int sock);
  void a_deqRecord(AuroraNode* pAI, string inpt, int conn, int sock);
  void a_xStartLoadGenerator(AuroraNode* pAI, string inpt, int conn);
  void a_xStopLoadGenerator(AuroraNode* pAI, string inpt, int conn);
};

#endif
