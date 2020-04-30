/*************************************************************************
 *    NAME: Akash Hasmukh Parikh
 *    USER: ahp
 *    FILE: ImportDelayedData.cpp
 *    DATE: Fri Apr 25  0:36:11 2003
 *************************************************************************/
#include "ImportDelayedData.H"
#include "NetworkData.H"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <netdb.h>
#include <stdio.h>
#include <assert.h>
#include <signal.h>


#include <vector.h>
#include <map.h>

/*************************************************************************
 * Function Name: ImportDelayedData::ImportDelayedData
 * Parameters: short port
 * Effects: 
 *************************************************************************/

ImportDelayedData::ImportDelayedData(short port, char *host, DataManager *dm)
{
  _port = port;
  _host = std::string(host);
  _dm = dm;

  //cout << "import created" << endl;

  if((_socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
  {
	perror("Could not make socket");
  }

  sigignore(SIGPIPE);
}


/*************************************************************************
 * Function Name: ImportDelayedData::~ImportDelayedData
 * Parameters: 
 * Effects: 
 *************************************************************************/

ImportDelayedData::~ImportDelayedData()
{
}


/*************************************************************************
 * Function Name: ImportDelayedData::~ImportDelayedData
 * Parameters: 
 * Effects: 
 *************************************************************************/

void
ImportDelayedData::setDataManager(DataManager *dm)
{
  _dm = dm;
}


/*************************************************************************
 * Function Name: ImportDelayedData::start
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::start()
{
  struct hostent *he = NULL;
  
  if( NULL == (he = gethostbyname(_host.c_str())))
  {
	herror("gethostbyname");
	exit(1);
  }


  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(_port);
  addr.sin_addr = *((struct in_addr*)he->h_addr);
  memset(&addr.sin_zero, 0, 8);

  if(connect(_socket, (struct sockaddr*) &addr, sizeof(struct sockaddr)) < 0)
  {
	perror("connect");
	exit(1);
  }

  /* the same information passed to both string but each needs its own
   * allocated version */
  thread_data	*td = new thread_data;
  td->_idd = this; 

  /* what should we do if the thread create or detach fails */
  if((pthread_create(&_recv_thr, NULL, 
		  (void* (*)(void*)) &ImportDelayedData::read_socket, (void*) td)))
  {
	printf("pthread_create failed to start read: %i\n", __LINE__);
	exit(1);
  }
	//cout << "import pthread_create success" << endl;
  if(pthread_detach(_recv_thr))
  {
	printf("pthread_detach failed to detach");
	exit(1);
  }
	//cout << "import pthread_detach success" << endl;
}


/*************************************************************************
 * Function Name: ImportDelayedData::start
 * Parameters: 
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::stop()
{
  close(_socket);
  pthread_join(_recv_thr, NULL);
}


/*************************************************************************
 * Function Name: ImportDelayedData::read_socket
 * Parameters: void *arg
 * Returns: void*
 * Effects: 
 *************************************************************************/
void*
ImportDelayedData::read_socket(void *arg)
{
  thread_data *td = (thread_data*) arg;

  data_packet_t	*dp_ptr = NULL;
  char buf[sizeof(data_packet_t)] = {0};
  int bytes_in;

  while(1)
  {
	  //cout << "import read thr created" << endl;
	bytes_in = 0;

	/* make sure we receive a whole data_packet_t */
	do {
	  bytes_in += recv(td->_idd->_socket, (buf+bytes_in), (sizeof(buf)-bytes_in), 0);

	  if(bytes_in <= 0)
		pthread_exit(NULL);

	} while(bytes_in < sizeof(data_packet_t));

	dp_ptr = (data_packet_t*) buf;

	/* get the stats object so that we can write into it */
	StatsImage &si = td->_idd->_dm->writeStatsImage();
	//cout << "import reading" << endl;
	
//	printf("switching on: %x\n", dp_ptr->_type);
	switch(dp_ptr->_type)
	{
	  case	BOXES_STATS:
		td->_idd->read_boxes_stats(td->_idd->_socket, si._boxesStats);
		break;

	  case	INPUT_ARCS:
		td->_idd->read_input_arcs(td->_idd->_socket, si._inputArcsStats);
		break;

	  case OUTPUT_ARCS:
		td->_idd->read_app_arcs(td->_idd->_socket, si._appArcsStats);
		break;

	  case NUM_TUPLES:
		td->_idd->read_num_tuples(td->_idd->_socket, si._numTuplesOnArcs);
		break;

	  case LOADSHEDDER_CURVE:
		td->_idd->read_load_shedder(td->_idd->_socket, si._utilityVsLsPrediction);
		break;

	  case DROPS_ON_ARCS:
		td->_idd->read_drops(td->_idd->_socket, si._dropsOnArcs);
		break;

	  case STR_TYPE:
		td->_idd->_dm->setQueryNetwork(td->_idd->read_query_network(td->_idd->_socket));
		td->_idd->_dm->updateHasData();
		break;

	  default:
		break;
	}

	/* release the lock so the gui can get it */
	td->_idd->_dm->releaseStatsImage();
//	td->_idd->_dm->printStatsImage();
  }

  delete td;
  return NULL;
}


/*************************************************************************
 * Function Name: ImportDelayedData::write_socket
 * Parameters: void *arg
 * Returns: void*
 * Effects: 
 *************************************************************************/
int
ImportDelayedData::full_send(int socket, char *buf, int size, int flags)
{
  int bytes_sent = 0;

  do {
	bytes_sent = send(socket, (buf+bytes_sent), (size-bytes_sent), flags);
	if(bytes_sent < 0) {
	  break;
	}
  } while(bytes_sent < size);

  return bytes_sent;
}


/*************************************************************************
 * Function Name: ImportDelayedData::read_boxes_stats
 * Parameters: int socket, map<int,BoxStats> &boxStats
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::read_boxes_stats(int socket, map<int, BoxStats> &boxesStats)
{
  /* first lets get all the data for the map and then put it in */
  map_packet_t	pkt_hdr;
  int	bytes_in = 0;

  /* make sure we get the full header */
  do {
	bytes_in += recv(socket, (char*) (((char*)(&pkt_hdr))+bytes_in), (sizeof(pkt_hdr)-bytes_in), 0);
  } while(bytes_in < sizeof(map_packet_t));

  char buffer[((pkt_hdr._key_size+pkt_hdr._entry_size) * pkt_hdr._num_entries)];

  /* read in the entire map */
  bytes_in = 0;
  do {
	bytes_in += recv(socket, (buffer+bytes_in), (sizeof(buffer)-bytes_in), 0);
  } while(bytes_in < sizeof(buffer));

  boxesStats.clear();

  char *start = buffer;
  for(int i=0; i<pkt_hdr._num_entries; i++)
  {
	/* make the curretn location in the buffer a pointer to an int and
	 * dereference it */
	int key = *((int*) start);

	/* increment your buffer pointer by the key size */
	start += pkt_hdr._key_size;

	/* make the current location in the buffer a pointer to BoxStats and
	 * dereference it */
	BoxStats elem = *((BoxStats*) start);
	start += pkt_hdr._entry_size;

	boxesStats[key] = elem;
  }
}


