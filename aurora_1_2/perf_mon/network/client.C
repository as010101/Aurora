#include "ImportDelayedData.H"
#include "DataManager.H"

using namespace std;

int
main(int ac, char **av)
{

  if(ac < 2) {
	printf("usages: %s <hostname>\n", av[0]);
	return 1;
  }

  DataManager dm;

  /*
  dm.populateStatsImage();
  dm.printStatsImage();

  dm.populateStatsImage();
  dm.printStatsImage();

  dm.populateStatsImage();
  dm.printStatsImage();
  */
  ImportDelayedData idd(4938, av[1], &dm);
  idd.start();

  return 1;
}
