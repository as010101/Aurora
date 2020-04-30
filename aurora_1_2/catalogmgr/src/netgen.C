#include <iostream>
#include <StorageMgr_Exceptions.H>
#include <parseutil.H>
#include <QueryNetwork.H>
#include <CatalogManager.H>
#include <RandomPoint.H>
#include <algorithm>
#include <strstream.h>

using namespace std;

const string defaultBoxCosts         = "*0.12,0.03,N";
const string defaultBoxSelectivities = "*0.7,0.2,N";
const string defaultQosSpecs         = "*0.0:1.0:5.0:1.0:20.0:1.0";

//===============================================================================

void printUsage()
{
  cout << "Usage: netgen <options>" << endl
       << endl
       << "   Options may appear in any order. All mandatory unless otherwise stated." << endl
       << endl
       << "   -dir        <output-dir>                (used to be 'D')" << endl
       << "               (Directory must already exist, and should be empty)" << endl
       << endl
       << "   -numapps    <Number of apps in network> (used to be 'G')"
       << endl
       << "   -boxcosts   <cost-specs>                (used to be 'A')" << endl
       << "               Optional. Defaults to " << defaultBoxCosts << endl
       << endl
       << "   -boxsels    <box-selectivities>         (used to be 'B')" << endl
       << "               Optional. Defaults to " << defaultBoxSelectivities << endl
       << endl
       << "   -width      <network-branching-factor>  (used to be 'W')" << endl
       << endl
       << "   -depth      <network-depth>             (used to be 'X')" << endl
       << endl
       << "   -qos        <QoS specs>                 (used to be 'U')" << endl
       << "               Optional. Defaults to " << defaultQosSpecs << endl
       << endl
       << "   -tuplesize  <bytes per tuple>           (used to be 'z')" << endl
       << endl;
}

//===============================================================================

// TODO: Convert these to studly caps, and prefix with underscore.
struct RuntimeParms
{
  string db;
  int    num_applications;
  string box_costs;
  string box_selectivities;
  double net_width;
  int    net_depth;
  string qos_specs;
  int    tuple_size;
};

//===============================================================================

// Throws an exception if anything's unacceptable about the cmd-line. Returned
// values have defaults when appropriate.
RuntimeParms parseCmdLine(int argc, const char* argv[])
{
  RuntimeParms returnVal;

  returnVal.db = getUniqueStringArg("-dir", argc, argv);

  returnVal.num_applications = getUniqueIntArg("-numapps", argc, argv, 
					0, numeric_limits<int>::max());

  returnVal.net_depth = getUniqueIntArg("-depth", argc, argv, 
					0, numeric_limits<int>::max());

  returnVal.tuple_size = getUniqueIntArg("-tuplesize", argc, argv, 
					 0, numeric_limits<int>::max());

  returnVal.net_width = getUniqueDoubleArg("-width", argc, argv, 
					   -(numeric_limits<double>::max()), numeric_limits<double>::max());



  if (argInCmdLine("-qos", argc, argv))
    {
      returnVal.qos_specs = getUniqueStringArg("-qos", argc, argv);
    }
  else
    {
      returnVal.qos_specs = defaultQosSpecs;
    }


  if (argInCmdLine("-boxcosts", argc, argv))
    {
      returnVal.box_costs = getUniqueStringArg("-boxcosts", argc, argv);
	  cout << " Got Box costs " <<  returnVal.box_costs << endl;
    }
  else
    {
      returnVal.box_costs = defaultBoxCosts;
    }


  if (argInCmdLine("-boxsels", argc, argv))
    {
      returnVal.box_selectivities = getUniqueStringArg("-boxsels", argc, argv);
    }
  else
    {
      returnVal.box_selectivities = defaultBoxCosts;
    }

  return returnVal;
}

//===============================================================================

