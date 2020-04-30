/*************************************************************************
 *    NAME: Akash Hasmukh Parikh
 *    USER: ahp
 *    FILE: ExportDelayedData.cpp
 *    DATE: Wed Apr 23  2:25:10 2003
 *************************************************************************/
#include "ExportDelayedData.H"
#include "NetworkData.H"
#include <RuntimeGlobals.H>
#include <PropsFile.H>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>

#include <vector.h>
#include <map.h>
#include <time.h>

/*************************************************************************
 * Function Name: ExportDelayedData::ExportDelayedData
 * Parameters: short port_num, int backlog
 * Effects: 
 *************************************************************************/

ExportDelayedData::ExportDelayedData(short port, int backlog)
{
  _port = port;
  _backlog = backlog;
//  _dm = new DataManager();
  _dm = RuntimeGlobals::getDelayedDataMgr(true);
  _ls = RuntimeGlobals::getLoadShedder(true);
  _shutdown = false;

  if(pthread_mutex_init(&_shutdown_mtx,NULL))
  {
	assert(0);
	abort();
  }

//  std::cout << "export created" << std::endl;
  if((_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
	perror("Could not make socket");
  }

  srand48(time(NULL));
}


/*************************************************************************
 * Function Name: ExportDelayedData::~ExportDelayedData
 * Parameters: 
 * Effects: 
 *************************************************************************/

ExportDelayedData::~ExportDelayedData()
{
  if(pthread_mutex_destroy(&_shutdown_mtx))
  {
	assert(0);
	abort();
  }

  for(int i=0; i<_connections.size(); i++) {
	delete _connections[i];
  }
}


/*************************************************************************
 * Function Name: ExportDelayedData::start
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::run() throw()
{
  struct sockaddr_in	addr;
  int yes = 1;

  if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
  {
	perror("Socket Option");
  }

  if(fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
  {
	perror("fcntl error");
  }

  /* generate the address struct to bind to */
  addr.sin_family 	= AF_INET;
  addr.sin_port		= htons(_port);
  addr.sin_addr.s_addr	= INADDR_ANY;
  memset(&addr.sin_zero, 0, 8);

  if(bind(_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0)
  {
	perror("Couldn't bind");
  }

  if(listen(_socket, _backlog))
  {
	perror("Couldn't listen");
  }

  while(!_shutdown)
  {
	int new_socket;
	struct sockaddr_in	in_addr;
	int in_addr_size = sizeof(struct sockaddr_in);
//	pthread_t read_thr, write_thr;

	fd_set r_fds;
	struct timeval tv = {2,0};
	
	FD_ZERO(&r_fds);
	FD_SET(_socket, &r_fds);

	if(select(_socket+1, &r_fds, NULL, NULL, &tv) < 0) 
	{
	  cout << "error in select" << endl;
	}

	if(!FD_ISSET(_socket, &r_fds)) {
	  continue;
	}

	if((new_socket = accept(_socket, (struct sockaddr*) &in_addr, (socklen_t*)&in_addr_size)) < 0)
	{
	  cout << "Couldnt accept" << endl;
	  //perror("Couldnt accept");
	  //break;
	  continue;
	}

	if(pthread_mutex_lock(&_shutdown_mtx))
	{
	  cout << "run: mutex_lock" << endl;
	  assert(0);
	  abort();
	}

	if(_shutdown) {
	  if(pthread_mutex_unlock(&_shutdown_mtx))
	  {
		cout << "run: mutex unlock " << __LINE__ << endl;
		assert(0);
		abort();
	  }
	  break;
	}

	/* the same information passed to both string but each needs its own
	 * allocated version */
	thread_data	*tdr = new thread_data;
	thread_data	*tdw = new thread_data;
	connection *nc = new connection;

	nc->sock = new_socket;

	tdr->_edd = tdw->_edd = this; 
	tdr->_connected_socket = tdw->_connected_socket = new_socket;

	_connections.push_back(nc);

	/* what should we do if the thread create or detach fails */
	if((pthread_create(&nc->read_thr, NULL, 
			(void* (*)(void*)) &ExportDelayedData::read_socket, (void*) tdr)))
	{
	  cout << "read thr create failed" << endl;
	  printf("pthread_create failed read: %i\n", __LINE__);
	  if(pthread_mutex_unlock(&_shutdown_mtx))
	  {
		cout << "run: mutex unlock " <<  __LINE__ << endl;
		assert(0);
		abort();
	  }
	  continue;
	}

	/*
	if((pthread_detach(nc->read_thr)))
	{
		cout << "read thr detach failed" << endl;
	  printf("pthread_detach failed read: %i\n", __LINE__);
	}
	*/

	if((pthread_create(&nc->write_thr, NULL, 
			(void* (*)(void*)) &ExportDelayedData::write_socket, (void*) tdw)))
	{
		cout << "write thr create failed" << endl;
	  printf("pthread_create failed write: %i\n", __LINE__);
	  if(pthread_mutex_unlock(&_shutdown_mtx))
	  {
		cout << "run: mutex unlock " <<  __LINE__ << endl;
		assert(0);
		abort();
	  }
	  continue;
	}

	/*
	if((pthread_detach(nc->write_thr)))
	{
		cout << "read thr detach failed" << endl;
	  printf("pthread_detach failed write: %i\n", __LINE__);
	}
	*/

	if(pthread_mutex_unlock(&_shutdown_mtx))
	{
	  cout << "run: mutex unlock " <<  __LINE__ << endl;
	  assert(0);
	  abort();
	}
  }
}


/*************************************************************************
 * Function Name: ExportDelayedData::read_socket
 * Parameters: void *arg
 * Returns: void *
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::shutdown()
{
  if(pthread_mutex_lock(&_shutdown_mtx))
  {
	cout << "shutdown: mutex_lock" << endl;
	assert(0);
	abort();
  }

  if(close(_socket))
  {
	cout << "close failed: " << _socket << endl;
  }

  _shutdown = true;

  connection *c = NULL;
  for(int i=0; i < _connections.size(); i++) {
	c = _connections[i];

	if(close(c->sock))
	{
	  cout << "close failed: " << c->sock << endl;
	}

//	cout << "joining write" << endl;
	if(pthread_join(c->write_thr, NULL))
	{
	  cout << "shutdown: pthread_join write" << endl;
	  assert(0);
	  abort();
	}
//	cout << "joined write" << endl;

//	cout << "joining read" << endl;
	if(pthread_join(c->read_thr, NULL))
	{
	  cout << "shutdown: pthread_join read" << endl;
	  assert(0);
	  abort();
	}
//	cout << "joined read" << endl;
  }

  if(pthread_mutex_unlock(&_shutdown_mtx))
  {
	  cout << "shutdown: mutex_unlock" << endl;
	assert(0);
	abort();
  }
}



/*************************************************************************
 * Function Name: ExportDelayedData::read_socket
 * Parameters: void *arg
 * Returns: void *
 * Effects: 
 *************************************************************************/
void *
ExportDelayedData::read_socket(void *arg)
{
  thread_data *td = (thread_data*) arg;
  int bytes_in;
  server_data_t	in;

  while(true)
  {

	bytes_in = 0;

	fd_set r_fds;
	struct timeval tv = {1,0};

	FD_ZERO(&r_fds);
	FD_SET(td->_connected_socket, &r_fds);

	if(select(td->_connected_socket+1, &r_fds, NULL, NULL, &tv) < 0) 
	{
	  break;
	}

	if(!FD_ISSET(td->_connected_socket, &r_fds)) {
	  continue;
	}

	/* make sure you get all the data */
	do {
	  bytes_in += recv(td->_connected_socket, (((char*)(&in))+bytes_in), (sizeof(in)-bytes_in), 0);
	  if(bytes_in <= 0) {
//		perror("recv:");
		break;
	  }

	} while(bytes_in < sizeof(server_data_t));
	
	/* if hte bytes_in values is zero then the connection was closed */
	if(bytes_in <= 0)
	  break;

	switch(in._type)
	{
	  case	LOADSHEDDER_VALUE:
	    //cout << "should be here" << endl;
		td->_edd->read_load_shedder_value(td->_connected_socket);
		break;

	  default:
		break;
	}
  }

  /* storage for the thread_data is allocated by the parent thread, but we were
   * detached, so we need to free it */
  //cout << "Never here...sort of" << endl;
  delete td;
  return NULL;
}


/*************************************************************************
 * Function Name: ExportDelayedData::write_socket
 * Parameters: void *arg
 * Returns: void *
 * Effects: 
 *************************************************************************/
void *
ExportDelayedData::write_socket(void *arg)
{
  thread_data *td = (thread_data*) arg;
	cout << "write thread created" << endl;

  sigignore(SIGPIPE);
  while(1)
  {
	/* this will be replaced by blocking waiting for the stats object to call on
	 * us */
	sleep(1);
//	td->_edd->_dm->populateStatsImage();

	StatsImage *si = td->_edd->_dm->getReadableImage();
	//cout << "exporter: got obj, going to send" << endl;

	/*
	if(!si->_boxesStats.empty())
	{
	  td->_edd->send_boxes_stats(td->_connected_socket, si->_boxesStats);
	} 
	*/

	/*
	if(!si->_inputArcsStats.empty())
	{
	  td->_edd->send_input_arcs(td->_connected_socket, si->_inputArcsStats);
	}
	*/

	//cout << "EDD DUMP: " << si->toString() << endl << endl;

	if(!si->_appArcsStats.empty())
	{
	  td->_edd->send_app_arcs(td->_connected_socket, si->_appArcsStats);
	}

	if(!si->_numTuplesOnArcs.empty())
	{
	  td->_edd->send_num_tuples(td->_connected_socket, si->_numTuplesOnArcs);
	}

	//cout << "@@@@@ si->_utilityVsLsPrediction.size() = " << si->_utilityVsLsPrediction.size() << endl;
	if(!si->_utilityVsLsPrediction.empty())
	{
	  td->_edd->send_load_shedder(td->_connected_socket, si->_utilityVsLsPrediction);
	}

	// BEGIN tatbul@cs.brown.edu
	// dropsOnArcs must be sent even if it is empty. Otherwise, there is
	// no way to show on the curve that the load shedding level is set to 0.
	// So, we don't need to check the condition for emptiness.
	//
	//if(!si->_dropsOnArcs.empty())
	//  {
	//    td->_edd->send_drops(td->_connected_socket, si->_dropsOnArcs);
	//  }
	td->_edd->send_drops(td->_connected_socket, si->_dropsOnArcs);
	//
	// END tatbul@cs.brown.edu

	td->_edd->_dm->releaseReadableImage(si);

	const PropsFile *pProps = RuntimeGlobals::getPropsFile();
	string catalog = pProps->getString("CatalogMgr.directory");

	// BEGIN tatbul@cs.brow.edu
	// The below line is replaced to resolve the problem with catalog
	// string. The string wasn't ending with a NULL character and this 
	// was causing a problem at the reader side.
	//
	//td->_edd->send_query_name(td->_connected_socket, catalog.c_str(), catalog.length());
	char buffer[catalog.length()+1];
	char *ptr = buffer;
	memcpy(ptr, catalog.c_str(), catalog.length());
	ptr += catalog.length();
	ptr[0] = '\0';
	td->_edd->send_query_name(td->_connected_socket, buffer, sizeof(buffer));
	//
	// END tatbul@cs.brow.edu

	//td->_edd->_dm->printStatsImage();
  }

  /* storage for the thread_data is allocated by the parent thread, but we were
   * detached, so we need to free it */
  delete td;
  return NULL;
}


/*************************************************************************
 * Function Name: ExportDelayedData::send_boxes_stats
 * Parameters: int, map<int, BoxStats> &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::send_boxes_stats(int socket, const map<int, BoxStats> &boxes)
{
  data_packet_t dp;
  dp._type = BOXES_STATS;

  full_send(socket, (char*) &dp, sizeof(data_packet_t), 0);

  map_packet_t mp;
  mp._key_size = sizeof(boxes.begin()->first);
  mp._entry_size = sizeof(boxes.begin()->second);
  mp._num_entries = boxes.size();

  full_send(socket, (char*) &mp, sizeof(map_packet_t), 0);

  char buffer[(mp._key_size + mp._entry_size) * mp._num_entries];

  char *ptr = buffer;

  printf("DEBUG1 SENDING boxe count %d real size %d\n", mp._num_entries, boxes.size()  );
  for(map<int,BoxStats>::const_iterator i = boxes.begin();
	  i != boxes.end();
	  i++ )
  {
	/* copy in the key */
	memcpy(ptr, (char*) &(i->first), mp._key_size);

	/* increment the pointer by the key size */
	ptr += mp._key_size;

	/* copy in the data */
	memcpy(ptr, (char*) &i->second, mp._entry_size);

	/* increment the pointer past the entry */
	ptr += mp._entry_size;
  }

  full_send(socket, buffer, sizeof(buffer), 0);
}


