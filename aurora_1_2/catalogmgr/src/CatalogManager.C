#include "CatalogManager.H"
#include <db_cxx.h>
#include "IntegerDbt.H"
#include "IntegerArrayDbt.H"
#include "BoxRecord.H"
#include "PortRecord.H"
#include "FilterBox.H"
#include "DropBox.H"
#include "MapBox.H"
#include "RestreamBox.H"
#include "UnionBox.H"
//#include "TumbleBox.H" bye bye! See Aggregate
#include "Application.H"
#include "ArcRecord.H"
//#include "XSectionBox.H"
//#include "WSortBox.H"
//#include "SlideBox.H"
#include "BSortBox.H"
#include "AggregateBox.H"
#include "JoinBox.H"
#include "ResampleBox.H"
#include "HelloWorldBox.H"
#include "UpdateRelationBox.H"
#include "ReadRelationBox.H"
#include "LRUpdateRelationBox.H"
#include "LRReadRelationBox.H"
#include <stdlib.h>

// This mess above would be NICER if it was just one h file including all boxes...
//  - eddie

// TODO: This is so the wrong place to initialize the database environment, but it
// works. Beat me if this is still here on 30 Jun 2003. -tibbetts
#include "RelationEnvironment.H"
RelationEnvironment *relation_environment;

#define BRANCH_RANGE 2  // this is the range that is formed around the
                        // the branch factor (fan-in)
#define BOX_MODIFIER 10000
// this one is going to be hard to explain :) Don wanted boxes to start
// at 0. GUI wants boxes (non-stream ones) to be above some constant
// so this is an artificial shift to accomodate everybody. done only
// on write-to-DB.

// this will be a macro.
//#define RAND( x ): (18.0*rand()/RAND_MAX);

string CatalogManager::getDirectory(const string & directory)
{
  if (directory.length() == 0 || directory.at(directory.length()-1) == '/')
    return directory;
  else
    return directory + '/';
}

CatalogManager::CatalogManager(const string & directory)
{
  m_directory = getDirectory(directory);

  arc_id = 0;
  box_id = 0;

  // I put this in to shut-up Valgrind when we don't call generate(...) on this
  // intance, but Alex really needs to address this. -cjc, 2 Mar 2003.
  qosBound = 0.0;
  
  //in_id = 0;
  out_id = 20000; 

  // TODO: This is so the wrong place to initialize the database environment, but it
  // works. Beat me if this is still here on 30 Jun 2003. -tibbetts
  // Put it in AUROA
  if (getenv("RUNNING_LINEAR_ROAD") != NULL) {

	  // I moved this setup stuff into the inside of this if-block, so that crashing
	  // from this could won't affect non-LinearRoad developers. 
	  //
	  // TODO: Longer-term: no LR-specific sniffing should be in any of the Aurora runtime.
	  // - cjc
	  char *home_dir = getenv("AURORA_SRC_DIR");
	  if (home_dir == NULL) {
		  cerr << "AURORA_SRC_DIR needs to be set. Dying." << endl;
		  assert(false);
	  }
	  string database_location(home_dir);
	  database_location.append("/dbs");
	  relation_environment = new RelationEnvironment(database_location);

    // Set up linear road custom stuff.
    relation_environment->setupLinearRoadEnvironment();
  }
}

string& CatalogManager::getDirectory()
{
  return m_directory;
}

