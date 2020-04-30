/*************************************************************************
 *    NAME: Joshua David Kern
 *    USER: jdkern
 *    FILE: main.C
 *    DATE: Tue Apr  8  0:51:03 2003
 *************************************************************************/

#include <qapplication.h>

#include <ImportDelayedData.H>
#include <MainWindow.H>
#include <DataManager.H>

int main( int argc, char **argv )
{
  if(argc<2) {
    printf("usage: %s [hostname] <update>\n", argv[0]);
    exit(1);
  }
  
  int updateSpeed = 1000;
  if (argc > 2) {
    updateSpeed = atoi(argv[2]);
  }
  
  QApplication app( argc, argv );
  DataManager data;
  ImportDelayedData	idd(10000, argv[1], &data);
  data.setImportDelayedData(&idd);

  idd.start();
  
  MainWindow window(&data, updateSpeed);
  app.setMainWidget( &window );
  window.show();
  int ret = app.exec();
  idd.stop();
  return ret;
}
