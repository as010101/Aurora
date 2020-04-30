/****************************************************************************
** LoadShedWidget meta object code from reading C++ file 'LoadShedWidget.H'
**
** Created: Wed Jun 4 16:49:28 2003
**      by: The Qt MOC ($Id: moc_LoadShedWidget.C,v 1.2 2003/07/03 16:23:01 cjc Exp $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "LoadShedWidget.H"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.2.0b1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *LoadShedWidget::className() const
{
    return "LoadShedWidget";
}

QMetaObject *LoadShedWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_LoadShedWidget( "LoadShedWidget", &LoadShedWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString LoadShedWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "LoadShedWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString LoadShedWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "LoadShedWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* LoadShedWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QHBoxLayout::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "val", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"changeValue", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "changeValue(int)", &slot_0, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"valueChanged", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "valueChanged(double)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"LoadShedWidget", parentObject,
	slot_tbl, 1,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_LoadShedWidget.setMetaObject( metaObj );
    return metaObj;
}

void* LoadShedWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "LoadShedWidget" ) )
	return this;
    return QHBoxLayout::qt_cast( clname );
}

// SIGNAL valueChanged
void LoadShedWidget::valueChanged( double t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

bool LoadShedWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: changeValue((int)static_QUType_int.get(_o+1)); break;
    default:
	return QHBoxLayout::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool LoadShedWidget::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: valueChanged((double)static_QUType_double.get(_o+1)); break;
    default:
	return QHBoxLayout::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool LoadShedWidget::qt_property( int id, int f, QVariant* v)
{
    return QHBoxLayout::qt_property( id, f, v);
}

bool LoadShedWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