/*************************************************************************
 * Function Name: ExportDelayedData::send_input_arcs
 * Parameters: int, map<int, InputArcStats> &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::send_input_arcs(int socket, const map<int, InputArcStats> &inputs)
{
  data_packet_t dp;
  dp._type = INPUT_ARCS;

  full_send(socket, (char*) &dp, sizeof(data_packet_t), 0);

  map_packet_t mp;
  mp._key_size = sizeof(inputs.begin()->first);
  mp._entry_size = sizeof(inputs.begin()->second);
  mp._num_entries = inputs.size();

  full_send(socket, (char*) &mp, sizeof(map_packet_t), 0);

  char buffer[(mp._key_size + mp._entry_size) * mp._num_entries];

  char *ptr = buffer;

  for(map<int,InputArcStats>::const_iterator i = inputs.begin();
	  i != inputs.end();
	  i++ )
  {
	/* copy in the key */
	memcpy(ptr, (char*) &(i->first), mp._key_size);

	/* increment the pointer by the key size */
	ptr += mp._key_size;

	/* copy in the data */
	memcpy(ptr, (char*) &i->second, mp._entry_size);

	/* increment the pointer past the entry */
	ptr += mp._entry_size;
  }

  full_send(socket, buffer, sizeof(buffer), 0);
}


