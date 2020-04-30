#include <QoS.H>
#include <sstream>


/*
  #include "QoS.H"
  #include <string.h>


  float QoS::getSlope(float point) {
  return 0;
  }

  void QoS::setSlope(float point, float slope) {
  }

  float QoS::getUtility(float point) {

  for (int i=0; i<_graph.size(); i++) {
  if (abs(point-(_graph[i]->x_point)) <= (abs(point)*0.00001))
  return(_graph[i]->utility);
  }
  cout << "Error: QoS::getUtility: given point does not exist!" << endl;
  return(0);	// future work: interpolation function
  }

  void QoS::setUtility(float point, float utility) {

  for (int i=0; i<_graph.size(); i++) {
  if (abs(point-(_graph[i]->x_point)) <= (abs(point)*0.00001)) {
  _graph[i]->utility = utility;
  return;
  }
  }   
  QoSEntry *new_entry = (QoSEntry *)malloc(sizeof(QoSEntry));
  new_entry->x_point = point;
  new_entry->utility = utility;
  _graph.push_back(new_entry);
  }

  float QoS::getCursor() {
  return(_cursor);
  }

  void QoS::setCursor(float cursor) {

  _cursor = cursor;
  }

  QoSEntryVector QoS::getGraph() {

  return(_graph);
  }

  void QoS::setGraph(QoSEntryVector g) {

  _graph = g;
  }

  int QoS::getGraphSize() {

  return(_graph.size());
  }

  void QoS::printQoS() {

  cout << "QoS Graph:" << endl;
  cout << "cursor = " << _cursor << endl;
  for (int i=0; i<_graph.size(); i++) {
  cout << "Entry " << i << ": x_point = " << _graph[i]->x_point
  << ", utility = " << _graph[i]->utility << endl;
  }
  }

  void MessageQoS::printQoS() {

  cout << "Message QoS Graph:" << endl;
  cout << "cursor = " << _cursor << endl;
  for (int i=0; i<_graph.size(); i++) {
  cout << "Entry " << i << ": x_point = " << _graph[i]->x_point
  << ", utility = " << _graph[i]->utility << endl;
  }
  }

  DelayQoS::DelayQoS(QoSEntryVector g, float threshold, float upper) : QoS(g) {

  _cursor = 0;
  _threshold = threshold;
  _upper = upper;
  }

  float DelayQoS::getThreshold() {

  return(_threshold);
  }

  void DelayQoS::setThreshold(float threshold) {

  _threshold = threshold;
  }

  float DelayQoS::getUpper() {

  return(_upper);
  }

  void DelayQoS::setUpper(float upper) {

  _upper = upper;
  }

  void DelayQoS::printQoS() {

  cout << "Delay QoS Graph:" << endl;
  cout << "cursor = " << _cursor << endl;
  cout << "threshold = " << _threshold << endl;
  cout << "upper = " << _upper << endl;
  for (int i=0; i<_graph.size(); i++) {
  cout << "Entry " << i << ": x_point = " << _graph[i]->x_point
  << ", utility = " << _graph[i]->utility << endl;
  }	
  }

  ValueQoS::ValueQoS(QoSEntryVector g, int index, float size, float upper, 
  float lower) : QoS(g) {

  _attr_index = index;
  _intSize = size;
  _upper = upper;
  _lower = lower;
  }

  int ValueQoS::getAttrIndex() {
  return(_attr_index);
  }

  void ValueQoS::setAttrIndex(int index) {

  _attr_index = index;
  }

  float ValueQoS::getSize() {
  return(_intSize);
  }

  void ValueQoS::setSize(float size) {

  _intSize = size;
  }

  float ValueQoS::getUpper() {

  return(_upper);
  }

  void ValueQoS::setUpper(float upper) {

  _upper = upper;
  }

  float ValueQoS::getLower() {

  return(_lower);
  }

  void ValueQoS::setLower(float lower) {

  _lower = lower;
  }

  void ValueQoS::printQoS() {

  cout << "Value QoS Graph:" << endl;
  cout << "cursor = " << _cursor << endl;
  cout << "attr_index = " << _attr_index << endl;
  cout << "intSize = " << _intSize << endl;
  cout << "upper = " << _upper << endl;
  cout << "lower = " << _lower << endl;
  for (int i=0; i<_graph.size(); i++) {
  cout << "Entry " << i << ": x_point = " << _graph[i]->x_point
  << ", utility = " << _graph[i]->utility << endl;
  }
  }


*/


