/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: FishTable.H
 *	Asgn: fishgui
 *	Date: Mon Jun 16 15:16:00 EDT 2003
 **************************************************************/

#ifndef FishTable_Header
#define FishTable_Header

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>

#include <qtable.h>
#include "constants.H"

class FishTable : public QTable {

  Q_OBJECT

public:

  FishTable(QWidget* parent);
  virtual ~FishTable();

  void updateData(const AlertEvent& ae);

protected:

private:

  static const int PRINT = 0;
  static const int VR = 1;
  static const int CR = 2;
  static const int PBM = 3;
  static const int VOLT = 4;
  static const int DEAD = 5;

  static const int ALERT = 0;
  static const int NONALERT = 1;

};

#endif