/*************************************************************************
 * Function Name: ExportDelayedData::send_app_arcs
 * Parameters: int, map<int, AppArcStats> &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::send_app_arcs(int socket, const map<int, AppArcStats> &apps)
{
  data_packet_t dp;
  dp._type = OUTPUT_ARCS;

  full_send(socket, (char*) &dp, sizeof(data_packet_t), 0);

  map_packet_t mp;
  mp._key_size = sizeof(apps.begin()->first);
  mp._entry_size = sizeof(apps.begin()->second);
  mp._num_entries = apps.size();

  full_send(socket, (char*) &mp, sizeof(map_packet_t), 0);

  char buffer[(mp._key_size + mp._entry_size) * mp._num_entries];

  char *ptr = buffer;

  for(map<int,AppArcStats>::const_iterator i = apps.begin();
	  i != apps.end();
	  i++ )
  {
	/* copy in the key */
	memcpy(ptr, (char*) &(i->first), mp._key_size);

	/* increment the pointer by the key size */
	ptr += mp._key_size;

	/* copy in the data */
	memcpy(ptr, (char*) &i->second, mp._entry_size);

	/* increment the pointer past the entry */
	ptr += mp._entry_size;
  }

  full_send(socket, buffer, sizeof(buffer), 0);
}


/*************************************************************************
 * Function Name: ExportDelayedData::send_app_arcs
 * Parameters: int, map<int, AppArcStats> &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::send_num_tuples(int socket, const map<int, int> &num_tuples)
{
  data_packet_t dp;
  dp._type = NUM_TUPLES;

  full_send(socket, (char*) &dp, sizeof(data_packet_t), 0);

  map_packet_t mp;
  mp._key_size = sizeof(num_tuples.begin()->first);
  mp._entry_size = sizeof(num_tuples.begin()->second);
  mp._num_entries = num_tuples.size();


  //cout << "##### mp._num_entries = " << mp._num_entries << endl;

  full_send(socket, (char*) &mp, sizeof(map_packet_t), 0);

  char buffer[(mp._key_size + mp._entry_size) * mp._num_entries];

  char *ptr = buffer;

  for(map<int,int>::const_iterator i = num_tuples.begin();
	  i != num_tuples.end();
	  i++ )
  {
	/* copy in the key */
	memcpy(ptr, (char*) &(i->first), mp._key_size);

	/* increment the pointer by the key size */
	ptr += mp._key_size;

	/* copy in the data */
	memcpy(ptr, (char*) &i->second, mp._entry_size);

	/* increment the pointer past the entry */
	ptr += mp._entry_size;
  }

  full_send(socket, buffer, sizeof(buffer), 0);
}


