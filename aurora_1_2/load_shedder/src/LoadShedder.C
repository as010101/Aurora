
// author: tatbul@cs.brown.edu & ksenthil@cs.brown.edu

#include <Parse.H>
#include <logutil.H>
#include <TypeManager.H>
#include <RuntimeGlobals.H>
#include <LoadShedder.H>

LoadShedder::LoadShedder(string mode, unsigned int period, string attribute) 
{
	_map.clear();
	_locations.clear();
	_lg_entries.clear();

	if (mode == "DropLS")
		_mode = DropLS;
	else if (mode == "FilterLS")
		_mode = FilterLS;
	else 
		_mode = NoLS;

	_period 	= period;
	_attribute 	= attribute;
	_level		= -1;	// no LSRM entry has been applied yet
	_utility	= 1.0;	// initially, no loss on Accuracy utility

	// note: we are now reading this from scheduler's global.H
	// later we have to read it from RuntimeGlobals
	//
	_arc_map 	= _catalog->getQNet()->getArcs();	
	fillNetTopology();
	_legal = checkBoxLegality();
	processApps();	
	setDropLocations();

	//_current_stats = new StatsImage(*(_catalog->getQNet()));
	_current_stats = NULL;

	_ls_level_mutex = new pthread_mutex_t;
	pthread_mutex_init(_ls_level_mutex, 0);

	_lsrm_mutex = new pthread_mutex_t;
	pthread_mutex_init(_lsrm_mutex, 0);

	_pDelayedDM = RuntimeGlobals::getDelayedDataMgr();
	_pLogger 	= RuntimeGlobals::getTraceLogger();
}

LoadShedder::~LoadShedder()
{
	pthread_mutex_destroy(_ls_level_mutex);
	pthread_mutex_destroy(_lsrm_mutex);
}

/* 
 * This is the main update loop for the LoadShedder. It periodically reads
 * the system stats and refreshes the LS curve. 
 * Aurora main thread has to use this as the function of a thread it will
 * create for the LoadShedder. 
 */
void LoadShedder::run() throw () 
{
	bool new_lsrm = false;

	while (__global_stop.getStop() != true) { 

		// read the current stats and construct the LSRM
		//
		_current_stats = _pDelayedDM->getReadableImage();
		if (_current_stats)
		{
			if (lockMutex(_lsrm_mutex) == 0)
			{
				new_lsrm = constructLSRM();
				_pDelayedDM->releaseReadableImage(_current_stats);

				// if a new LSRM has been constructed, push new LS curve
				// to the stats
				//
				if (new_lsrm) {

					StatsImage *s = &(_pDelayedDM->getWritableImage());
				
					s->_utilityVsLsPrediction.clear();
					for (int i = 0; i < _map.size(); i++) 
						s->_utilityVsLsPrediction.push_back(
													_map[i]->getAggUtility());

					_pDelayedDM->releaseWritableImage();

					new_lsrm = false;
				}

				unlockMutex(_lsrm_mutex);
			}
			else
			{
				_pDelayedDM->releaseReadableImage(_current_stats);
				_pLogger->log("LS",
						  "in LoadShedder::run(): couldn't update the LSRM");
			}
		}
		else
		{
			_pDelayedDM->releaseReadableImage(_current_stats);
		}
			
		sleep(_period);
	}
}

/*
 * This function sets the desired accuracy utility level requested by the
 * Performance Monitor. It is to be called by the PerformanceMonitor thread.
 */
void LoadShedder::setUtilityLevel(double utility)
{
	int i;
	bool found = false;

	if (lockMutex(_ls_level_mutex) == 0)
	{
		if (lockMutex(_lsrm_mutex) == 0)
		{
			/*
			if (equalDoubles(1.0, utility) && (_mode == DropLS))
			{
				_level = -1;
				_utility = 1.0;
				_pLogger->log("LS",
					"in LoadShedder::setUtilityLevel(): utility set to 1.0");
				cout << "1in LoadShedder::setUtilityLevel(): utility set to 1.0"
					 << endl;
			}
			*/
			if (_map.empty())
			{
				_level = -1;
				_utility = 1.0;
				_pLogger->log("LS", 
						"in LoadShedder::setUtilityLevel(): LSRM is empty");
			}
			else
			{
				for (i = 0; i < _map.size(); i++)
				{
					if (equalDoubles(_map[i]->getAggUtility(), utility) ||
						(_map[i]->getAggUtility() < utility))
					{
						found = true;
						break;
					}
				}

				if (found)
				{
					_level = i;
					_utility = utility;
				}
				else
				{
					// if not found, don't change the level and the utility
					//
					_pLogger->log("LS",
						makeDebugLogLines("in LoadShedder::setUtilityLevel(): ",
										  "utility level not found:", utility));
				}
			}

			// push the new drop info to the stats object
			// also, assign the values for expected accuracy/utility
			//
			StatsImage *s = &(_pDelayedDM->getWritableImage());
			
			// initialize
			//
			s->_dropsOnArcs.clear();
			AppTopMapIter aiter;
			for (aiter = _topology.apps.begin(); aiter != _topology.apps.end(); 
				 aiter++)
			{
				(s->_appArcsStats[aiter->first])._accuracy = 100;
				(s->_appArcsStats[aiter->first])._accuracyUtility = 1.0;
			}

			if (_level > -1)
			{
				DropInfoMap plan = _map[_level]->_dropInsertionPlan;
				DropInfoMapIter iter;
				for(iter = plan.begin(); iter != plan.end(); iter++)
				{
					if ((iter->second).pred_flag)
					{
						if ((iter->second).predicate_str == "")
						{
							s->_dropsOnArcs[iter->first] = "FALSE";
						}
						else
						{
							s->_dropsOnArcs[iter->first] = 
								reFormatAttribute((iter->second).predicate_str);
						}
					}
					else if ((iter->second).drop_rate > 0)
					{
						ostringstream dr_str;
						dr_str << (iter->second).drop_rate;
						s->_dropsOnArcs[iter->first] = dr_str.str();
					}
					else
					{
						_pLogger->log("LS", 
						makeDebugLogLines("in LoadShedder::setUtilityLevel(): ",
										  "no drop info on arc ", iter->first));
					}
				}

				FloatMap cursors = _map[_level]->_QoSCursors;
				FloatMapIter fiter;
				for(fiter = cursors.begin(); fiter != cursors.end(); fiter++)
				{
					(s->_appArcsStats[fiter->first])._accuracy = fiter->second;
					(s->_appArcsStats[fiter->first])._accuracyUtility =
						_topology.apps[fiter->first].lt_qos->
													getUtility(fiter->second);
				}
			}
			_pDelayedDM->releaseWritableImage();

			unlockMutex(_lsrm_mutex);
		}
		else
		{
	 		_pLogger->log("LS", 
				"in LoadShedder::setUtilityLevel(): couldn't read the LSRM");
		}
		unlockMutex(_ls_level_mutex);
	}
	else
	{
	 	_pLogger->log("LS", 
	    "in LoadShedder::setUtilityLevel(): couldn't update the utility level");
	}
}

/* 
 * This function returns the drop information for the given box at the
 * current load shedding level set by the Performance Monitor.
 * If the level is -1, then no load shedding is to be done.
 * Note: Map entries are indexed by arc_id's.
 */
void LoadShedder::fillLSInfo(int box_id, QBoxDropInfoMap *ls_info)
{
	if (lockMutex(_ls_level_mutex) == 0)
	{
		if (_level == -1)
		{
	 		_pLogger->log("LS", 
			   "in LoadShedder::fillLSInfo(): level is set to NO LOADSHEDDING");

			unlockMutex(_ls_level_mutex);
			return;
		}

		// find the arcs preceeding the box with id = box_id
		//
		assert(_topology.boxes.find(box_id) != _topology.boxes.end());

		IdList arcs = _topology.boxes[box_id].prev;	
		IdListIter iter;

		if (lockMutex(_lsrm_mutex) == 0)
		{

			for (iter = arcs.begin(); iter != arcs.end(); iter++)
			{
				assert(iter->type == ARC);
				assert(_topology.arcs.find(iter->id) != _topology.arcs.end());

				DropInfoMap plan = _map[_level]->_dropInsertionPlan;

				// if a drop entry for that arc exists, insert it to the ls_info
				//
				if (plan.find(iter->id) != plan.end())
				{
					QBoxDropInfo *d_info = (QBoxDropInfo *)
										   malloc(sizeof(QBoxDropInfo));

					if (plan[iter->id].pred_flag) 
					{
						//cout << "drop predicate is: " 
						//	 << plan[iter->id].predicate_str << endl;

						if (plan[iter->id].predicate_str != "")
						{
							Parse *parser = new Parse();
							d_info->drop_predicate = parser->parsePred(
										(plan[iter->id].predicate_str).data());
							d_info->drop_rate = plan[iter->id].drop_rate;
							delete(parser);
						}
						else
						{
							d_info->drop_predicate = NULL;
							d_info->drop_rate = plan[iter->id].drop_rate;
						}
					}
					else if (plan[iter->id].drop_rate > 0) 
					{
						d_info->drop_rate      = plan[iter->id].drop_rate;
						d_info->drop_predicate = NULL;
					}
					else 
					{	
	 					_pLogger->log("LS", 
								"in LoadShedder::fillLSInfo(): invalid case!");
						free(d_info);
						continue;
					}

					(*ls_info)[iter->id] = d_info;
				}
			}

			unlockMutex(_lsrm_mutex);
		}
		else
		{
	 		_pLogger->log("LS", 
					"in LoadShedder::fillLSInfo(): couldn't read the LSRM");
		}

		unlockMutex(_ls_level_mutex);
	}
	else
	{
		_pLogger->log("LS", makeDebugLogLines( "in LoadShedder::fillLSInfo(): ",
					"couldn't read load shedding info for box ", box_id));
	}
}

