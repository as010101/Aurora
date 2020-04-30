#include "ExportDelayedData.H"
#include <pthread.h>


int
main(int ac, char **av) {
	cout << " Server starts " << endl;
  ExportDelayedData	exp(4938, 10);
  exp.run();
  return 1;
}
