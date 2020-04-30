#include <StatsImage.H>
#include <QueryNetwork.H>
#include <StorageMgr_Exceptions.H>
#include <algorithm>
#include <sstream>
#include <set>
#include <RuntimeGlobals.H>

#include <BoxRecord.H>

using namespace std;

StatsImage::StatsImage(QueryNetwork & qn)
	throw (exception)
{
	BoxMap & boxes = qn.getBoxes();

	BoxStats bstats;
	bstats._numConsumedTuples = 0;
	

	for (BoxMapIter pos = boxes.begin();
		 pos != boxes.end();
		 ++ pos)
		{
			Box * pBox = pos->second;
			int boxId = pBox->getId();
			int numOutputPorts = qn.getBoxOutputPortCount(boxId);

			// Populate StatsImage::_boxesStats...
			bstats._numProducedTuples.resize(numOutputPorts);

			// For now, we're simply going to assign box costs during startup,
			// using per-box-type values supplied in the props file.
			const PropsFile * pProps = RuntimeGlobals::getPropsFile();
			switch (pBox->getBoxType())
				{
				case FILTER:
					bstats._cost = float(pProps->getDouble("Runtime.FilterBoxCost"));
					break;
				case MAP:
					bstats._cost = float(pProps->getDouble("Runtime.MapBoxCost"));
					break;
				case AGGREGATE:
					bstats._cost = float(pProps->getDouble("Runtime.AggregateBoxCost"));
					break;
				case BSORT:
					bstats._cost = float(pProps->getDouble("Runtime.BsortBoxCost"));
					break;
				case RESTREAM:
					bstats._cost = float(pProps->getDouble("Runtime.RestreamBoxCost"));
					break;
				case UNION:
					bstats._cost = float(pProps->getDouble("Runtime.UnionBoxCost"));
					break;
				case RESAMPLE:
					bstats._cost = float(pProps->getDouble("Runtime.ResampleBoxCost"));
					break;
				case JOIN:
					bstats._cost = float(pProps->getDouble("Runtime.JoinBoxCost"));
					break;
				case DROP:
					bstats._cost = float(pProps->getDouble("Runtime.DropBoxCost"));
					break;
				case HELLO_WORLD:
				case UPDATE_RELATION:
				case READ_RELATION:
				case LR_UPDATE_RELATION:
				case LR_READ_RELATION:
				  // TODO: Maybe these should do something?
				  break;
				case INPUTPORT:
				case OUTPUTPORT:
				case SUPERBOX:
					break;
				default: 
					cerr << "Unknown box type encountered in StatsImage::StatsImage(QueryNetwork &)" << endl;
					assert(false);
					abort();
				}

			_boxesStats[boxId] = bstats;
		}


	// Populate StatsImage::_numTuplesOnArcs...
    ArcMap arcs = qn.getArcs();
	for (ArcMapIter pos = arcs.begin();
		 pos != arcs.end();
		 ++ pos)
		{
			bool success = 
				_numTuplesOnArcs.insert(make_pair(pos->second->getId(), 0)).second;

			if (! success)
				{
					ostringstream os;
					os << "Couldn't insert arc id " << pos->second->getId() << " into _numTuplesOnArcs";
					throw SmException(__FILE__, __LINE__, os.str());
				}
		}

	// Populate StatsImage::_inputArcsStats...
	unsigned int historyLen = 
		RuntimeGlobals::getPropsFile()->getUInt("LoadShedder.inputRateMaxHistory");

	StreamMap & inputs = qn.getInputStreams();
	for (StreamMapIter pos = inputs.begin();
		 pos != inputs.end(); 
		 ++pos)
		{
			int boxId = pos->first;
			InputArcStats & ias = _inputArcsStats[boxId]; // Creates a new map entry.

			// also sets the entries to 0, which is what we want
			ias._inputRateHistory.resize(historyLen);
		}

	// Populate StatsImage::_appArcsStats...
	ApplicationMap & appArcs = qn.getApplications();
	for (ApplicationMapIter pos = appArcs.begin(); pos != appArcs.end(); ++pos)
		{
			int arcId = pos->second->getIncomingArcId();
			_appArcsStats[arcId]._avgLatency = -1;
			_appArcsStats[arcId]._avgLatencyUtility = -1;

			_appArcsStats[arcId]._accuracy = 100;
			_appArcsStats[arcId]._accuracyUtility = 1;
		}
}

