#include "AggregateBox.H"
#include <strstream.h>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"
#include <vector>
#include <sys/shm.h>
#include <sys/stat.h>

// The catalog creates this box to represent every box in memory during the lifetime
// of a network. Any type of state must be stored here.
AggregateBox::AggregateBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height), _tuple_counter(0) {

  // We take from the modifer what we want, and voila it gets saved here
  // into its "state" which is how the schedulers AggregateQBox can
  // get all that stuff back!
  m_modifier = modifier;
  
  // Stuff Aggregate wants as "state"
  _hash_of_everything = new HashForAggregate();
  _tuple_store = new vector<char*>();

  // Tuple counter
  _tuple_counter = new unsigned int;
  *_tuple_counter = 0;

}

AggregateBox::~AggregateBox() {
  // This is here to support the fidelity demo, to cleanup upon destruction (shutting down of aurora)
  // the shared memory segment that tracks alarms...
  // ACTUALLY NO - (this doesn't get called if you quit via queuemon, it seems)
  // see FidelityAlarm as to how we deal with this
  /**
  int seg_id = shmget(31337, 4, 0);
  if (seg_id == -1) {
    if (errno == ENOENT) {
      // no problems
      return;
    } else {
      perror("AggregateBox destructor: shmget failed:");
    }
  } else { // found, destroy
    cout << " Found shared memory segment, and destroying..." << endl;
    shmctl(seg_id,IPC_RMID,NULL);
  }
  */
}

HashForAggregate* AggregateBox::getHash() {
  return _hash_of_everything;
}


vector<char*>* AggregateBox::getTupleStore() {
  return _tuple_store;
}

unsigned int* AggregateBox::getTupleCounter() {
  return _tuple_counter;
}


int AggregateBox::getBoxType() {
  return AGGREGATE;
}

const char* AggregateBox::getModifier() {
  return m_modifier.c_str();
}

string AggregateBox::toString() {
  strstream s;
  s << "AggregateBox (" << Box::toString() << ", [thats all!] )" << '\0'; // huh? I dont think u need the null
  return s.str();
}
