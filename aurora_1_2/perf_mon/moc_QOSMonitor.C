/****************************************************************************
** QOSMonitor meta object code from reading C++ file 'QOSMonitor.H'
**
** Created: Wed Jun 4 16:49:30 2003
**      by: The Qt MOC ($Id: moc_QOSMonitor.C,v 1.2 2003/07/03 16:23:01 cjc Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "QOSMonitor.H"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.2.0b1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *QOSMonitor::className() const
{
    return "QOSMonitor";
}

QMetaObject *QOSMonitor::metaObj = 0;
static QMetaObjectCleanUp cleanUp_QOSMonitor( "QOSMonitor", &QOSMonitor::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString QOSMonitor::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QOSMonitor", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString QOSMonitor::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "QOSMonitor", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* QOSMonitor::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QVBoxLayout::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "newval", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"update", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "util", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"setExpectedUtil", 1, param_slot_1 };
    static const QMetaData slot_tbl[] = {
	{ "update(int)", &slot_0, QMetaData::Public },
	{ "setExpectedUtil(int)", &slot_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"QOSMonitor", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_QOSMonitor.setMetaObject( metaObj );
    return metaObj;
}

void* QOSMonitor::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "QOSMonitor" ) )
	return this;
    return QVBoxLayout::qt_cast( clname );
}

bool QOSMonitor::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: update((int)static_QUType_int.get(_o+1)); break;
    case 1: setExpectedUtil((int)static_QUType_int.get(_o+1)); break;
    default:
	return QVBoxLayout::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool QOSMonitor::qt_emit( int _id, QUObject* _o )
{
    return QVBoxLayout::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool QOSMonitor::qt_property( int id, int f, QVariant* v)
{
    return QVBoxLayout::qt_property( id, f, v);
}

bool QOSMonitor::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