/*************************************************************************
 * Function Name: ImportDelayedData::read_input_arcs
 * Parameters: int socket, map<int, InputArcsStats> &inputArcs
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::read_drops(int socket, map<int,string> &drops)
{
  /* first lets get all the data for the map and then put it in */
  map_packet_t	pkt_hdr;
  int	bytes_in = 0;

  /* make sure we get the full header */
  do {
	bytes_in += recv(socket, (char*) (((char*)(&pkt_hdr))+bytes_in), (sizeof(pkt_hdr)-bytes_in), 0);
  } while(bytes_in < sizeof(map_packet_t));

  drops.clear();

  char buffer[pkt_hdr._key_size+sizeof(map_element_t)];

  for(int i=0; i<pkt_hdr._num_entries; i++)
  {
	char *start = buffer;

	/* read entry of map */
	bytes_in = 0;
	do {
	  bytes_in += recv(socket, (buffer+bytes_in), (sizeof(buffer)-bytes_in), 0);
	} while(bytes_in < sizeof(buffer));

	/* make the curretn location in the buffer a pointer to an int and
	 * dereference it */
	int key = *((int*) start);

	/* increment your buffer pointer by the key size */
	start += pkt_hdr._key_size;

	//cout << "(map_element_t*) start)->_element_size" << ((map_element_t*) start)->_element_size << endl;
	char str[((map_element_t*) start)->_element_size];
	bytes_in = 0;
	do {
	  bytes_in += recv(socket, (str+bytes_in), (sizeof(str)-bytes_in), 0);
	  //cout << "in loop: " << bytes_in << endl;
	} while(bytes_in < sizeof(str));

	/* make the current location in the buffer a pointer to BoxStats and
	 * dereference it */
	string elem = string((char*) str);

	drops[key] = elem;
  }
}
/*************************************************************************
 * Function Name: ImportDelayedData::read_input_arcs
 * Parameters: int socket, map<int, InputArcsStats> &inputArcs
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::read_input_arcs(int socket, map<int,InputArcStats> &inputArcs)
{
  /* first lets get all the data for the map and then put it in */
  map_packet_t	pkt_hdr;
  int	bytes_in = 0;

  /* make sure we get the full header */
  do {
	bytes_in += recv(socket, (char*) (((char*)(&pkt_hdr))+bytes_in), (sizeof(pkt_hdr)-bytes_in), 0);
  } while(bytes_in < sizeof(map_packet_t));

  char buffer[((pkt_hdr._key_size+pkt_hdr._entry_size) * pkt_hdr._num_entries)];

  /* read in the entire map */
  bytes_in = 0;
  do {
	bytes_in += recv(socket, (buffer+bytes_in), (sizeof(buffer)-bytes_in), 0);
  } while(bytes_in < sizeof(buffer));

  inputArcs.clear();

  char *start = buffer;
  for(int i=0; i<pkt_hdr._num_entries; i++)
  {
	/* make the curretn location in the buffer a pointer to an int and
	 * dereference it */
	int key = *((int*) start);

	/* increment your buffer pointer by the key size */
	start += pkt_hdr._key_size;

	/* make the current location in the buffer a pointer to BoxStats and
	 * dereference it */
	InputArcStats elem = *((InputArcStats*) start);
	start += pkt_hdr._entry_size;

	inputArcs[key] = elem;
  }
}