// accepts the box id on which to build the network tree and
// the branching factor float (avg # of branches) and desired depth.
// ASSUMES THAT we only deal with union boxes!
void CatalogManager::generate_application( QueryNetwork* q, int ID, float branch, int depth, SelectBoxMap *sbm, int appl_id, RandomPoint *cost, RandomPoint *select )
{
  //  printf("\nSTARTING generate app on on %d, depth %d\n", ID, (depth));

  bool test = false;

  if ( depth == 1 )
    {
      Arc *arc;
      int typeId = 0;
      float rate = .1;
      int periodicity = 1;
      int adjustability = 1;
      //int temp = 10011;
      int numberOfTuples = 1000000000; // one billion
      q->addInputStream
	(new SourceStream( box_id++, rate, m_tm.getCompositeType( typeId ), 
			   periodicity,adjustability,numberOfTuples) );

      arc = new Arc( arc_id++, (float)1.0, m_tm.getCompositeType( 0 ),
		     box_id-1, 0, ID, 0, test );
      arc->makeInputArc();
      q->addArc( arc );
      ((UnionBox*)(q->getBox( box_id-2 )))->addInputArc( arc );
      // This is really input stream... Need to look it up differently.
      // probably does not (yet) need to know parent application...
      //((Box *)(q->getBox( box_id-1 )))->addParentAppl( appl_id );
      //      printf("RETURN from generate Catalog");
      return;
    }
  
  int actual_branch;
  //      printf(" Float branch %f, actual? %d\n", branch, actual_branch );
  if ( branch < 0 )
    {
    actual_branch = -( ( int )branch );
    }
    else
      {
	actual_branch = (int)(((1.0 * rand())/RAND_MAX)*BRANCH_RANGE*branch);
	if ( actual_branch < 1 ) actual_branch = 1;
      }
  //  printf("\nCM: actual branch?$%d curr depth %d\n", actual_branch, depth );

  for ( int i = 0; i < actual_branch; i++ )
    {
      //cout << endl << " GENERATE " << box_id << " APP " << appl_id << endl ;
      //cout << " PARA GENERATE " << cost->toString();
      //cout << "  " << select->toString();
      UnionBox *b = new UnionBox( box_id++, cost->nextPoint(), select->nextPoint() );
      q->addBox( b );
      
      /*if  (1.0*rand()/RAND_MAX > shared_boxes )
	if (sbm->find(b->getId()) == sbm->end())
	(*sbm)[b->getId()] = b; */

      q->addArc( new Arc( arc_id++, (float)1.0, m_tm.getCompositeType( 0 ),
			  box_id-1, 0, ID, 0, test ) );
      ((UnionBox*)(q->getBox( ID )))->addInputArc( q->getArc( arc_id-1 ) );
      //((UnionBox*)(q->getBox( box_id-1 )))->addParentAppl( appl_id );
      
      //      printf("\nCALLING generate app on on %d, depth %d INDEX? %i\n", box_id-1, (depth-1), i);
      generate_application( q, box_id-1, branch, depth-1, sbm, appl_id, cost, select );
      //shared_boxes );
    }  
  //  printf("\nEXITING generate app on on %d, depth %d\n", ID, (depth));

}

// Walk an application and randomly share (plug-in) into other
// applications.
void CatalogManager::walkAndShare( Box *ub, float probability, QueryNetwork *q,
				   int level )
{
  Application *app;
  Arc *arc;
  Box *box;
  vector<Arc*> inputArcs = ((UnionBox *)ub)->getInputArcs();
  int dependant = 0;

  double roll = (1.0*rand()/RAND_MAX);
  if ( level != 0 && probability > roll )
    {
      int id = 	20000+(int)((q->getMaxAppId()+1-20000)*(1.0*rand()/RAND_MAX ));

      if ( id > q->getMaxAppId() ) id = q->getMaxAppId();
      //printf(" roll %f, prob %f, so why?\n", roll, probability );
      //      printf(" CM: Attempt to share %d with APP %d at level %d max app %d\n", ub->getId(), id, level, q->getMaxAppId() );

      list<int> p;
      list<int>::iterator t_iter;

      //if ( !ub->isDescendantOf( id ) )
	{
	  app = q->getApplication( id ); 
	  // find right level box 
	  // add to current;
	  //	  printf("Got to B (sharing is successful)\n");

	  Box *b = q->getBox( (q->getArc
			       (app->getIncomingArcId()))->getSourceId() );
	  b = getBoxAtLevel( level - 1, b, q );
	  //  printf(" GOT to C consider make link from box %d to box %d\n", ub->getId(), b->getId() );

	  // consider.
	  p = b->getParentAppls();
	  for ( t_iter = p.begin(); (t_iter != p.end()); t_iter++ )
	    if ( ub->isDescendantOf( (*t_iter) ) )
	      dependant = 1;

	  p = ub->getParentAppls();
	  for ( t_iter = p.begin(); (t_iter != p.end()); t_iter++ )
	    if ( b->isDescendantOf( (*t_iter) ) )
	      dependant = 1;
	  
	  if ( !dependant )
	    {
	      arc = new Arc( arc_id++, (float)1.0, m_tm.getCompositeType( 0 ),
			     ub->getId(), 0, b->getId(), 0, 5 );
	      //cout << " NEW ARC " << arc->toString() << endl;
	      q->addArc( arc );
	      ((UnionBox *)b)->addInputArc( arc );
	      
	      // propogate application dependancy down, to avoid loops
	      p = b->getParentAppls();
	      for ( t_iter = p.begin(); t_iter != p.end(); t_iter++ )
		{
		  // printf("ADDING parent %d, (curr app %d, this box? %d)\n", (*t_iter), id, b->getId() );
		  addParentAppl( ub, (*t_iter), q );
		}

	      // propogate the connecting dependancies up, so that no one 
	      // else can can connect above the destination box and violate
	      // dependancies.
	      p = ub->getParentAppls();
	      for ( t_iter = p.begin(); t_iter != p.end(); t_iter++ )
		{
		  // printf("ADDING parent %d, (curr app %d, this box? %d)\n", (*t_iter), id, b->getId() );
		  addSuperAppl( b, (*t_iter), q );
		}
	    }
	  //else
	  //printf("CM: share failed, already shared\n");
	}
    }

  vector<Arc*>::iterator iter = inputArcs.begin();
  for ( iter == inputArcs.begin(); iter != inputArcs.end(); iter++ )
    {
      arc = (Arc*)(*iter);
      if ( !arc->isInputArc() )
	{
	  box = q->getBox( arc->getSourceId() );
	  //cout << "CM using arc " << arc->toString() << endl;
	  //printf("CM: bred, source node %d, got box? %d\n", arc->getSourceId(), q->getBox( arc->getSourceId() )->getId() );
	  //printf(" CM: recursively call self on box %d (prev box %d)\n", box->getId(), ub->getId() );
	  walkAndShare( box, probability, q, level+1 );
	}
    }
}

