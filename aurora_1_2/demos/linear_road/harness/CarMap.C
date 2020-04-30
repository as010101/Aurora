#include "CarMap.H"
#include <sys/time.h>
#include <iomanip>
#include <iostream>

#define MILLION (1000000)

namespace linear_road{

  CarMap::CarMap()
  {
    cerr << "Creating carmap." << endl;
    mutex = new pthread_mutex_t;
    cars = new map<int, struct timeval *>;
    pthread_mutex_init(mutex, NULL);
    most_recent_receive_second = -1;
    most_recent_send_second = -1;

    logf.open("LRStats.out", ios::out | ios::trunc);
    all_cars.open("LRall_cars.out", ios::out | ios::trunc);
    if (!logf || !all_cars) {
      cerr << "Can not open file." << endl;
      exit(1);
    }
    logf << "sec\tlag\trec'd\twaiting\tlate" << endl;
  }

  CarMap::~CarMap() {
    pthread_mutex_destroy(mutex);
    delete mutex;
    delete cars;
    logf.close();
    all_cars.close();
  }
    
  // Register a car sending its data.
  void CarMap::carSends(int carid) {
    pthread_mutex_lock(mutex);
    struct timeval * tv = (*cars)[carid];
    if (tv == NULL) {
      tv = new struct timeval;
      (*cars)[carid] = tv;
    } else {
      if (tv->tv_sec != 0 || tv->tv_usec != 0) {
	// We haven't dequeued a previous send.
	if (getenv("DEBUG")) {
	  cerr << "Got a new send from carid " << carid << " without a receive." << endl;
	}
      }
    }
    gettimeofday(tv, NULL);
    all_cars << (most_recent_send_second - first_second) << ": "
	     << carid << " sent." << endl;

    if (most_recent_send_second == tv->tv_sec) {
      send_count++;
    } else {
      if (most_recent_send_second != -1) {
	cout << "Second " << (most_recent_send_second - first_second)
	     << ": Send " << send_count << " cars." << endl;
      } else {
	first_second = tv->tv_sec;
      }
      // Reset counters.
      most_recent_send_second = tv->tv_sec;
      send_count = 1;
    }

    pthread_mutex_unlock(mutex);
  }


  // Register a car recieving a toll notification.
  void CarMap::carGets(int carid){
    pthread_mutex_lock(mutex);
    struct timeval *old_tv = (*cars)[carid];
    if (old_tv == NULL) {
      cerr << "Got toll notification for carid " << carid
	   << " with no input." << endl;
      return;
    }

    struct timeval tv;
    struct timeval *new_tv = &tv;
    gettimeofday(new_tv, NULL);

    // Calculate.
    long usecs = ((new_tv->tv_usec - old_tv->tv_usec) +
		  MILLION * (new_tv->tv_sec - old_tv->tv_sec));

    all_cars << (most_recent_receive_second - first_second) << ": "
	     << carid << " received. " << usecs << " usecs." << endl;

    if (most_recent_receive_second == new_tv->tv_sec) {
      if (usecs < 0) {
	cerr << "carid " << carid << " took " << usecs << " usecs" << endl;
      }
      sum_of_delay_msec += usecs/1000;
      receive_count++;
    } else {
      if (most_recent_receive_second != -1) {
	// New second. Output.
	double avg_delay = (double)sum_of_delay_msec / (double)(receive_count * 1000);
	cout << "Second " << (most_recent_receive_second - first_second)
	     << ": " << receive_count << " cars with average lag "
	     << setprecision(4) << avg_delay << " seconds." << endl;
	{
	  // Find the message that has been outstanding for the longest.
	  int outstanding_messages = 0;
	  int late_messages = 0;
	  int oldest_carid;
	  struct timeval *oldest_tv = NULL;
	  map<int, struct timeval *>::iterator i;
	  for (i = cars->begin(); i != cars->end(); i++) {
	    struct timeval *tmp_tv = (*i).second;
	    if (tmp_tv->tv_sec != 0) {
	      outstanding_messages++;
	      if (oldest_tv == NULL || tmp_tv->tv_sec < oldest_tv->tv_sec) {
		// Older timeval.
		oldest_tv = tmp_tv;
		oldest_carid = (*i).first;
	      }
	      if ((tmp_tv->tv_sec +16) < new_tv->tv_sec) {
		late_messages++;
	      }
	    }
	  }
	  cout << "Messages outstanding: " << outstanding_messages << endl;
	  cout << "Messages more than 15 seconds outstanding: " << late_messages << endl;
	  cout << "Oldest outstanding car is " << oldest_carid << ", "
	       << (new_tv->tv_sec - oldest_tv->tv_sec) << " seconds old."
	       << endl;
	  logf << (most_recent_receive_second - first_second) << "\t"
	       << setprecision(4) << avg_delay << "\t"
	       << receive_count << "\t"
	       << outstanding_messages << "\t"
	       << late_messages << endl;
	}
      }
      // Reset counters.
      most_recent_receive_second = new_tv->tv_sec;
      sum_of_delay_msec = usecs/1000;
      receive_count = 1;
    }
    // Zero the tv structure.
    old_tv->tv_sec = 0;
    old_tv->tv_usec = 0;
    pthread_mutex_unlock(mutex);
  }
}



