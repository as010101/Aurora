#include "MapBox.H"
#include <strstream.h>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"
#include <vector>


MapBox::MapBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
Box(id, cost, sel, x, y, width, height)
{
	char tmpToken[500]; // this should not be hardcoded .. have to fix in future
	vector<string> v;
	const char *str = modifier.c_str();
	int len = strlen(str);
	istrstream ssLine(str);
	while(ssLine.good() != 0)
	{
		ssLine.getline(tmpToken, len+1, ',');
		//cerr << "Read: " << tmpToken << endl;
		v.push_back(tmpToken);
	}
	//vector<Expression*> m_expressions;
	for ( int i = 0; i < v.size(); i++ )
	{
	  //printf("expression[%i]: %s\n",i,(v[i]).c_str());
		Parse* p = new Parse();
		//p->parseExpr((char*)(v[i]).c_str());
		const char* temp = (v[i].c_str());
		int len = v[i].size();
		int j = 0;
		while ((j < len) && (temp[j] != '='))
		  j++;
		if (j == len) {
		  printf("No '=' character found in MAP expression. Giving up...\n");
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
		m_expressions.push_back(p->parseExpr(to_parse));
		delete[] to_parse;
	}
	//printf("m_expressions.size: %i\n",m_expressions.size());
	
	m_modifier = modifier;
}

	MapBox::~MapBox()
{
}


vector<Expression*> *MapBox::getExpression()
{
	return &m_expressions;
}

int MapBox::getBoxType()
{
  return MAP;
}

string MapBox::toString()
{
  strstream s;
  s << "MapBox (" << Box::toString() << ", expression: " << m_modifier << ")" << '\0';
  return s.str();
}

