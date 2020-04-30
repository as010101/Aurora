#include "ReadRelationBox.H"
#include <ios>
#include "BoxRecord.H"

/**
 * This is an ReadRelation box.
 * Richard Tibbetts (tibbetts@mit.edu)
 */

ReadRelationBox::ReadRelationBox(int id, float cost, float sel, string modifier,
				 int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height)
{
  cout << "ReadRelationBox::ReadRelationBox(" << id << ", " << modifier << ")" << endl;

  db = relation_environment->createRelation(string("BasicRelationBox"), true)->getDb();

  key_length = (int)strtol(modifier.c_str(), NULL, 10);
  if (key_length == 0) {
    cout << "ReadRelationBox::ReadRelationBox Invalid key length." << endl;
    assert(false);
  }
}

ReadRelationBox::~ReadRelationBox()
{
  db->close(0);
  delete db;
}

int ReadRelationBox::getBoxType()
{
  return READ_RELATION;
}

string ReadRelationBox::toString()
{
  ostringstream oss;
  oss << "ReadRelationBox (" << Box::toString() << ", key_length: " << key_length << ")" << '\0';
  return oss.str();
}

