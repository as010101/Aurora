#include "Box.H"
#include <strstream.h>

Box::Box(int id, float cost, float sel, int x, int y, int width, int height)
{
  m_id = id;
  m_cost = cost;
  m_selectivity = sel;
  m_x = x;
  m_y = y;
  m_width = width;
  m_height = height;
  m_mutex = new pthread_mutex_t;
  pthread_mutex_init(m_mutex, 0);
}

Box::~Box() {}

int Box::getId()
{
  return m_id;
}

int Box::getX()
{
  return m_x;
}

int Box::getY()
{
  return m_y;
}

int Box::getWidth()
{
  return m_width;
}

int Box::getHeight()
{
  return m_height;
}

float Box::getCost()
{
  return m_cost;
}

void Box::setCost(float cost)
{
  m_cost = cost;
}

float Box::getSelectivity() 
{
  return m_selectivity;
}

void Box::setSelectivity(float selectivity)
{
  m_selectivity = selectivity;
}

int Box::getBoxType()
{
  cout << " HEY! DONT CALL ME! " << endl;
  return 999;
}

void Box::lockBox()
{
  pthread_mutex_lock(m_mutex);
}

int Box::lockBoxTry()
{
  return pthread_mutex_trylock(m_mutex);
}

Predicate *Box::getPredicate()
{
  return NULL;
}
vector<Expression*> *Box::getExpression()
{
  return NULL;
}

const char *Box::getModifier()
{
  return NULL;
}

AggregateState *Box::getState()
{
  return NULL;
}

float Box::getDropRate()
{
  return 0.0;
}

void Box::unlockBox()
{
  pthread_mutex_unlock(m_mutex);
}

void Box::addParentAppl( int appl )
{
  //list<int>::iterator iter = appl_ids.begin();

  if ( !isDescendantOf( appl ) )
    {
      //      printf("Box %d: adding a parent application %d\n", getId(), appl );
      appl_ids.push_back( appl );
    }
  //  else
  //    printf("Box: Duplicate %d in box %d. not added\n", appl, getId() );
}

int Box::getParentAppl()
{
  return (int)(*appl_ids.begin());
}

list<int> Box::getParentAppls()
{
  //  list<int>::iterator iter = appl_ids.begin();
  //  for ( iter = appl_ids.begin(); iter != appl_ids.end(); iter++ )
  //  printf("NOW AT getParentAppls%d\n", (*iter) );


  return appl_ids;
}


int Box::isDescendantOf( int appl )
{
  //printf(" DEBUG: call is desc %d sz %d\n", appl, appl_ids.size() );
  int sz = appl_ids.size();
  appl_ids.remove( appl );

  if ( appl_ids.size() < sz )
    {
      appl_ids.push_front( appl );
      return 1;
    }

  return 0;
}

string Box::toString()
{
  strstream s;
  s << "id: " << m_id << ", cost: " << m_cost << ", selectivity: " << m_selectivity << '\0';
  return s.str();
}

/*
AggregateState* Box::getState()
{
  return m_state;
}
*/
