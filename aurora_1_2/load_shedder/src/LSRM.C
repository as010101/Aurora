
// author: tatbul@cs.brown.edu

#include "LSRM.H"

LSRMEntry::LSRMEntry(IntVector app_ids)
{
	// make sure it is empty
	//
	_dropInsertionPlan.erase(_dropInsertionPlan.begin(), 
							 _dropInsertionPlan.end());

	// init all cursors to 100
	//
	int i;
	for (i = 0; i < app_ids.size(); i++)
	{
		_QoSCursors[app_ids[i]] = 100;
	}

	// init all utilities to 1.0
	//
	_aggUtility = 1.0;
}

LSRMEntry::LSRMEntry(LSRMEntry& entry)
{
	int i;

	_dropInsertionPlan.clear();
	_QoSCursors.clear();

	DropInfoMapIter iter1;
	for (iter1 = entry._dropInsertionPlan.begin(); 
		 iter1 != entry._dropInsertionPlan.end(); iter1++)
	{
		_dropInsertionPlan[iter1->first].arc_id    = (iter1->second).arc_id;
		_dropInsertionPlan[iter1->first].drop_rate = (iter1->second).drop_rate;

		if ((iter1->second).pred_flag)
		{
			_dropInsertionPlan[iter1->first].predicate_str = 
												(iter1->second).predicate_str;
			_dropInsertionPlan[iter1->first].pred_flag = true;
			for (i = 0; i < (iter1->second).intervals.size(); i++)
			{
				_dropInsertionPlan[iter1->first].intervals.push_back(
												(iter1->second).intervals[i]);
			}
		}
		else
		{
			_dropInsertionPlan[iter1->first].pred_flag = false;
			_dropInsertionPlan[iter1->first].intervals.clear();
		}
	}

	FloatMapIter iter2;
	for (iter2 = entry._QoSCursors.begin(); iter2 != entry._QoSCursors.end(); 
		 iter2++)
	{
		_QoSCursors[iter2->first] = iter2->second;
	}

	_aggUtility = entry._aggUtility;
}

LSRMEntry::~LSRMEntry()
{
}

/*
 * This function checks whether a drop info for the given arc id exists or not.
 */
bool LSRMEntry::exists(int arc_id)
{
	if (_dropInsertionPlan.find(arc_id) == _dropInsertionPlan.end())
		return(false);
	else
		return(true);
}

/* 
 * This function sets the loss-tolerance QoS cursor of the given application 
 * to the specified percentage.
 */
void LSRMEntry::setCursor(int app_id, float percentage)
{
	_QoSCursors[app_id] = percentage;
}

void LSRMEntry::setAggUtility(double utility)
{
	_aggUtility = utility;
}

double LSRMEntry::getAggUtility()
{
	return(_aggUtility);
}

