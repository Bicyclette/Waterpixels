/****************************************************************************
** Meta object code from reading C++ file 'window.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "window.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'window.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Window_t {
    QByteArrayData data[22];
    char stringdata0[302];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Window_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Window_t qt_meta_stringdata_Window = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Window"
QT_MOC_LITERAL(1, 7, 7), // "openImg"
QT_MOC_LITERAL(2, 15, 0), // ""
QT_MOC_LITERAL(3, 16, 7), // "saveDoc"
QT_MOC_LITERAL(4, 24, 18), // "computeHexagonGrid"
QT_MOC_LITERAL(5, 43, 18), // "computeWaterpixels"
QT_MOC_LITERAL(6, 62, 17), // "showOriginalImage"
QT_MOC_LITERAL(7, 80, 17), // "hideOriginalImage"
QT_MOC_LITERAL(8, 98, 10), // "showSmooth"
QT_MOC_LITERAL(9, 109, 10), // "hideSmooth"
QT_MOC_LITERAL(10, 120, 8), // "showGrid"
QT_MOC_LITERAL(11, 129, 8), // "hideGrid"
QT_MOC_LITERAL(12, 138, 12), // "showGradient"
QT_MOC_LITERAL(13, 151, 12), // "hideGradient"
QT_MOC_LITERAL(14, 164, 11), // "showMarkers"
QT_MOC_LITERAL(15, 176, 11), // "hideMarkers"
QT_MOC_LITERAL(16, 188, 19), // "showDistanceMarkers"
QT_MOC_LITERAL(17, 208, 19), // "hideDistanceMarkers"
QT_MOC_LITERAL(18, 228, 23), // "showRegularizedGradient"
QT_MOC_LITERAL(19, 252, 23), // "hideRegularizedGradient"
QT_MOC_LITERAL(20, 276, 12), // "showContours"
QT_MOC_LITERAL(21, 289, 12) // "hideContours"

    },
    "Window\0openImg\0\0saveDoc\0computeHexagonGrid\0"
    "computeWaterpixels\0showOriginalImage\0"
    "hideOriginalImage\0showSmooth\0hideSmooth\0"
    "showGrid\0hideGrid\0showGradient\0"
    "hideGradient\0showMarkers\0hideMarkers\0"
    "showDistanceMarkers\0hideDistanceMarkers\0"
    "showRegularizedGradient\0hideRegularizedGradient\0"
    "showContours\0hideContours"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Window[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  114,    2, 0x0a /* Public */,
       3,    0,  115,    2, 0x0a /* Public */,
       4,    0,  116,    2, 0x0a /* Public */,
       5,    0,  117,    2, 0x0a /* Public */,
       6,    0,  118,    2, 0x0a /* Public */,
       7,    0,  119,    2, 0x0a /* Public */,
       8,    0,  120,    2, 0x0a /* Public */,
       9,    0,  121,    2, 0x0a /* Public */,
      10,    0,  122,    2, 0x0a /* Public */,
      11,    0,  123,    2, 0x0a /* Public */,
      12,    0,  124,    2, 0x0a /* Public */,
      13,    0,  125,    2, 0x0a /* Public */,
      14,    0,  126,    2, 0x0a /* Public */,
      15,    0,  127,    2, 0x0a /* Public */,
      16,    0,  128,    2, 0x0a /* Public */,
      17,    0,  129,    2, 0x0a /* Public */,
      18,    0,  130,    2, 0x0a /* Public */,
      19,    0,  131,    2, 0x0a /* Public */,
      20,    0,  132,    2, 0x0a /* Public */,
      21,    0,  133,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void Window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Window *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->openImg(); break;
        case 1: _t->saveDoc(); break;
        case 2: _t->computeHexagonGrid(); break;
        case 3: _t->computeWaterpixels(); break;
        case 4: _t->showOriginalImage(); break;
        case 5: _t->hideOriginalImage(); break;
        case 6: _t->showSmooth(); break;
        case 7: _t->hideSmooth(); break;
        case 8: _t->showGrid(); break;
        case 9: _t->hideGrid(); break;
        case 10: _t->showGradient(); break;
        case 11: _t->hideGradient(); break;
        case 12: _t->showMarkers(); break;
        case 13: _t->hideMarkers(); break;
        case 14: _t->showDistanceMarkers(); break;
        case 15: _t->hideDistanceMarkers(); break;
        case 16: _t->showRegularizedGradient(); break;
        case 17: _t->hideRegularizedGradient(); break;
        case 18: _t->showContours(); break;
        case 19: _t->hideContours(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject Window::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_Window.data,
    qt_meta_data_Window,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Window::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Window.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int Window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