Box* CatalogManager::getBoxAtLevel( int level, Box *ub, QueryNetwork *q )
{
  if ( level == 0 )
    return ub;

  Arc *arc;
    vector<Arc*> inputArcs = ((UnionBox *)ub)->getInputArcs();

  vector<Arc*>::iterator iter = inputArcs.begin();

  int go = (int)(inputArcs.size())*(rand()/RAND_MAX );
  if ( go == inputArcs.size() ) go = inputArcs.size() -1;

  arc = (*(iter + go));

  if ( arc->isInputArc() )
    return ub;

  return getBoxAtLevel( level-1, q->getBox( arc->getSourceId() ), q );
}

void CatalogManager::addSuperAppl( Box *ub, int appl, QueryNetwork *q )
{
  Box *box;
  Arc *arc;

  vector<Arc*> outArcs;
  ArcMap& arcs = q->getArcs();

  for (ArcMapIter aiter = arcs.begin(); aiter != arcs.end(); aiter++)
    {
      arc = ((*aiter).second);
      if ( arc->getSourceId() == ub->getId() )
	outArcs.push_back( arc );
    }
  
  ub->addParentAppl( appl );
  
  vector<Arc*>::iterator iter = outArcs.begin();
  for ( iter == outArcs.begin(); iter != outArcs.end(); iter++ )
    {
      //printf(" CM: Got X %d for appl %d (box %d)\n", -37, appl, ub->getId() );
      arc = (Arc*)(*iter);

      if ( !arc->isOutputArc() )
	{
	  box = q->getBox( arc->getDestinationId() );
	  addSuperAppl( box, appl, q );
	}
    }
}

void CatalogManager::addParentAppl( Box *ub, int appl, QueryNetwork *q )
{
  Box *box;
  Arc *arc;
    vector<Arc*> inputArcs = ((UnionBox *)ub)->getInputArcs();

    ub->addParentAppl( appl );

  vector<Arc*>::iterator iter = inputArcs.begin();
  for ( iter == inputArcs.begin(); iter != inputArcs.end(); iter++ )
    {
      //printf(" CM: Got X %d for appl %d (box %d)\n", ub->getId(), appl, ub->getId() );
      arc = (Arc*)(*iter);

      if ( !arc->isInputArc() )
	{
	  box = q->getBox( arc->getSourceId() );
	  addParentAppl( box, appl, q );
	}
    }
}

