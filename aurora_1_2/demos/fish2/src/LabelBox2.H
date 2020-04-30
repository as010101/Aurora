/**************************************************************
 *	Name: Andrea Michelle Fein (afein)
 *	File: LabelBox2.H
 *	Asgn: src
 *	Date: Fri Jun 27 13:50:55 EDT 2003
 **************************************************************/

#ifndef LabelBox2_Header
#define LabelBox2_Header

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>

class LabelBox2 : public QHBox {

  Q_OBJECT

public:

  LabelBox2(QWidget* parent, int rowHeight, int labelWidth, int blankWidth);
  virtual ~LabelBox2();

protected:

private:

  QVBox* m_labels;
  QLabel* m_vr;
  QLabel* m_cr;
  QLabel* m_pbm;
  QLabel* m_volt;
  QLabel* m_dead;
  QLabel* m_blank;

};

#endif
