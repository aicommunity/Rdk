/****************************************************************************
** Meta object code from reading C++ file 'UProjectDeployerQt.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Core/Application/Qt/UProjectDeployerQt.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'UProjectDeployerQt.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_RDK__UProjectDeployProcessingThread_t {
    QByteArrayData data[4];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RDK__UProjectDeployProcessingThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RDK__UProjectDeployProcessingThread_t qt_meta_stringdata_RDK__UProjectDeployProcessingThread = {
    {
QT_MOC_LITERAL(0, 0, 35), // "RDK::UProjectDeployProcessing..."
QT_MOC_LITERAL(1, 36, 29), // "processReadyReadStandardError"
QT_MOC_LITERAL(2, 66, 0), // ""
QT_MOC_LITERAL(3, 67, 30) // "processReadyReadStandardOutput"

    },
    "RDK::UProjectDeployProcessingThread\0"
    "processReadyReadStandardError\0\0"
    "processReadyReadStandardOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RDK__UProjectDeployProcessingThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RDK::UProjectDeployProcessingThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UProjectDeployProcessingThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->processReadyReadStandardError(); break;
        case 1: _t->processReadyReadStandardOutput(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject RDK::UProjectDeployProcessingThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_RDK__UProjectDeployProcessingThread.data,
    qt_meta_data_RDK__UProjectDeployProcessingThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RDK::UProjectDeployProcessingThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RDK::UProjectDeployProcessingThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RDK__UProjectDeployProcessingThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int RDK::UProjectDeployProcessingThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_RDK__UProjectResultsUploadingThread_t {
    QByteArrayData data[4];
    char stringdata0[98];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RDK__UProjectResultsUploadingThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RDK__UProjectResultsUploadingThread_t qt_meta_stringdata_RDK__UProjectResultsUploadingThread = {
    {
QT_MOC_LITERAL(0, 0, 35), // "RDK::UProjectResultsUploading..."
QT_MOC_LITERAL(1, 36, 29), // "processReadyReadStandardError"
QT_MOC_LITERAL(2, 66, 0), // ""
QT_MOC_LITERAL(3, 67, 30) // "processReadyReadStandardOutput"

    },
    "RDK::UProjectResultsUploadingThread\0"
    "processReadyReadStandardError\0\0"
    "processReadyReadStandardOutput"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RDK__UProjectResultsUploadingThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   24,    2, 0x08 /* Private */,
       3,    0,   25,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void RDK::UProjectResultsUploadingThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<UProjectResultsUploadingThread *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->processReadyReadStandardError(); break;
        case 1: _t->processReadyReadStandardOutput(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject RDK::UProjectResultsUploadingThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_RDK__UProjectResultsUploadingThread.data,
    qt_meta_data_RDK__UProjectResultsUploadingThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RDK::UProjectResultsUploadingThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RDK::UProjectResultsUploadingThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RDK__UProjectResultsUploadingThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int RDK::UProjectResultsUploadingThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_RDK__UProjectRunThread_t {
    QByteArrayData data[1];
    char stringdata0[23];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_RDK__UProjectRunThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_RDK__UProjectRunThread_t qt_meta_stringdata_RDK__UProjectRunThread = {
    {
QT_MOC_LITERAL(0, 0, 22) // "RDK::UProjectRunThread"

    },
    "RDK::UProjectRunThread"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_RDK__UProjectRunThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void RDK::UProjectRunThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject RDK::UProjectRunThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_RDK__UProjectRunThread.data,
    qt_meta_data_RDK__UProjectRunThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *RDK::UProjectRunThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RDK::UProjectRunThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RDK__UProjectRunThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int RDK::UProjectRunThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
