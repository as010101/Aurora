/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: LabelBox.H
 *	Asgn: fishgui
 *	Date: Wed Jun 18 16:16:48 EDT 2003
 **************************************************************/

#ifndef LabelBox_Header
#define LabelBox_Header

#include <qvbox.h>
#include <qlabel.h>

class LabelBox : public QVBox {

  Q_OBJECT

public:

  LabelBox(QWidget* parent, int row, int header);
  virtual ~LabelBox();

protected:

private:

  QLabel* m_blank;
  QLabel* m_print;
  QLabel* m_time;
  QLabel* m_temp;
  QLabel* m_ph;
  QLabel* m_cond;
  QLabel* m_o2;

};

#endif
