/****************************************************************************
** Meta object code from reading C++ file 'UServerTransportTcpQt.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Core/Application/Qt/UServerTransportTcpQt.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UServerTransportTcpQt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_UServerSocketQt_t {
    QByteArrayData data[8];
    char stringdata0[84];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UServerSocketQt_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UServerSocketQt_t qt_meta_stringdata_UServerSocketQt = {
    {
QT_MOC_LITERAL(0, 0, 15), // "UServerSocketQt"
QT_MOC_LITERAL(1, 16, 14), // "onDisconnected"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 11), // "std::string"
QT_MOC_LITERAL(4, 44, 4), // "bind"
QT_MOC_LITERAL(5, 49, 11), // "onReadyRead"
QT_MOC_LITERAL(6, 61, 9), // "readyRead"
QT_MOC_LITERAL(7, 71, 12) // "disconnected"

    },
    "UServerSocketQt\0onDisconnected\0\0"
    "std::string\0bind\0onReadyRead\0readyRead\0"
    "disconnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UServerSocketQt[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,
       5,    1,   37,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   40,    2, 0x08 /* Private */,
       7,    0,   41,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void UServerSocketQt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UServerSocketQt *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onDisconnected((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 1: _t->onReadyRead((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 2: _t->readyRead(); break;
        case 3: _t->disconnected(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (UServerSocketQt::*)(std::string );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UServerSocketQt::onDisconnected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (UServerSocketQt::*)(std::string );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&UServerSocketQt::onReadyRead)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UServerSocketQt::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_UServerSocketQt.data,
    qt_meta_data_UServerSocketQt,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UServerSocketQt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UServerSocketQt::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UServerSocketQt.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int UServerSocketQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void UServerSocketQt::onDisconnected(std::string _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void UServerSocketQt::onReadyRead(std::string _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_UServerTransportTcpQt_t {
    QByteArrayData data[8];
    char stringdata0[120];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_UServerTransportTcpQt_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_UServerTransportTcpQt_t qt_meta_stringdata_UServerTransportTcpQt = {
    {
QT_MOC_LITERAL(0, 0, 21), // "UServerTransportTcpQt"
QT_MOC_LITERAL(1, 22, 19), // "ServerNewConnection"
QT_MOC_LITERAL(2, 42, 0), // ""
QT_MOC_LITERAL(3, 43, 15), // "SocketReadyRead"
QT_MOC_LITERAL(4, 59, 11), // "std::string"
QT_MOC_LITERAL(5, 71, 4), // "bind"
QT_MOC_LITERAL(6, 76, 18), // "SocketDisconnected"
QT_MOC_LITERAL(7, 95, 24) // "CommandQueueTimerTimeout"

    },
    "UServerTransportTcpQt\0ServerNewConnection\0"
    "\0SocketReadyRead\0std::string\0bind\0"
    "SocketDisconnected\0CommandQueueTimerTimeout"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_UServerTransportTcpQt[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x0a /* Public */,
       3,    1,   35,    2, 0x0a /* Public */,
       6,    1,   38,    2, 0x0a /* Public */,
       7,    0,   41,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,

       0        // eod
};

void UServerTransportTcpQt::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UServerTransportTcpQt *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->ServerNewConnection(); break;
        case 1: _t->SocketReadyRead((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 2: _t->SocketDisconnected((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        case 3: _t->CommandQueueTimerTimeout(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject UServerTransportTcpQt::staticMetaObject = { {
    QMetaObject::SuperData::link<QTcpServer::staticMetaObject>(),
    qt_meta_stringdata_UServerTransportTcpQt.data,
    qt_meta_data_UServerTransportTcpQt,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *UServerTransportTcpQt::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *UServerTransportTcpQt::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_UServerTransportTcpQt.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "RDK::UServerTransportTcp"))
        return static_cast< RDK::UServerTransportTcp*>(this);
    return QTcpServer::qt_metacast(_clname);
}

int UServerTransportTcpQt::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
