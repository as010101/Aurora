/*************************************************************************
 *    NAME: Andrea Michelle Fein
 *    USER: afein
 *    FILE: LabelBox.C
 *    DATE: Wed Jun 18 16:19:03 2003
 *************************************************************************/
#include "LabelBox.H"

/*************************************************************************
 * Function Name: LabelBox::LabelBox
 * Parameters: QWidget* parent, int width, int height
 * Effects: 
 *************************************************************************/

LabelBox::LabelBox(QWidget* parent, int row, int header) : QVBox(parent)
{
  QFont font;
  font.setStyleHint(QFont::SansSerif);
  font.setPointSize(16);
  font.setBold(true);
  setFixedHeight(18*row+header);

  m_blank = new QLabel(this);
  m_blank->setFixedHeight(header);

  m_print = new QLabel("Print Interval", this);
  m_print->setFrameShape(QFrame::WinPanel);
  m_print->setFrameShadow(QFrame::Raised);
  m_print->setFixedHeight(row);
  m_print->setFont(font);

  m_time = new QLabel("Time", this);
  m_time->setFrameShape(QFrame::WinPanel);
  m_time->setFrameShadow(QFrame::Raised);
  m_time->setFixedHeight(row);
  m_time->setFont(font);

  m_temp = new QLabel("Temperature", this);
  m_temp->setFrameShape(QFrame::WinPanel);
  m_temp->setFrameShadow(QFrame::Raised);
  m_temp->setFixedHeight(4*row);
  m_temp->setFont(font);

  m_ph = new QLabel("pH", this);
  m_ph->setFrameShape(QFrame::WinPanel);
  m_ph->setFrameShadow(QFrame::Raised);
  m_ph->setFixedHeight(4*row);
  m_ph->setFont(font);

  m_cond = new QLabel("Conductivity", this);
  m_cond->setFrameShape(QFrame::WinPanel);
  m_cond->setFrameShadow(QFrame::Raised);
  m_cond->setFixedHeight(4*row);
  m_cond->setFont(font);

  m_o2 = new QLabel("Oxygen", this);
  m_o2->setFrameShape(QFrame::WinPanel);
  m_o2->setFrameShadow(QFrame::Raised);
  m_o2->setFixedHeight(4*row);
  m_o2->setFont(font);
}


/*************************************************************************
 * Function Name: LabelBox::~LabelBox
 * Parameters: 
 * Effects: 
 *************************************************************************/

LabelBox::~LabelBox()
{
  if (NULL != m_blank)
  {
    delete m_blank;
    m_blank = NULL;
  }
  if (NULL != m_print)
  {
    delete m_print;
    m_print = NULL;
  }
  if (NULL != m_time)
  {
    delete m_time;
    m_time = NULL;
  }
  if (NULL != m_temp)
  {
    delete m_temp;
    m_temp = NULL;
  }
  if (NULL != m_ph)
  {
    delete m_ph;
    m_ph = NULL;
  }
  if (NULL != m_cond)
  {
    delete m_cond;
    m_cond = NULL;
  }
  if (NULL != m_o2)
  {
    delete m_o2;
    m_o2 = NULL;
  }
  
}


