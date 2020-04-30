/*************************************************************************
 *    NAME: Andrea Michelle Fein
 *    USER: afein
 *    FILE: FishTable.C
 *    DATE: Mon Jun 16 15:17:13 2003
 *************************************************************************/
#include "FishTable.H"
#include <qpixmap.h>
#include <sstream>
#include <iostream>
#include <string>

using namespace std;

/*************************************************************************
 * Function Name: FishTable::FishTable
 * Parameters: 
 * Effects: 
 *************************************************************************/

FishTable::FishTable(QWidget* parent) : QTable(parent)
{
  setNumRows(6);
  setNumCols(2);

  QHeader* columns = horizontalHeader();
  QHeader* rows = verticalHeader();
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

  rows->setLabel(0, "Print Interval");
  rows->setLabel(1, "Fish with out of control\nventilatory rates");
  rows->setLabel(2, "Fish with out of control\ncough rates");
  rows->setLabel(3, "Fish with out of control\npercent body movements");
  rows->setLabel(4, "Fish with out of control\nvoltages");
  rows->setLabel(5, "Dead fish");
  rows->adjustHeaderSize();
  rows->setFocusPolicy(QWidget::NoFocus);

  for (int i = 1; i < numRows(); i++) 
  {
    setRowHeight(i, 50);
  }

  setColumnWidth(0, 200);
  setColumnWidth(1, 200);

  setFixedHeight(rowHeight(0) + (numRows()-1)*rowHeight(1) + columns->height() + 5);
  setSelectionMode(QTable::NoSelection);
  setFocusPolicy(QWidget::NoFocus);
  setReadOnly(true);

  //----------------- FONT ----------------
  font.setPointSize(14);
  font.setBold(false);
  font.setStyleHint(QFont::SansSerif);
  setFont(font);

  //----------------- CELL WIDGETS ------------------
  for (int row = 0; row < numRows(); row++)
  {
    for (int col = 0; col < numCols(); col++) 
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
 * Function Name: FishTable::~FishTable
 * Parameters: 
 * Effects: 
 *************************************************************************/

FishTable::~FishTable()
{
}


/*************************************************************************
 * Function Name: FishTable::updateData
 * Parameters: const AlertEvent& ae
 * Returns: void
 * Effects: 
 *************************************************************************/
void
FishTable::updateData(const AlertEvent& ae)
{
  stringstream printConverter, vrConverter, crConverter, pbmConverter, voltConverter, 
    deadConverter;
  string str;
  QLabel* label;

  printConverter << ae._alertPrint;
  printConverter >> str;
  label = (QLabel*)cellWidget(PRINT, ALERT);
  label->setText(str);

  deadConverter << ae._alertNumDead;
  deadConverter >> str;
  label = (QLabel*)cellWidget(DEAD, ALERT);
  label->setText(str);

  vrConverter << ae._alertNumOocVentilatoryRates;
  vrConverter >> str;
  label = (QLabel*)cellWidget(VR, ALERT);
  label->setText(str);

  crConverter << ae._alertNumOocCoughRates;
  crConverter >> str;
  label = (QLabel*)cellWidget(CR, ALERT);
  label->setText(str);
  
  pbmConverter << ae._alertNumOocPercentBodyMovements;
  pbmConverter >> str;
  label = (QLabel*)cellWidget(PBM, ALERT);
  label->setText(str);

  voltConverter << ae._alertNumOocVoltages;
  voltConverter >> str;
  label = (QLabel*)cellWidget(VOLT, ALERT);
  label->setText(str);
  

  stringstream printConverter2, vrConverter2, crConverter2, pbmConverter2, voltConverter2, 
    deadConverter2;
 
  printConverter2 << ae._nonAlertPrint;
  printConverter2 >> str;
  label = (QLabel*)cellWidget(PRINT, NONALERT);
  label->setText(str);

  deadConverter2 << ae._nonAlertNumDead;
  deadConverter2 >> str;
  label = (QLabel*)cellWidget(DEAD, NONALERT);
  label->setText(str);

  vrConverter2 << ae._nonAlertNumOocVentilatoryRates;
  vrConverter2 >> str;
  label = (QLabel*)cellWidget(VR, NONALERT);
  label->setText(str);

  crConverter2 << ae._nonAlertNumOocCoughRates;
  crConverter2 >> str;
  label = (QLabel*)cellWidget(CR, NONALERT);
  label->setText(str);
  
  pbmConverter2 << ae._nonAlertNumOocPercentBodyMovements;
  pbmConverter2 >> str;
  label = (QLabel*)cellWidget(PBM, NONALERT);
  label->setText(str);

  voltConverter2 << ae._nonAlertNumOocVoltages;
  voltConverter2 >> str;
  label = (QLabel*)cellWidget(VOLT, NONALERT);
  label->setText(str);
 
}
