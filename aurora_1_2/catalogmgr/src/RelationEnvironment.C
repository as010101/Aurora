#include "RelationEnvironment.H"

static int get_position_for_accts(Db *dbp, const Dbt *pkey,
				  const Dbt *pdata, Dbt *skey);

static int compare_accidents(DB *dbp, const DBT *a, const DBT *b);

/**
 * Hack function to set up secondary keys for linear road relations and such.
 */
void
RelationEnvironment::setupLinearRoadEnvironment() {
  try {
    // Add a secondary index over accts by (lastpos, lastlane, lasttoll)
    Db *accts = createRelation(string("accts"), true)->getDb();

    // Creating accts_by_pos by hand and set it to allow duplicates.
    const char *name = "accts_by_pos";
    Db *accts_by_pos = new Db(dbenv, 0);
    accts_by_pos->set_flags(DB_DUP);
    accts_by_pos->open(name, NULL, DB_BTREE, DB_CREATE, 0664);
    Relation *r = new Relation(accts_by_pos);
    relations[string(name)] = r;

    accts->associate(accts_by_pos, &get_position_for_accts, 0);
  } catch (DbException& e) {
    cout << "setupLinearRoadEnvironment associate accts_by_pos DbException: ("
	 << e.get_errno() << ") " << e.what() << endl;
    assert(false);
  }

  // Set up a custom btree comparison function for acc.
  Db *acc = createRelation(string("acc"), true, &compare_accidents)->getDb();
}

/**
 * Pull position (values 2,3,4 of the data) out as a secondary key.
 */
static int
get_position_for_accts(Db *dbp, const Dbt *pkey,
		       const Dbt *pdata, Dbt *skey)
{
  assert(pdata->get_size() == 9 * sizeof(int));
  int *data = (int *)pdata->get_data();
  skey->set_data(&data[2]);
  skey->set_size(3*sizeof(int));
  return 0;
}

static inline int
compare_offset(int *a, int *b, int pos) {
  if (a[pos] < b[pos]) {
    return -1;
  } else if (a[pos] > b[pos]) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * Compare first by position, then by time.
 */
static int
compare_accidents(DB *dbp, const DBT *ap, const DBT *bp)
{
  // Key is (time, pos exp, pos seg, pos dir)
  assert(ap->size == 4*sizeof(int));
  assert(bp->size == 4*sizeof(int));
  int *a = (int*)ap->data;
  int *b = (int*)bp->data;
  int ret;
  ret = compare_offset(a, b, 1);
  if (ret == 0) {
    ret = compare_offset(a, b, 2);
  }
  if (ret == 0) {
    ret = compare_offset(a, b, 3);
  }
  if (ret == 0) {
    ret = compare_offset(a, b, 0);
  }
  return ret;
}

/**
 * Implementation of RelationEnvironment
 * Richard Tibbetts (tibbetts@mit.edu)
 */
RelationEnvironment::RelationEnvironment(string directory)
{
  dbenv = new DbEnv(0);
  dbenv->open(directory.c_str(),
	      DB_CREATE | DB_INIT_CDB | DB_INIT_MPOOL,
	      0);
}

RelationEnvironment::~RelationEnvironment()
{
  dbenv->close(0);
  delete dbenv;
}

Relation *
RelationEnvironment::createRelation(string name, bool create,
				    bt_compare_fcn_type bt_func)
{
  Relation *retval = relations[name];
  if (retval == NULL && create) {
    retval = new Relation(dbenv, name, bt_func);
    relations[name] = retval;
  } else {
    if (bt_func != NULL) {
      cerr << "Tried to set bt_func on existing Relation. Fail.";
      assert(false);
    }
  }
  return retval;
}

void
RelationEnvironment::destroyRelation(string name)
{
  Relation *r = relations[name];
  if (r != NULL) {
    delete r;
  } else {
    // TODO: Deal with error if relation doesn't exist.
  }
  relations[name] = NULL;
}

