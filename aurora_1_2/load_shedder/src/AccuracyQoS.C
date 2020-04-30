
// author: tatbul@cs.brown.edu

#include "AccuracyQoS.H"
#include "RuntimeGlobals.H"
#include "TraceLogger.H"
#include "logutil.H"

#include <iostream>

LossToleranceQoS::LossToleranceQoS()
{
	_graph.erase(_graph.begin(), _graph.end());
	_cursor = 100;

	_pLogger    = RuntimeGlobals::getTraceLogger();
}	

LossToleranceQoS::LossToleranceQoS(EntryVector graph)
{
	_graph.erase(_graph.begin(), _graph.end());
	_cursor = 100;

	for (int i = 0; i < graph.size(); i++)
	{
		Entry e;
		e.x_value = graph[i].x_value;
		e.utility = graph[i].utility;
		e.slope   = graph[i].slope;
		_graph.push_back(e);
	}

	_pLogger    = RuntimeGlobals::getTraceLogger();
}

LossToleranceQoS::LossToleranceQoS(LossToleranceQoS& qos)
{
	_graph.erase(_graph.begin(), _graph.end());

	for (int i = 0; i < qos._graph.size(); i++)
	{
		Entry e;
		e.x_value = qos._graph[i].x_value;
		e.utility = qos._graph[i].utility;
		e.slope   = qos._graph[i].slope;
		_graph.push_back(e);
	}

	_cursor = qos._cursor;

	_pLogger    = RuntimeGlobals::getTraceLogger();
}

float LossToleranceQoS::getUtility(float x_value)
{
	for (int i = 0; i < _graph.size(); i++)
	{
		if (equalFloats(_graph[i].x_value, x_value)) 
			return(_graph[i].utility);
	}

	_pLogger->log("LS", makeDebugLogLines("in LossToleranceQoS::getUtility(): ",
									"utility not found for ", x_value));
	abort();
}

float LossToleranceQoS::getSlope(float x_value)
{
	for (int i = 0; i < _graph.size(); i++)
	{
		if (equalFloats(_graph[i].x_value, x_value)) 
		{
			return(_graph[i].slope);
		}
	}

	_pLogger->log("LS", makeDebugLogLines("in LossToleranceQoS::getSlope(): ",
									"slope not found for", x_value));
	abort();
}

void LossToleranceQoS::print() 
{
	cout << "Quality of Service Points:" << endl;
	cout << "x-Value Utility         Slope    " << endl;
	cout << "------- ------------    ---------" << endl;
	for (int i = 0; i < _graph.size(); i++)
	{
		cout.width(7);
		cout.fill(' ');
		cout << _graph[i].x_value;
		cout.width(13);
		cout.fill(' ');
		cout << _graph[i].utility;
		cout.width(13);
		cout.fill(' ');
		cout << _graph[i].slope << endl;
	}
}

bool LossToleranceQoS::equalDoubles(double d1, double d2)
{
	if (d1 > d2)
	{
		if (FEPS > (d1-d2))
			return true;
		else
			return false;
	}
	else
	{
		if (FEPS > (d2-d1))
			return true;
		else
			return false;
	}
}

bool LossToleranceQoS::equalFloats(float f1, float f2)
{
	if (f1 > f2)
	{
		if (FEPS > (f1-f2))
			return true;
		else
			return false;
	}
	else
	{
		if (FEPS > (f2-f1))
			return true;
		else
			return false;
	}
}


ValueQoS::ValueQoS()
{
	_intervals.erase(_intervals.begin(), _intervals.end());
}

ValueQoS::ValueQoS(ValueQoS& qos)
{
	_num_intervals = qos._num_intervals;
	_min_val = qos._min_val;
	_max_val = qos._max_val;
	assert(_min_val <= _max_val);

	_intervals.erase(_intervals.begin(), _intervals.end());

	for (int i = 0; i < qos._num_intervals; i++) 
	{
		ValueIntervalEntry ventry;

		ventry.lower_value = qos._intervals[i].lower_value;
		ventry.upper_value = qos._intervals[i].upper_value;
		ventry.utility = qos._intervals[i].utility;
		ventry.relative_frequency = qos._intervals[i].relative_frequency;
		ventry.weighted_utility = qos._intervals[i].weighted_utility;
		ventry.normalized_utility = qos._intervals[i].normalized_utility;
		_intervals.push_back(ventry);
	}
}

