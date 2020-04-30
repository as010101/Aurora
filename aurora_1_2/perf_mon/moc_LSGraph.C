/****************************************************************************
** LSGraph meta object code from reading C++ file 'LSGraph.H'
**
** Created: Wed Jun 4 16:49:15 2003
**      by: The Qt MOC ($Id: moc_LSGraph.C,v 1.2 2003/07/03 16:23:01 cjc Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "LSGraph.H"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.2.0b1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *LSGraph::className() const
{
    return "LSGraph";
}

QMetaObject *LSGraph::metaObj = 0;
static QMetaObjectCleanUp cleanUp_LSGraph( "LSGraph", &LSGraph::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString LSGraph::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "LSGraph", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString LSGraph::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "LSGraph", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* LSGraph::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QCanvasView::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "curve", &static_QUType_ptr, "std::vector<double>", QUParameter::In }
    };
    static const QUMethod slot_0 = {"updateGraph", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ "newposition", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"moveSlider", 1, param_slot_1 };
    static const QMetaData slot_tbl[] = {
	{ "updateGraph(std::vector<double>)", &slot_0, QMetaData::Public },
	{ "moveSlider(int)", &slot_1, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_ptr, "float", QUParameter::In }
    };
    static const QUMethod signal_0 = {"changeLoadShed", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "changeLoadShed(float)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"LSGraph", parentObject,
	slot_tbl, 2,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_LSGraph.setMetaObject( metaObj );
    return metaObj;
}

void* LSGraph::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "LSGraph" ) )
	return this;
    return QCanvasView::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL changeLoadShed
void LSGraph::changeLoadShed( float t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,&t0);
    activate_signal( clist, o );
}

bool LSGraph::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: updateGraph((std::vector<double>)(*((std::vector<double>*)static_QUType_ptr.get(_o+1)))); break;
    case 1: moveSlider((int)static_QUType_int.get(_o+1)); break;
    default:
	return QCanvasView::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool LSGraph::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: changeLoadShed((float)(*((float*)static_QUType_ptr.get(_o+1)))); break;
    default:
	return QCanvasView::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool LSGraph::qt_property( int id, int f, QVariant* v)
{
    return QCanvasView::qt_property( id, f, v);
}

bool LSGraph::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