/*************************************************************************
 * Function Name: ExportDelayedData::send_app_arcs
 * Parameters: int, map<int, AppArcStats> &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::send_drops(int socket, const map<int, string> &drops)
{
  data_packet_t dp;
  dp._type = DROPS_ON_ARCS;

  full_send(socket, (char*) &dp, sizeof(data_packet_t), 0);

  map_packet_t mp;
  mp._key_size = sizeof(drops.begin()->first);
  mp._entry_size = 0;
  mp._num_entries = drops.size();

  full_send(socket, (char*) &mp, sizeof(map_packet_t), 0);



  for(map<int,string>::const_iterator i = drops.begin();
	  i != drops.end();
	  i++ )
  {
	char buffer[mp._key_size + sizeof(map_element_t) + i->second.length() + 1];
	char *ptr = buffer;

	/* copy in the key */
	memcpy(ptr, (char*) &(i->first), mp._key_size);

	/* increment the pointer by the key size */
	ptr += mp._key_size;

	/* set the size of the element */
	((map_element_t*) ptr)->_element_size = (i->second.length() + 1);

	/* incr the ptr by the map element size */
	ptr += sizeof(map_element_t);

	/* copy in the data */
	memcpy(ptr, i->second.c_str(), i->second.length());

	/* increment the pointer past the entry */
	ptr += i->second.length();
	ptr[0] = '\0';

	//cout << "()()()()()()() string size " << i->second.length() << endl;
	full_send(socket, buffer, sizeof(buffer), 0);
  }
}

