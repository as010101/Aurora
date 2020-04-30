/**************************************************************/
/*	Name: Zachary Aaron Kaplowitz (zkaplowi)
/*	File: server.C
/*	Date: 
/**************************************************************/


#include "server.H"



#define MAXLINE 512
/*
This constructor is called to start listening for calls to the aurora API set forth in AuroraNode.H.
It takes a user specified port number to begin running.  
The user provides this port number at the command line using the executable 
~/aurora/auroralib/bin/javaapi_server <portNumber>
Once this listener is running,  a user may create a java program and interface with aurora through
the methods in Client.java which mirrors AuroraNode.H but in the Java language.  
As of now, this server can only support one program and then server closes when that program
calls aurora_shutdown.
Also as of now, this API does not support multi-threads. 
It seems as though these two aspects could be easily implemented.
*/

server::server(unsigned int hP) {

  //Create the socket
  int ssock;
  struct sockaddr_in addr;

  ssock = socket(AF_INET, SOCK_STREAM, 0);
  if(ssock<0) {
    cerr<<"cant open stream socket"<<endl;
  }

  //bind the socket
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr =htonl(INADDR_ANY);
  unsigned int sp = hP;
  addr.sin_port = htons(sp);

  if ((bind (ssock, (struct sockaddr*)&addr, sizeof(addr)))<0) {    
    cerr<<"could not bind to port"<<endl;
    return;
  }


  //start the server

  listen(ssock, SOMAXCONN);
  
  AuroraNode * auroraInstance;
  bool shutdownCalled = false;

  while (!shutdownCalled) {

    //block until a client calls and accept that call
    int new_conn;
    struct sockaddr_in caddr;
    unsigned int caddrlen;
    caddrlen = sizeof(caddr);
    new_conn = accept(ssock,(struct sockaddr*)&caddr, &caddrlen);
    if (new_conn<0) {
      cerr<<"accept error"<<endl;
    }

    //read from the client
    int  n;
    char numbyts[MAXLINE+1];
    int rc;
    char c;
    ///read the number of bits to be read
    for (n=0; n<MAXLINE; n++) {
      rc = read(new_conn, &c, 1);
      if (rc==1) {
	numbyts[n]=c;
	if (c==' ')
	  break;
      } else if (rc==0) {
	break;
      } else {
	n= -1;
	break;
      }
    }
    if (n<0) {
      cerr<<"readline error"<<endl;
    }
    //read the line
    int byts = atoi(numbyts);
    int totalBytesRead = 0;
    char recvline[byts];
    while (totalBytesRead<byts) {
      rc = read(new_conn, &(recvline[totalBytesRead]),byts-totalBytesRead);
      if (rc>=0) {
	totalBytesRead+= rc;
      }
      else {
	cerr<<"readline error 2"<<endl;
      }
    }

    //for some reason there are extra bytes that need to be read.  I dont know why.
    for (n=0; n<MAXLINE; n++) {
      rc = read(new_conn, &c, 1);
      if (rc==1) {
	if (c=='\n')
	  break;
      } else if (rc==0) {
	break;
      }
    }

    //cut off extra byts which appear for some smelly reason
    string inpt = recvline;
    inpt = inpt.substr(0,byts);
    
    
    //use first bit of recvline to determine command and call command
    int pos = inpt.find(" ", 0);
    string meths = inpt.substr(0, pos);
    int meth = atoi(meths.c_str());
    inpt.erase(0,pos+1);
    //initialization must be done here
    if (meth==1) {      
      int pos = inpt.find("\n", 0);
      //cerr<<"calling aurora_init("<<inpt.substr(0,pos)<<")"<<endl;
	  char* rt = "1 ";
      string rtemp = rt;
      
      try
	{
	  
	  auroraInstance = AuroraNode::createAuroraNode(inpt.substr(0,pos));
	}
      catch (exception e)
	{
	  cerr<<e.what()<<endl;
	  rtemp = "0 ";
	  rtemp.append(e.what());
	  rt = (char*)rtemp.c_str();
	} 
      
      int size = rtemp.size();
      writeData(new_conn, rt, size);
    }

    else if (meth==2) {
      a_shutdown(auroraInstance, inpt, new_conn);
      shutdownCalled = true;
    }
    else if (meth==3) {
      a_getNumInputPorts(auroraInstance, inpt, new_conn);
    }
    
    else if (meth==4) {
      a_getInputPortNum(auroraInstance, inpt, new_conn);
    }
    else if (meth==5) {
      a_getNumOutputPorts(auroraInstance, inpt, new_conn);
    }
    else if (meth==6) {
      a_getOutputPortNum(auroraInstance, inpt, new_conn);      
    }
    else if (meth==7) {
      a_enqRecord(auroraInstance, inpt, new_conn, ssock);  
    }
    else if (meth==8) {
      a_deqRecord(auroraInstance, inpt, new_conn, ssock); 
    }
    else if (meth==9) {    
      a_xStartLoadGenerator(auroraInstance, inpt, new_conn); 
    }
    else if (meth==10) {
      a_xStopLoadGenerator(auroraInstance, inpt, new_conn); 
    }
    else {
      // make new methods and number for their call here
      
      cerr<<"method unknown: "<<meth<<endl;
      cerr<<inpt<<endl;
    }
    
    if (!((meth==7)||(meth==8))) {
      close(new_conn);   
    }
  }
  
  delete auroraInstance;
 
  close(ssock);

 
}
server::~server() {}


