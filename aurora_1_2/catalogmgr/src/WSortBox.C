#include "WSortBox.H"
#include <strstream.h>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"
//#include "SortList.H"
#include <vector>


WSortBox::WSortBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height)
{
  char tmpToken[500]; // this should not be hardcoded .. have to fix in future .. but copied from MapBox code
  vector<string> v;

  //  cout << "##############      MODIFIER         ################################ " << endl << "\t\t\t" << modifier << endl;

  const char *str = modifier.c_str();
  int len = strlen(str);
  istrstream ssLine(str);
  //Parse* p = new Parse();

  m_state = new AggregateState();
  while(ssLine.good() != 0)
    {
      ssLine.getline(tmpToken, len+1, '~');
      //cerr << "Read: " << tmpToken << endl;
      v.push_back(tmpToken);
    }
  //  printf("paramater[1]: %s\n", (v[0]).c_str());

  m_state->group_by = new char[v[0].size() + 1];
  memcpy(m_state->group_by, v[0].c_str(), v[0].size());
  m_state->group_by[v[0].size()] = '\0';

  int i = 0;
  const char* temp = (v[1].c_str());
  while ((i < v[1].size()) && (!isdigit(temp[i])))
    i++;
  if (i == v[1].size()) {
    printf("Completely blank SLACK parameter. Giving up...\n");
    abort();
  }
  float unless = atof(temp+i);
  long secs = (long) unless;
  long micro = (long) ((unless - secs) * 1000000);         // convert the decimal part to microseconds = 6 zeros

  m_state->unless_timeout = *(new Timestamp(secs, micro));

  //  m_state->slack_time = (int) slack;  //CHANGE LATER WHEN SLACK CAN BE A FLOAT

  // skip over the second thing because right now buffer size is supposed
  // to be of infinite size
  // deal with this later on if buffer size is implemented

  //  m_state->group_by = v[2].c_str();



  /*
const char* temp = (v[0].c_str());
  len = v[0].size();
  int j = 0;
  while ((j < len) && (temp[j] != '='))
    j++;
  if (j == len) {
    printf("No '=' character found in WSort expression. Giving up...\n");
    abort();
  }
  char* to_parse = new char[len - j];
  int k = 0;
  while (k < (len - j - 1))
    {
      to_parse[k] = temp[j+1+k];
      k++;
    }
  to_parse[k] = '\0';
  if (strcmp(to_parse, "AVG") == 0)
    m_state->af = new AverageAF(v[1].c_str());
  else if (strcmp(to_parse, "COUNT") == 0)
    m_state->af = new CountAF();
  else if (strcmp(to_parse, "SUM") == 0)
    m_state->af = new SumAF(v[1].c_str());
  else if (strcmp(to_parse, "MAX") == 0)
    m_state->af = new MaxAF(v[1].c_str());
  else if (strcmp(to_parse, "MIN") == 0)
    m_state->af = new MinAF(v[1].c_str());
  else
    {
    printf("Aggegate function not {AVG, COUNT, SUM, MAX, MIN}. Giving up...\n");
    abort();
    }
  delete[] to_parse;

  m_state->group_by = v[2].c_str();
  int i = 0;
  temp = (v[3].c_str());
  while ((i < v[3].size()) && (!isalpha(temp[i])))
    i++;
  if (i == v[3].size()) {
    printf("Completely blank OUTPUT parameter. Giving up...\n");
    abort();
  }
  if (strncmp(temp+i, "ALL", 3) == 0) {
    m_state->output = ALL;
  }
  else if (strncmp(temp+i, "LAST", 4) == 0) {
    m_state->output = LAST;
  }
  else if (strncmp(temp+i, "WHENEVER", 8) == 0) {
    m_state->output = WHENEVER;    
    while (temp[i] != ' ')
      i++;
    char *pred_string = new char[v[3].size() - i - 1];
    int k = 0; 
    while ((i + k) <= v[3].size())
      {
	pred_string[k] = temp[i + k];
	k++;
      }
    m_state->whenever_pred = p->parsePred(pred_string);    
    //NEED TO ADD MORE HERE!!
  }
  else {
	printf("Illegal OUTPUT parameter. Giving up...\n");
	abort();
  }
 
  i = 0;
  temp = (v[4].c_str());
  while ((i < v[4].size()) && (!isdigit(temp[i])))
    i++;
  if (i == v[4].size()) {
    printf("Completely blank SLACK parameter. Giving up...\n");
    abort();
  }
  float slack = atof(temp+i);
  m_state->slack_time = (int) slack; //CHANGE LATER WHEN SLACK CAN BE A FLOAT

  i = 0;
  temp = (v[5].c_str());
  while ((i < v[5].size()) && (!isalpha(temp[i])))
    i++;
  if (i == v[5].size()) {
    printf("Completely blank UNLESS parameter. Giving up...\n");
    abort();
  }
  if (strncmp(temp+i, "TIMEOUT", 7) == 0) {
    m_state->until = TIMEOUT;
   while (temp[i] != ' ')
     i++;
   float timeout = atof(temp + i);
   m_state->unless_timeout = (int) timeout;
   
   //NEED TO ADD MORE HERE!!
   
  }
  else if (strncmp(temp+i, "SATISFIES", 4) == 0) {
    m_state->until = SATISFIES;
   while (temp[i] != ' ')
     i++;
   char *pred_string = new char[v[5].size() - i - 1];
    int k = 0; 
    while ((i + k) <= v[5].size())
      {
	pred_string[k] = temp[i + k];
	k++;
      }
     m_state->satisfies_pred = p->parsePred(pred_string);    

    //NEED TO ADD MORE HERE!!
  }
  else if (strncmp(temp+i, "NONE", 8) == 0) 
    m_state->until = NONE;
  else {
	printf("Illegal UNLESS parameter. Giving up...\n");
	abort();
  }
  *****************************  */


  //  m_state->group_by = ":0:i:0:4:";
  // m_state->unless_timeout = 8;
  //  cout << "m_state set: ";

  //  m_state->printAll();
  m_modifier = modifier;

  m_state->buffer = new SortList();
}

WSortBox::~WSortBox()
{
}


AggregateState *WSortBox::getState()
{
  return m_state;
}

int WSortBox::getBoxType()
{
  return WSORT;
}

string WSortBox::toString()
{
  strstream s;
  s << "WSortBox (" << Box::toString() << ", expression: " << m_modifier << ")" << '\0';
  return s.str();
}

