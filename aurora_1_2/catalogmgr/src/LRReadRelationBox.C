#include "LRReadRelationBox.H"
#include <ios>
#include "BoxRecord.H"

/**
 * This is an LRReadRelation box.
 * Richard Tibbetts (tibbetts@mit.edu)
 */

LRReadRelationBox::LRReadRelationBox(int id, float cost, float sel, string modifier,
				 int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height)
{
  if (getenv("RUNNING_LINEAR_ROAD") == NULL) {
    cerr << "You much set the environment variable RUNNING_LINEAR_ROAD to use"
	 << endl << "Linear Road operators." << endl;
    assert(false);
  }

  cout << "LRReadRelationBox::LRReadRelationBox(" << id << ", " << modifier << ")" << endl;
  
  RelationEnvironment *re = relation_environment;
  accts_db = re->createRelation(string("accts"), true)->getDb();
  accts_by_pos_db = re->createRelation(string("accts_by_pos"), false)->getDb();
  daily_db = re->createRelation(string("daily"), true)->getDb();
  stats_db = re->createRelation(string("stats"), true)->getDb();
  acc_db = re->createRelation(string("acc"), true)->getDb();

  magic_number = (int)strtol(modifier.c_str(), NULL, 10);
  if (magic_number == 0) {
    cout << "LRReadRelationBox::LRReadRelationBox Invalid key length." << endl;
    assert(false);
  }
}

LRReadRelationBox::~LRReadRelationBox()
{
}

int LRReadRelationBox::getBoxType()
{
  return LR_READ_RELATION;
}

string LRReadRelationBox::toString()
{
  ostringstream oss;
  oss << "LRReadRelationBox (" << Box::toString() << ", magic_number: " << magic_number << ")" << '\0';
  return oss.str();
}