//===============================================================================

string StatsImage::toString()
{
	ostringstream os;

	os << "StatsImage (this = " << (void*)this << ")" << endl
	   << "   _boxesStats (this = " << (void*)(& _boxesStats) << ")" << endl;

	for (map<int, BoxStats>::const_iterator pos = _boxesStats.begin();
		 pos != _boxesStats.end(); ++pos)
		{
			os << "      boxId=" << pos->first << endl
			   << "         BoxStats._numConsumedTuples = " << pos->second._numConsumedTuples << endl
			   << "         BoxStats._numProducedTuples = { ";

			const vector<int> & v = pos->second._numProducedTuples;
			for (int i = 0; i < v.size(); ++i)
				{
					os << v[i] << " ";
				}
			os << "}" << endl;

			os << "         BoxStats._cost = " << pos->second._cost << endl;
		}

	os << endl
	   << "   _numTuplesOnArcs (this = " << (void*) (& _numTuplesOnArcs) << ")" << endl;

	for (map<int, int>::const_iterator pos = _numTuplesOnArcs.begin();
		 pos != _numTuplesOnArcs.end(); ++pos)
		{
			os << "      arcId=" << pos->first << ", # tuples = " << pos->second << endl;
		}

	os << endl
	   << "   _inputArcsStats (this = " << (void*) (& _inputArcsStats) << ")" << endl;


	for (map<int, InputArcStats>::const_iterator pos = _inputArcsStats.begin();
		 pos != _inputArcsStats.end(); ++pos)
		{
			os << "      arcId=" << pos->first << ", InputArcStats (this = " 
			   << (const void*) (& pos->second) << ")" << endl;

			os << "         _inputRateHistory = { ";

			const vector<int> & v = pos->second._inputRateHistory;
			for (int i = 0; i < v.size(); ++i)
				{
					os << v[i] << " ";
				}
			os << "}" << endl;		
		}


	os << endl
	   << "   _appArcsStats (this = " << (void *) (& _appArcsStats) << ")" << endl;

	for (map<int, AppArcStats>::const_iterator  pos = _appArcsStats.begin();
		 pos != _appArcsStats.end(); ++pos)
		{
			int arcId = pos->first;
			const AppArcStats & s = pos->second;

			os << "      arcId = " << arcId << endl
			   << "      _valueHistogram = { ";

			for (size_t i = 0; i < s._valueHistogram.size(); ++i)
				{
					os << s._valueHistogram[i] << " ";
				}
			os << "}" << endl
			   //<< "      _qosUtility         = " << s._qosUtility << endl
			   << "      _accuracy			 = " << s._accuracy << endl
			   << "      _accuracyUtility	 = " << s._accuracyUtility << endl
			   << "      _outputValue		 = " << s._outputValue << endl
			   << "      _outputValueUtility = " << s._outputValueUtility 
			   << endl
			   << "      _avgLatency         = " << s._avgLatency << endl
			   << "      _avgLatencyUtility  = " << s._avgLatencyUtility << endl;
		}

	os << endl
	   << "   _utilityVsLsPrediction " << endl
	   << "       size = " << _utilityVsLsPrediction.size() << endl;
	for (size_t i = 0; i < _utilityVsLsPrediction.size(); ++i)
		{
			os << _utilityVsLsPrediction[i] << " ";
		}
	os << "}" << endl;

	return os.str();
}

//===============================================================================
