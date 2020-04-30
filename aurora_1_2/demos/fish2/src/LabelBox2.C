/*************************************************************************
 *    NAME: Andrea Michelle Fein
 *    USER: afein
 *    FILE: LabelBox2.C
 *    DATE: Fri Jun 27 13:53:51 2003
 *************************************************************************/
#include "LabelBox2.H"

/*************************************************************************
 * Function Name: LabelBox2::LabelBox2
 * Parameters: QWidget* parent, rowHeight, labelWidth, blankWidth
 * Effects: 
 *************************************************************************/

LabelBox2::LabelBox2(QWidget* parent, int rowHeight, int labelWidth, int blankWidth) : QHBox(parent)
{
  QFont font;
  font.setStyleHint(QFont::SansSerif);
  font.setPointSize(16);
  font.setBold(true);

  m_labels = new QVBox(this);
  m_blank = new QLabel(this);

  m_vr = new QLabel("Vent Rates", m_labels);
  m_cr = new QLabel("Cough Rates", m_labels);
  m_pbm = new QLabel("Percent body move", m_labels);
  m_volt = new QLabel("Voltages", m_labels);
  m_dead = new QLabel("Dead", m_labels);

  m_vr->setFrameShape(QFrame::WinPanel);
  m_vr->setFrameShadow(QFrame::Raised);
  m_vr->setFixedHeight(rowHeight);
  m_vr->setFont(font);
  m_cr->setFrameShape(QFrame::WinPanel);
  m_cr->setFrameShadow(QFrame::Raised);
  m_cr->setFixedHeight(rowHeight);
  m_cr->setFont(font);
  m_pbm->setFrameShape(QFrame::WinPanel);
  m_pbm->setFrameShadow(QFrame::Raised);
  m_pbm->setFixedHeight(rowHeight);
  m_pbm->setFont(font);
  m_volt->setFrameShape(QFrame::WinPanel);
  m_volt->setFrameShadow(QFrame::Raised);
  m_volt->setFixedHeight(rowHeight);
  m_volt->setFont(font);
  m_dead->setFrameShape(QFrame::WinPanel);
  m_dead->setFrameShadow(QFrame::Raised);
  m_dead->setFixedHeight(rowHeight);
  m_dead->setFont(font);

  m_blank->setFixedHeight(rowHeight*5);
  m_blank->setFixedWidth(blankWidth);

  setFixedHeight(rowHeight*5);
  
}


/*************************************************************************
 * Function Name: LabelBox2::~LabelBox2
 * Parameters: 
 * Effects: 
 *************************************************************************/

LabelBox2::~LabelBox2()
{
  
  if (m_vr != NULL)
  {
    delete m_vr;
    m_vr = NULL;
  }
  if (m_cr != NULL)
  {
    delete m_cr;
    m_cr = NULL;
  }
  if (m_pbm != NULL)
  {
    delete m_pbm;
    m_pbm = NULL;
  }
  if (m_volt != NULL)
  {
    delete m_volt;
    m_volt = NULL;
  }
  if (m_dead != NULL)
  {
    delete m_dead;
    m_dead = NULL;
  }
  if (m_blank != NULL)
  {
    delete m_blank;
    m_blank = NULL;
  }
  if (m_labels != NULL)
  {
    delete m_labels;
    m_labels = NULL;
  }

}


