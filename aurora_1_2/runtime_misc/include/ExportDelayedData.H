/**************************************************************
*	Name: Akash Hasmukh Parikh (ahp)
*	File: ExportDelayedData.h
*	Asgn: perfmon
*	Date: Wed Apr 23 02:23:17 EDT 2003
**************************************************************/

#ifndef __ExportDelayedData_Header__
#define __ExportDelayedData_Header__

#include <Runnable.H>
//#include "DataManager.H"
//#include <DelayedDataMgr.H>
#include <DelayedDataMgr.H>
#include <LoadShedder.H>
#include <vector>
#include <pthread.h>

class ExportDelayedData : public Runnable {

public:

  ExportDelayedData(short port, int backlog);
  virtual ~ExportDelayedData();

  void	run() throw();
  void	shutdown();

private:
  static void*	read_socket(void *arg);
  static void*	write_socket(void *arg);

  int	full_send(int socket, const char *buf, int size, int flags);

  void	send_boxes_stats(int, const map<int, BoxStats> &);
  void	send_input_arcs(int, const map<int, InputArcStats> &);
  void	send_app_arcs(int, const map<int, AppArcStats> &);
  void	send_num_tuples(int, const map<int, int> &);
  void	send_drops(int, const map<int, string> &);
  void	send_load_shedder(int, const LoadShedderCurve &);
  void	send_query_name(int, const char*, int);

  void	read_load_shedder_value(int socket);

  typedef struct {
	int					_connected_socket;
	ExportDelayedData	*_edd;
  } thread_data;

  typedef struct {
	pthread_t	read_thr;
	pthread_t	write_thr;
	int			sock;
  } connection;


  int	_socket;
  short	_port;
  int	_backlog;
  bool	_shutdown;
//  DataManager 	*_dm;
  DelayedDataMgr	*_dm;
  LoadShedder		*_ls;
  vector<connection*>	_connections;
  pthread_mutex_t	_shutdown_mtx;

protected:


};

#endif