void LSRMEntry::insertDrop(DropInfo d_info)
{
	if (_dropInsertionPlan.find(d_info.arc_id) == _dropInsertionPlan.end())
	{
		// it's a new entry
		//
		_dropInsertionPlan[d_info.arc_id].arc_id 	= d_info.arc_id;
		_dropInsertionPlan[d_info.arc_id].drop_rate = d_info.drop_rate;
		if (d_info.pred_flag)
		{
			_dropInsertionPlan[d_info.arc_id].predicate_str =
														d_info.predicate_str;

		    for (int i = 0; i < d_info.intervals.size(); i++)
		    {
				_dropInsertionPlan[d_info.arc_id].intervals.push_back(
														d_info.intervals[i]);
		    }

			_dropInsertionPlan[d_info.arc_id].pred_flag = true;
		}
		else
		{
			_dropInsertionPlan[d_info.arc_id].pred_flag = false;
			_dropInsertionPlan[d_info.arc_id].intervals.clear();
		}
	}
	else
	{
		// it already exists; we need to update its drop rate or predicate
		//
		if (_dropInsertionPlan[d_info.arc_id].pred_flag)
		{
			// if nothing is in the predicate, then it is already false
			//
			if ((_dropInsertionPlan[d_info.arc_id].intervals.empty()) ||
				(d_info.intervals.empty()))
			{
				_dropInsertionPlan[d_info.arc_id].predicate_str = "";
			}
			else
			{
				IntervalVec all_intervals;
				simplifyANDPredicate(
								_dropInsertionPlan[d_info.arc_id].intervals, 
								d_info.intervals,
								&all_intervals);
				string and_pred;
				if (!(all_intervals.empty())) 
				{
					and_pred = makeORPredicate(all_intervals, 0);
				}
				else
				{
					and_pred = "";
				}

				_dropInsertionPlan[d_info.arc_id].intervals.clear();
		    	for (int i = 0; i < all_intervals.size(); i++)
		    	{
					_dropInsertionPlan[d_info.arc_id].intervals.
												push_back(all_intervals[i]);
		    	}

				_dropInsertionPlan[d_info.arc_id].predicate_str = and_pred;
				//_dropInsertionPlan[d_info.arc_id].predicate_str =
				//	"AND(" + _dropInsertionPlan[d_info.arc_id].predicate_str +
				//	", " + d_info.predicate_str + ")";
			}
		}
		else
		{
			_dropInsertionPlan[d_info.arc_id].drop_rate = 1 - 
								(1-_dropInsertionPlan[d_info.arc_id].drop_rate)*
								(1-d_info.drop_rate);
		}

	}
}

void LSRMEntry::simplifyANDPredicate(IntervalVec v, IntervalVec u, 
									 IntervalVec *result)
{
	int i, j;
	result->clear();

	assert(!(v.empty()));
	assert(!(u.empty()));

	for (i = 0; i < v.size(); i++)
	{
		for (j = 0; j < u.size(); j++)
		{
			IntervalType current;

			if ((v[i].right_val < u[j].left_val) ||
				(v[i].left_val > u[j].right_val) ||
				((equalFloats(v[i].right_val, u[j].left_val) && 
				  !(v[i].right_op) && !(u[j].left_op))) ||
				((equalFloats(v[i].left_val, u[j].right_val) &&
                  !(v[i].left_op) && !(u[j].right_op))))
			{
				// no intersection
				//
				continue;
			}
			else
			{
				if (equalFloats(v[i].left_val, u[j].left_val))
				{
					current.left_val = v[i].left_val;

					if (v[i].left_op && u[j].left_op)
						current.left_op = 1;
					else
						current.left_op = 0;
				}
				else if (v[i].left_val > u[j].left_val)
				{
					current.left_val = v[i].left_val;
					current.left_op  = v[i].left_op;
				}
				else
				{
					current.left_val = u[j].left_val;
					current.left_op  = u[j].left_op;
				}

				if (equalFloats(v[i].right_val, u[j].right_val))
				{
					current.right_val = v[i].right_val;

					if (v[i].right_op && u[j].right_op)
						current.right_op = 1;
					else
						current.right_op = 0;
				}
				else if (v[i].right_val < u[j].right_val)
				{
					current.right_val = v[i].right_val;
					current.right_op  = v[i].right_op;
				}
				else
				{
					current.right_val = u[j].right_val;
					current.right_op  = u[j].right_op;
				}
			}

			current.attribute = u[j].attribute;
			result->push_back(current);
		}
	}
}

string LSRMEntry::makeORPredicate(IntervalVec intervals, int offset)
{
	assert(!(intervals.empty()));
	string left_op, right_op, left_val, right_val;

	if (intervals[offset].left_op)
		left_op = ">=";
	else
		left_op = ">";

	if (intervals[offset].right_op)
		right_op = "<=";
	else
		right_op = "<";

	ostringstream val1, val2;
	string attr = intervals[offset].attribute;

	if ((attr.data())[3] == 'i')
	{
		val1 << int(intervals[offset].left_val);
        val2 << int(intervals[offset].right_val);
	}
	else
	{
		val1 << showpoint << intervals[offset].left_val;
    	val2 << showpoint << intervals[offset].right_val;
	}

	left_val = val1.str();
    right_val = val2.str();

	string p = string("AND(" + intervals[offset].attribute + " " + left_op +
                      " " + left_val + ", " + intervals[offset].attribute +
                      " " + right_op + " " + right_val + ")");

	if (offset == (intervals.size()-1))
	{
		return(p);
	}
	else
	{
		return(string("OR(" + p + ", " +
                      makeORPredicate(intervals, ++offset) + ")"));
	}
}

