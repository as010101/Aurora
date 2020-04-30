#include "StreamBuffer.H"

// C++ headers
#include <iostream>			// for cout

using namespace std;
namespace workload_generator{

void StreamBuffer::initialize(StreamSender* sender) 
{
    _sender = sender; 
}

void StreamBuffer::addTuples(tuplelist_t* tuple_list)
{
   if (tuple_list->size() == 0)
      return;
   send_item_t* item = new send_item_t(_sender, tuple_list);
   _buffer.AddEvent(
        string("Send tuple"),
        tuple_list->back()->getTimestamp(), 
        reinterpret_cast<threadtools::eventfunc_t>(_sender->sendTuple),
        item,
        0);
}

void StreamBuffer::printQueue() 
{ 
   _buffer.DumpEventQueue(cout); 
}

}