bool LoadShedder::isALegalNetwork()
{
	if (_legal)
		_pLogger->log("LS", "in LoadShedder::isALegalNetwork(): true");
	else
		_pLogger->log("LS", "in LoadShedder::isALegalNetwork(): false");

	return(_legal);
}

bool LoadShedder::checkBoxLegality()
{
	int type;

	BoxTopMapIter iter;

	for (iter = _topology.boxes.begin(); iter != _topology.boxes.end(); iter++)
	{
		type = _catalog->getQNet()->getBox((iter->second).id.id)->getBoxType();

		if (!isLegal(type))
			return(false);
	}

	return(true);
}

bool LoadShedder::isLegal(int box_type)
{
	if (_mode == DropLS)
	{
		if ((box_type == FILTER) || (box_type == MAP) || (box_type == UNION))
			return true;
		else
			return false;
	}
	else if (_mode == FilterLS)
	{
		if ((box_type == FILTER) || (box_type == JOIN) || (box_type == UNION))
			return true;
		else
			return false;
	}
	else
		return false;
}

/*
 * This function fills in the _topology field of the LoadShedder.
 */
void LoadShedder::fillNetTopology()
{
	int i;
	Id arc_id; 
	ArcMapIter aiter; 
	for (aiter = _arc_map.begin(); aiter != _arc_map.end(); aiter++) 
	{
		arc_id.id   = aiter->second->getId(); 
		arc_id.type = ARC; 

	    // if this arc is both an input and an output arc, then
		// it must be connecting an INPUT to an APP.
		//
	    if ((aiter->second->isInputArc()) && (aiter->second->isOutputArc())) 
		{
			// since multiple arcs can have the same input stream as
			// their input, we need to check to make sure that this
			// particular input has not already been added to the
			// _topology before.
			//
			if (_topology.inputs.find(aiter->second->getSourceId()) == 
													_topology.inputs.end())
			{
				InputTop input_top; 

				input_top.id.id   = aiter->second->getSourceId();
				input_top.id.type = INPUT;
				input_top.next.push_back(arc_id);

				_topology.inputs[aiter->second->getSourceId()] = input_top;
			}
			else
			{
				_topology.inputs[aiter->second->getSourceId()].next.
															push_back(arc_id);
			}

			// since an app can only have one incoming arc, this can't be
			// an existing arc. so, we can directly insert it.
			//
			AppTop app_top;

			app_top.id.id     = aiter->second->getDestinationId(); 
			app_top.id.type   = APP; 
			app_top.prev.id   = arc_id.id;     
			app_top.prev.type = arc_id.type;

			// assign the QoS fields of the app_top
			//
			Application *app = 
							_catalog->getQNet()->getApplication(app_top.id.id);

			if (_mode == DropLS)
			{
				// assign the lt_qos
				//
				EntryVector graph;
				QoSEntryVector qev = app->getQoS(1)->getGraph();
				for (i = 0; i < qev.size(); i++)
				{
					Entry e;
					e.x_value = qev[i]->x; 
					e.utility = qev[i]->utility;
					graph.push_back(e);
				}
				formatLTQoS(&graph);
				app_top.lt_qos  = new LossToleranceQoS(graph);
				app_top.val_qos = NULL;
			}
			else if (_mode == FilterLS)
			{
				// assign the val_qos
				//
				float min, max;
				IntervalVector intervals;
				QoSEntryVector qev = app->getQoS(2)->getGraph();
				for (i = 0; i < (qev.size()-1); i++)
				{
					ValueIntervalEntry v;
					v.lower_value = qev[i]->x;
					if (i < (qev.size()-2))
						//v.upper_value =	qev[i+1]->x-1;
						v.upper_value =	qev[i+1]->x;
					else
					{
						v.upper_value = qev[i+1]->x;
						max = v.upper_value;
					}
					v.utility     =	qev[i]->utility;
					v.relative_frequency = 0;
					v.weighted_utility = 0;
					v.normalized_utility = 0;
					intervals.push_back(v);
				}
				min = intervals[0].lower_value;
				app_top.val_qos = 
						new ValueQoS(intervals.size(), min, max, intervals);
			}
			else
			{
				app_top.lt_qos = NULL;
				app_top.val_qos = NULL;
			}

			_topology.apps[aiter->second->getDestinationId()] = app_top;
	    } 
	    
	    // if this arc is just an input arc, then it must be connecting
		// an INPUT to a BOX.
		//
		else if (aiter->second->isInputArc()) 
		{
			// since multiple arcs can have the same input stream as 
			// their input, we need to check to make sure that this 
			// particular input has not already been added to the 
			// _topology before.
			// 
			if (_topology.inputs.find(aiter->second->getSourceId()) ==
													_topology.inputs.end())
			{
				InputTop input_top; 

		    	input_top.id.id   = aiter->second->getSourceId(); 
		    	input_top.id.type = INPUT;
		    	input_top.next.push_back(arc_id); 

		    	_topology.inputs[aiter->second->getSourceId()] = input_top;
			}
			else 
			{
				_topology.inputs[aiter->second->getSourceId()].next.
															push_back(arc_id);
			}
		 
			if (_topology.boxes.find(aiter->second->getDestinationId()) ==
														_topology.boxes.end()) 
			{ 
		    	BoxTop box_top;
		    
		    	box_top.id.id   = aiter->second->getDestinationId(); 
		    	box_top.id.type = BOX; 
		    	box_top.prev.push_back(arc_id); 

		    	_topology.boxes[aiter->second->getDestinationId()] = box_top;
			}
			else 
			{
				_topology.boxes[aiter->second->getDestinationId()].prev.
		    												push_back(arc_id); 
			}
	    }

	    // if this arc is just an output arc, then it must be connecting
		// a BOX to an APP.
		//
	    else if (aiter->second->isOutputArc()) 
		{
			if (_topology.boxes.find(aiter->second->getSourceId()) == 
														_topology.boxes.end()) 
			{
		    	BoxTop box_top;
		    
		    	box_top.id.id   = aiter->second->getSourceId(); 
		    	box_top.id.type = BOX; 
		    	box_top.next.push_back(arc_id); 

		    	_topology.boxes[aiter->second->getSourceId()] = box_top;
			}
			else 
			{
				_topology.boxes[aiter->second->getSourceId()].next.
		    												push_back(arc_id); 
			}

			// since an app can only have one incoming arc, this can't be
			// an existing arc. so, we can directly insert it.
			//
			assert(_topology.apps.find(aiter->second->getDestinationId()) == 
														_topology.apps.end());
			AppTop app_top; 

	    	app_top.id.id     = aiter->second->getDestinationId(); 
	    	app_top.id.type   = APP;
	    	app_top.prev.id   = arc_id.id; 
	    	app_top.prev.type = arc_id.type; 

			// assign the QoS fields of the app_top
			//
			Application *app = 
							_catalog->getQNet()->getApplication(app_top.id.id);

			if (_mode == DropLS)
			{
				// assign the lt_qos
				//
				EntryVector graph;
				QoSEntryVector qev = app->getQoS(1)->getGraph();
				for (i = 0; i < qev.size(); i++)
				{
					Entry e;
					e.x_value = qev[i]->x; 
					e.utility = qev[i]->utility;
					graph.push_back(e);
				}
				formatLTQoS(&graph);
				app_top.lt_qos  = new LossToleranceQoS(graph);
				app_top.val_qos = NULL;
			}
			else if (_mode == FilterLS)
			{
				// assign the val_qos
				//
				float min, max;
				IntervalVector intervals;
				QoSEntryVector qev = app->getQoS(2)->getGraph();
				for (i = 0; i < (qev.size()-1); i++)
				{
					ValueIntervalEntry v;
					v.lower_value = qev[i]->x;
					if (i < (qev.size()-2))
						//v.upper_value =	qev[i+1]->x-1;
						v.upper_value =	qev[i+1]->x;
					else
					{
						v.upper_value = qev[i+1]->x;
						max = v.upper_value;
					}
					v.utility     =	qev[i]->utility;
					v.relative_frequency = 0;
					v.weighted_utility = 0;
					v.normalized_utility = 0;
					intervals.push_back(v);
				}
				min = intervals[0].lower_value;
				app_top.val_qos = 
						new ValueQoS(intervals.size(), min, max, intervals);
			}
			else 
			{
				app_top.lt_qos = NULL;
				app_top.val_qos = NULL;
			}

	    	_topology.apps[aiter->second->getDestinationId()] = app_top;
	    }

	    // if this arc is neither an input nor an output arc, then 
		// it must be connecting a BOX to another BOX.
	    // 
	    else if ((!aiter->second->isInputArc()) && 
		     	 (!aiter->second->isOutputArc())) 
		{
			// if the previous box is not in the _topology, then insert it.  
			//
			if (_topology.boxes.find(aiter->second->getSourceId()) == 
														_topology.boxes.end()) 
			{
				BoxTop box_top_prev;

		    	box_top_prev.id.id   = aiter->second->getSourceId(); 
		    	box_top_prev.id.type = BOX; 
		    	box_top_prev.next.push_back(arc_id); 

		    	_topology.boxes[aiter->second->getSourceId()] = box_top_prev; 
			}
			else 
			{
				_topology.boxes[aiter->second->getSourceId()].next.
															push_back(arc_id);
			}

			// if the next box is not in the _topology, then insert it.
			//
			if (_topology.boxes.find(aiter->second->getDestinationId()) == 
														_topology.boxes.end()) 
			{ 
				BoxTop box_top_next;

		    	box_top_next.id.id   = aiter->second->getDestinationId(); 
		    	box_top_next.id.type = BOX; 
		    	box_top_next.prev.push_back(arc_id); 

		    	_topology.boxes[aiter->second->getDestinationId()] = 
																box_top_next; 
			}	
			else 
			{
				_topology.boxes[aiter->second->getDestinationId()].prev.
															push_back(arc_id);
			}
	    }

	    // finally, we need to add this arc to _topology
		//
	    ArcTop arc_top; 

	    arc_top.id.id   = aiter->second->getId(); 
	    arc_top.id.type = ARC; 
	    arc_top.prev.id = aiter->second->getSourceId(); 
	    arc_top.next.id = aiter->second->getDestinationId(); 

	    // if this arc is coming from an input stream
		//
	    if (aiter->second->isInputArc()) 
			arc_top.prev.type = INPUT; 
	    else 
			arc_top.prev.type = BOX; 

	    // if this arc is going to an app
		//
	    if (aiter->second->isOutputArc()) 
			arc_top.next.type = APP; 
	    else 
			arc_top.next.type = BOX; 

	    _topology.arcs[aiter->second->getId()] = arc_top; 
	}
}