QoS::QoS(string modifier)
{
	//cout << "               MODIFIER " << modifier << endl;
	float old_x = -1.0;
	string s;
	int pos = modifier.find('|');
	s = modifier.substr(0, pos);
	modifier = modifier.substr(pos+1, modifier.length()-pos);
	int numberOfPoints = 0;
	sscanf(s.c_str(), "%d", &numberOfPoints);

	pos = modifier.find('|');
	s = modifier.substr(0, pos);
	modifier = modifier.substr(pos+1, modifier.length()-pos);
	m_qosType = 0;
	sscanf(s.c_str(), "%d", &m_qosType);
	for (register int i = 0; i < numberOfPoints; i++)
		{
			pos = modifier.find('|');
			if (pos < 0)
				{
					pos = modifier.length();
					s = modifier.substr(0, pos);
				}
			else
				{
					s = modifier.substr(0, pos);
					modifier = modifier.substr(pos+1, modifier.length()-pos);
				}
			QoSEntry* e = new QoSEntry;
			sscanf(s.c_str(), "%f~%f", &(e->x), &(e->utility));
			//cout << e->x << ", " << e->utility << endl;
			if ( old_x >= 0 )
				{
					//printf(" qos? OLD %f, new %f #pts %d\n", old_x, e->x, i );
					if ( e->x < old_x ) // don't allow illegal sequences of x.
						e->x = old_x+.01;
				}
			old_x = e->x;
			m_graph.push_back(e);
		}
}

QoS::~QoS()
{
}

float QoS::getUtility( float point )
{
	if ( point < 0 )
		{
			printf(" QoS: utility of %f computing, return -1\n", point );
			return -1;
		}
	float left = -1, right = -1;  //assume that graph ranges in [ 0:inf ]
	int i_left, i_right;

	if ( m_graph.size() == 0 )
		{
			perror(" QoS: no qos graph specified\n");
			return -1;
		}

	// -->Alex: please implment body of the method
	for (int i = 0; i < m_graph.size(); i++)
		{
			//printf(" I qos I %d\n", i );
			// left bound
			if ( m_graph[ i ]->x <= point &&
				 ( m_graph[ i ]->x > left || left == -1 ) )
				{
					left = m_graph[ i ]->x;
					i_left = i;
				}

			// right bound
			if ( m_graph[ i ]->x > point &&
				 ( m_graph[ i ]->x < right || right == -1 ) )
				{
					right = m_graph[ i ]->x;
					i_right = i;
				}
        }

	//cout << " Left " << left << " indexLeft " << i_left << " UT LEFT " << m_graph[ i_left ]->utility << " RIGHT " << right << " indexRigth " << i_right  << " UT RIGHT " << m_graph[ i_right ]->utility <<" WTF? " << endl;
	if ( right == -1 )
		return 0;

	//printf(" QOS done di done %f \n", point );
	return (m_graph[ i_left ]->utility +
			( ( point - m_graph[ i_left ]->x ) *
			  ( (m_graph[ i_right ]->utility - m_graph[ i_left ]->utility) /
				( m_graph[ i_right ]->x - m_graph[ i_left ]->x ) )));
}

QoSEntryVector& QoS::QoS::getGraph()
{
	return m_graph;
}

int QoS::getQosType()
{
	return m_qosType;
}

string QoS::toString()
{
	ostringstream s;
	s << "(type: " << m_qosType;
	if (m_graph.size() > 0)
		{
			s << ", points: ";
			for (register int i = 0; i < m_graph.size(); i++)
				{
					if (i != 0)
						s << ", ";
					s << "<" << m_graph[i]->x << ", " << m_graph[i]->utility << ">";
				}

		}
	s << ")" << '\0';
	return s.str();
}
