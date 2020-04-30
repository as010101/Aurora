#ifndef _TIMEVAL_H_
#define _TIMEVAL_H_

// C headers
#include <cmath>				// for floor

// Linux headers
#include <sys/time.h>			// for timeval 

namespace workload_generator{

class Timeval:public timeval 
{
public:
   Timeval()
   {
      tv_sec = 0;
      tv_usec = 0;
   };
   Timeval(long sec, long usec)
   {
      tv_sec = sec;
      tv_usec = usec;
   }
   Timeval(const timeval &t) 
   {
      tv_sec = t.tv_sec;
      tv_usec = t.tv_usec;
   }
   ~Timeval(){};
   Timeval(double t)
   {
      tv_sec = static_cast<long>(floor(t));
      tv_usec = static_cast<long>((t - floor(t)) * 1000000);
   }

   double doubleValue() {
      return static_cast<double>(tv_sec) + 
             static_cast<double>(tv_usec) / 1000000;
   }
   void operator=(const Timeval &t) 
   {
      tv_sec = t.tv_sec;
      tv_usec = t.tv_usec;
   }

   void operator+=(const timeval &t)
   {
      tv_sec += t.tv_sec;
      tv_usec += t.tv_usec;
      if (tv_usec >= 1000000) {
         tv_sec += 1;
         tv_usec -= 1000000;
      }
   }

   void operator-=(const timeval &t)
   {
      tv_sec -= t.tv_sec;
      tv_usec -= t.tv_usec;
      if (tv_usec < 0) {
         tv_sec -= 1;
         tv_usec += 1000000;
      }
   }

   bool operator==(const timeval &t) const
   {
      return ((tv_sec == t.tv_sec) &&
              (tv_usec == t.tv_usec));
   }

   bool operator<(const timeval &t) const
   {
      return ((tv_sec < t.tv_sec) ||
             ((tv_sec == t.tv_sec) &&
             (tv_usec < t.tv_usec)));
   }
};

}
#endif
