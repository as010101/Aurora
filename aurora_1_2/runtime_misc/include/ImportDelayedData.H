/**************************************************************
*	Name: Akash Hasmukh Parikh (ahp)
*	File: ImportDelayedData.h
*	Asgn: perfmon
*	Date: Wed Apr 23 03:43:42 EDT 2003
**************************************************************/

#ifndef __ImportDelayedData_Header__
#define __ImportDelayedData_Header__

#include <string>
#include "DataManager.H"
#include <pthread.h>

using namespace std;

class ImportDelayedData {

public:

  ImportDelayedData(short port, char *host, DataManager *dm);
  virtual ~ImportDelayedData();

  void	setDataManager(DataManager *dm);
  void	send_load_shedder_value(float value);
  void	start();
  void	stop();

private:
  static void*	read_socket(void *arg);
  int	full_send(int socket, char *buf, int size, int flags);

  void	read_boxes_stats(int, map<int, BoxStats> &);
  void	read_input_arcs(int, map<int, InputArcStats> &);
  void	read_app_arcs(int, map<int, AppArcStats> &);
  void	read_num_tuples(int, map<int, int> &);
  void	read_drops(int, map<int, string> &);
  void	read_load_shedder(int, LoadShedderCurve &);
  string	read_query_network(int);

  typedef struct {
	ImportDelayedData	*_idd;
  } thread_data;

  int			_socket;
  short			_port;
  std::string	_host;
  DataManager	*_dm;
  pthread_t		_recv_thr;

protected:
};

#endif