void LoadShedder::printNetTopology()
{
	cout << "Network Topology:" << endl;
	cout << "-----------------" << endl;

	cout << "INPUTS:" << endl;
	InputTopMapIter iter1;
	for (iter1 = _topology.inputs.begin(); iter1 != _topology.inputs.end();
		 iter1++)
		cout << (iter1->second).id.id << " ";
	cout << endl;

	cout << "ARCS:" << endl;
	ArcTopMapIter iter2;
	for (iter2 = _topology.arcs.begin(); iter2 != _topology.arcs.end();
		 iter2++)
		cout << (iter2->second).id.id << ": " << (iter2->second).rate_coef
			 << " " << (iter2->second).load_coef << endl;

	cout << "BOXES:" << endl;
	BoxTopMapIter iter3;
	for (iter3 = _topology.boxes.begin(); iter3 != _topology.boxes.end();
         iter3++)
	{
		cout << (iter3->second).id.id << ": ";
		for (int i = 0; i < ((iter3->second).to_apps).size(); i++)
			cout << ((iter3->second).to_apps)[i] << " ";
		cout << endl;
	}

	cout << "APPS:" << endl;
	AppTopMapIter iter4;
	for (iter4 = _topology.apps.begin(); iter4 != _topology.apps.end(); iter4++)
	{
		cout << (iter4->second).id.id << ": " << endl;
		if ((iter4->second).lt_qos)
			(iter4->second).lt_qos->print();
		if ((iter4->second).val_qos)
			(iter4->second).val_qos->print();
	}
}

void LoadShedder::printLGRatios()
{
	cout << "LG Ratios:" << endl;
	cout << "----------" << endl;

	for (int i = 0; i < _lg_entries.size(); i++)
	{
		cout << "RatioEntry " << i << ":";
		cout << _lg_entries[i].loc_id << " " << _lg_entries[i].max_per << " "
			 << _lg_entries[i].min_per << " " << _lg_entries[i].slope << " "
			 << _lg_entries[i].lg_ratio << endl;
	}
}

/* This function fills in interpolated points for the given LT-QoS graph.
 * The aim is to have one point per percentage value (i.e., x_value).
 * The assumption is that, even though x-axis is of type float, we 
 * always care about discrete integer percentage values 
 * (i.e., 0, 1, 2, .., 100). It also computes slope values for each point.
 */
void LoadShedder::formatLTQoS(EntryVector *graph)
{
	int i;

	assert(graph != NULL);
	assert(graph->size() > 0);

    // first, create a temp entry vector with interpolated points and slopes
	//
	EntryVector temp;
	temp.push_back((*graph)[graph->size()-1]);

	for (i = graph->size()-2; i >= 0; i--)
	{
		assert((*graph)[i+1].utility >= (*graph)[i].utility);
		assert((*graph)[i+1].x_value > (*graph)[i].x_value);

		float slope = ((*graph)[i+1].utility-(*graph)[i].utility)/
					   ((*graph)[i+1].x_value-(*graph)[i].x_value);

		(*graph)[i+1].slope = slope;

		if (i == (graph->size()-2))
			temp[0].slope = slope;

		float x_val = (*graph)[i+1].x_value-1;

		while (x_val > (*graph)[i].x_value)
		{
			Entry e;
			e.x_value = x_val;
			e.utility = (*graph)[i+1].utility -
						((*graph)[i+1].x_value-x_val)*slope; 
			e.slope   = slope;
			temp.push_back(e);
			x_val -= 1;
		}

		(*graph)[i].slope = slope;
		temp.push_back((*graph)[i]);
	}

	// now, replace graph by temp
	//
	graph->erase(graph->begin(), graph->end());

	assert(!temp.empty());
	for (i = temp.size()-1; i >= 0; i--)
		graph->push_back(temp[i]);
}

/* 
 * This function assigns each box the ids of the applications that receive 
 * input from that box.
 */
void LoadShedder::processApps()
{
	AppTopMapIter aiter;

	for (aiter = _topology.apps.begin(); aiter != _topology.apps.end(); aiter++)
	{
	    // get the id of the apps prev field. this must be an arc. 
		// if not, then there is probably an error somewhere.
		//
	    Id arc_id = aiter->second.prev; 
	    assert(arc_id.type == ARC); 

	    // if this arc has a box in front of it, then we need to call
	    // assignApps with this information.
		//
	    ArcTopMapIter arc_iter = _topology.arcs.find(arc_id.id); 
	    assert(arc_iter != _topology.arcs.end()); 
	    assert(arc_iter->second.id.type == ARC); 

	    // assign app ids only if the prev is a box (as opposed to an input).
		//
	    if (arc_iter->second.prev.type == BOX) 
			assignApps(arc_iter->second.prev.id, aiter->second.id.id); 
	}
}

/* 
 * This is a recursive function that assigns app_index to box_id and all
 * the boxes preceedding it.
 */
void LoadShedder::assignApps(int box_id, int app_index)
{
	bool found = false; 

	IntVector iv = _topology.boxes[box_id].to_apps; 

	// go through all the apps already listed in the to_apps
	// field from this box. if the app_index is already there, then 
	// don't do anything.  if it is not there, then insert it.
	//
	for (int i = 0; i < iv.size(); i++) 
	{
	    if (iv[i] == app_index) 
	    {
			found = true; 
			break; 
	    }
	}

	if (!found) 
	{
	    _topology.boxes[box_id].to_apps.push_back(app_index); 

	    IdList prev_list = _topology.boxes[box_id].prev; 
	    IdListIter iter; 

	    // go through the list of arcs that are inputs for this box.
	    // if any of these have boxes as their inputs, then we need to
	    // recurse.
		//
	    for (iter = prev_list.begin(); iter != prev_list.end(); iter++) 
	    {
			ArcTopMapIter arc_iter = _topology.arcs.find((*iter).id); 
			assert(arc_iter != _topology.arcs.end());
			assert(arc_iter->second.id.type == ARC);

			// assign app ids only if the prev is a box.
			//
			if (arc_iter->second.prev.type == BOX) 
		    	assignApps(arc_iter->second.prev.id, app_index); 
	    }
	}
}

/*
 * This function determines the potential locations for drops.
 */
void LoadShedder::setDropLocations()
{
	setAppCombinations();
	setLocations();
}

/* 
 * This function visits all query boxes to collect different application effect
 * sets. For sets that are intersecting, their union is also created as
 * another set.
 */
void LoadShedder::setAppCombinations()
{
	int i, j;
	BoxTopMapIter biter;

	DropLocationsVector temp_dl;
	temp_dl.clear();

	// inserting distinct _to_apps into _locations
	//
	for (biter = _topology.boxes.begin(); biter != _topology.boxes.end(); 
		 biter++)
	{
		BoxTop box = biter->second;
		IntVector app_ids = box.to_apps;

		bool same_found = false;
		bool superset_found = false;
		bool intersect_found = false;

		for (i = 0; i < _locations.size(); i++)
		{
			BoolVector bv;
			bv.clear();
			_locations[i]->compareAppIds(app_ids, &bv);

			// nothing new to be added.
			//
			if (bv[0])
			{
				same_found = true;
				break;
			}
			// for superset, new location needs to be created.
			//
			else if (bv[1])
			{
				superset_found = true;
				continue;
			}
			// for intersection, create a new location as a union.
			//
			else if (bv[2])
			{
				DropLocation *loc = new DropLocation();
				loc->unionAppIds((_locations[i])->_app_ids, app_ids);
				temp_dl.push_back(loc);
				intersect_found = true;
			}
		}

		if (same_found)
			continue;

		// new location if the following condition is met:
		// (superset_found || intersect_found || distinct)
		//
		DropLocation *loc = new DropLocation();
		loc->setAppIds(app_ids);
		_locations.push_back(loc);

		// union locations
		//
		if (intersect_found)
		{
			for (j = 0; j < temp_dl.size(); j++)
			{
				DropLocation *loc = new DropLocation(*(temp_dl[j]));
				_locations.push_back(loc);
			}
			
			temp_dl.clear();
		}
	}
}

/*
 * For each application effect set computed, we collect the arcs
 * that have that effect set.
 */
