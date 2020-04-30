
// author: tatbul@cs.brown.edu

#include "DropLocations.H"

#include <iostream>

DropLocation::DropLocation()
{
	_app_ids.clear();
	_arc_ids.clear();
	_qos = NULL;
}

DropLocation::DropLocation(DropLocation& dl)
{
	int i;

	IntVector app_ids = dl._app_ids;

	for (i = 0; i < app_ids.size(); i++) 
		_app_ids.push_back(app_ids[i]);

	IntVector arc_ids = dl._arc_ids;
	
	for (i = 0; i < arc_ids.size(); i++)
		_arc_ids.push_back(arc_ids[i]);

	if (dl._qos)
		_qos = new LossToleranceQoS(*(dl._qos));
	else
		_qos = NULL;
}

DropLocation::~DropLocation()
{
	delete _qos;
}

/* 
 * This function returns the comparison result for v and _app_ids
 * BoolVector[0] for same
 * BoolVector[1] for v is a subset
 * BoolVector[2] for intersect
 */
void DropLocation::compareAppIds(IntVector v, BoolVector *result)
{
	int i, j;
	bool found;

	for (i = 0; i < 3; i++)
		result->push_back(false);

	if (isSymmetricSubset(v, _app_ids)) 
	{
		(*result)[1] = true;
		(*result)[2] = true;
		if (_app_ids.size() == v.size())
			(*result)[0] = true;
	}
	else 
	{	
		// still need to check for intersect
		//
		for (i = 0; i < v.size(); i++) 
		{
			found = false;
			for (j = 0; j < _app_ids.size(); j++) 
			{
				if (v[i] == _app_ids[j]) 
				{
					found = true;
					(*result)[2] = true;
					break;
				}
			}
			if (found)
				break;
		}
	}
}

/*
 * This function checks if v is a subset of u, OR, if u is a subset of v.
 */
bool DropLocation::isSymmetricSubset(IntVector v, IntVector u)
{
	int i, j;
	bool found, subset = true;

	for (i = 0; i < v.size(); i++) 
	{
		found = false;
		for (j = 0; j < u.size(); j++) 
		{
			if (v[i] == u[j]) 
			{
				found = true;
				break;
			}
		}
		if (!found) 
		{
			subset = false;
			break;
		}
	}

	if (subset)
		return true;

	subset = true;

	for (j = 0; j < u.size(); j++) 
	{
		found = false;
		for (i = 0; i < v.size(); i++) 
		{
			if (u[j] == v[i]) 
			{
				found = true;
				break;
			}
		}
		if (!found) 
		{
			subset = false;
			break;
		}
	}

	return subset;
}

/* 
 * This function checks if v is a subset of u.
 */
bool DropLocation::isSubset(IntVector v, IntVector u)
{
	int i, j;
	bool found;

	for (i = 0; i < v.size(); i++) 
	{
		found = false;
		for (j = 0; j < u.size(); j++) 
		{
			if (v[i] == u[j]) 
			{
				found = true;
				break;
			}
		}
		if (!found) 
			return false;
	}

	return true;
}

void DropLocation::addArc(int arc_id)
{
	int i;

	// if not already there, insert it
	//
	for (i = 0; i < _arc_ids.size(); i++) 
	{
		if (_arc_ids[i] == arc_id)
			return;
	}

	_arc_ids.push_back(arc_id);
}

void DropLocation::unionAppIds(IntVector app_ids1, IntVector app_ids2)
{
	int i, j;

	// insert app_ids1 members directly
	//
	for (i = 0; i < app_ids1.size(); i++)
		_app_ids.push_back(app_ids1[i]);

	// insert app_ids2 members that are not in app_ids1
	//
	for (j = 0; j < app_ids2.size(); j++) 
	{
		bool found = false;
		for (i = 0; i < app_ids1.size(); i++) 
		{
			if (app_ids2[j] == app_ids1[i]) 
			{
				found = true;
				break;
			}
       }
       if (!found)
             _app_ids.push_back(app_ids2[j]);
	}
}

void DropLocation::setArcIds(IntVector arc_ids)
{
	for (int i = 0; i < arc_ids.size(); i++)
		_arc_ids.push_back(arc_ids[i]);
}

void DropLocation::setAppIds(IntVector app_ids)
{
	for (int i = 0; i < app_ids.size(); i++)
		_app_ids.push_back(app_ids[i]);
}

void DropLocation::print()
{
	int i;

	cout << "App_ids: ";
	for (i = 0; i < _app_ids.size(); i++)
		cout << _app_ids[i] << " ";
	cout << endl;

	cout << "Arc_ids: ";
	for (i = 0; i < _arc_ids.size(); i++)
		cout << _arc_ids[i] << " ";
	cout << endl;

	if (_qos)
		_qos->print();
}