//writes data whenever needed back to client
void server::writeData(int conn, char* rt, int size) {
  while (size>0) {
    int nwritten = write(conn, rt, 1);
    size -= nwritten;
    rt += nwritten;
  } 
  
}

//shuts down aurora
void server::a_shutdown(AuroraNode* aI, string inpt, int conn){
  char* rt = "1 ";
  string rtemp = rt;
  try
    {	  
      aI->shutdown();
      

    }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    } 
  int size = rtemp.size();
  writeData(conn, rt, size);
}

//gets the number of input ports
void server::a_getNumInputPorts(AuroraNode* aI, string inpt, int conn){
  char* rt = "1 ";
  string rtemp = rt;
  try
    {
      vector<int> inPorts;
      vector<int> outPorts;
      aI->listPorts(inPorts,outPorts);
      int stmp = inPorts.size();
      std::ostringstream number_str;
      number_str << stmp;
      rtemp.append(number_str.str());
      rt = (char*)rtemp.c_str();
      
    }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    } 
  
  int size = rtemp.size();
  writeData(conn, rt, size);
}

//gets an input port number
void server::a_getInputPortNum(AuroraNode* aI, string inpt, int conn){

  int pos = inpt.find("\n", 0);
  int x1 = atoi(inpt.substr(0,pos).c_str());
  char* rt = "1 ";
  string rtemp = rt;
  try
    {
      vector<int> inPorts;
      vector<int> outPorts;
      aI->listPorts(inPorts,outPorts);
      if ((x1<0)||(x1>=inPorts.size())) {
	rt="0 ";
	rtemp=rt;
      }
      else {
	
	std::ostringstream number_str;
	int ntmp = inPorts.at(x1);
	number_str << ntmp;
	rtemp.append(number_str.str());
	rt = (char*)rtemp.c_str();
      }
    }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    } 
  
  int size = rtemp.size();
  writeData(conn, rt, size);
}


//get the number of output ports
void server::a_getNumOutputPorts(AuroraNode* aI, string inpt, int conn){

  char* rt = "1 ";
  string rtemp = rt;
  try
    {
      vector<int> inPorts;
      vector<int> outPorts;
      aI->listPorts(inPorts,outPorts);
      int stmp = outPorts.size();
      std::ostringstream number_str;
      number_str << stmp;
      rtemp.append(number_str.str());
      rt = (char*)rtemp.c_str();
      
    }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    } 
  
  int size = rtemp.size();
  writeData(conn, rt, size);
}

//get an output port number
void server::a_getOutputPortNum(AuroraNode* aI, string inpt, int conn){
  int pos = inpt.find("\n", 0);
  int x1 = atoi(inpt.substr(0,pos).c_str());
  char* rt = "1 ";
  string rtemp = rt;
  try
    {
      vector<int> inPorts;
      vector<int> outPorts;
      aI->listPorts(inPorts,outPorts);
      if ((x1<0)||(x1>=outPorts.size())) {
	rt="0 OutputPort out of bounds";
	rtemp=rt;
      }
      else {
	
	std::ostringstream number_str;
	int ntmp = outPorts.at(x1);
	number_str << ntmp;
	rtemp.append(number_str.str());
	rt = (char*)rtemp.c_str();
      }
    }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    } 
  
  int size = rtemp.size();
  writeData(conn, rt, size);
}

