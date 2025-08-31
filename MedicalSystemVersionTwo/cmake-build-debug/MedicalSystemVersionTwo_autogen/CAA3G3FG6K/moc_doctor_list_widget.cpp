/****************************************************************************
** Meta object code from reading C++ file 'doctor_list_widget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/ui/widgets/patient/doctor_list_widget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'doctor_list_widget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DoctorListWidget_t {
    QByteArrayData data[16];
    char stringdata0[197];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DoctorListWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DoctorListWidget_t qt_meta_stringdata_DoctorListWidget = {
    {
QT_MOC_LITERAL(0, 0, 16), // "DoctorListWidget"
QT_MOC_LITERAL(1, 17, 14), // "doctorSelected"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 8), // "doctorId"
QT_MOC_LITERAL(4, 42, 10), // "doctorName"
QT_MOC_LITERAL(5, 53, 10), // "department"
QT_MOC_LITERAL(6, 64, 5), // "title"
QT_MOC_LITERAL(7, 70, 19), // "onFilterTextChanged"
QT_MOC_LITERAL(8, 90, 4), // "text"
QT_MOC_LITERAL(9, 95, 25), // "onDepartmentFilterChanged"
QT_MOC_LITERAL(10, 121, 5), // "index"
QT_MOC_LITERAL(11, 127, 21), // "onDoctorDoubleClicked"
QT_MOC_LITERAL(12, 149, 11), // "QModelIndex"
QT_MOC_LITERAL(13, 161, 14), // "onDoctorsReply"
QT_MOC_LITERAL(14, 176, 14), // "QNetworkReply*"
QT_MOC_LITERAL(15, 191, 5) // "reply"

    },
    "DoctorListWidget\0doctorSelected\0\0"
    "doctorId\0doctorName\0department\0title\0"
    "onFilterTextChanged\0text\0"
    "onDepartmentFilterChanged\0index\0"
    "onDoctorDoubleClicked\0QModelIndex\0"
    "onDoctorsReply\0QNetworkReply*\0reply"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DoctorListWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    4,   39,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,   48,    2, 0x08 /* Private */,
       9,    1,   51,    2, 0x08 /* Private */,
      11,    1,   54,    2, 0x08 /* Private */,
      13,    1,   57,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::QString, QMetaType::QString,    3,    4,    5,    6,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::Int,   10,
    QMetaType::Void, 0x80000000 | 12,   10,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

void DoctorListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DoctorListWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->doctorSelected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 1: _t->onFilterTextChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->onDepartmentFilterChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onDoctorDoubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 4: _t->onDoctorsReply((*reinterpret_cast< QNetworkReply*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (DoctorListWidget::*)(const QString & , const QString & , const QString & , const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&DoctorListWidget::doctorSelected)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DoctorListWidget::staticMetaObject = { {
    &QWidget::staticMetaObject,
    qt_meta_stringdata_DoctorListWidget.data,
    qt_meta_data_DoctorListWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DoctorListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DoctorListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DoctorListWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int DoctorListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DoctorListWidget::doctorSelected(const QString & _t1, const QString & _t2, const QString & _t3, const QString & _t4)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
