#include "UpdateRelationBox.H"
#include <ios>
#include "BoxRecord.H"

/**
 * This is an UpdateRelation box.
 * Richard Tibbetts (tibbetts@mit.edu)
 */

UpdateRelationBox::UpdateRelationBox(int id, float cost, float sel, string modifier,
			     int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height)
{
  cout << "UpdateRelationBox::UpdateRelationBox(" << id << ", " << modifier << ")" << endl;

  db = relation_environment->createRelation(string("BasicRelationBox"), true)->getDb();

  key_length = (int)strtol(modifier.c_str(), NULL, 10);
  if (key_length == 0) {
    cout << "UpdateRelationBox::UpdateRelationBox Invalid key length." << endl;
    assert(false);
  }
}

UpdateRelationBox::~UpdateRelationBox()
{
  db->close(0);
  delete db;
}

int UpdateRelationBox::getBoxType()
{
  return UPDATE_RELATION;
}

string UpdateRelationBox::toString()
{
  ostringstream oss;
  oss << "UpdateRelationBox (" << Box::toString() << ", key_length: " << key_length << ")" << '\0';
  return oss.str();
}

