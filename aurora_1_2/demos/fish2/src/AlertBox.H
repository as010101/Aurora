/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: AlertBox.H
 *	Asgn: fishgui
 *	Date: Mon Jun 16 15:06:12 EDT 2003
 **************************************************************/

#ifndef AlertBox_Header
#define AlertBox_Header

#include <qlistbox.h>
#include "constants.H"

class AlertBox : public QListBox {

  Q_OBJECT

public:

  AlertBox(QWidget* parent, int width, int height);
  virtual ~AlertBox();

protected:

private:

};

#endif