void LoadShedder::setLocations()
{
	int i, j;

	DropLocationsVector temp_dl;
	temp_dl.clear();

	// create a static copy of the drop locations for traversal
	//
	for (i = 0; i < _locations.size(); i++)
		temp_dl.push_back(new DropLocation(*(_locations[i])));

	// now collect the arc ids for each potential drop location
	//
	for (i = 0; i < temp_dl.size(); i++)
	{
		IntVector app_ids = temp_dl[i]->_app_ids;

		for (j = 0; j < app_ids.size(); j++)
		{
			Id arc_id = _topology.apps[app_ids[j]].prev;
			Id prev_id = _topology.arcs[arc_id.id].prev;
			BoxTop cur_box;

			// if what precedes this app is a box
			// 
			if (prev_id.type == BOX) 
			{
				cur_box = _topology.boxes[prev_id.id];

				// if there is a split
				//
				if (isSplit(prev_id.id))
				{
					// continue recursively if the application
					// effect set is a subset of the current location
					//
					if (_locations[i]->isSubset(cur_box.to_apps,
												_locations[i]->_app_ids))
						collectArcs(i, cur_box);
					// else, just insert the current arc and this app_id is
					// done
					//
					else
						_locations[i]->addArc(arc_id.id);
				}
				// if no split, then can continue recursing
				//
				else
					collectArcs(i, cur_box);
			}
			// if what precedes this app is an input, then just insert the
			// current arc and this app_id is done
			//
			else if (prev_id.type == INPUT)
				_locations[i]->addArc(arc_id.id);
		}
	}
}

/* 
 * This function recursively collects arcs for a given application effect
 * set.
 */
void LoadShedder::collectArcs(int loc_index, BoxTop current_box)
{
	IdList prev_arcs = current_box.prev;
	IdListIter iter;

	// do for each arc preceeding the current_box
	//
	for (iter = prev_arcs.begin(); iter != prev_arcs.end(); iter++)
	{
		// if it is an input, just insert the current arc
		//
		if (_topology.arcs[(*iter).id].prev.type == INPUT)
		{
			_locations[loc_index]->addArc((*iter).id);
		}
		else 
		{
			assert(_topology.arcs[(*iter).id].prev.type == BOX);

			// check if there is a split
			//
			if (isSplit(_topology.arcs[(*iter).id].prev.id))
			{
				BoxTop parent = 
						_topology.boxes[_topology.arcs[(*iter).id].prev.id];
				if (_locations[loc_index]->isSubset(parent.to_apps,
											_locations[loc_index]->_app_ids))
				{
					collectArcs(loc_index, 
						_topology.boxes[_topology.arcs[(*iter).id].prev.id]);
				}
				else
				{
					_locations[loc_index]->addArc((*iter).id);
				}
			}
			else
			{
	 			collectArcs(loc_index, 
						_topology.boxes[_topology.arcs[(*iter).id].prev.id]); 
			}
		}
	}
}

bool LoadShedder::isSplit(int box_id)
{
	// first, check if the box exists in the topology
	//
	assert(_topology.boxes.find(box_id) != _topology.boxes.end());

	if (_topology.boxes[box_id].next.size() > 1)
		return true;
	else
		return false;
}

/*
 * This function assigns rate coefficients to each arc in the network,
 * walking back from each application.
 */
void LoadShedder::assignRateCoefs()
{
	// first, initialize the rate coef's of all the arcs to -1
	//
	ArcTopMapIter arc_iter;

	for (arc_iter = _topology.arcs.begin(); arc_iter != _topology.arcs.end();
		 arc_iter++)
	{
		(arc_iter->second).rate_coef = -1;
	}
	
	// now, for each application, walk back to compute the coefficients
	//
	AppTopMapIter app_iter;	

	for (app_iter = _topology.apps.begin(); app_iter != _topology.apps.end(); 
		 app_iter++) 
	{
		computeRateCoef(_topology.arcs[(app_iter->second).prev.id]);
	}
}

/*
 * This function recursively computes the rate coefficient for a given arc.
 * Box types are not taken into account. The assumption is that rate at the
 * output of a box is total input rate to this box times the selectivity of
 * this box, independent from the box type.
 */
double LoadShedder::computeRateCoef(ArcTop arc)
{
	double r = 0;
	double sel;

	if (arc.prev.type == INPUT)
	{
		// sliding window average on the input rate history
		//
		vector<int> input_rate_history = 
				_current_stats->_inputArcsStats[arc.prev.id]._inputRateHistory;
		int num_tuples = 0;
		for (int i = 0; i < input_rate_history.size(); i++)
		{
			num_tuples += input_rate_history[i];
		}

		if (input_rate_history.size() > 0)
		{
			int s_period = RuntimeGlobals::getPropsFile()->getUInt(
									"LoadShedder.inputRateSamplePeriodMs");
			r = double(num_tuples)/double(input_rate_history.size()*s_period);
		}
		else
			r = 0;
	}
	else 
	{
		assert(arc.prev.type == BOX);

		BoxTop prev_box = _topology.boxes[arc.prev.id];

		// for now, we are taking the first arc for simplicity
		//
		if (_current_stats->_boxesStats[prev_box.id.id]._numConsumedTuples > 0)
		{
			sel = 
				double(_current_stats->_boxesStats[prev_box.id.id].
				   	_numProducedTuples[0])/
				double(_current_stats->_boxesStats[prev_box.id.id].
				   	_numConsumedTuples);
		}
		else
		{
			sel = 0;
		}

		IdListIter iter;

		for (iter = prev_box.prev.begin(); iter != prev_box.prev.end(); iter++)
		{
			if (_topology.arcs[iter->id].rate_coef == -1)
			{
				r = r + computeRateCoef(_topology.arcs[iter->id])*sel;
			}
			else
			{
				r = r + (_topology.arcs[iter->id].rate_coef)*sel;
			}
		}
	}
	
	_topology.arcs[arc.id.id].rate_coef = r;

	return(r);
}

/*
 * This function assigns load coefficients to each arc in the network,
 * walking forward from each input.
 */
void LoadShedder::assignLoadCoefs()
{
	// first, initialize the load coef's of all the arcs to -1
	//
	ArcTopMapIter arc_iter;

	for (arc_iter = _topology.arcs.begin(); arc_iter != _topology.arcs.end();
		 arc_iter++)
	{
		(arc_iter->second).load_coef = -1;
	}

	// now, for each input, walk forward to compute the coefficients
	//
	InputTopMapIter inp_iter;

	for (inp_iter = _topology.inputs.begin(); 
		 inp_iter != _topology.inputs.end(); inp_iter++)
	{
		IdListIter iter;

		for (iter = (inp_iter->second).next.begin(); 
			 iter != (inp_iter->second).next.end(); iter++)
		{
			computeLoadCoef(_topology.arcs[iter->id]);
		}
	}
}

/*
 * This function recursively computes the load coefficient for a given arc.
 */
double LoadShedder::computeLoadCoef(ArcTop arc)
{
	double load = 0;
	double sel;

	if (arc.next.type == APP)
	{
		load = 0;
	}
	else
	{
		assert(arc.next.type == BOX);

		BoxTop next_box = _topology.boxes[arc.next.id];

		// for now, we are taking the first arc for simplicity
		//
		if (_current_stats->_boxesStats[next_box.id.id]._numConsumedTuples > 0)
		{
			sel = double(_current_stats->_boxesStats[next_box.id.id].
				  		 _numProducedTuples[0])/
				  double(_current_stats->_boxesStats[next_box.id.id].
				  		 _numConsumedTuples);
		}
		else
		{
			sel = 0;
		}

		double cost = _current_stats->_boxesStats[next_box.id.id]._cost;
		
		load = cost;

		IdListIter iter;

		for (iter = next_box.next.begin(); iter != next_box.next.end(); iter++)
		{
			if (_topology.arcs[iter->id].load_coef == -1)	
			{
				load = load + computeLoadCoef(_topology.arcs[iter->id])*sel;
			}
			else
			{
				load = load + (_topology.arcs[iter->id].load_coef)*sel;
			}
		}
	}

	_topology.arcs[arc.id.id].load_coef = load;

	return(load);
}

/*
 * For each drop location, this function computes the cumulative
 * loss-tolerance QoS based on the app_ids lists.
 */
void LoadShedder::assignQoS() 
{
	int i, j, k;

	DropLocationsVector temp_dl;
	temp_dl.clear();

	for (i = 0; i < _locations.size(); i++)
		temp_dl.push_back(new DropLocation(*(_locations[i])));

	for (i = 0; i < temp_dl.size(); i++)
	{
		EntryVector graph;
		graph.clear();

		for (k = 0; k <= 100; k++)
		{
			Entry e;
			e.x_value = k;
			e.utility = 0;
			e.slope   = 0;
			graph.push_back(e);
		}

		IntVector app_ids = temp_dl[i]->_app_ids;

		for (j = 0; j < app_ids.size(); j++)
		{
			LossToleranceQoS *cur_qos = _topology.apps[app_ids[j]].lt_qos;

			for (k = 0; k <= 100; k++)
			{
				graph[k].utility += float(cur_qos->_graph[k].utility);
				graph[k].slope   += float(cur_qos->_graph[k].slope);
			}
		}

		_locations[i]->_qos = new LossToleranceQoS(graph);
	}
}

/*
 * This function fills in the histogram related pieces of the 
 * val_qos of the apps.
 */
