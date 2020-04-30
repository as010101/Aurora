/**************************************************************
 *	Name: Joshua David Kern (jdkern)
 *	File: GUIConstants.H
 *	Asgn: auroraStuff
 *	Date: Wed Apr  9 13:30:28 EDT 2003
 **************************************************************/

#ifndef GUIConstants_Header
#define GUIConstants_Header

const int GUI_WIDTH = 900;
const int GUI_HEIGHT = 600;
const int QOS_MONITOR_WIDTH = 100;
const int QOS_MONITOR_HEIGHT = 50;
const int QOS_MONITOR_LINES = 50;
const int LOADSHED_SLIDER_WIDTH = 20;
const int LOADSHED_SLIDER_HEIGHT = 500;
const int LOADSHED_GRAPH_SIZE = 500;
const int LOADSHED_WIDGET_HEIGHT = LOADSHED_GRAPH_SIZE + 10;
//const int MAX_NUM_APPLICATIONS = 24;
//const int MAX_NUM_QOS_MONITOR_ROWS = 4;

enum QOSTYPE {
  LATENCY = 0,
  UTILITY = 1
};

#endif