/*
string LSRMEntry::simplifyANDPredicate(PredicateStrVector preds)
{
	float lower, upper;
	char attr1[15], attr2[15], attr3[15];

	float smallest_lower, smallest_lower1, smallest_lower3;
	float largest_upper1, largest_upper2, largest_upper3;

	bool first1 = true, first2 = true, first3 = true;
	int type, r;

	char *pred_str = (char *)malloc(sizeof(char)*50);

	for (int i = 0; i < preds.size(); i++)
	{
		const char *str = preds[i].data();
		cout << str << endl;

		if (str[0] == 'O')	// OR'ed predicate
		{
			r = sscanf(str, "OR(%s < %f, %s >%s)", attr1, &lower, attr2, attr3);

			if (attr3[0] == '=') 
			{
				r = sscanf(str, "OR(%s < %f, %s >= %f)", attr1, &lower, attr2,
					   &upper);
				cout << "type 3" << endl;

				type = 3;
				if (first3) // init
				{
					first3 = false;
					smallest_lower3 = lower;
					largest_upper3 = upper;
				}
			}
			else
			{
				r = sscanf(str, "OR(%s < %f, %s > %f)", attr1, &lower, attr2, 
					   &upper);

				cout << "type 1" << endl;

				type = 1;
				if (first1) // init
				{
					first1 = false;
					smallest_lower1 = lower;
					largest_upper1 = upper;
					cout << "largest_upper1 = " << largest_upper1 << endl;
					cout << "smallest_lower1 = " << smallest_lower1 << endl;
				}
			}
		}
		else if (str[0] == ':')	// simpler predicate
		{
			sscanf(str, "%s >= %f", attr1, &upper);

			cout << "type 2" << endl;

			type = 2;
			if (first2)// init
			{
				first2 = false;
				largest_upper2 = upper;
			}
		}
		else
		{
			cout << "invalid case!!" << endl;
			abort();
		}

		switch (type)
		{
			case 1:
				if (lower < smallest_lower1)
					smallest_lower1 = lower;
				if (upper > largest_upper1)
					largest_upper1 = upper;
				break;
			case 2:
				if (upper > largest_upper2)
					largest_upper2 = upper;
				break;
			case 3:
				if (lower < smallest_lower3)
					smallest_lower3 = lower;
				if (upper > largest_upper3)
					largest_upper3 = upper;
				break;
			default:
				cout << "invalid case!!" << endl;
				abort();
		}
	}

	cout << "ok so far" << endl;

	if (!first1 && !first2 && !first3)
	{
		if (smallest_lower1 < smallest_lower3)
			smallest_lower = smallest_lower1;
		else
			smallest_lower = smallest_lower3;

		if ((largest_upper1 > largest_upper2) && 
			(largest_upper1 > largest_upper3))
		{
			if (largest_upper1 >= smallest_lower)
				sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, smallest_lower,
						attr1, largest_upper1);
			else
				pred_str = NULL;
		}
		else if ((largest_upper2 > largest_upper1) &&
				 (largest_upper2 > largest_upper3))
		{
			if (largest_upper2 > smallest_lower)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						smallest_lower, attr1, largest_upper2);
			else
				pred_str = NULL;
		}
		else if ((largest_upper3 > largest_upper1) &&
				 (largest_upper3 > largest_upper2))
		{
			if (largest_upper3 > smallest_lower)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						smallest_lower, attr1, largest_upper3);
			else
				pred_str = NULL;
		}
	}
	else if (!first1 && !first2 && first3)
	{
		if (largest_upper1 > largest_upper2)
		{
			if (largest_upper1 >= smallest_lower1)
				sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, 
						smallest_lower1, attr1, largest_upper1);
			else
				pred_str = NULL;
		}
		else
		{
			if (largest_upper2 > smallest_lower1)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						smallest_lower1, attr1, largest_upper2);
			else
				pred_str = NULL;
		}
	}
	else if (!first1 && first2 && first3)
	{
		cout << "largest_upper1 = " << largest_upper1 << endl;
		cout << "smallest_lower1 = " << smallest_lower1 << endl;
		if (largest_upper1 >= smallest_lower1)
			sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, smallest_lower1, 
					attr1, largest_upper1);
		else
			pred_str = NULL;
	}
	else if (!first1 && first2 && !first3)
	{
		if (smallest_lower1 < smallest_lower3)
			smallest_lower = smallest_lower1;
		else
			smallest_lower = smallest_lower3;

		if (largest_upper1 > largest_upper3)
		{
			if (largest_upper1 >= smallest_lower)
				sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, smallest_lower,
						attr1, largest_upper1);
			else
				pred_str = NULL;
		}
		else
		{
			if (largest_upper3 > smallest_lower)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						smallest_lower, attr1, largest_upper3);
			else
				pred_str = NULL;
		}
	}
	else if (first1 && !first2 && !first3)
	{
		if (largest_upper2 > largest_upper3)
		{
			if (largest_upper2 > smallest_lower3)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						smallest_lower3, attr1, largest_upper2);
			else
				pred_str = NULL;
		}
		else
		{
			if (largest_upper3 > smallest_lower3)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						smallest_lower3, attr1, largest_upper3);
			else
				pred_str = NULL;
		}
	}
	else if (first1 && !first2 && first3)
	{
		sprintf(pred_str, "%s >= %f)", attr1, largest_upper2);
	}
	else if (first1 && first2 && !first3)
	{
		if (largest_upper3 > smallest_lower3)
			sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, smallest_lower3, 
					attr1, largest_upper3);
		else
			pred_str = NULL;
	}
	else if (first1 && first2 && first3)
	{
		cout << "invalid case!!" << endl;
		abort();
	}

	if (pred_str)
	{
		return(string(pred_str));
	}
	else
	{
		return(string("")) ;
	}
}
*/

