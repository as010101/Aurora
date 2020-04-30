/*	Name: Joshua David Kern (jdkern)
 *	File: LoadShedWidget.C
 *	Asgn: auroraStuff
 *	Date: Tue Apr  8 00:47:10 EDT 2003
 **************************************************************/

#include <LoadShedWidget.H>
 
#include <qlayout.h>
#include <qslider.h>
#include <qlcdnumber.h>
#include <qlabel.h>

#include <LSGraph.H>
#include <GUIConstants.H>

LoadShedWidget::LoadShedWidget( QWidget *parent, QLayout *layout)
        : QHBoxLayout( layout)
{
  setAlignment(Qt::AlignTop);

  m_slider = new QSlider(Qt::Vertical, parent, "slider" );
  m_slider->setFixedSize(LOADSHED_SLIDER_WIDTH, LOADSHED_SLIDER_HEIGHT);
  m_slider->setRange(0, 1000);
  m_slider->setValue(0);
  m_slider->setTickInterval(10);
  m_slider->setTickmarks(QSlider::Right);

  m_lcd  = new QLCDNumber( 5, parent, "lcd"  );
  m_lcd->setFixedHeight(20);
  m_lcd->setFixedWidth(50);
  m_lcd->setSmallDecimalPoint(true);
  m_lcd->setPaletteBackgroundColor(blue);
  m_lcd->display(1.0);

  m_ylabel = new QLabel("Expected Utility", parent);

  addWidget(m_ylabel);
  addWidget(m_lcd);
  addWidget(m_slider, 0, Qt::AlignTop);
  
  m_qvbox = new QVBoxLayout(this);
  m_graph = new LSGraph(parent, "graph");
  m_graph->setFixedSize(LOADSHED_GRAPH_SIZE, LOADSHED_GRAPH_SIZE);
  m_graph->setPaletteBackgroundColor(white);
  m_xlabel = new QLabel("Degree of Load Shedding", parent);
  m_xlabel->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
  m_qvbox->addWidget(m_graph, 0, Qt::AlignTop );
  m_qvbox->addWidget(m_xlabel, 0, Qt::AlignTop);
 
  connect( this, SIGNAL(valueChanged(double)),
	   m_lcd, SLOT(display(double)) );
  connect( m_slider, SIGNAL(valueChanged(int)),
	   this, SLOT(changeValue(int)) );
  connect( m_slider, SIGNAL(valueChanged(int)),
	   m_graph, SLOT(moveSlider(int)) );
  connect( m_slider, SIGNAL(valueChanged(int)),
	   parent, SLOT(setMonitorsExpectedUtility(int)) );
}

/*************************************************************************
 * Function Name: LoadShedWidget::~LoadShedWidget
 * Parameters: 
 * Effects: 
 *************************************************************************/

LoadShedWidget::~LoadShedWidget()
{

}

int LoadShedWidget::value() const
{
  return m_slider->value();
}

/*************************************************************************
 * Function Name: LoadShedWidget::changeValue
 * Parameters: 
 * Effects: emits signal of what value should appear in lcd
 *************************************************************************/

void 
LoadShedWidget::changeValue(int val)
{
  val = 1000 - val;
  emit valueChanged((double)val/1000);
}

/*************************************************************************
 * Function Name: LoadShedWidget::updateGraph
 * Parameters: 
 * Effects: updates the graph with latest curve
 *************************************************************************/

void 
LoadShedWidget::updateGraph(std::vector<double> curve)
{
  m_graph->updateGraph(curve);
}
