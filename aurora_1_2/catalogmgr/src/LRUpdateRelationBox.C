#include "LRUpdateRelationBox.H"
#include <ios>
#include "BoxRecord.H"

/**
 * This is an LRUpdateRelation box.
 * Richard Tibbetts (tibbetts@mit.edu)
 */

LRUpdateRelationBox::LRUpdateRelationBox(int id, float cost, float sel, string modifier,
					 int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height)
{
  if (getenv("RUNNING_LINEAR_ROAD") == NULL) {
    cerr << "You much set the environment variable RUNNING_LINEAR_ROAD to use"
	 << endl << "Linear Road operators." << endl;
    assert(false);
  }

  cout << "LRUpdateRelationBox::LRUpdateRelationBox(" << id << ", " << modifier << ")" << endl;

  RelationEnvironment *re = relation_environment;
  accts_db = re->createRelation(string("accts"), true)->getDb();
  accts_by_pos_db = re->createRelation(string("accts_by_pos"), false)->getDb();
  daily_db = re->createRelation(string("daily"), true)->getDb();
  stats_db = re->createRelation(string("stats"), true)->getDb();
  acc_db = re->createRelation(string("acc"), true)->getDb();

  magic_number = (int)strtol(modifier.c_str(), NULL, 10);
  if (magic_number == 0) {
    cout << "LRUpdateRelationBox::LRUpdateRelationBox Invalid key length." << endl;
    assert(false);
  }
}

LRUpdateRelationBox::~LRUpdateRelationBox()
{
}

int LRUpdateRelationBox::getBoxType()
{
  return LR_UPDATE_RELATION;
}

string LRUpdateRelationBox::toString()
{
  ostringstream oss;
  oss << "LRUpdateRelationBox (" << Box::toString() << ", magic_number: " << magic_number << ")" << '\0';
  return oss.str();
}