//enqueue a tuple - this method makes new connections because the client has to send
//more than one thing
void server::a_enqRecord(AuroraNode* aI, string inpt, int conn,int sock){
  close(conn);
  int pos = inpt.find (" ", 0);
  int x1 = atoi(inpt.substr(0,pos).c_str());
  inpt.erase(0,pos+1);
  pos = inpt.find (" ", 0);
  int x2 = atoi(inpt.substr(0,pos).c_str());
  inpt.erase(0,pos+1);
  pos = inpt.find ("\n ", 0);
  int x3 = atoi(inpt.substr(0,pos).c_str());    


  int new_conn;
  struct sockaddr_in caddr;
  unsigned int caddrlen;
  caddrlen = sizeof(caddr);
  new_conn = accept(sock,(struct sockaddr*)&caddr, &caddrlen);
  if (new_conn<0) {
    cerr<<"accept error"<<endl;
  }
  char tupleAddr[x3];
  char* pTupleAddr = tupleAddr;

  int totalBytesRead = 0;
  int rc = 0;
  while (totalBytesRead<x3) {
    rc = read(new_conn, pTupleAddr,x3-totalBytesRead);
    if (rc>=0) {
      totalBytesRead+= rc;
    }
    else {
      cerr<<"readline error for tuple"<<endl;
    }
  }
  
  

  char* rt = "1 ";
  string rtemp = rt;
  try {   
    //enq
    aI->enqRecord(x1,x2,pTupleAddr,x3);   
    
    rtemp.append(pTupleAddr);
    rt = (char*)rtemp.c_str();
  }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    }
  int size = rtemp.size();
  writeData(new_conn, rt, size);
  close(new_conn);

  int new_conn2;
  struct sockaddr_in caddr2;
  unsigned int caddrlen2;
  caddrlen2 = sizeof(caddr2);
  new_conn2 = accept(sock,(struct sockaddr*)&caddr2, &caddrlen2);
  if (new_conn2<0) 
    cerr<<"accept error"<<endl;

  writeData(new_conn2, pTupleAddr, x3);
  close(new_conn2);
}

//dequeue a tuple - this creates new connections because it sends back more than one thing
void server::a_deqRecord(AuroraNode* aI, string inpt, int conn, int sock){
  int pos = inpt.find ("\n", 0);
  int x1 = atoi(inpt.substr(0,pos).c_str());
  inpt.erase(0,pos-1);
  char* rt = "1 ";
  string rtemp = rt;
  int outputPortID = 0;
  char tupleAddr[x1];
  char* pTupleAddr = tupleAddr;
  try {

    if (aI->deqRecord(outputPortID,pTupleAddr,x1)){
      int wasDq = 1;
      
      std::ostringstream number_str;
      number_str << wasDq;
      rtemp.append(number_str.str());
      rtemp.append(" ");
      std::ostringstream number_str2;
      number_str2 << outputPortID;
      rtemp.append(number_str2.str());
      rt = (char*)rtemp.c_str();
      
    }
    else {
      int wasDq = 0;
      
      std::ostringstream number_str;
      number_str << wasDq;
      rtemp.append(number_str.str());
    }	
  }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    }
  int size = rtemp.size();
  writeData(conn, rt, size);  
  close(conn);

  int new_conn;
  struct sockaddr_in caddr;
  unsigned int caddrlen;
  caddrlen = sizeof(caddr);
  new_conn = accept(sock,(struct sockaddr*)&caddr, &caddrlen);
  if (new_conn<0) 
    cerr<<"accept error"<<endl;

  writeData(new_conn, pTupleAddr, x1);
  close (new_conn);
}

//starts Load Generator
void server::a_xStartLoadGenerator(AuroraNode* aI, string inpt, int conn){
  char* rt = "1 ";
  string rtemp = rt;
  try
    {
      aI->xStartLoadGenerator();
    }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    } 
  
  int size = rtemp.size();
  writeData(conn, rt, size);
}


//stops Load Generator
void server::a_xStopLoadGenerator(AuroraNode* aI, string inpt, int conn){
  char* rt = "1 ";
  string rtemp = rt;
  try
    {
      aI->xStopLoadGenerator();
    }
  catch (exception e)
    {
      cerr<<e.what()<<endl;
      rtemp = "0 ";
      rtemp.append(e.what());
      rt = (char*)rtemp.c_str();
    } 
  
  int size = rtemp.size();
  writeData(conn, rt, size);
}

