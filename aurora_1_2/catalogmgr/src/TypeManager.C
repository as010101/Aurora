#include "TypeManager.H"
#include <db_cxx.h>
#include "IntegerDbt.H"
#include "IntegerArrayDbt.H"
#include "CompositeTypeRecord.H"
#include "TypeFieldRecord.H"
#include <stdio.h>
#include <strstream.h>

const int TypeManager::UNDEFINED_TYPE = 0;
const int TypeManager::INTEGER = -1;
const int TypeManager::FLOAT = -2;
const int TypeManager::DOUBLE = -3;
const int TypeManager::STRING = -4;
const int TypeManager::BOOL = -5;
const int TypeManager::TIMESTAMP = -6;

    // note: all primitive types have values < 0,
    // all composite types have values > 0.

const char TypeManager::INTEGER_NAME[] = "integer";
const char TypeManager::FLOAT_NAME[] = "float";
const char TypeManager::DOUBLE_NAME[] = "double";
const char TypeManager::STRING_NAME[] = "string";
const char TypeManager::BOOL_NAME[] = "bool";
const char TypeManager::TIMESTAMP_NAME[] = "timestamp";

const int TypeManager::INTEGER_SIZE = 4;
const int TypeManager::FLOAT_SIZE = 4;
const int TypeManager::DOUBLE_SIZE = 8;
const int TypeManager::STRING_SIZE = -1;
const int TypeManager::BOOL_SIZE = 1;
const int TypeManager::TIMESTAMP_SIZE = 8;

TypeManager::TypeManager()
{
}

void TypeManager::addCompositeType(CompositeType* compositeType)
{
  m_compositeTypes.push_back(compositeType);
}

CompositeType* TypeManager::getCompositeType(int index)
{
  if ((index >= 0) && (index < m_compositeTypes.size()))
    return (m_compositeTypes[index]);
  else
    return NULL;
}

void TypeManager::generate( int type_size )
{
  m_compositeTypes.clear();

  CompositeType* c = new CompositeType( "Type1" );
  
  for ( int i = 0; i < type_size/4; i++ )
  {
	  ostrstream myO;
	  myO << "Attribute" << (i+1);
	  c->addAttribute( new Attribute( myO.str(), -1, 4 ) );
	  //	  printf("DEBUG: adding attribute: %s\n", myO.str() );
  }

  addCompositeType(c);
}

void TypeManager::load(string& directory)
{
  m_compositeTypes.clear();
  Db* pCompositeTypeDb = new Db(NULL, 0);
  if (pCompositeTypeDb != NULL)
  {
    Db* pTypeFieldDb = new Db(NULL, 0);
    if (pTypeFieldDb != NULL)
    {
      try
      {
        string fileName;
        fileName = directory+CompositeTypeRecord::databaseFileName;
        pCompositeTypeDb->open((directory+"CompositeType.db").c_str(), NULL, DB_BTREE, DB_CREATE, 664);
        fileName = directory+TypeFieldRecord::databaseFileName;
        pTypeFieldDb->open((fileName).c_str(), NULL, DB_BTREE, DB_CREATE, 664);

		    Dbc* iterator;
        Dbc* innerIterator;
		    pCompositeTypeDb->cursor(NULL, &iterator, 0);
		    pTypeFieldDb->cursor(NULL, &innerIterator, 0);

        IntegerDbt outerKey;
        CompositeTypeRecord data;

	      while (iterator->get(&outerKey, &data, DB_NEXT) == 0)
        {
          data.parse();
	  //          cout << outerKey.toString() << ": " << data.toString() << " is retrieved." << endl;
          CompositeType* c = new CompositeType(data.getName());

          int ints[] = {outerKey.getInteger(), 0};
          IntegerArrayDbt innerKey(ints, 2);
          TypeFieldRecord typeFieldRecord;
          if (innerIterator->get(&innerKey, &typeFieldRecord, DB_SET_RANGE) == 0)
          {
            int* indices = innerKey.getIntegers();
            if (indices[0] == outerKey.getInteger())
            {
                typeFieldRecord.parse();
		//                cout << innerKey.toString() << ": " << typeFieldRecord.toString() << " is retrieved." << endl;
                c->addAttribute(new Attribute(typeFieldRecord.getAttributeName(), typeFieldRecord.getAttributeTypeId(), typeFieldRecord.getAttributeSize()));
                while (innerIterator->get(&innerKey, &typeFieldRecord, DB_NEXT) == 0)
                {
                    indices = innerKey.getIntegers();
                    if (indices[0] != outerKey.getInteger()) break;
                    typeFieldRecord.parse();
		    //                    cout << innerKey.toString() << ": " << typeFieldRecord.toString() << " is retrieved." << endl;
                    c->addAttribute(new Attribute(typeFieldRecord.getAttributeName(), typeFieldRecord.getAttributeTypeId(), typeFieldRecord.getAttributeSize()));
                }
            }
          }
          addCompositeType(c);
        }
        pTypeFieldDb->close(0);
        pCompositeTypeDb->close(0);
      }
      catch(DbException& e) 
	  {
		  cout << "TypeManager::load(): DbException: ("
		       << e.get_errno() << ") " << e.what() << endl;
	      assert(false);
	  }
      delete pTypeFieldDb;
    }
    delete pCompositeTypeDb;
  }
}

