/*************************************************************************
 *    NAME: Andrea Michelle Fein
 *    USER: afein
 *    FILE: WaterTable.C
 *    DATE: Mon Jun 16 15:17:08 2003
 *************************************************************************/
#include "WaterTable.H"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <qlabel.h>

using namespace std;

/*************************************************************************
 * Function Name: WaterTable::WaterTable
 * Parameters: 
 * Effects: 
 *************************************************************************/

WaterTable::WaterTable(QWidget* parent, int width, int height) : QTable(5, 3, parent)
{
  QHeader* columns = horizontalHeader();
  QHeader* rows = verticalHeader();
  setNumRows(18);
  setNumCols(2);
  QFont font;
  font.setStyleHint(QFont::SansSerif);
  font.setPointSize(16);
  font.setBold(true);
  columns->setFont(font);
  rows->setFont(font);

  columns->setLabel(1, "Last Good Data");
  columns->setLabel(0, "Alert Data");
  columns->adjustHeaderSize();
  columns->setFocusPolicy(QWidget::NoFocus);

  rows->setLabel(0, "Current");
  rows->setLabel(1, "Current");
  rows->setLabel(2, "Current");
  rows->setLabel(3, "1 Hour Change");
  rows->setLabel(4, "2 Hour Change");
  rows->setLabel(5, "4 Hour Change");
  rows->setLabel(6, "Current");
  rows->setLabel(7, "1 Hour Change");
  rows->setLabel(8, "2 Hour Change");
  rows->setLabel(9, "4 Hour Change");
  rows->setLabel(10, "Current");
  rows->setLabel(11, "1 Hour Change");
  rows->setLabel(12, "2 Hour Change");
  rows->setLabel(13, "4 Hour Change");
  rows->setLabel(14, "Current");
  rows->setLabel(15, "1 Hour Change");
  rows->setLabel(16, "2 Hour Change");
  rows->setLabel(17, "4 Hour Change");
  rows->adjustHeaderSize();
  rows->setFocusPolicy(QWidget::NoFocus);

  setColumnWidth(1, 200);
  setColumnWidth(0, 200);

  font.setPointSize(14);
  setFont(font);
  int headerWidth = rows->width();
  int headerHeight = columns->height();
  setFixedHeight(numRows()*rowHeight(0) + headerHeight + 5);
  setSelectionMode(QTable::NoSelection);
  setFocusPolicy(QWidget::NoFocus);
  setReadOnly(true);

  font.setBold(false);
  font.setPointSize(14);
  for (int row = 0; row < numRows(); row++)
  {
    for (int col = 0; col < 2; col++) 
    {
      QLabel* label = new QLabel(this);
      QColor color("white");
      label->setPaletteBackgroundColor(color);
      label->setFont(font);
      setCellWidget(row, col, label);
    }
  }
}


/*************************************************************************
 * Function Name: WaterTable::~WaterTable
 * Parameters: 
 * Effects: 
 *************************************************************************/

WaterTable::~WaterTable()
{
}


/*************************************************************************
 * Function Name: WaterTable::updateData
 * Parameters: const AlertEvent& ae
 * Returns: void
 * Effects: 
 *************************************************************************/
void
WaterTable::updateData(const AlertEvent& ae)
{ 
  FishSlidingWindows alert = ae._alertWindows;
  FishSlidingWindows nonAlert = ae._nonAlertWindows;
  QLabel* label;
   
  label = (QLabel*)cellWidget(TIME, NONALERT);
  label->setText(ae._nonAlertWindows._timestamp);
  label = (QLabel*)cellWidget(TIME, ALERT);
  label->setText(ae._alertWindows._timestamp);

  updateRow(PRINT, (float)ae._alertPrint, (float)ae._nonAlertPrint);

  updateRow(TEMPC, ae._alertTemp, ae._nonAlertTemp);
  updateRow(TEMP1, alert._tempDelta1, nonAlert._tempDelta1);
  updateRow(TEMP2, alert._tempDelta2, nonAlert._tempDelta2);
  updateRow(TEMP4, alert._tempDelta4, nonAlert._tempDelta4);
  
  updateRow(PHC, ae._alertPH, ae._nonAlertPH);
  updateRow(PH1, alert._phDelta1, nonAlert._phDelta1);
  updateRow(PH2, alert._phDelta2, nonAlert._phDelta2);
  updateRow(PH4, alert._phDelta4, nonAlert._phDelta4);
  
  updateRow(CONDC, ae._alertConductivity, ae._nonAlertConductivity);
  updateRow(COND1, alert._condDelta1, nonAlert._condDelta1);
  updateRow(COND2, alert._condDelta2, nonAlert._condDelta2);
  updateRow(COND4, alert._condDelta4, nonAlert._condDelta4);
  
  updateRow(OC, ae._alertO2, ae._nonAlertO2);
  updateRow(O1, alert._o2Delta1, nonAlert._o2Delta1);
  updateRow(O2, alert._o2Delta2, nonAlert._o2Delta2);
  updateRow(O4, alert._o2Delta4, nonAlert._o2Delta4);
  
}


/*************************************************************************
 * Function Name: WaterTable::updateRow
 * Parameters: int row, float alert, float nonAlert, float current
 * Returns: void
 * Effects: 
 *************************************************************************/
void
WaterTable::updateRow(int row, float alert, float nonAlert)
{
  QLabel* label;
  string alertStr, nonAlertStr;
  stringstream converter1, converter2;

  alert = alert * 100;
  int a1 = (int)alert;
  if (a1 % 10 >= 5)
    a1 += 1;
  double a2 = (double)a1/100.0;

  nonAlert = nonAlert * 100;
  int n1 = (int)nonAlert;
  if (n1 % 10 >= 5)
    n1 += 1;
  double n2 = (double)n1/100.0;

  converter1 << a2;
  converter1 >> alertStr;
  
  converter2 << n2;
  converter2 >> nonAlertStr;

  QColor color;
  float percentDiff = 0;
  if (a2 - n2 >= 0 & a2 != 0)
  {
    percentDiff = (a2 - n2)/a2;
  }
  else if (n2 != 0)
  {
    percentDiff = (n2 - a2)/n2;
  }

  if (percentDiff > .2)
  {
    color.setNamedColor("red");
  }
  else
  {
    color.setNamedColor("black");
  }

  label = (QLabel*)cellWidget(row, 0);
  label->setText(alertStr);
  label->setPaletteForegroundColor(color);
  
  label = (QLabel*)cellWidget(row, 1);
  label->setText(nonAlertStr);
  label->setPaletteForegroundColor(color);

}
  