void LoadShedder::completeValueQoS(int app_id)
{
	int i;
	float min_val = MAX_VAL;
	float max_val = MIN_VAL;
	float total_weighted_utility = 0;

	ValueQoS *qos = _topology.apps[app_id].val_qos;

	for (i = 0; i < (qos->_intervals).size(); i++) 
	{
		//qos->_intervals[i].relative_frequency = 
		//	_current_stats->_appArcsStats[app_id]._valueHistogram[i];
		//
		qos->_intervals[i].relative_frequency = 
									float(1)/float((qos->_intervals).size());
		// temporary value until StatsImage provides it
		
		qos->_intervals[i].weighted_utility = 
			qos->_intervals[i].utility*qos->_intervals[i].relative_frequency;
		total_weighted_utility += qos->_intervals[i].weighted_utility;

		if ((qos->_intervals[i].lower_value < min_val) &&
			(qos->_intervals[i].relative_frequency > 0))
			min_val = qos->_intervals[i].lower_value;

		if ((qos->_intervals[i].upper_value > max_val) &&
			(qos->_intervals[i].relative_frequency > 0))
			max_val = qos->_intervals[i].upper_value;
	}

	_topology.apps[app_id].val_qos->_min_val = min_val;
	_topology.apps[app_id].val_qos->_max_val = max_val;

	for (i = 0; i < (qos->_intervals).size(); i++)
	{
		qos->_intervals[i].normalized_utility = 
			qos->_intervals[i].weighted_utility/total_weighted_utility;
	}

	qos->sortIntervals();
}

/* 
 * This function computes the L/G ratios for the drop locations and sorts
 * the drop locations in ascending order of their L/G ratios.
 * For each drop location, an L/G ratio is computed per QoS function piece 
 * in its loss-tolerance QoS.
 * This function also returns the min_load in the network to be used in
 * STEP_SIZE verification.
 */
void LoadShedder::fillLGRatios()
{
	int i, j, k;
	float percentage, current_slope, temp_slope;
	double gain;
	IntVector arc_ids;
	LossToleranceQoS *qos;

	// we will compute L/G ratios for each drop location
	//
	for (i = 0; i < _locations.size(); i++) 
	{		
		percentage = 100;
		qos = _locations[i]->_qos;
		arc_ids = _locations[i]->_arc_ids;
		gain = 0;

		// we will compute a gain over all arcs that belong to this location
		//
		for (j = 0; j < arc_ids.size(); j++) 
		{
			gain = gain + _topology.arcs[arc_ids[j]].load_coef*
						  _topology.arcs[arc_ids[j]].rate_coef;
		}

		// don't create an entry for this location if there's no gain
		//
		if (equalDoubles(gain, 0)) 
        	continue;

		// create one entry per QoS function piece;
		// each function piece has a uniform slope
		//
		do
		{
			RatioEntry entry;
			entry.loc_id = i;

			current_slope = qos->getSlope(percentage);

			k = 0;
			do 
			{
				k++;
				temp_slope = qos->getSlope(percentage-k);

			} while (((percentage-k) > 0) && 
				     equalFloats(temp_slope, current_slope));

			entry.max_per  = percentage;
			entry.min_per  = percentage-k;
			entry.slope    = current_slope;
			if (gain > 0)
				entry.lg_ratio = double(current_slope)/gain;
			else
				entry.lg_ratio = MAX_RATIO;

			_lg_entries.push_back(entry);

			percentage = percentage-k;

		} while (percentage > 0); 
	}

	// sort the L/G ratios
	//
	quickSort(&_lg_entries, 0, _lg_entries.size()-1);
}

/* 
 * This function implements quick sort for sorting L/G entries.
 */
void LoadShedder::quickSort(RatioEntryVector *v, int p, int r) 
{
	int q;

	if (p < r) 
	{
		q = partition(v, p, r);
		quickSort(v, p, q);
		quickSort(v, q+1, r);
	}
}

/* 
 * This is an internal function only to be called by quickSort.
 */
int LoadShedder::partition(RatioEntryVector *v, int p, int r) 
{
	RatioEntry temp1, temp2;

	temp1.loc_id   = (*v)[p].loc_id;
	temp1.max_per  = (*v)[p].max_per;
	temp1.min_per  = (*v)[p].min_per;
	temp1.slope    = (*v)[p].slope;
	temp1.lg_ratio = (*v)[p].lg_ratio;

	int i = p-1;
	int j = r+1;

	while (1) 
	{
		do 
		{
	    	j = j-1;
		} while ((*v)[j].lg_ratio > temp1.lg_ratio);

		do 
		{
			i = i+1;
		} while ((*v)[i].lg_ratio < temp1.lg_ratio);

		if (i < j)
		{
			temp2.loc_id   = (*v)[i].loc_id;
            temp2.max_per  = (*v)[i].max_per;
            temp2.min_per  = (*v)[i].min_per;
            temp2.slope    = (*v)[i].slope;
            temp2.lg_ratio = (*v)[i].lg_ratio;

			(*v)[i].loc_id   = (*v)[j].loc_id;
            (*v)[i].max_per  = (*v)[j].max_per;
            (*v)[i].min_per  = (*v)[j].min_per;
            (*v)[i].slope    = (*v)[j].slope;
            (*v)[i].lg_ratio = (*v)[j].lg_ratio;

			(*v)[j].loc_id   = temp2.loc_id;
            (*v)[j].max_per  = temp2.max_per;
            (*v)[j].min_per  = temp2.min_per;
            (*v)[j].slope    = temp2.slope;
            (*v)[j].lg_ratio = temp2.lg_ratio;
		}
		else
			return(j);
	}
}

/*
 * This function fills a new LSRM entry. Load is shed from the specified
 * applications. The amount of load shedding is specified by the
 * goal_percentage parameter. All applications are to receive that much
 * of the output they are supposed to receive. This version is for
 * inserting random drops.
 */
bool LoadShedder::fillDropEntry(LSRMEntry *entry, IntVector app_ids, 
								float goal_percentage)
{
	int 	i; 
	float 	current_cursor;
	bool 	new_entry = false;
	double 	drop_rate;
	//double 	agg_utility = 1.0*_topology.apps.size();
	double 	agg_utility = entry->getAggUtility();

	// all applications in app_ids will drop to a percentage of goal_percentage
	//
	for (i = 0; i < app_ids.size();  i++) 
	{
		// first check if the app has a smaller cursor than goal_percentage.
		// if so, we will skip this application
		//
		current_cursor = _topology.apps[app_ids[i]].lt_qos->_cursor;
		if (current_cursor <= goal_percentage)
		{
			/*
			agg_utility = agg_utility - 
			(1.0-_topology.apps[app_ids[i]].lt_qos->getUtility(current_cursor))
			/(1.0*_topology.apps.size());
			*/
			continue;
		}

		assert(current_cursor > 0);

		drop_rate = 1 - double(goal_percentage)/double(current_cursor);	

		DropInfo d_info;
		d_info.arc_id 	 = _topology.apps[app_ids[i]].prev.id;
		d_info.drop_rate = drop_rate;
		d_info.pred_flag = false;
		d_info.intervals.clear();

		_topology.apps[app_ids[i]].lt_qos->_cursor = goal_percentage;

		entry->setCursor(app_ids[i], goal_percentage);

		moveDropBox(entry, d_info);

		new_entry = true;

		// update the agg_utility
		//
		agg_utility = agg_utility - 
	     (_topology.apps[app_ids[i]].lt_qos->getUtility(current_cursor) -
			_topology.apps[app_ids[i]].lt_qos->getUtility(goal_percentage))
		 /(1.0*_topology.apps.size());
	}

	entry->setAggUtility(agg_utility);

	return(new_entry);
}