void TypeManager::save(string& directory)
{
  //  printf("\nDEBUG: saving type manager\n");
  Db* pCompositeTypeDb = new Db(NULL, 0);
  if (pCompositeTypeDb != NULL)
  {
    Db* pTypeFieldDb = new Db(NULL, 0);
    if (pTypeFieldDb != NULL)
    {
      try
      {
        string fileName;
        fileName = directory+CompositeTypeRecord::databaseFileName;
	//        if(remove(fileName.c_str()) == -1 )
	  //          cout << "Could not delete" << fileName.c_str() << endl;
        pCompositeTypeDb->open((fileName).c_str(), NULL, DB_BTREE, DB_CREATE, 0664);

        fileName = directory+TypeFieldRecord::databaseFileName;
	//        if(remove(fileName.c_str()) == -1 )
	//          cout << "Could not delete" << fileName.c_str() << endl;
        pTypeFieldDb->open((fileName).c_str(), NULL, DB_BTREE, DB_CREATE, 0664);

        int offset = 0;
        for (register int i = 0; i < m_compositeTypes.size(); i++)
        {
          CompositeType& c = *(m_compositeTypes[i]);
          IntegerDbt outerKey(i+1);
          CompositeTypeRecord data(i+1, c.getName(), c.getNumberOfAttributes());
          pCompositeTypeDb->put(NULL, &outerKey, &data, DB_NOOVERWRITE);
	  //          cout << outerKey.toString() << ": " << data.toString() << endl;

          for (int j = 0; j < c.getNumberOfAttributes(); j++)
          {
            Attribute& a = c.getAttribute(j);
            int ints[] = {i+1, j};
            IntegerArrayDbt innerKey(ints, 2);
            TypeFieldRecord typeFieldRecord(i+1, j, a.m_fieldName, a.m_fieldType, a.m_size, offset);
            offset += a.m_size;
            pTypeFieldDb->put(NULL, &innerKey, &typeFieldRecord, DB_NOOVERWRITE);
	    //            cout << innerKey.toString() << ": " << typeFieldRecord.toString() << endl;
          }
        }
        pTypeFieldDb->close(0);
        pCompositeTypeDb->close(0);
      }
      catch(DbException& e) 
		{
			cout << "TypeManager::save(): DbException: ("
				 << e.get_errno() << ") " << e.what() << endl;
			assert(false);
		}
      delete pTypeFieldDb;
    }
    delete pCompositeTypeDb;
  }
}

string TypeManager::toString()
{
    strstream s;
    bool first = true;
    s << "Type Manager: " << endl << "[";
    for (register int i = 0; i < m_compositeTypes.size(); i++)
    {
        if (first)
            first = false;
        else
            s << ", ";
        CompositeType* a = m_compositeTypes[i];
        s << a->toString();
    }
    s << "]" << '\0';
    return s.str();
}



