#include "Relation.H"


Relation::Relation(Db *dbp) :
  dbp(dbp) {
}

Relation::Relation(DbEnv *dbenv, string name,
		   bt_compare_fcn_type bt_func) {
  try {
    // Use the relation environment.
    dbp = new Db(dbenv, 0);
  } catch (DbException& e) {
    cout << "Relation::Relation create DbException: ("
	 << e.get_errno() << ") " << e.what() << endl;
    assert(false);
  }

  if (bt_func != NULL) {
    try {
      dbp->set_bt_compare(bt_func);
    } catch (DbException& e) {
      cout << "setupLinearRoadEnvironment set_bt_compare acc DbException: ("
	   << e.get_errno() << ") " << e.what() << endl;
      assert(false);
    }
  }

  try {
    // TODO: HASH instead?
    // TODO: Transacbtions?
    dbp->open(name.c_str(), NULL, DB_BTREE, DB_CREATE, 0664);
  } catch (DbException& e) {
    cout << "Relation::Relation open DbException: ("
	 << e.get_errno() << ") " << e.what() << endl;
    assert(false);
  }
}

Relation::~Relation() {
  dbp->close(0);
  delete dbp;

}