void LoadShedder::moveDropBox(LSRMEntry *entry, DropInfo d_info)
{
	// check if we are at an input point; if so, the drop should be
	// inserted there
	//
	if (_topology.arcs[d_info.arc_id].prev.type == INPUT) 
	{
		entry->insertDrop(d_info);
		return;
	}
	else 
	{
		// else, check if we can push the drop further upstream
		//
		
		assert(_topology.arcs[d_info.arc_id].prev.type == BOX);

		// for now, just assume we only cover a restricted set of box types
		//
		int box_type = _catalog->getQNet()->getBox(
						_topology.arcs[d_info.arc_id].prev.id)->getBoxType();
		assert((box_type == FILTER) || (box_type == MAP) || 
			   (box_type == UNION));

		// if box type is not in {FILTER, MAP, UNION} then can't move it
		// further
		//
		//int box_type = _catalog->getQNet()->getBox(
		//				_topology.arcs[d_info.arc_id].prev.id)->getBoxType();

		//if ((box_type != FILTER) && (box_type != MAP) && (box_type != UNION))
		//{
			// insert here and return
			//
			//entry->insertDrop(d_info);

			//return;
		//}

		// if we are at a split point, then check if we can combine drops
		//
		if (_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].next.size() 
																		> 1)
		{
			// count the number of drops on the following arcs
			// also, compute min_drop_rate amoing them
			// also, check if they have equal drop rates
			//
			int  count = 1;
			bool equal = true;
			double min_drop_rate = d_info.drop_rate;
			double new_drop_rate = d_info.drop_rate;

			IdList next_arcs = 
					_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].next;
			IdListIter iter;
			for (iter = next_arcs.begin(); iter != next_arcs.end(); iter++)
			{
				if (iter->id != d_info.arc_id) 
				{
					if (entry->exists(iter->id))
					{
						count++;
						double d_rate = 
								entry->_dropInsertionPlan[iter->id].drop_rate;

						if (d_rate < min_drop_rate)
						{
							min_drop_rate = d_rate;
							equal = false;
						}
						else if (equal)
						{
							if (!equalDoubles(d_rate, new_drop_rate))
								equal = false;
						}
					}
					else
						break;
				}
			}

			if (equal && 
				(count == _topology.boxes
				 		  [_topology.arcs[d_info.arc_id].prev.id].next.size()))
			{
				// no need to insert the drop box
				// we can have a combined drop and get rid of the others
				//

				// first, remove the old drops from the entry
				//
				for (iter = next_arcs.begin(); iter != next_arcs.end(); iter++)
				{
					if (iter->id != d_info.arc_id)
						entry->deleteDrop(iter->id);
				}

				// then push the drop further upstream
				//
				IdList pre_arcs = 
					_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].prev;	
				for (iter = pre_arcs.begin(); iter != pre_arcs.end(); iter++)
				{
					DropInfo new_d_info;
					new_d_info.arc_id 	 = iter->id;
					new_d_info.drop_rate = d_info.drop_rate;
					new_d_info.pred_flag = false;
					new_d_info.intervals.clear();
					moveDropBox(entry, new_d_info);
				}
				return;
			}
			else if (count == 
			 _topology.boxes[_topology.arcs[d_info.arc_id].prev.id].next.size())
			{
				// insert the drop box
				// let the old ones stay and also have a combined one to push

				assert(!equalDoubles(min_drop_rate, 1));
				// otherwise, equal would be true

				// first, insert it where it is supposed to be
				//
				entry->insertDrop(d_info);
				
				// now update drop rates based on min_drop_rate
				//
				for (iter = next_arcs.begin(); iter != next_arcs.end(); iter++)
				{
					entry->updateDropRateBy(iter->id, min_drop_rate); 

					// if drop rate becomes 0, then remove this drop from entry
					//
					if (equalDoubles(
						entry->_dropInsertionPlan[iter->id].drop_rate, 0))

					{
						entry->deleteDrop(iter->id);
					}
				}

				// now push the combined drop
				//
				IdList pre_arcs = 
					_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].prev;
				for (iter = pre_arcs.begin(); iter != pre_arcs.end(); iter++)
				{
					DropInfo new_d_info;
					new_d_info.arc_id 	 = iter->id;
					new_d_info.drop_rate = min_drop_rate;
					new_d_info.pred_flag = false;
					new_d_info.intervals.clear();
					moveDropBox(entry, new_d_info);
				}

				return;
			}
			else
			{
				// cannot combine drops 
				// insert where it is supposed to be and return
				//
				entry->insertDrop(d_info);

				return;
			}
		}
		else
		{
			// neither at input source nor at split point, can push further
			//
			// for now, let's ignore the box type in
			// _catalog->getQNet()->getBox(d_info.arc_id.prev.id)->getBoxType()
			// and just push the drop upstream
			//
			IdList pre_arcs = 
				_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].prev;	
			IdListIter iter;
			for (iter = pre_arcs.begin(); iter != pre_arcs.end(); iter++)
			{
				DropInfo new_d_info;
				new_d_info.arc_id 	 = iter->id;
				new_d_info.drop_rate = d_info.drop_rate;
				new_d_info.pred_flag = false;
				new_d_info.intervals.clear();
				moveDropBox(entry, new_d_info);
			}
			return;
		}
	}
}

/*
 * This function fills a new LSRM entry. Load is shed from the specified
 * applications. The amount of load shedding is specified by the
 * goal_percentage parameter. All applications are to receive that much
 * of the output they are supposed to receive. This version is for
 * inserting semantic drops (i.e., filters).
 */
bool LoadShedder::fillFilterEntry(LSRMEntry *entry, IntVector app_ids, 
								  float goal_percentage, bool first)
{
	int i;
	float current_cursor;
	bool new_entry = false;
	//double agg_utility = 1.0*_topology.apps.size();
	double 	agg_utility = entry->getAggUtility();

	// for the very first entry, an attempt to create a free entry
	//
	if (first) 
	{
		AppTopMapIter aiter;
		AppTopMap apps = _topology.apps;

		new_entry = true;

		for (aiter = apps.begin(); aiter != apps.end(); aiter++)
		{
			// create a new filtering predicate
			// AND(value >= _min_val, value <= _max_val)
			//
			string f_attr =
			  formatAttribute(_attribute, _topology.apps[aiter->first].prev.id);

			ostringstream val1, val2;

			if ((f_attr.data())[3] == 'i') 
			{
				val1 << int((aiter->second).val_qos->_min_val);
				val2 << int((aiter->second).val_qos->_max_val);
			}
			else
			{
				val1 << showpoint << (aiter->second).val_qos->_min_val;
				val2 << showpoint << (aiter->second).val_qos->_max_val;
			}

			string val1_str = val1.str();
			string val2_str = val2.str();
			string pred_str = "AND(" + f_attr + " >= " + val1_str + ", " +
							  f_attr + " <= " + val2_str + ")";

			IntervalType interval;
			interval.left_op   = 1;
			interval.left_val  = (aiter->second).val_qos->_min_val;
			interval.right_op  = 1;
			interval.right_val = (aiter->second).val_qos->_max_val;
			interval.attribute = f_attr;

			// insert the new filter box to the entry and push it upstream
			//
			DropInfo d_info;

			d_info.arc_id        = _topology.apps[aiter->first].prev.id;
			d_info.drop_rate     = 0; // ignore 
			d_info.predicate_str = pred_str;
			d_info.intervals.push_back(interval);
			d_info.pred_flag     = true;

			// cursor and aggregate utility don't change
			//
			moveFilterBox(entry, d_info);

			new_entry = true;
		}

		return(new_entry);
	}
	else
	{
		// all applications in app_ids will drop to a percentage of 
		// goal_percentage
		//
		for (i = 0; i < app_ids.size();  i++)
		{
			// first check if the app has a smaller cursor than goal_percentage
			// if so, we will skip this application
			//
			current_cursor = _topology.apps[app_ids[i]].lt_qos->_cursor;
			if (current_cursor <= goal_percentage)
			{
				/*
				agg_utility = agg_utility - (1.0 -
				 _topology.apps[app_ids[i]].lt_qos->getUtility(current_cursor));
				*/
				continue;
			}

			// drop_rate = 1 - double(goal_percentage)/double(current_cursor);
			// create a new filtering predicate
			// must filter out current_cursor-goal_percentage of the values
			//
			DropInfo d_info;

			d_info.arc_id        = _topology.apps[app_ids[i]].prev.id;
			d_info.drop_rate     = 0; // ignore
			d_info.predicate_str = _topology.apps[app_ids[i]].val_qos->
				computeFilterPredicate(current_cursor-goal_percentage, 
								    current_cursor, 
								    formatAttribute(_attribute, d_info.arc_id),
									&(d_info.intervals));
			d_info.pred_flag     = true;

			_topology.apps[app_ids[i]].lt_qos->_cursor = goal_percentage;

			entry->setCursor(app_ids[i], goal_percentage);

			moveFilterBox(entry, d_info);

			new_entry = true;

			// update the agg_utility
			//
			agg_utility = agg_utility - 
		     (_topology.apps[app_ids[i]].lt_qos->getUtility(current_cursor) - 
				_topology.apps[app_ids[i]].lt_qos->getUtility(goal_percentage))
			 /(1.0*_topology.apps.size());
		}

		entry->setAggUtility(agg_utility);

		return(new_entry);
	}
}

void LoadShedder::moveFilterBox(LSRMEntry *entry, DropInfo d_info)
{
	// check if we are at an input point; if so, the filter should be
	// inserted there
	//
	if (_topology.arcs[d_info.arc_id].prev.type == INPUT)
	{
		entry->insertDrop(d_info);
		return;
	}
	else
	{
		// else, check if we can push the filter further upstream
		//

		assert(_topology.arcs[d_info.arc_id].prev.type == BOX);

		// for now, just assume we only cover a restricted set of box types
		//
		int box_type = _catalog->getQNet()->getBox(
						_topology.arcs[d_info.arc_id].prev.id)->getBoxType();
		assert((box_type == FILTER) || (box_type == JOIN) || 
			   (box_type == UNION));

		// if we are at a split point, then insert it there.
		// also, try to push a combined filter with disjuncted predicate 
		// upstream
		//
		if (_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].next.size() 
																		> 1)
		{
			entry->insertDrop(d_info);

			// now check if we can have a combined filter
			//
			int  count = 1;

			IdList next_arcs = 
				_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].next;

			IdListIter iter;
			for (iter = next_arcs.begin(); iter != next_arcs.end(); iter++)
			{
				if (iter->id != d_info.arc_id)
				{
					if (entry->exists(iter->id))
						count++;
					else
						break;
				}
			}

			if (count == 
			 _topology.boxes[_topology.arcs[d_info.arc_id].prev.id].next.size())
			{
				// we can have a combined filter
				//
				
				// create a filter with disjuncted predicate
				//
				PredicateStrVector preds;
				IntervalVec all_intervals;
				for (iter = next_arcs.begin(); iter != next_arcs.end(); iter++)
				{
					preds.push_back(
							entry->_dropInsertionPlan[iter->id].predicate_str);

					IntervalVec iv = 
							entry->_dropInsertionPlan[iter->id].intervals;
					for (int i = 0; i < iv.size(); i++)
					{
						all_intervals.push_back(iv[i]);
					}
				}

				string or_pred;

				if (all_intervals.empty()) 
				{
					or_pred = "";
				}
				else
				{
					simplifyORPredicate(&(all_intervals));
					or_pred = makeORPredicate(all_intervals, 0);
				}

				// push the new filter upstream
				//
				IdList pre_arcs = 
					_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].prev;
				for (iter = pre_arcs.begin(); iter != pre_arcs.end(); iter++)
				{
					DropInfo new_d_info;

					new_d_info.arc_id        = iter->id;
					new_d_info.drop_rate     = 0; // ignore
					new_d_info.predicate_str = or_pred;
					new_d_info.intervals	 = all_intervals;
					new_d_info.pred_flag     = true;

					moveFilterBox(entry, new_d_info);
				}
				return;
			}
			else
			{
				// nothing to move further upstream
				//
				return;
			}
		}
		else 
		{
			// neither at input source nor at split point, can push further
			//
			IdList pre_arcs = 
				_topology.boxes[_topology.arcs[d_info.arc_id].prev.id].prev;

			IdListIter iter;
			for (iter = pre_arcs.begin(); iter != pre_arcs.end(); iter++)
			{
				DropInfo new_d_info;

				new_d_info.arc_id        = iter->id;
				new_d_info.drop_rate     = 0; // ignore
				new_d_info.predicate_str = d_info.predicate_str;
				new_d_info.intervals	 = d_info.intervals;
				new_d_info.pred_flag     = true;

				moveFilterBox(entry, new_d_info);
			}
			return;
		}
	}
}

