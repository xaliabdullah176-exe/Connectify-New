/****************************************************************************
** Meta object code from reading C++ file 'newsfeedpage.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../newsfeedpage.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'newsfeedpage.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN12FeedPostCardE_t {};
} // unnamed namespace

template <> constexpr inline auto FeedPostCard::qt_create_metaobjectdata<qt_meta_tag_ZN12FeedPostCardE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FeedPostCard",
        "likeClicked",
        "",
        "postID",
        "commentClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'likeClicked'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'commentClicked'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FeedPostCard, qt_meta_tag_ZN12FeedPostCardE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FeedPostCard::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12FeedPostCardE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12FeedPostCardE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12FeedPostCardE_t>.metaTypes,
    nullptr
} };

void FeedPostCard::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FeedPostCard *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->likeClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->commentClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FeedPostCard::*)(int )>(_a, &FeedPostCard::likeClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FeedPostCard::*)(int )>(_a, &FeedPostCard::commentClicked, 1))
            return;
    }
}

const QMetaObject *FeedPostCard::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FeedPostCard::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12FeedPostCardE_t>.strings))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int FeedPostCard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void FeedPostCard::likeClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void FeedPostCard::commentClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}
namespace {
struct qt_meta_tag_ZN12NewsFeedPageE_t {};
} // unnamed namespace

template <> constexpr inline auto NewsFeedPage::qt_create_metaobjectdata<qt_meta_tag_ZN12NewsFeedPageE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "NewsFeedPage",
        "createPostClicked",
        "",
        "content",
        "imagePath",
        "likeClicked",
        "postID",
        "commentClicked",
        "logoutClicked",
        "goToProfile",
        "goToMessages",
        "goToSearch",
        "goToNotifications",
        "onPostBtnClicked",
        "onSelectImageClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'createPostClicked'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 }, { QMetaType::QString, 4 },
        }}),
        // Signal 'likeClicked'
        QtMocHelpers::SignalData<void(int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Signal 'commentClicked'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Signal 'logoutClicked'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'goToProfile'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'goToMessages'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'goToSearch'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'goToNotifications'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onPostBtnClicked'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSelectImageClicked'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<NewsFeedPage, qt_meta_tag_ZN12NewsFeedPageE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject NewsFeedPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12NewsFeedPageE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12NewsFeedPageE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12NewsFeedPageE_t>.metaTypes,
    nullptr
} };

void NewsFeedPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<NewsFeedPage *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->createPostClicked((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 1: _t->likeClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->commentClicked((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->logoutClicked(); break;
        case 4: _t->goToProfile(); break;
        case 5: _t->goToMessages(); break;
        case 6: _t->goToSearch(); break;
        case 7: _t->goToNotifications(); break;
        case 8: _t->onPostBtnClicked(); break;
        case 9: _t->onSelectImageClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)(const QString & , const QString & )>(_a, &NewsFeedPage::createPostClicked, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)(int )>(_a, &NewsFeedPage::likeClicked, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)(int )>(_a, &NewsFeedPage::commentClicked, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)()>(_a, &NewsFeedPage::logoutClicked, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)()>(_a, &NewsFeedPage::goToProfile, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)()>(_a, &NewsFeedPage::goToMessages, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)()>(_a, &NewsFeedPage::goToSearch, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (NewsFeedPage::*)()>(_a, &NewsFeedPage::goToNotifications, 7))
            return;
    }
}

const QMetaObject *NewsFeedPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *NewsFeedPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12NewsFeedPageE_t>.strings))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int NewsFeedPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void NewsFeedPage::createPostClicked(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void NewsFeedPage::likeClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void NewsFeedPage::commentClicked(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void NewsFeedPage::logoutClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void NewsFeedPage::goToProfile()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void NewsFeedPage::goToMessages()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void NewsFeedPage::goToSearch()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void NewsFeedPage::goToNotifications()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}
QT_WARNING_POP
