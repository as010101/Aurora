#include "SlideBox.H"
#include <strstream.h>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"
#include <vector>


SlideBox::SlideBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height)
{
  char tmpToken[500]; // this should not be hardcoded .. have to fix in future .. but copied from MapBox code
  vector<string> v;
  const char *str = modifier.c_str();
  int len = strlen(str);
  istrstream ssLine(str);
  Parse* p = new Parse();
  m_state = new AggregateState();
  while(ssLine.good() != 0)
    {
      ssLine.getline(tmpToken, len+1, '~');
      //cerr << "Read: " << tmpToken << endl;
      v.push_back(tmpToken);
    }
  //  printf("paramater[1]: %s\n", (v[0]).c_str());
  const char* temp = (v[0].c_str());
  len = v[0].size();
  int j = 0;

  while ((j < len) && (temp[j] != '='))
    j++;
  if (j == len) {
    printf("No '=' character found in Slide expression. Giving up...\n");
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
  int i = 0;
  temp = (v[1].c_str());
  while (temp[i] != ':')
    i++;
  i++;
  while (temp[i] != ':')
    i++;
  i++;
  char temp_type = temp[i];

  if (strcmp(to_parse, "AVG") == 0)
    {
      if (temp_type == 'i')
	m_state->af = new IntAverageAF(v[1].c_str());
      else
  	m_state->af = new FloatAverageAF(v[1].c_str());
    }
  else if (strcmp(to_parse, "COUNT") == 0)
    m_state->af = new CountAF();
  else if (strcmp(to_parse, "SUM") == 0)
    {
      if (temp_type == 'i')
	m_state->af = new IntSumAF(v[1].c_str());
      else
  	m_state->af = new FloatSumAF(v[1].c_str());
    }
  else if (strcmp(to_parse, "MAX") == 0)
    {
      if (temp_type == 'i')
	m_state->af = new IntMaxAF(v[1].c_str());
      else
  	m_state->af = new FloatMaxAF(v[1].c_str());
    }
  else if (strcmp(to_parse, "MIN") == 0)
    {
      if (temp_type == 'i')
	m_state->af = new IntMinAF(v[1].c_str());
      else
  	m_state->af = new FloatMinAF(v[1].c_str());
    }
  else
    {
      printf("Aggegate function not {AVG, COUNT, SUM, MAX, MIN}. Giving up...\n");
      abort();
    }
  delete[] to_parse;

  m_state->group_by = new char[v[2].size() + 1];
  memcpy(m_state->group_by, v[2].c_str(), v[2].size());
  m_state->group_by[v[2].size()] = '\0';

  i = 0;
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
  long secs = (long) slack;
  long micro = (long) ((slack - secs) * 1000000);         // convert the decimal part to microseconds = 6 zeros
  m_state->slack_time = *(new Timestamp(secs, micro));
   
  //  m_state->slack_time = (int) slack; //CHANGE LATER WHEN SLACK CAN BE A FLOAT

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
   float unless = atof(temp + i);
   long secs = (long) unless;
   long micro = (long) ((unless - secs) * 1000000);         // convert the decimal part to microseconds = 6 zeros
   m_state->unless_timeout = *(new Timestamp(secs, micro));
   
   //   m_state->unless_timeout = (int) timeout;

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
  }
  else if (strncmp(temp+i, "NONE", 8) == 0) 
    m_state->until = NONE;
  else {
	printf("Illegal UNLESS parameter. Giving up...\n");
	abort();
	}

  i = 0;
  temp = (v[6].c_str());
  while ((i < v[6].size()) && (!isdigit(temp[i])))
    i++;
  if (i == v[6].size()) {
    printf("Completely blank WINDOW RANGE parameter. Giving up...\n");
    abort();
  }
  int range = atoi(temp+i);
  m_state->window_range = range; 



  //The UNIQUE FOR parameter is in limbo right now so I will hard code it
  //to 100 for now. Later on, this will have to change when the group figures
  //out what it wants. If indeed it becomes a size system parameter (rather
  //than a time slide parameter, the following line of code will be deleted.
  //  m_state->af = new IntAverageAF(":0:i:0:4:");
  // m_state->group_by = ":0:i:4:4:";
  //  m_state->output = ALL;
  //m_state->window_range = 3;
  //m_state->slack_time = 0;
  //m_state->until = NONE;
  //  m_state->unique_time = 10; //HACK...see above
  m_state->unique_time = *(new Timestamp(10, 0));  // HACK .... see above
  
  m_state->list_hash = new HashWithList();
  m_state->trash = new TrashHash();
  //  cout << "m_state set: "; m_state->printAll();

  m_modifier = modifier;
}

SlideBox::~SlideBox()
{
}

AggregateState *SlideBox::getState()
{
  return m_state;
}

int SlideBox::getBoxType()
{
  return SLIDE;
}

string SlideBox::toString()
{
  strstream s;
  s << "SlideBox (" << Box::toString() << ", expression: " << m_modifier << ")" << '\0';
  return s.str();
}

