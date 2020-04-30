/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: WaterTable.H
 *	Asgn: fishgui
 *	Date: Mon Jun 16 15:15:54 EDT 2003
 **************************************************************/

#ifndef WaterTable_Header
#define WaterTable_Header

#include <qtable.h>
#include "constants.H"

class WaterTable : public QTable {

  Q_OBJECT

public:

  WaterTable(QWidget* parent, int width, int height);
  virtual ~WaterTable();

  void updateData(const AlertEvent& ae);
  void updateRow(int row, float alert, float nonAlert);

protected:

private:

  //defines row and column numbers
  static const int PRINT = 0;
  static const int TIME = 1;
  static const int TEMPC = 2;
  static const int TEMP1 = 3;
  static const int TEMP2 = 4;
  static const int TEMP4 = 5;
  static const int PHC = 6;
  static const int PH1 = 7;
  static const int PH2 = 8;
  static const int PH4 = 9;
  static const int CONDC = 10;
  static const int COND1 = 11;
  static const int COND2 = 12;
  static const int COND4 = 13;
  static const int OC = 14;
  static const int O1 = 15;
  static const int O2 = 16;
  static const int O4 = 17;
  static const int NONALERT = 1;
  static const int ALERT = 0;

};

#endif