ValueQoS::ValueQoS(int num, float min, float max, IntervalVector v)
{
	_num_intervals = num;
	_min_val = min;
	_max_val = max;
	assert(_min_val <= _max_val);

	_intervals.erase(_intervals.begin(), _intervals.end());

	for (int i = 0; i < v.size(); i++) 
	{
		ValueIntervalEntry ventry;

		ventry.lower_value 			= v[i].lower_value;
		ventry.upper_value 			= v[i].upper_value;
		ventry.utility 				= v[i].utility;
		ventry.relative_frequency 	= v[i].relative_frequency;
		ventry.weighted_utility 	= v[i].weighted_utility;
		ventry.normalized_utility 	= v[i].normalized_utility;

		_intervals.push_back(ventry);
	}

	// sort intervals in ascending order on utility
	// if two intervals with same utility, order them in ascending
	// relative frequency
	//
	sortIntervals();
}

LossToleranceQoS *ValueQoS::computeLTQoS()
{
	int i, j, width, p_cursor=100;
	float n_util, slope, u_cursor=1.0;
	EntryVector graph;

	graph.resize(101);

	for (i = 0; i < _num_intervals; i++) 
	{
		width  = int((_intervals[i].relative_frequency)*100);
		n_util = _intervals[i].normalized_utility;
		assert(width > 0);
		slope  = n_util/float(width);

		for (j = p_cursor; j >= p_cursor-width; j--)
		{
			graph[j].x_value = j;
			graph[j].slope = slope;
			if (equalFloats((u_cursor - (slope*(p_cursor - j))), 0))
				graph[j].utility = 0;
			else
				graph[j].utility = u_cursor - (slope*(p_cursor - j));
		}
		u_cursor = u_cursor - n_util;
		p_cursor = p_cursor - width;
	}

	graph[0].x_value = 0;
	graph[0].slope = slope;
	graph[0].utility = 0;

	LossToleranceQoS *qos = new LossToleranceQoS(graph);

	return(qos);
}

string ValueQoS::computeFilterPredicate(float filter_percentage, 
									    float current_percentage, 
										string attr, IntervalVec *iv)
{
	int i;
	float percentage = 100, start, end;
	float width, value1, value2;

	for (i = 0; i < _num_intervals; i++) 
	{
		width = float((_intervals[i].relative_frequency)*100);

		if ((percentage-width >= current_percentage)&&(i < (_num_intervals-1))) 
		{
			percentage = percentage - width;
			continue;
		}
		else
		{
			// _intervals[i] is the interval that we will filter from
			//
			//start = percentage - current_percentage;
			//end   = start + filter_percentage;
			start = 0;
			end   = percentage - current_percentage + filter_percentage;

			assert(width > 0);

			value1 = start/width 
					 * (_intervals[i].upper_value - _intervals[i].lower_value) 
					 + _intervals[i].lower_value;

			value2 = end/width
					 * (_intervals[i].upper_value - _intervals[i].lower_value)
					 + _intervals[i].lower_value;

			break;
		}
	}

	ostringstream val1, val2, min, max;

	if ((attr.data())[3] == 'i')
	{
		val1 << int(value1);
		val2 << int(value2);
		min << int(_min_val);
		max << int(_max_val);
	}
	else
	{
		val1 << showpoint << value1;
		val2 << showpoint << value2;
		min << showpoint << _min_val;
		max << showpoint << _max_val;
	}

	string val1_str = val1.str();
	string val2_str = val2.str();
	string min_str = min.str();
	string max_str = max.str();

	string pred_str;

	// assuming only floats
	//
	IntervalType interval;

	if ((equalFloats(_min_val, value1)) && equalFloats(value2, _max_val))
	{
		pred_str = "";
		iv->clear();
	}
	else if (equalFloats(_min_val, value1))
	{
		pred_str = "AND(" + attr + " > " + val2_str + ", " 
				   + attr + " <= " + max_str + ")";

		interval.left_op   = 0;
		interval.left_val  = value2;
		interval.right_op  = 1;
		interval.right_val = _max_val;
		interval.attribute = attr;
		iv->push_back(interval);
	}
	else if (equalFloats(value2, _max_val))
	{
		pred_str = "AND(" + attr + " >= " + min_str + ", "
				   + attr + " < " + val1_str + ")";

    	interval.left_op   = 1;
    	interval.left_val  = _min_val;
    	interval.right_op  = 0;
    	interval.right_val = value1;
		interval.attribute = attr;
		iv->push_back(interval);
	}
	else
	{
		pred_str = "OR(AND(" + attr + " >= " + min_str + ", " + attr + " < " 
				   + val1_str + "), AND(" + attr + " > " + val2_str + ", " 
				   + attr + " <= " + max_str + "))";

    	interval.left_op   = 1;
    	interval.left_val  = _min_val;
    	interval.right_op  = 0;
    	interval.right_val = value1;
		interval.attribute = attr;
		iv->push_back(interval);

    	interval.left_op   = 0;
    	interval.left_val  = value2;
    	interval.right_op  = 1;
    	interval.right_val = _max_val;
		interval.attribute = attr;
		iv->push_back(interval);

	}

	return(pred_str);
}