/*************************************************************************
 * Function Name: ExportDelayedData::send_load_shedder
 * Parameters: int, LoadShedderCurve &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::send_load_shedder(int socket, const LoadShedderCurve &curve)
{
  data_packet_t dp;
  dp._type = LOADSHEDDER_CURVE;

  full_send(socket, (char*) &dp, sizeof(data_packet_t), 0);

  vector_packet_t vp;
  vp._type_size = sizeof(curve[0]);
  vp._num_entries = curve.size();

  full_send(socket, (char*) &vp, sizeof(vector_packet_t), 0);

  char buffer[vp._type_size * vp._num_entries];
  memcpy(buffer, &curve[0], vp._type_size * vp._num_entries);

  full_send(socket, buffer, sizeof(buffer), 0);
}


/*************************************************************************
 * Function Name: ExportDelayedData::send_load_shedder
 * Parameters: int, LoadShedderCurve &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::send_query_name(int socket, const char* str, int len)
{
  data_packet_t dp;
  dp._type = STR_TYPE;

  full_send(socket, (char*) &dp, sizeof(data_packet_t), 0);

  str_packet_t sp;
  sp._info = QUERY_NAME;
  sp._len = len;

  full_send(socket, (char*) &sp, sizeof(str_packet_t), 0);
  full_send(socket, str, len, 0);
}



/*************************************************************************
 * Function Name: ExportDelayedData::send_load_shedder
 * Parameters: int, LoadShedderCurve &
 * Returns: void
 * Effects: 
 *************************************************************************/
int
ExportDelayedData::full_send(int socket, const char *buf, int size, int flags)
{

	//cout << "|||||| full_send: size = " << size << endl;

  int bytes_sent = 0;

  do {
	bytes_sent = send(socket, (buf+bytes_sent), (size-bytes_sent), flags);

	if(bytes_sent < 0) {
	  //cout << "send close" << endl;
	  pthread_exit(NULL);
	  break;
	}

  } while(bytes_sent < size);

  return bytes_sent;
}



/*************************************************************************
 * Function Name: ExportDelayedData::send_load_shedder
 * Parameters: int, LoadShedderCurve &
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ExportDelayedData::read_load_shedder_value(int socket)
{
  load_shedder_t ls_pkt;
  int bytes_in = 0;

  do {
	bytes_in += recv(socket, ((&ls_pkt)+bytes_in), (sizeof(ls_pkt)-bytes_in), 0);
  } while(bytes_in < sizeof(ls_pkt));

  //cout << "***** Setting Load Shedding to: " << ls_pkt._value << endl;
    _ls->setUtilityLevel(ls_pkt._value);
}

