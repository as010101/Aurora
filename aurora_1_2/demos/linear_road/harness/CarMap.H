#ifndef _CAR_MAP_H_
#define _CAR_MAP_H_

#include <pthread.h>
#include <map>
#include <iostream>
#include <fstream>

using namespace std;
namespace linear_road{

  class CarMap
  {
  public:
    CarMap();
    ~CarMap();
    
    // Register a car sending its data.
    void carSends(int carid);
    // Register a car recieving a toll notification.
    void carGets(int carid);
    
  private:
    map<int, struct timeval *> *cars;
    pthread_mutex_t *mutex;
    int first_second;
    // Send variable
    int  most_recent_send_second;
    int send_count;
    // Receive variables.
    int most_recent_receive_second;
    long long sum_of_delay_msec;
    int receive_count;
    // Log file.
    fstream logf;
    fstream all_cars;
  };
  
}
#endif