Predicate *LoadShedder::makeORPredicate(PredicateVector preds, int offset)
{
	if (offset == (preds.size()-1))
	{
		return(preds[offset]);
	}
	else
	{
		return(new ORPredicate(preds[offset], 
							   makeORPredicate(preds, ++offset)));
	}
}

string LoadShedder::makeORPredicate(PredicateStrVector preds, int offset)
{
	if (offset == (preds.size()-1))
	{
		return(preds[offset].data());
	}
	else
	{
		return(string("OR(" + preds[offset] + ", " + 
					   makeORPredicate(preds, ++offset) + ")"));
	}
}

string LoadShedder::makeORPredicate(IntervalVec intervals, int offset)
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

void LoadShedder::simplifyORPredicate(IntervalVec *intervals)
{
	int i, j, k;
	IntervalVec temp;			// a copy to traverse on
	IntervalVec result;			// will put the result in this
	IntervalVec temp_result;	// temp result to traverse on
	IntervalVecIter iter;

	for (i = 0; i < intervals->size(); i++)
	{
		temp.push_back((*intervals)[i]);
	}

	for (i = 0; i < temp.size(); i++)
	{
		if (result.empty())
		{
			result.push_back(temp[i]);
		}
		else
		{
			// take a copy of result to traverse
			//
			temp_result.clear();
			for (j = 0; j < result.size(); j++)
				temp_result.push_back(result[j]);
			
			int adjust = 0;
			IntervalType current = temp[i];
			for (j = 0; j < temp_result.size(); j++)
			{
				//iter = temp_result.begin()+j;
				if ((current.right_val < temp_result[j].left_val) ||
				    (current.left_val > temp_result[j].right_val) ||
				   ((equalFloats(current.right_val, temp_result[j].left_val))
					&& !current.right_op && !temp_result[j].left_op) ||
				   ((equalFloats(current.left_val, temp_result[j].right_val))
				    && !current.left_op && !temp_result[j].right_op))
				{
					// result[j] stays
					// current does not change
				}
				else
				{
					if (equalFloats(temp_result[j].left_val, current.left_val))
					{
						if (temp_result[j].left_op || current.left_op)
							current.left_op = 1;
						else
							current.left_op = 0;
					}
					else if (temp_result[j].left_val < current.left_val)
					{
						current.left_val = temp_result[j].left_val;
						current.left_op = temp_result[j].left_op;
					}

					if (equalFloats(temp_result[j].right_val,
										 current.right_val))
					{
						if (temp_result[j].right_op || current.right_op)
							current.right_op = 1;
						else
							current.right_op = 0;
					}
					else if (temp_result[j].right_val > current.right_val)
					{
						current.right_val = temp_result[j].right_val;
						current.right_op = temp_result[j].right_op;
					}

					// remove result[j] 
					//
					assert(!(result.empty()));
					iter = result.begin();
					for (k = 0; k < (j-adjust); k++)
					{
						iter++;
					}
					result.erase(iter);
					adjust++;
				}
			}

			// insert the current
			//
			result.push_back(current);
		}
	}

	// refresh the contents of the intervals
	//
	intervals->clear();
	for (i = 0; i < result.size(); i++)
		intervals->push_back(result[i]);
}

/*
string LoadShedder::simplifyORPredicate(PredicateStrVector preds)
{
	cout << "in LoadShedder::simplifyORPredicate()" << endl;

	float lower, upper;
	char attr1[15], attr2[15], attr3[15];

	float largest_lower, largest_lower1, largest_lower3;
	float smallest_upper1, smallest_upper2, smallest_upper3;

	bool first1 = true, first2 = true, first3 = true;
	int type;

	char *pred_str;

	for (int i = 0; i < preds.size(); i++)
	{
		const char *str = preds[i].data();
		cout << str << endl;

		if (str[0] == 'O')	// OR'ed predicate
		{
			sscanf(str, "OR(%s < %f, %s >%s)", attr1, &lower, attr2, attr3);

			if (attr3[0] == '=') 
			{
				cout << "type 3" << endl;

				sscanf(str, "OR(%s < %f, %s >= %f)", attr1, &lower, attr2,
					   &upper);

				cout << "sscanf ok" << endl;
				cout << attr1 << endl;
				cout << lower << endl;
				cout << upper << endl;

				type = 3;
				if (first3) // init
				{
					first3 = false;
					largest_lower3 = lower;
					smallest_upper3 = upper;
				}
			}
			else
			{
				cout << "type 1" << endl;

				sscanf(str, "OR(%s < %f, %s > %f)", attr1, &lower, attr2, 
					   &upper);

				cout << "sscanf ok" << endl;
				cout << attr1 << endl;
				cout << lower << endl;
				cout << upper << endl;

				type = 1;
				if (first1) // init
				{
					first1 = false;
					largest_lower1 = lower;
					smallest_upper1 = upper;
				}
			}
		}
		else if (str[0] == ':')	// simpler predicate
		{
			cout << "type 2" << endl;

			sscanf(str, "%s >= %f", attr1, &upper);

			type = 2;
			if (first2)// init
			{
				first2 = false;
				smallest_upper2 = upper;
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
				if (lower > largest_lower1)
					largest_lower1 = lower;
				if (upper < smallest_upper1)
					smallest_upper1 = upper;
				break;
			case 2:
				if (upper < smallest_upper2)
					smallest_upper2 = upper;
				break;
			case 3:
				if (lower > largest_lower3)
					largest_lower3 = lower;
				if (upper < smallest_upper3)
					smallest_upper3 = upper;
				break;
			default:
				cout << "invalid case!!" << endl;
				abort();
		}
	}

	cout << "ok so far" << endl; 
	pred_str = (char *) malloc(sizeof(char)*50);

	if (!first1 && !first2 && !first3)
	{
		if (largest_lower1 > largest_lower3)
			largest_lower = largest_lower1;
		else
			largest_lower = largest_lower3;

		if ((smallest_upper1 < smallest_upper2) && 
			(smallest_upper1 < smallest_upper3))
		{
			if (smallest_upper1 >= largest_lower)
				sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, largest_lower, 
						attr1, smallest_upper1);
			else
				pred_str = NULL;
		}
		else if ((smallest_upper2 < smallest_upper1) &&
				 (smallest_upper2 < smallest_upper3))
		{
			if (smallest_upper2 > largest_lower)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, largest_lower,
						attr1, smallest_upper2);
			else
				pred_str = NULL;
		}
		else if ((smallest_upper3 < smallest_upper1) &&
				 (smallest_upper3 < smallest_upper2))
		{
			if (smallest_upper3 > largest_lower)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, largest_lower,
						attr1, smallest_upper3);
			else
				pred_str = NULL;
		}
	}
	else if (!first1 && !first2 && first3)
	{
		if (smallest_upper1 < smallest_upper2)
		{
			if (smallest_upper1 >= largest_lower1)
				sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, largest_lower1,
						attr1, smallest_upper1);
			else
				pred_str = NULL;
		}
		else
		{
			if (smallest_upper2 > largest_lower1)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						largest_lower1, attr1, smallest_upper2);
			else
				pred_str = NULL;
		}
	}
	else if (!first1 && first2 && first3)
	{
		if (smallest_upper1 >= largest_lower1)
			sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, largest_lower1, 
					attr1, smallest_upper1);
		else
			pred_str = NULL;
	}
	else if (!first1 && first2 && !first3)
	{
		if (largest_lower1 > largest_lower3)
			largest_lower = largest_lower1;
		else
			largest_lower = largest_lower3;

		if (smallest_upper1 < smallest_upper3)
		{
			if (smallest_upper1 >= largest_lower)
				sprintf(pred_str, "OR(%s < %f, %s > %f)", attr1, largest_lower, 
						attr1, smallest_upper1);
			else
				pred_str = NULL;
		}
		else
		{
			if (smallest_upper3 > largest_lower)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, largest_lower,
						attr1, smallest_upper3);
			else
				pred_str = NULL;
		}
	}
	else if (first1 && !first2 && !first3)
	{
		if (smallest_upper2 < smallest_upper3)
		{
			if (smallest_upper2 > largest_lower3)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						largest_lower3, attr1, smallest_upper2);
			else
				pred_str = NULL;
		}
		else
		{
			if (smallest_upper3 > largest_lower3)
				sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, 
						largest_lower3, attr1, smallest_upper3);
			else
				pred_str = NULL;
		}
	}
	else if (first1 && !first2 && first3)
	{
		sprintf(pred_str, "%s>=%f)", attr1, smallest_upper2);
	}
	else if (first1 && first2 && !first3)
	{
		if (smallest_upper3 > largest_lower3)
			sprintf(pred_str, "OR(%s < %f, %s >= %f)", attr1, largest_lower3, 
					attr1, smallest_upper3);
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
		cout << "quitting with " << pred_str << endl;
		return(string(pred_str));
	}
	else
	{
		cout << "quitting with NULL" << endl;
		return(string("")) ;
	}
}
*/