int main(int argc, const char* argv[])
{

  if ((argc == 1) || 
      (argInCmdLine("-help", argc, argv)) ||
      (argInCmdLine("--help", argc, argv)) ||
      (argInCmdLine("-?", argc, argv)))
    {
      printUsage();
      return 1;
    }

  cout << " What does that mean?? " << endl;
  try
    {
      RuntimeParms cmdLineParms = parseCmdLine(argc, argv);


      RandomPoint ***qos_graphs;
      RandomPoint **cost;
      RandomPoint **selectivity;
      int currPoint = 0;
      istrstream *parser;
      char token[50];
      int len = 0;

      // continue initialization. should probably make a common function
      // eventually to shorten code:
	  cout << " I am here " << endl;
      cost = ( RandomPoint ** )malloc( sizeof( RandomPoint **)*cmdLineParms.num_applications);

      if ( cmdLineParms.box_costs.c_str()[ 0 ] == '*' )
	{
	  currPoint = 0;
	  //cout << strlen( cmdLineParms.box_costs.c_str() ) << " DEBUGGY: " << cmdLineParms.box_costs.c_str() << endl;
	  parser = new istrstream( cmdLineParms.box_costs.c_str()+1 );
			
	  parser->getline( token, strlen( cmdLineParms.box_costs.c_str() ), ':' );
	  for ( int f = 0; f < cmdLineParms.num_applications; f++ )
	    {
	      cost[ currPoint++ ] = new RandomPoint( token );
	      cout << "Cost of the box: " << cost[ currPoint-1 ]->toString() << endl;
	    }
	  free( parser );
	}
      else
	{
	  throw SmException(__FILE__, __LINE__, "DO NOT HANDLE MULTIPLE PTS yet");
	}
		
      selectivity = ( RandomPoint ** )malloc( sizeof( RandomPoint **) 
					      * cmdLineParms.num_applications );
      if ( cmdLineParms.box_selectivities.c_str()[ 0 ] == '*' )
	{
	  currPoint = 0;
	  //memcpy( token2, optarg, 50 );
	  parser = new istrstream( cmdLineParms.box_selectivities.c_str()+1 );
			
	  parser->getline( token, strlen( cmdLineParms.box_selectivities.c_str() ), ':' );
	  for ( int f = 0; f < cmdLineParms.num_applications; f++ )
	  {
		  selectivity[ currPoint++ ] = new RandomPoint(token);
		  cout << " Here here " << selectivity[ currPoint-1 ]->toString() << endl;
	  }
	  free( parser );
	}
      else
	{
	  throw SmException(__FILE__, __LINE__, "DO NOT HANDLE MULTIPLE PTS yet");
	}
		
      // QOS specifications
      qos_graphs = ( RandomPoint *** )malloc( sizeof( RandomPoint **) * cmdLineParms.num_applications);
      for ( int z = 0; z < cmdLineParms.num_applications; z++ )
	qos_graphs[ z ] = ( RandomPoint ** )malloc( sizeof( RandomPoint *) * 6 ); // this is a single QoS, 6 points always.
      if ( cmdLineParms.qos_specs[ 0 ] == '*' )
	{
	  for ( int f = 0; f < cmdLineParms.num_applications; f++ )
	    {
	      currPoint = 0;
	      parser = new istrstream( cmdLineParms.qos_specs.c_str() + 1 );
	      len--;
	      while ( parser->good() != 0 )
		{
		  parser->getline( token, cmdLineParms.qos_specs.length(), ':' );
		  qos_graphs[ f ][ currPoint++ ] = new RandomPoint(token); 			    
		}
	      free( parser );
	    }
	}
      else
	{
	  //for ( int f = 0; f < cmdLineParms.num_applications*6; f++ )
			
	  int f=0;
	  {
	    currPoint = 0;
	    parser = new istrstream( cmdLineParms.qos_specs.c_str() );
	    //len--;
	    while ( parser->good() != 0 )
	      {
		parser->getline( token, cmdLineParms.qos_specs.length(), ':' );
		//printf("qos token: %s\n",token);
		//cout << " App " << f/6 << " Curr Pt " <<  (currPoint+1)%6 << endl;
		qos_graphs[ f/6 ][ (currPoint++)%6 ] = new RandomPoint(token); 			    
		f++;
	      }
	    free( parser );
	  }
	  //printf("DO NOT HANDLE MULTIPLE PTS yet. \n");
	  //exit( 1 );
	}

      //printf("GOT TO B\n");

      errno=0;
      setbuf(stdout,NULL);


      CatalogManager catalog(cmdLineParms.db.c_str());

      QueryNetwork *q_net;
      // Automatic Network Generation Stuff
      //float br[] = {3.1, 2.6, 2.9};
      //float br[] = {1.5, 1.5, 1.5};
      //int dp[] = {6, 6, 6};
      // the last fraction denotes number of (randomly picked)
      // shared boxes.
      //QueryNetwork *q_net = catalog.generate( 3, dp, br, .20 );
      //This generates a query network instead of loading it. Directory 
      //param is ignored. 3 = # of Appl, br = branching, dp = depth.


      /*
	float br[] = { cmdLineParms.net_width };
	int dp[] = { cmdLineParms.net_depth };
	float sh[] = { 0.0 };
	float qos1[] = {	0.0,	1.0,
	0.1,	1.0,
	20.0,	0.0  };
	printf(" NUM APP %d\n", cmdLineParms.num_applications );
	float **qos = ( float** )malloc( sizeof( *qos ) * cmdLineParms.num_applications );
      */
		
      /*
	float br[] = {	1.2,
	1.2,
	1.2,
	1.2,
	1.2};
	int dp[] = {5,
	5,
	5,
	5,
	5};
	float sh[] = { .1, .2, .1, .1, .9 }; //share coeff.
      */
      float *br = new float[cmdLineParms.num_applications];
      int *dp = new int[cmdLineParms.num_applications];
      float *sh = new float[cmdLineParms.num_applications];
      for ( int i = 0; i < cmdLineParms.num_applications; i++ )
	{
	  br[i] = cmdLineParms.net_width;
	  dp[i] = cmdLineParms.net_depth;
	  sh[i] = 0.0;
	}
      //float qos1[] = {0,1,10,1,20,0};
      //float qos2[] = {0,1,10.5,1,20,0};
      //float qos3[] = {0,1,10,1,20,0};
      //float qos4[] = {0,1,10.3,1,20.1,0};
      //float qos5[] = {0,1,10,1,20,0};
      //float **qos = ( float** )malloc( sizeof( *qos ) * 5 );
      //qos[ 0 ] = qos1;  qos[ 1 ] = qos2; 
      //qos[ 2 ] = qos3;  qos[ 3 ] = qos4; qos[ 4 ] = qos5; 
      //printf("----------------------cmdLineParms.num_applications: %i\n",cmdLineParms.num_applications);
      q_net = catalog.generate( cmdLineParms.num_applications, dp, br, sh, qos_graphs, cost, 
				selectivity, cmdLineParms.tuple_size );
    }
  catch (exception & e)
    {
      cout << "netgen: *** FAILED ***: main() caught exception: " << endl
	   << e.what() << endl;
      return 1;
    }

  cout << endl
       << "netgen: *** SUCCESS ***" << endl;
}


//===============================================================================