void ValueQoS::print() 
{
	cout << "ValueQoS Intervals: " << endl;
	cout << "Interval    Utility   Rel_Freq   W_Utility   N_Utility" << endl;
	cout << "---------   -------   --------   ---------   ---------" << endl;

	for (int i = 0; i < _num_intervals; i++) 
	{
		cout.width(4);
		cout.fill(' ');
		cout << _intervals[i].lower_value;
		cout.width(5);
		cout.fill(' ');
		cout << _intervals[i].upper_value;
		cout.width(10);
		cout.fill(' ');
		cout << _intervals[i].utility;
		cout.width(11);
		cout.fill(' ');
		cout << _intervals[i].relative_frequency;
		cout.width(12);
		cout.fill(' ');
		cout << _intervals[i].weighted_utility;
		cout.width(12);
		cout.fill(' ');
		cout << _intervals[i].normalized_utility << endl;
	}
}

void ValueQoS::sortIntervals()
{
	int i, j;
	bool swap;
	ValueIntervalEntry temp_entry;

	for (i = 0; i < _num_intervals; i++) 
	{
		for (j = 0; j < (_num_intervals-1-i); j++) 
		{ 
			swap = false;

			if (equalFloats(_intervals[j+1].utility, _intervals[j].utility))
			{
				// sort them on relative_frequency
				//
				if (_intervals[j+1].relative_frequency < 
										_intervals[j].relative_frequency) 
				{
					// swap j and j+1
					//
					swap = true;
				}
			}
			else if (_intervals[j+1].utility < _intervals[j].utility) 
			{
				// swap j and j+1
				//
				swap = true;
			}

			if (swap) 
			{
				temp_entry.lower_value = _intervals[j].lower_value;
				temp_entry.upper_value = _intervals[j].upper_value;
				temp_entry.utility = _intervals[j].utility;
				temp_entry.relative_frequency = 
										_intervals[j].relative_frequency;
				temp_entry.weighted_utility = 
										_intervals[j].weighted_utility;
				temp_entry.normalized_utility = 
										_intervals[j].normalized_utility;

				_intervals[j].lower_value = _intervals[j+1].lower_value;
				_intervals[j].upper_value = _intervals[j+1].upper_value;
				_intervals[j].utility = _intervals[j+1].utility;
				_intervals[j].relative_frequency = 
										_intervals[j+1].relative_frequency;
				_intervals[j].weighted_utility = 
										_intervals[j+1].weighted_utility;
				_intervals[j].normalized_utility = 
										_intervals[j+1].normalized_utility;
				
				_intervals[j+1].lower_value = temp_entry.lower_value;
				_intervals[j+1].upper_value = temp_entry.upper_value;
				_intervals[j+1].utility = temp_entry.utility;
				_intervals[j+1].relative_frequency = 
											temp_entry.relative_frequency;
				_intervals[j+1].weighted_utility = 
											temp_entry.weighted_utility;
				_intervals[j+1].normalized_utility = 
											temp_entry.normalized_utility;
			}
		}
	}
}

bool ValueQoS::equalDoubles(double d1, double d2)
{
	if (d1 > d2)
	{
		if (FEPS > (d1-d2))
			return true;
		else
			return false;
	}
	else
	{
		if (FEPS > (d2-d1))
			return true;
		else
			return false;
	}
}

bool ValueQoS::equalFloats(float f1, float f2)
{
	if (f1 > f2)
	{
		if (FEPS > (f1-f2))
			return true;
		else
			return false;
	}
	else
	{
		if (FEPS > (f2-f1))
			return true;
		else
			return false;
	}
}