QueryNetwork* CatalogManager::generate( int count, int *depth, float *branches,
					float *shared_boxes,RandomPoint ***qos,
					RandomPoint **costs, RandomPoint **selects, int tuple_size)
{
  Arc* arc;
  Box *b; //, *b1, *b2;
  bool test = false;
  const char *format = "0:1:3|0|%f~%f|%f~%f|%f~%f";
  //const char *format = "0:1:3|0|0~1|%d~1|20~0";
  char *qosSpecs;
  //int T;

  m_tm.generate( tuple_size );

  QueryNetwork* q = new QueryNetwork();
  Application *application;

  string s= getDirectory();
  m_tm.save( s );

  SelectBoxMap m_selectBoxes;
  bool blah;
  blah = false; // = ( 5 < 1 );
  for ( int app = 0; app < count; app++ )
    {
/*
      T = (int) (18.0*rand()/RAND_MAX);
      if ( T < 8 )
	qosSpecs = ( char * ) malloc( strlen( format ) );
      else
      qosSpecs = ( char * ) malloc( strlen( format ) +1 ); 
*/
      qosSpecs = (char *)malloc( strlen( format ) + 220 );// just to be safe...
      //printf(" LEN %d, specs? %d\n", strlen( format ), strlen( qosSpecs ) );
      sprintf( qosSpecs, format, qos[ app ][ 0 ]->nextPoint(), qos[ app ][ 1 ]->nextPoint(), 
	       qos[ app ][ 2 ]->nextPoint(), qos[ app ][ 3 ]->nextPoint(), qos[ app ][ 4 ]->nextPoint(), 
	       qos[ app ][ 5 ]->nextPoint() );
	  qosBound = qos[ app ][ 2 ]->getValue();
	  //printf(" QOS specs %s\n", qosSpecs );
      string str4 = string( qosSpecs );
      application = new Application( out_id++, str4 );
      q->addApplication( application );
      application->setSharing( *(shared_boxes+app) );
      
      b = new UnionBox( box_id++, costs[ app ]->nextPoint(), selects[ app ]->nextPoint() ); // id, cost, selectivity
      q->addBox( b );

      arc = new Arc( arc_id++, (float)1.0, m_tm.getCompositeType( 0 ),
		     box_id-1, 0, out_id-1, 0, test );
      arc->makeOutputArc(); // sort of a hack, to know where to stop traversing

      q->addArc( arc );
      q->getApplication( out_id-1 )->setIncomingArcId( arc_id-1 );
      
      //printf("\n CALLING generate %d from main part with branching factor of %f\n", (*(depth+app)), (float)(*(branches+app)));
      generate_application( q,box_id-1,(float)(*(branches+app)),(*(depth+app)),
			    &m_selectBoxes, out_id-1, costs[ app ], selects[ app ] ); 

      //printf(" THIS IS SPINAL %d\n", b->getId() );
      addParentAppl( b, out_id-1, q );
    }

  //SelectBoxMapIter i1, i2;
  Application *app;
  Box *b_walk;

  // this part randomly shares the connections between applications.
  // print number of to be sharred boxes.
  for (ApplicationMapIter aiter = q->getApplications().begin(); 
       aiter != q->getApplications().end(); aiter++)
    {
	      app = (Application*)((*aiter).second);

	      //printf("Got to A\n");
	      b_walk = q->getBox( (q->getArc
				   (app->getIncomingArcId()))->getSourceId() );

	      walkAndShare( b_walk, app->getSharing(), q, 0 );
    }

  /*
  printf( "DEBUG: to share %d out of %d\n", m_selectBoxes.size(), q->getBoxes().size() );
  while ( m_selectBoxes.size() > 1 )
    {
     i1 = m_selectBoxes.begin();
      i2 = i1;

      while ( i2 != m_selectBoxes.end()  &&
	      ((Box *)((*i2).second))->isDescendantOf
	      ( ((Box *)((*i1).second))->getParentAppl() ))
	i2++;

      if ( i2 != m_selectBoxes.end() )
	//( !((Box *)((*i2).second))->isDescendantOf
	//( ((Box *)((*i1).second))->getParentAppl() ) )
	{

	  b1 = ((*i1).second);  
	  b2 = ((*i2).second);

	  cout << "\nCONNECT " << b1->toString() << " AND " <<b2->toString()<<endl;
	  arc = new Arc( arc_id++, (float)1.0, m_tm.getCompositeType( 0 ),
			 b1->getId(), 0, b2->getId(), 0, test );
	  q->addArc( arc );
	  ((UnionBox *)b2)->addInputArc( arc );
	  
	  m_selectBoxes.erase( ((*i2).first) );
	}	
      m_selectBoxes.erase( ((*i1).first) );
      }
  */

  write( q );
  return q;
}

