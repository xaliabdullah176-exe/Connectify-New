/****************************************************************************
** Meta object code from reading C++ file 'admindashboard.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../admindashboard.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'admindashboard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN14AdminDashboardE_t {};
} // unnamed namespace

template <> constexpr inline auto AdminDashboard::qt_create_metaobjectdata<qt_meta_tag_ZN14AdminDashboardE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AdminDashboard",
        "deleteUserClicked",
        "",
        "userID",
        "banUserClicked",
        "unbanUserClicked",
        "viewUserClicked",
        "deletePostClicked",
        "postID",
        "logoutClicked",
        "navDashboard",
        "navUsers",
        "navPosts"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'deleteUserClicked'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'banUserClicked'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'unbanUserClicked'
        QtMocHelpers::SignalData<void(int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'viewUserClicked'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'deletePostClicked'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'logoutClicked'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'navDashboard'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'navUsers'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'navPosts'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AdminDashboard, qt_meta_tag_ZN14AdminDashboardE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AdminDashboard::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14AdminDashboardE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14AdminDashboardE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14AdminDashboardE_t>.metaTypes,
    nullptr
} };

void AdminDashboard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AdminDashboard *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->deleteUserClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->banUserClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->unbanUserClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->viewUserClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->deletePostClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->logoutClicked(); break;
        case 6: _t->navDashboard(); break;
        case 7: _t->navUsers(); break;
        case 8: _t->navPosts(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)(int )>(_a, &AdminDashboard::deleteUserClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)(int )>(_a, &AdminDashboard::banUserClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)(int )>(_a, &AdminDashboard::unbanUserClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)(int )>(_a, &AdminDashboard::viewUserClicked, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)(int )>(_a, &AdminDashboard::deletePostClicked, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)()>(_a, &AdminDashboard::logoutClicked, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)()>(_a, &AdminDashboard::navDashboard, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)()>(_a, &AdminDashboard::navUsers, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (AdminDashboard::*)()>(_a, &AdminDashboard::navPosts, 8))
            return;
    }
}

const QMetaObject *AdminDashboard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AdminDashboard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14AdminDashboardE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int AdminDashboard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void AdminDashboard::deleteUserClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void AdminDashboard::banUserClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void AdminDashboard::unbanUserClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void AdminDashboard::viewUserClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void AdminDashboard::deletePostClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void AdminDashboard::logoutClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void AdminDashboard::navDashboard()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void AdminDashboard::navUsers()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void AdminDashboard::navPosts()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}
QT_WARNING_POP
