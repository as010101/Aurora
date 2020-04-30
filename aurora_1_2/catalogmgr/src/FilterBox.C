#include "FilterBox.H"
#include <strstream.h>
#include "Parse.H"
#include "FunPred.H"
#include "BoxRecord.H"

/**
 * This is the !NEW! filter box, that can take multiple predicates.
 * The modifier comes in like this (regex):
 * (predicate)[,(predicate)]*~[DROP|PASS]
 * The # of predicates gives the # of outputs,
 *  and if you read PASS then there's one last output, which is the
 *  "false" port (if all tuples fail)
 *
 * This class will hold the predicates in a vector of Predicate objects
 *  and will hold a boolean to represent the DROP or PASS (_useFalsePort)
 */


FilterBox::FilterBox(int id, float cost, float sel, string modifier, int x, int y, int width, int height):
Box(id, cost, sel, x, y, width, height)
{

  /**
 _predicates = list<Predicate*>();
  string::size_type idx;
  Parse* p = new Parse();
  //_predicates = new list<Predicate*>;
  //list<Predicate*> _predicates;
  
  idx = modifier.find('~');
  // The useFalsePort boolean...
  if (modifier.substr(idx+1) == "PASS") _useFalsePort = true;
  else _useFalsePort = false;
  // The predicates
  string preds = modifier.substr(0,idx); // careful, 2nd arg is # of chars, not end index
  idx = preds.find(',');
  if (idx == string::npos) { // No "," means only one predicate
    _predicates.push_back(p->parsePred(preds.c_str()));
  } else {
    do {
      _predicates.push_back(p->parsePred(preds.substr(0,idx).c_str()));
      preds = preds.substr(idx+1); // the rest after the comma
      idx = preds.find(',');
    } while (idx != string::npos);
    
  }
	
  */
	//const char *str = modifier.c_str();
	//m_predicate = p->parsePred(str);
	
	// Store the modifier
	//cout << " FilterBox being created -- received modifier (string) as [" << modifier << "]" << endl;
  m_modifier = modifier;
  
}

/**
 *
 * Return the modifier
 */
const char *FilterBox::getModifier()
{
  return m_modifier.c_str();
}

FilterBox::~FilterBox()
{
}

/*
FilterBox::FilterBox(int id, float cost, float sel, string modifier): Box(id, cost, sel) 
{
	m_predicate = new Predicate(modifier);
}

FilterBox::~FilterBox()
{
  delete m_predicate;
}
*/

//list<Predicate*> *FilterBox::getPredicates()
  //Predicate*[] FilterBox::getPredicates()
//{
//	return _predicates;
//}

int FilterBox::getBoxType()
{
  return FILTER;
}

string FilterBox::toString()
{
  strstream s;
  s << "FilterBox (" << Box::toString() << ", original modifier: " << m_modifier << ")" << '\0';
  return s.str();
}

