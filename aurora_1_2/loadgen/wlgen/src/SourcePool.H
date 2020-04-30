#ifndef _SCOURCE_POOL_H_
#define _SCOURCE_POOL_H_

// C++ headers
#include <map>				// for map
#include <cstdio>			// for perror
#include <cstring>			// for strcmp

// Linux headers
#include <pthread.h>			// for pthread_*

// Local headers
#include "SingleSource.H"		// for SingleSource

using namespace std;
namespace workload_generator{

typedef map<source_stream_id_t, SingleSource*>		 source_pool_t;
typedef map<source_stream_id_t, SingleSource*>::iterator source_pool_iter_t;

typedef vector<SingleSource*>		sources_t;
typedef vector<SingleSource*>::iterator	sources_iter_t;

class SourcePool
{
public:
   SourcePool();
   ~SourcePool(){};

   void            lock();
   void            unlock();

   //Need to lock/unlock before/after using these functions 
   void            begin();
   bool            end() const;
   void            step();
   SingleSource&   getSource() const;
   void		   getSources(
         const agg_stream_id_t agg_id, 
         sources_t& result_sources);

   //Not need to lock.
   void		addSource(SingleSource* source); 
   void 	addSources(sources_t sources); 
   void 	delSources(stream_ids_t &_sources);
   void		delAggSources(const agg_stream_id_t& agg_id);		
   void		emptyPool();
   void		changeRate(double ratio);

private: 
   source_pool_t      _pool;
   source_pool_iter_t _pos;
   pthread_mutex_t    _mutex;

   void            delSource(const source_stream_id_t source_id);
};


}
#endif
