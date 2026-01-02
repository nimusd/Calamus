/****************************************************************************
** Meta object code from reading C++ file 'calamusmain.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../calamusmain.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'calamusmain.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
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
struct qt_meta_tag_ZN11CalamusMainE_t {};
} // unnamed namespace

template <> constexpr inline auto CalamusMain::qt_create_metaobjectdata<qt_meta_tag_ZN11CalamusMainE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "CalamusMain",
        "stopAllPlayback",
        "",
        "onTabChanged",
        "index",
        "onNewSounit",
        "onLoadSounit",
        "onSaveSounitAs",
        "onSounitNameChanged",
        "name",
        "onSounitCommentChanged",
        "comment",
        "onSounitSelectorChanged",
        "onSounitNameEdited",
        "onSounitCommentEdited",
        "onAddTrack",
        "onContainerSelected",
        "Container*",
        "container",
        "onConnectionSelected",
        "connectionIndex",
        "onSelectionCleared",
        "onInstanceNameChanged",
        "onConnectionFunctionChanged",
        "onConnectionWeightChanged",
        "value",
        "onDisconnectClicked",
        "onConfigParameterChanged",
        "QListWidgetItem*",
        "item",
        "onDnaSelectChanged",
        "onNumHarmonicsChanged",
        "onPhraseSelectionChanged",
        "onPhraseNameChanged",
        "onPhraseEasingChanged",
        "onLoadPhraseTemplateClicked",
        "onApplyPhraseTemplateClicked",
        "onCreatePhraseClicked",
        "onUngroupPhraseClicked",
        "onSavePhraseTemplateClicked"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'stopAllPlayback'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onTabChanged'
        QtMocHelpers::SlotData<void(int)>(3, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onNewSounit'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLoadSounit'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSaveSounitAs'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSounitNameChanged'
        QtMocHelpers::SlotData<void(const QString &)>(8, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'onSounitCommentChanged'
        QtMocHelpers::SlotData<void(const QString &)>(10, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'onSounitSelectorChanged'
        QtMocHelpers::SlotData<void(int)>(12, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onSounitNameEdited'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSounitCommentEdited'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddTrack'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onContainerSelected'
        QtMocHelpers::SlotData<void(Container *)>(16, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 17, 18 },
        }}),
        // Slot 'onConnectionSelected'
        QtMocHelpers::SlotData<void(int)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 20 },
        }}),
        // Slot 'onSelectionCleared'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onInstanceNameChanged'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionFunctionChanged'
        QtMocHelpers::SlotData<void(int)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onConnectionWeightChanged'
        QtMocHelpers::SlotData<void(double)>(24, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Double, 25 },
        }}),
        // Slot 'onDisconnectClicked'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConfigParameterChanged'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(27, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 28, 29 },
        }}),
        // Slot 'onDnaSelectChanged'
        QtMocHelpers::SlotData<void(int)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onNumHarmonicsChanged'
        QtMocHelpers::SlotData<void(int)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 25 },
        }}),
        // Slot 'onPhraseSelectionChanged'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPhraseNameChanged'
        QtMocHelpers::SlotData<void(const QString &)>(33, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Slot 'onPhraseEasingChanged'
        QtMocHelpers::SlotData<void(int)>(34, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 4 },
        }}),
        // Slot 'onLoadPhraseTemplateClicked'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onApplyPhraseTemplateClicked'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCreatePhraseClicked'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onUngroupPhraseClicked'
        QtMocHelpers::SlotData<void()>(38, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSavePhraseTemplateClicked'
        QtMocHelpers::SlotData<void()>(39, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<CalamusMain, qt_meta_tag_ZN11CalamusMainE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject CalamusMain::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11CalamusMainE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11CalamusMainE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11CalamusMainE_t>.metaTypes,
    nullptr
} };

void CalamusMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<CalamusMain *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->stopAllPlayback(); break;
        case 1: _t->onTabChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->onNewSounit(); break;
        case 3: _t->onLoadSounit(); break;
        case 4: _t->onSaveSounitAs(); break;
        case 5: _t->onSounitNameChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: _t->onSounitCommentChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 7: _t->onSounitSelectorChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 8: _t->onSounitNameEdited(); break;
        case 9: _t->onSounitCommentEdited(); break;
        case 10: _t->onAddTrack(); break;
        case 11: _t->onContainerSelected((*reinterpret_cast<std::add_pointer_t<Container*>>(_a[1]))); break;
        case 12: _t->onConnectionSelected((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 13: _t->onSelectionCleared(); break;
        case 14: _t->onInstanceNameChanged(); break;
        case 15: _t->onConnectionFunctionChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 16: _t->onConnectionWeightChanged((*reinterpret_cast<std::add_pointer_t<double>>(_a[1]))); break;
        case 17: _t->onDisconnectClicked(); break;
        case 18: _t->onConfigParameterChanged((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 19: _t->onDnaSelectChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 20: _t->onNumHarmonicsChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 21: _t->onPhraseSelectionChanged(); break;
        case 22: _t->onPhraseNameChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 23: _t->onPhraseEasingChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 24: _t->onLoadPhraseTemplateClicked(); break;
        case 25: _t->onApplyPhraseTemplateClicked(); break;
        case 26: _t->onCreatePhraseClicked(); break;
        case 27: _t->onUngroupPhraseClicked(); break;
        case 28: _t->onSavePhraseTemplateClicked(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 11:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< Container* >(); break;
            }
            break;
        }
    }
}

const QMetaObject *CalamusMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CalamusMain::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11CalamusMainE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int CalamusMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}
QT_WARNING_POP
