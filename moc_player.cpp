/****************************************************************************
** Meta object code from reading C++ file 'player.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "player.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'player.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Player_t {
    QByteArrayData data[14];
    char stringdata0[171];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Player_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Player_t qt_meta_stringdata_Player = {
    {
QT_MOC_LITERAL(0, 0, 6), // "Player"
QT_MOC_LITERAL(1, 7, 8), // "mpdEvent"
QT_MOC_LITERAL(2, 16, 0), // ""
QT_MOC_LITERAL(3, 17, 5), // "event"
QT_MOC_LITERAL(4, 23, 13), // "statusChanged"
QT_MOC_LITERAL(5, 37, 9), // "newStatus"
QT_MOC_LITERAL(6, 47, 18), // "currentSongChanged"
QT_MOC_LITERAL(7, 66, 7), // "FapSong"
QT_MOC_LITERAL(8, 74, 7), // "newSong"
QT_MOC_LITERAL(9, 82, 15), // "songListChanged"
QT_MOC_LITERAL(10, 98, 22), // "QList<Player::FapSong>"
QT_MOC_LITERAL(11, 121, 11), // "newSongList"
QT_MOC_LITERAL(12, 133, 20), // "queueSongListChanged"
QT_MOC_LITERAL(13, 154, 16) // "newQueueSongList"

    },
    "Player\0mpdEvent\0\0event\0statusChanged\0"
    "newStatus\0currentSongChanged\0FapSong\0"
    "newSong\0songListChanged\0QList<Player::FapSong>\0"
    "newSongList\0queueSongListChanged\0"
    "newQueueSongList"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Player[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       4,    1,   42,    2, 0x06 /* Public */,
       6,    1,   45,    2, 0x06 /* Public */,
       9,    1,   48,    2, 0x06 /* Public */,
      12,    1,   51,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    5,
    QMetaType::Void, 0x80000000 | 7,    8,
    QMetaType::Void, 0x80000000 | 10,   11,
    QMetaType::Void, 0x80000000 | 10,   13,

       0        // eod
};

void Player::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Player *_t = static_cast<Player *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->mpdEvent((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->statusChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->currentSongChanged((*reinterpret_cast< FapSong(*)>(_a[1]))); break;
        case 3: _t->songListChanged((*reinterpret_cast< QList<Player::FapSong>(*)>(_a[1]))); break;
        case 4: _t->queueSongListChanged((*reinterpret_cast< QList<Player::FapSong>(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Player::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Player::mpdEvent)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Player::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Player::statusChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Player::*)(FapSong );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Player::currentSongChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Player::*)(QList<Player::FapSong> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Player::songListChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Player::*)(QList<Player::FapSong> );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Player::queueSongListChanged)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Player::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_Player.data,
      qt_meta_data_Player,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Player::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Player::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Player.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Player::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void Player::mpdEvent(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Player::statusChanged(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Player::currentSongChanged(FapSong _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Player::songListChanged(QList<Player::FapSong> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Player::queueSongListChanged(QList<Player::FapSong> _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
