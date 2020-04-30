/*************************************************************************
 *    NAME: Andrea Michelle Fein
 *    USER: afein
 *    FILE: AlertBox.C
 *    DATE: Mon Jun 16 15:07:16 2003
 *************************************************************************/
#include "AlertBox.H"

/*************************************************************************
 * Function Name: AlertBox::AlertBox
 * Parameters: 
 * Effects: 
 *************************************************************************/

AlertBox::AlertBox(QWidget* parent, int width, int height) : QListBox(parent)
{
  setSelectionMode(QListBox::Single);
}


/*************************************************************************
 * Function Name: AlertBox::~AlertBox
 * Parameters: 
 * Effects: 
 *************************************************************************/

AlertBox::~AlertBox()
{
}