/*************************************************************************
 * Function Name: ImportDelayedData::read_app_arcs
 * Parameters: int socket, map<int, AppArcStats> &appArcs
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::read_app_arcs(int socket, map<int, AppArcStats> &appArcs)
{
  /* first lets get all the data for the map and then put it in */
  map_packet_t	pkt_hdr;
  int	bytes_in = 0;

  /* make sure we get the full header */
  do {
	bytes_in += recv(socket, (char*) (((char*)(&pkt_hdr))+bytes_in), (sizeof(pkt_hdr)-bytes_in), 0);
  } while(bytes_in < sizeof(map_packet_t));

  char buffer[((pkt_hdr._key_size+pkt_hdr._entry_size) * pkt_hdr._num_entries)];

  /* read in the entire map */
  bytes_in = 0;
  do {
	bytes_in += recv(socket, (buffer+bytes_in), (sizeof(buffer)-bytes_in), 0);
  } while(bytes_in < sizeof(buffer));

  appArcs.clear();

  char *start = buffer;

  for(int i=0; i<pkt_hdr._num_entries; i++)
  {
	/* make the curretn location in the buffer a pointer to an int and
	 * dereference it */
	int key = *((int*) start);

	/* increment your buffer pointer by the key size */
	start += pkt_hdr._key_size;

	/* make the current location in the buffer a pointer to BoxStats and
	 * dereference it */
	AppArcStats elem = *((AppArcStats*) start);
	start += pkt_hdr._entry_size;

	appArcs[key] = elem;
  }
}


