/****************************************************************************
** LoadShedPage meta object code from reading C++ file 'LoadShedPage.H'
**
** Created: Wed Jun 4 16:49:30 2003
**      by: The Qt MOC ($Id: moc_LoadShedPage.C,v 1.2 2003/07/03 16:23:01 cjc Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "LoadShedPage.H"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.2.0b1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *LoadShedPage::className() const
{
    return "LoadShedPage";
}

QMetaObject *LoadShedPage::metaObj = 0;
static QMetaObjectCleanUp cleanUp_LoadShedPage( "LoadShedPage", &LoadShedPage::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString LoadShedPage::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "LoadShedPage", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString LoadShedPage::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "LoadShedPage", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* LoadShedPage::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "monitornum", &static_QUType_int, 0, QUParameter::In },
	{ "latencyvalue", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"updateMonitor", 2, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "curve", &static_QUType_ptr, "std::vector<double>", QUParameter::In }
    };
    static const QUMethod slot_1 = {"updateGraph", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "val", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"setMonitorsExpectedUtility", 1, param_slot_2 };
    static const QMetaData slot_tbl[] = {
	{ "updateMonitor(int,int)", &slot_0, QMetaData::Public },
	{ "updateGraph(std::vector<double>)", &slot_1, QMetaData::Public },
	{ "setMonitorsExpectedUtility(int)", &slot_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"LoadShedPage", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_LoadShedPage.setMetaObject( metaObj );
    return metaObj;
}

void* LoadShedPage::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "LoadShedPage" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool LoadShedPage::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: updateMonitor((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    case 1: updateGraph((std::vector<double>)(*((std::vector<double>*)static_QUType_ptr.get(_o+1)))); break;
    case 2: setMonitorsExpectedUtility((int)static_QUType_int.get(_o+1)); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool LoadShedPage::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool LoadShedPage::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool LoadShedPage::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