void LSRMEntry::deleteDrop(int arc_id)
{
	// make sure it is there
	//
	assert(_dropInsertionPlan.find(arc_id) != _dropInsertionPlan.end());

	// delete it
	//
	_dropInsertionPlan.erase(arc_id);

	// make sure it is not there
	//
	assert(_dropInsertionPlan.find(arc_id) == _dropInsertionPlan.end());
}

void LSRMEntry::updateDropRateBy(int arc_id, double rate)
{
	assert(!equalDoubles(rate, 1));

	_dropInsertionPlan[arc_id].drop_rate =
			1 - double(1-_dropInsertionPlan[arc_id].drop_rate)/double(1-rate);
}

void LSRMEntry::print()
{
	cout << "DIP:" << endl;
	DropInfoMapIter iter;
	for (iter = _dropInsertionPlan.begin(); iter != _dropInsertionPlan.end();
		 iter++)
	{
		cout << iter->second.arc_id << " " << iter->second.drop_rate << " ";

		if (iter->second.pred_flag)
			cout << iter->second.predicate_str.data();
		cout << endl;
	}

	cout << "QoSCursors:" << endl;
	FloatMapIter iter2;
	for (iter2 = _QoSCursors.begin(); iter2 != _QoSCursors.end(); iter2++)
		cout << iter2->second << " ";
	cout << endl;

	cout << "aggUtility: " << _aggUtility << endl;
}

bool LSRMEntry::equalDoubles(double d1, double d2)
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

bool LSRMEntry::equalFloats(float f1, float f2)
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