QueryNetwork* CatalogManager::load()
{
  typedef vector<int> IntVector;

  typedef map<int, IntVector*, less<int> > SuperBoxMap;
  typedef SuperBoxMap::iterator SuperBoxMapIter;

  SuperBoxMap superBoxInputMap;
  SuperBoxMap superBoxOutputMap;

  typedef map<int, int, less<int> > Int2LongMap;

  Int2LongMap inputPortMap;
  Int2LongMap outputPortMap;

  typedef map<long, int, less<long> > Long2IntMap;
  Long2IntMap target2ArcMap;

  m_tm.load(m_directory);
  QueryNetwork* q = new QueryNetwork();

  Db* pBoxDb = new Db(NULL, 0);
  if (pBoxDb != NULL)
  {
    try
    {
      string fileName;
      fileName = m_directory+BoxRecord::databaseFileName;
      pBoxDb->open(fileName.c_str(), NULL, DB_BTREE, DB_CREATE, 664);

      Dbc* iterator;
      pBoxDb->cursor(NULL, &iterator, 0);

      IntegerDbt key;
      BoxRecord data;

      while (iterator->get(&key, &data, DB_NEXT) == 0)
      {
        data.parse();
        //cout << key.toString() << ": " << data.toString() << " is retrieved." << endl;

	switch(data.getBoxType())
	  {
          case FILTER:
            q->addBox(new FilterBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
		break;
	  case DROP:
	    {     
	      string s = data.getModifier();
	      float f;
	      sscanf(s.c_str(), "%f", &f);
	      q->addBox(new DropBox(data.getBoxId(), data.getCost(), data.getSelectivity(), f, data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    }
	    break;
	  case MAP:
            q->addBox(new MapBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
            break;
          case RESTREAM:
            q->addBox(new RestreamBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
            break;
	  case UNION:
            q->addBox(new UnionBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    break;
	    //case TUMBLE:
	    //q->addBox(new TumbleBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    //break;
	  case AGGREGATE:
	    q->addBox(new AggregateBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    break;
	  case BSORT:
	    q->addBox(new BSortBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    break;
	  case JOIN:
	    q->addBox(new JoinBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    break;
	  case RESAMPLE:
	    q->addBox(new ResampleBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    break;
	    //case SLIDE:
	    //q->addBox(new SlideBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    //break;
	    //case XSECTION:
	    //q->addBox(new XSectionBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    //break;
	    // WSORT DOESNT EXIST ANYMORE. DUMP ME. - eddie
	    //case WSORT:
	    //q->addBox(new WSortBox(data.getBoxId(), data.getCost(), data.getSelectivity(), data.getModifier(), data.getX(), data.getY(), data.getWidth(), data.getHeight()));
	    //break;
	  case INPUTPORT:
            if (data.getParentId() == -1)
            {
              int typeId;
              float rate;
              int periodicity;
              int adjustability;
              int temp;
              int numberOfTuples;
              sscanf(data.getModifier().c_str(), "%d:%f:%d:%d:%d:%d", &typeId, &rate, &periodicity, &adjustability, &temp, &numberOfTuples);
              q->addInputStream(new SourceStream(data.getBoxId(), rate, m_tm.getCompositeType(typeId), periodicity, adjustability, numberOfTuples));
            }
           else
           {
              if (superBoxInputMap.find(data.getParentId()) != superBoxInputMap.end())
              {
                IntVector* v = superBoxInputMap[data.getParentId()];
                v->push_back(data.getBoxId());
                inputPortMap[data.getBoxId()] = data.getParentId()*(long)0x10000+v->size()-1;
              }
              else
              {
                IntVector* v = new IntVector();
                v->push_back(data.getBoxId());
                superBoxInputMap[data.getParentId()] = v;
                inputPortMap[data.getBoxId()] = data.getParentId()*(long)0x10000+v->size()-1;
              }
            }
            break;
          case OUTPUTPORT:
            if (data.getParentId() == -1)
            {
              q->addApplication(new Application(data.getBoxId(), data.getModifier()));
            }
            else
           {
              if (superBoxOutputMap.find(data.getParentId()) != superBoxOutputMap.end())
              {
                IntVector* v = superBoxOutputMap[data.getParentId()];
                v->push_back(data.getBoxId());
                outputPortMap[data.getBoxId()] = data.getParentId()*(long)0x10000+v->size()-1;
              }
              else
              {
                IntVector* v = new IntVector();
                v->push_back(data.getBoxId());
                superBoxOutputMap[data.getParentId()] = v;
                outputPortMap[data.getBoxId()] = data.getParentId()*(long)0x10000+v->size()-1;
              }
            }
            break;
	  case HELLO_WORLD:
            q->addBox(new HelloWorldBox(data.getBoxId(), data.getCost(),
					data.getSelectivity(), data.getModifier(),
					data.getX(), data.getY(),
					data.getWidth(), data.getHeight()));
	    break;
	  case UPDATE_RELATION:
            q->addBox(new UpdateRelationBox(data.getBoxId(), data.getCost(),
					    data.getSelectivity(), data.getModifier(),
					    data.getX(), data.getY(),
					    data.getWidth(), data.getHeight()));
	    break;
	  case READ_RELATION:
            q->addBox(new ReadRelationBox(data.getBoxId(), data.getCost(),
					  data.getSelectivity(), data.getModifier(),
					  data.getX(), data.getY(),
					  data.getWidth(), data.getHeight()));
	    break;
	  case LR_UPDATE_RELATION:
            q->addBox(new LRUpdateRelationBox(data.getBoxId(), data.getCost(),
					      data.getSelectivity(), data.getModifier(),
					      data.getX(), data.getY(),
					      data.getWidth(), data.getHeight()));
	    break;
	  case LR_READ_RELATION:
            q->addBox(new LRReadRelationBox(data.getBoxId(), data.getCost(),
					    data.getSelectivity(), data.getModifier(),
					    data.getX(), data.getY(),
					    data.getWidth(), data.getHeight()));
	    break;
	  default:
	    cerr << "Found box with unknown type " << data.getBoxType() << " and id "
		 << data.getBoxId() << endl;
	    assert(false);
	  }
      }
      pBoxDb->close(0);
    }
    catch(DbException& e) 
      {
	cout << "CatalogManager::load(): open pBoxDb: DbException: ("
	     << e.get_errno() << ") " << e.what() << endl;
    	assert(false);
      }
    delete pBoxDb;
  }
  //cout << endl;

  Db* pArcDb = new Db(NULL, 0);
  if (pArcDb != NULL)
  {
    try
    {
      string fileName;
      fileName = m_directory+ArcRecord::databaseFileName;
      pArcDb->open(fileName.c_str(), NULL, DB_BTREE, DB_CREATE, 664);


      Dbc* iterator;
      pArcDb->cursor(NULL, &iterator, 0);

      IntegerDbt key;
      ArcRecord data;

      while (iterator->get(&key, &data, DB_NEXT) == 0)
      {
        data.parse();
        //cout << key.toString() << ": " << data.toString() << " is retrieved." << endl;
        target2ArcMap[(long)0x10000*data.getTargetNodeId()+data.getTargetPortIndex()] = data.getId();
        Arc* arc = new Arc(data.getId(), data.getRate(), m_tm.getCompositeType(data.getTypeId()),
            data.getSourceNodeId(), data.getSourcePortIndex(),
            data.getTargetNodeId(), data.getTargetPortIndex(), data.getCpFlag());
        q->addArc(arc);
        Box* b = q->getBox(data.getTargetNodeId());
        if ((b != NULL) && (b->getBoxType() == UNION))
	  ((UnionBox*)b)->addInputArc(arc);
        Application* a = q->getApplication(data.getTargetNodeId());
        if (a != NULL)
          a->setIncomingArcId(data.getId());

		// BEGIN (tatbul@cs.brown.edu)
		// the following lines are added 
		// to fix the input/output flags in Arc.H
		//
		Application *app = q->getApplication(data.getTargetNodeId());
		if (app)
			q->getArc(data.getId())->makeOutputArc();
		SourceStream *i_str = q->getInputStream(data.getSourceNodeId());
		if (i_str)
			q->getArc(data.getId())->makeInputArc();
		//
		//	END (tatbul@cs.brown.edu)
      }
      pArcDb->close(0);
    }
    catch(DbException& e) 
	{
		cout << "CatalogManager::load(): open pArcDb: DbException: ("
	         << e.get_errno() << ") " << e.what() << endl;
    	assert(false);
    }
    delete pArcDb;
  }
  //cout << endl;

  //int m_sourceId;
  //int m_sourcePortId;
  //int m_destinationId;
  //int m_destinationPortId;

  //cout << q->toString() << endl;

  for (SuperBoxMapIter siter = superBoxInputMap.begin(); siter != superBoxInputMap.end(); siter++)
  {
    int superBoxId = ((*siter).first);
    IntVector& v = *(((*siter).second));
    for (int i = 0; i < v.size(); i++)
    {
      ArcMap& arcs = q->getArcs();

      int nodeId = -1;
      int portId = -1;
      long temp = (long)0x10000*superBoxId+i;
      if (target2ArcMap.find(temp) != target2ArcMap.end())
      {
        int arcId = target2ArcMap[temp];
        Arc* a = q->getArc(arcId);
        nodeId = a->getSourceId();
        portId = a->getSourcePortId();
        arcs.erase(arcId);
      }
      for (ArcMapIter aiter = arcs.begin(); aiter != arcs.end(); aiter++)
      {
        Arc& arc = *(((*aiter).second));
        if (arc.getSourceId() == v[i])
        {
          if (nodeId >= 0)
          {
            arc.setSourceId(nodeId);
            arc.setSourcePortId(portId);
          }
          else
          {
            ArcMapIter ii = aiter;
            aiter--;
            arcs.erase(ii);
          }
        }
      }
    }
  }

  for (SuperBoxMapIter siter = superBoxOutputMap.begin(); siter != superBoxOutputMap.end(); siter++)
  {
    int superBoxId = ((*siter).first);
    IntVector& v = *(((*siter).second));
    for (int i = 0; i < v.size(); i++)
    {
      ArcMap& arcs = q->getArcs();

      int nodeId = -1;
      int portId = -1;
      long temp = (long)0x10000*v[i];
      if (target2ArcMap.find(temp) != target2ArcMap.end())
      {
        int arcId = target2ArcMap[temp];
        Arc* a = q->getArc(arcId);
        nodeId = a->getSourceId();
        portId = a->getSourcePortId();
        arcs.erase(arcId);
      }
      for (ArcMapIter aiter = arcs.begin(); aiter != arcs.end(); aiter++)
      {
        Arc& arc = *(((*aiter).second));
        if (arc.getSourceId() == superBoxId && arc.getSourcePortId() == i)
        {
          if (nodeId >= 0)
          {
            arc.setSourceId(nodeId);
            arc.setSourcePortId(portId);
          }
          else
          {
            ArcMapIter ii = aiter;
            aiter--;
            arcs.erase(ii);
          }
        }
      }
    }
  }
  return q;
}

QueryNetwork* CatalogManager::write( QueryNetwork *q )
{
  typedef vector<int> IntVector;

  typedef map<int, IntVector*, less<int> > SuperBoxMap;
  typedef SuperBoxMap::iterator SuperBoxMapIter;

  SuperBoxMap superBoxInputMap;
  SuperBoxMap superBoxOutputMap;

  typedef map<int, int, less<int> > Int2LongMap;

  Int2LongMap inputPortMap;
  Int2LongMap outputPortMap;

  typedef map<long, int, less<long> > Long2IntMap;
  Long2IntMap target2ArcMap;

  //cout << "DEBUG: CatalogManager: Begin writing boxes";
  Db* pBoxDb = new Db(NULL, 0);
  if (pBoxDb != NULL)
    {
      try
	{
	  string fileName;
	  string blah = "";
	  fileName = m_directory+BoxRecord::databaseFileName;
	  //printf("DBtrying to open %s\n", fileName.c_str());
	  pBoxDb->open(fileName.c_str(), NULL, DB_BTREE, DB_CREATE, 0664);
	  BoxRecord *data;
	  Box *box;
	  
	  for (BoxMapIter biter = q->getBoxes().begin(); 
	       biter != q->getBoxes().end(); biter++)
	    {
	      box = (Box*)((*biter).second);

	      //cout << "DB Writing box: " << box->toString() << endl;

	      int ints[] = {box->getId()+BOX_MODIFIER, -1};
	      IntegerArrayDbt key(ints, 2);
	     
	      //printf(" key init done\n");
	      data = new BoxRecord(box->getId()+BOX_MODIFIER,
				   box->getCost(), 
				   // Made the following change to get a clean compile. -cjc 17 Feb 2003
				   //box->getSelectivity(), 
				   box->getSelectivity(), 
				   "",
				   // Made the following change to get a clean compile. -cjc 17 Feb 2003
				   //box->getBoxType() );
				   (box->getBoxType()) );
	      pBoxDb->put( NULL, &key, data, 0 );
	      //Dbc* iterator;
	      //pBoxDb->cursor(NULL, &iterator, 0);
	    }
	  
	  SourceStream *stream;
	  
	  for (StreamMapIter siter = q->getInputStreams().begin(); 
	       siter != q->getInputStreams().end(); siter++)
	    {
	      stream = (SourceStream*)((*siter).second);

	      //cout << "Writing box (input str): " << stream << endl;

	      int ints[] = { stream->getId(), -1 };
	      IntegerArrayDbt key(ints, 2);
	     
	      data = new BoxRecord( stream->getId(), -1, -1.0, 
				    stream->getModifier(), INPUTPORT );
	      pBoxDb->put( NULL, &key, data, 0 );
	    }
	  
	  Application *app;
	  
	  for (ApplicationMapIter aiter = q->getApplications().begin(); 
	       aiter != q->getApplications().end(); aiter++)
	    {
	      app = (Application*)((*aiter).second);

	      //cout << "Writing box (appl): " << app->getModifier() << endl;

	      int ints[] = { app->getId()+BOX_MODIFIER, -1 };
	      IntegerArrayDbt key(ints, 2);
	     
	      data = new BoxRecord( app->getId()+BOX_MODIFIER, -1, -1.0, 
				    app->getModifier(),OUTPUTPORT );
	      
	      //cout << "Writing box record (appl): " << data->toString();
	      pBoxDb->put( NULL, &key, data, 0 );
	    }
	  pBoxDb->close(0);
	}
	catch(DbException& e) 
	{ 
	  cout << "CatalogManager::write(): open pBoxDb: DbException: ("
	       << e.get_errno() << ") " << e.what() << endl;
	  assert(false);
	}
      delete pBoxDb;
    }
  
  //cout << endl;

  //cout << "\nDEBUG: CatalogManager: Begin writing Arcs\n";

  Db* pArcDb = new Db(NULL, 0);
  if (pArcDb != NULL)
  {
    try
    {
      string fileName;
      fileName = m_directory+ArcRecord::databaseFileName;
      pArcDb->open(fileName.c_str(), NULL, DB_BTREE, DB_CREATE, 0664);
      
      ArcRecord *data;
      Arc *arc;
      int real_src = 0;

      for (ArcMapIter arciter = q->getArcs().begin(); 
	   arciter != q->getArcs().end(); arciter++)
	{
	      arc = (Arc*)((*arciter).second);

	      int ints[] = { arc->getId(), -1 };
	      IntegerArrayDbt key(ints, 2);

	      real_src = arc->getSourceId();
	      if ( !arc->isInputArc() ) real_src += BOX_MODIFIER;
	      data = new ArcRecord( arc->getId(),real_src,//arc->getSourceId(),
				    arc->getSourcePortId(), 
				    arc->getDestinationId()+BOX_MODIFIER, 
				    arc->getDestinationPortId() );
	      
	      pArcDb->put( NULL, &key, data, 0 );
	}
      
      pArcDb->close(0);
    }
    catch(DbException& e) 
    {
		cout << "CatalogManager::write(): open pArcDb: DbException: ("
	         << e.get_errno() << ") " << e.what() << endl;
    	assert(false);
    }
    delete pArcDb;
  }
  
  Db* pPortDb = new Db(NULL, 0);
  if (pPortDb != NULL)
  {
    try
    {
      string fileName;
      fileName = m_directory+PortRecord::databaseFileName;
      pPortDb->open(fileName.c_str(), NULL, DB_BTREE, DB_CREATE, 0664);
      
      Arc *arc;
      PortRecord *data;

      for (ArcMapIter arciter = q->getArcs().begin(); 
	   arciter != q->getArcs().end(); arciter++)
	{
	  int real_src = arc->getSourceId();
	  if ( !arc->isInputArc() ) real_src += BOX_MODIFIER;
	    arc = (Arc*)((*arciter).second);

	      // Note, -1 is the parent. No support for SuperBoxes (yet).
	    int intsS[] = { real_src, /*arc->getSourceId(), */arc->getSourcePortId(),
			    PortRecord::OUTPUTPORTTYPE, -1 };
	      IntegerArrayDbt keyS(intsS, 4);

	      // output port
	      data =new PortRecord( real_src, /*arc->getSourceId(),*/ arc->getSourcePortId(),
				    0,
				    PortRecord::OUTPUTPORTTYPE );
	      
	      pPortDb->put( NULL, &keyS, data, 0 );
	
	      // Note, -1 is the parent. No support for SuperBoxes (yet).
	      int intsT[] ={ arc->getDestinationId()+BOX_MODIFIER,
			     arc->getDestinationPortId(),
			     PortRecord::INPUTPORTTYPE, -1 };
	      IntegerArrayDbt keyT(intsT, 4);

	      // input (target) port
	      data =new PortRecord( arc->getDestinationId()+BOX_MODIFIER,
				    arc->getDestinationPortId(),
				    0, 
				    PortRecord::INPUTPORTTYPE );
	      
	      pPortDb->put( NULL, &keyT, data, 0 );
	}
      
      pPortDb->close(0);
    }
    catch(DbException& e) {
      cout << "CatalogManager::write(): open pPortDb: DbException: ("
	   << e.get_errno() << ") " << e.what() << endl;
      assert(false);
    }
    delete pPortDb;
  }
  
  //cout << endl;
      
  //cout << q->toString() << endl;

  return q;
}

TypeManager& CatalogManager::getTypeManager()
{
  return m_tm;
}




