#include "HelloWorldBox.H"
#include <ios>
#include "BoxRecord.H"

/**
 * This is a HelloWorld box, written as a minimal box implementation and to learn how.
 * Richard Tibbetts (tibbetts@mit.edu)
 */

HelloWorldBox::HelloWorldBox(int id, float cost, float sel, string message,
			     int x, int y, int width, int height):
  Box(id, cost, sel, x, y, width, height)
{
  cout << "HelloWorldBox::HelloWorldBox(" << id << ", " << message << ")" << endl;
  this->message = message;
}

/**
 *
 * Return the message
 */
const char *HelloWorldBox::getModifier()
{
  return message.c_str();
}

HelloWorldBox::~HelloWorldBox()
{
}

int HelloWorldBox::getBoxType()
{
  return HELLO_WORLD;
}

string HelloWorldBox::toString()
{
  ostringstream oss;
  oss << "HelloWorldBox (" << this->toString() << ", message: " << message << ")" << '\0';
  return oss.str();
}