/*************************************************************************
 * Function Name: ImportDelayedData::read_app_arcs
 * Parameters: int socket, map<int, AppArcStats> &appArcs
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::read_num_tuples(int socket, map<int, int> &num_tuples)
{
  /* first lets get all the data for the map and then put it in */
  map_packet_t	pkt_hdr;
  int	bytes_in = 0;

  /* make sure we get the full header */
  do {
	bytes_in += recv(socket,(char*) (((char*)(&pkt_hdr))+bytes_in), (sizeof(pkt_hdr)-bytes_in), 0);
  } while(bytes_in < sizeof(map_packet_t));

  char buffer[((pkt_hdr._key_size+pkt_hdr._entry_size) * pkt_hdr._num_entries)];

  /* read in the entire map */
  bytes_in = 0;
  do {
	bytes_in += recv(socket, (buffer+bytes_in), (sizeof(buffer)-bytes_in), 0);
  } while(bytes_in < sizeof(buffer));

  num_tuples.clear();

  char *start = buffer;
  for(int i=0; i<pkt_hdr._num_entries; i++)
  {
	/* make the curretn location in the buffer a pointer to an int and
	 * dereference it */
	int key = *((int*) start);

	/* increment your buffer pointer by the key size */
	start += pkt_hdr._key_size;

	/* make the current location in the buffer a pointer to BoxStats and
	 * dereference it */
	int elem = *((int*) start);
	start += pkt_hdr._entry_size;

	num_tuples[key] = elem;
  }
}


/*************************************************************************
 * Function Name: ImportDelayedData::read_load_shedder
 * Parameters: int socket , LoadShedderCurve &curve
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::read_load_shedder(int socket, LoadShedderCurve &curve)
{
  /* first lets get all the vector information */
  vector_packet_t pkt_hdr;
  int	bytes_in = 0;

  /* make sure we get the full header */
  do {
	bytes_in += recv(socket, (char*) (((char*)(&pkt_hdr))+bytes_in), (sizeof(pkt_hdr)-bytes_in), 0);
  } while(bytes_in < sizeof(vector_packet_t));

  char buffer[(pkt_hdr._type_size * pkt_hdr._num_entries)];

  /* read in the entire vector */
  bytes_in = 0;
  do {
	bytes_in += recv(socket, (buffer+bytes_in), (sizeof(buffer)-bytes_in), 0);
  } while(bytes_in < sizeof(buffer));

  curve.clear();
  char *start = buffer;

  for( int i = 0; i<pkt_hdr._num_entries; i++)
  {
	curve.push_back( *((double*) start));
	start += pkt_hdr._type_size;
  }

}


/*************************************************************************
 * Function Name: ImportDelayedData::read_load_shedder
 * Parameters: int socket , LoadShedderCurve &curve
 * Returns: void
 * Effects: 
 *************************************************************************/
string
ImportDelayedData::read_query_network(int socket)
{
  str_packet_t pkt_hdr;
  int	bytes_in = 0;

  do {
	bytes_in += recv(socket, (char*) (((char*)&pkt_hdr)+bytes_in), (sizeof(pkt_hdr)-bytes_in), 0);
  } while(bytes_in < sizeof(str_packet_t));

  char buffer[pkt_hdr._len];

  bytes_in = 0;
  do {
	bytes_in += recv(socket, (buffer+bytes_in), (sizeof(buffer)-bytes_in), 0);
  } while(bytes_in < sizeof(buffer));

  return string(buffer);
}


/*************************************************************************
 * Function Name: ImportDelayedData::send_load_shedder_value
 * Parameters: float value
 * Returns: void
 * Effects: 
 *************************************************************************/
void
ImportDelayedData::send_load_shedder_value(float value)
{
  char buffer[sizeof(server_data_t) + sizeof(load_shedder_t)];

  ((server_data_t*) buffer)->_type = LOADSHEDDER_VALUE;
  ((load_shedder_t*) ((server_data_t*) buffer)->_data)->_value = value;

  //cout << "idd send load shedder" << endl;
  full_send(_socket, buffer, sizeof(buffer), 0);
}


