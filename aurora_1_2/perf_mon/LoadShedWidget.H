/*	Name: Joshua David Kern (jdkern)
 *	File: LoadShedWidget.H
 *	Asgn: auroraStuff
 *	Date: Tue Apr  8 00:47:10 EDT 2003
 **************************************************************/

#ifndef LOADSHEDSLIDER_H
#define LOADSHEDSLIDER_H

#include <qlayout.h>
#include <vector>

class QSlider;
class QLCDNumber;
class QLabel;
class QBoxLayout;
class LSGraph;

class LoadShedWidget : public QHBoxLayout {
  Q_OBJECT
public:
  LoadShedWidget(QWidget *parent=0, QLayout *layout=0);
  
  virtual ~LoadShedWidget();
  
  int value() const;

  void updateGraph(std::vector<double> curve);
  
public slots:

  void changeValue(int val);
  
  signals:
  void valueChanged( double );
  
private:
  QBoxLayout * m_qvbox;
  QSlider  * m_slider;
  QLCDNumber * m_lcd;
  LSGraph * m_graph;
  QLabel * m_xlabel, * m_ylabel;
};


#endif // SLIDER_H