/*
 * This function converts the attribute name for a tuple on arc arc_id 
 * into offset format that can be used by the predicate parser.
 */
string LoadShedder::formatAttribute(string attribute, int arc_id)
{
	assert(_arc_map.find(arc_id) != _arc_map.end());
	
	int i, type, size, offset = 0;
	bool found = false;
	string str;

	CompositeType *schema = _arc_map[arc_id]->getSchema();

	int num_attributes = schema->getNumberOfAttributes();

	for (i = 0; i < num_attributes; i++ )
	{
		if ((schema->getAttribute(i)).m_fieldName != attribute)
		{
			offset += (schema->getAttribute(i)).m_size;
		}
		else
		{
			type = (schema->getAttribute(i)).m_fieldType;
			size = (schema->getAttribute(i)).m_size;
			found = true;
			break;
		}
	}

	if (!found)
	{
		_pLogger->log("LS",
			makeDebugLogLines("in LoadShedder::formatAttribute(): ",
							  "attribute not found: ", attribute));
		abort();	
	}

	ostringstream val1, val2;
	val1 << offset;
   	string val1_str = val1.str();
	val2 << size;
	string val2_str = val2.str();

	TypeManager tm;

	if (type == tm.INTEGER)
		str = ":0:i:" + val1_str + ":" + val2_str + ":";
	else if (type == tm.FLOAT)
		str = ":0:f:" + val1_str + ":" + val2_str + ":";
	else if (type == tm.DOUBLE)
		str = ":0:d:" + val1_str + ":" + val2_str + ":";
	else if (type == tm.TIMESTAMP)
		str = ":0:t:" + val1_str + ":" + val2_str + ":";
	else if (type == tm.BOOL)
		str = ":0:b:" + val1_str + ":" + val2_str + ":";
	else if (type == tm.STRING)
		str = ":0:s:" + val1_str + ":" + val2_str + ":";
	else
	{
		_pLogger->log("LS",
			makeDebugLogLines("in LoadShedder::formatAttribute(): ",
							  "invalid type ", type, " for attribute ", 
							  attribute));
		abort();    
	}

	return(str);
}

string LoadShedder::reFormatAttribute(string str)
{
	string result; 

	int i = 0;
	while (i < strlen(str.data()))
	{
		if (str.data()[i]  == ':')
		{
			i += 9;	// skip the modifier for attribute offset/type etc.
			result = result + _attribute;
		}
		else
		{
			ostringstream c;
			c << str.data()[i];
			string c_str = c.str();
			result = result + c_str; 
			i++;
		}
	}

	return(result);
}

bool LoadShedder::constructLSRM() 
{
	int i;
	bool new_entry = false;
	float current_percentage, offset;
	IntVector v_app_ids;

	v_app_ids.clear();
	_map.clear();

	// if we will shed based on values, we have to derive the new
	// loss-tolerance QoS from the value-based QoS and the current
	// histograms
	//
	if (_mode == FilterLS) 
	{
		AppTopMapIter iter;
		for (iter = _topology.apps.begin(); iter != _topology.apps.end(); 
		 	 iter++)
		{
			// complete or update the val_qos based on the current histogram
			//
			completeValueQoS(iter->first);
		
			// derive loss-tolerance QoS from value-based
			//
			(iter->second).lt_qos = (iter->second).val_qos->computeLTQoS();

			v_app_ids.push_back(iter->first);
		}

		// fill in no-ls entry
		//
		//LSRMEntry *map_entry = new LSRMEntry(v_app_ids);
		//_map.push_back(map_entry);

		// fill in the free entry first
		//
		LSRMEntry *map_entry = new LSRMEntry(v_app_ids);
		IntVector dummyvector;
		new_entry = fillFilterEntry(map_entry, dummyvector, 100, true);
		if (new_entry)
			_map.push_back(map_entry);
	}
	else 
	{
		AppTopMapIter iter;
		for (iter = _topology.apps.begin(); iter != _topology.apps.end();
		 	 iter++)
		{
			v_app_ids.push_back(iter->first);
			(iter->second).lt_qos->_cursor = 100;
		}

		// fill in no-ls entry
		//
		LSRMEntry *map_entry = new LSRMEntry(v_app_ids);
		_map.push_back(map_entry);
	}

	// assign rate coefs
	// this totally depends on the current input rates and box selectivities
	//
	assignRateCoefs();

	// assign load coefs
	// this depends on current box costs and selectivities
	//
	assignLoadCoefs();

	// assign loss-tolerance QoS graphs of the drop locations
	//
	assignQoS();

	// fill and sort L/G ratios
	//
	_lg_entries.clear();
	fillLGRatios();

	// process the L/G ratios in order to construct the LSRM entries
	//
	for (i = 0; i < _lg_entries.size(); i++) 
	{
		// apply the entry in increments of STEP_SIZE
		//
		RatioEntry current_entry  = _lg_entries[i];
		DropLocation *current_location = _locations[current_entry.loc_id];

		current_percentage = current_entry.max_per;
		offset = 0;

		while (current_percentage > current_entry.min_per) 
		{
			LSRMEntry *map_entry;
			// create a new entry, initializing it to the last one created
			//
			if (!(_map.empty()))
			{
				assert(_map.back() != NULL);
				map_entry = new LSRMEntry(*(_map.back()));
			}
			else
				map_entry = new LSRMEntry(v_app_ids);

			new_entry = false;	

			// set the offset
			//
			if ((current_percentage - STEP_SIZE) < current_entry.min_per) 
				offset = offset + (current_percentage - current_entry.min_per);
			else
				offset = offset + STEP_SIZE;

			// apps will drop to a percentage of current_entry.max_per-offset
			//
			IntVector app_ids = current_location->_app_ids;	

			if (_mode == DropLS)
			{
				new_entry = fillDropEntry(map_entry, app_ids, 
										  current_entry.max_per-offset);
			}
			else if (_mode == FilterLS)
			{
				new_entry = fillFilterEntry(map_entry, app_ids,
									current_entry.max_per-offset, false);
			}

			if (new_entry) 
				_map.push_back(map_entry);

			current_percentage = current_entry.max_per - offset;
		}
	}

	/*
	cout << "LSRM:" << endl;
	cout << "-----" << endl;
	for (i = 0; i < _map.size(); i++)
		_map[i]->print();
	*/

	return(true); // for now always true; this will change later..
}

void LoadShedder::printCurve()
{
	if (tryLockMutex(_lsrm_mutex) == 0)
	{
		for (int i = 0; i < _map.size(); i++)
			cout << i << " " << _map[i]->getAggUtility() << endl;
		unlockMutex(_lsrm_mutex);
	}
	else
	{
		_pLogger->log("LS", 
					  "in LoadShedder::printCurve(): couldn't read the LSRM");
	}
}

int LoadShedder::lockMutex(pthread_mutex_t *mutex)
{
	_pLogger->log("LS", makeDebugLogLines("in LoadShedder::lockMutex()", 
										  " with mutex ", mutex));

	int rc = pthread_mutex_lock(mutex);

	string s;

	switch (rc)
	{
		case 0:
			s = "OK";
			break;
		case EINVAL:
			s = "EINVAL";
			break;
		case EDEADLK:
			s = "EDEADLK";
			break;
		case EBUSY:
			s = "EBUSY";
			break;
		case EPERM:
			s = "EPERM";
			break;
		default:
			ostringstream rc_val;
			rc_val << rc;
			s = rc_val.str();
	}

	_pLogger->log("LS", makeDebugLogLines("out LoadShedder::lockMutex()", 
										  " with rc ", s)); 

	return(rc);
}

int LoadShedder::tryLockMutex(pthread_mutex_t *mutex)
{
	_pLogger->log("LS", makeDebugLogLines("in LoadShedder::tryLockMutex()", 
										  " with mutex ", mutex));

	int rc = pthread_mutex_trylock(mutex);

	string s;

	switch (rc)
	{
		case 0:
			s = "OK";
			break;
		case EINVAL:
			s = "EINVAL";
			break;
		case EDEADLK:
			s = "EDEADLK";
			break;
		case EBUSY:
			s = "EBUSY";
			break;
		case EPERM:
			s = "EPERM";
			break;
		default:
			ostringstream rc_val;
			rc_val << rc;
			s = rc_val.str();
	}

	_pLogger->log("LS", makeDebugLogLines("out LoadShedder::tryLockMutex()",
										  " with rc ", s));

	return(rc);
}

int LoadShedder::unlockMutex(pthread_mutex_t *mutex)
{
	 _pLogger->log("LS", makeDebugLogLines("in LoadShedder::unlockMutex()",
							 			   " with mutex ", mutex));

	int rc = pthread_mutex_unlock(mutex);

	string s;

	switch (rc)
	{
		case 0:
			s = "OK";
			break;
		case EINVAL:
			s = "EINVAL";
			break;
		case EDEADLK:
			s = "EDEADLK";
			break;
		case EBUSY:
			s = "EBUSY";
			break;
		case EPERM:
			s = "EPERM";
			break;
		default:
			ostringstream rc_val;
			rc_val << rc;
			s = rc_val.str();
	}

	_pLogger->log("LS", makeDebugLogLines("out LoadShedder::unlockMutex()",
										  " with rc ", s));

	return(rc);
}

bool LoadShedder::equalDoubles(double d1, double d2)
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

bool LoadShedder::equalFloats(float f1, float f2)
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

