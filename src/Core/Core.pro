# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities - Qt Utility Libraries
# Qtilities Core Library
# ***************************************************************************
QTILITIES += logging
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore

CONFIG += qt \
    dll \
    ordered
QT += core
QT -= gui

equals(BUILD_MODE, release) {
    TARGET = QtilitiesCore$${QTILITIES_FILE_VER}
} else {
    TARGET = QtilitiesCored$${QTILITIES_FILE_VER}
}

TEMPLATE = lib
DEFINES += QTILITIESCORE_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR = $$QTILITIES_TEMP/Core
MOC_DIR = $$QTILITIES_TEMP/Core
RCC_DIR = $$QTILITIES_TEMP/Core
UI_DIR = $$QTILITIES_TEMP/Core

# --------------------------
# Qtilities Core Files
# --------------------------
HEADERS += source/QtilitiesCore_global.h \
    source/QtilitiesCoreConstants.h \
    source/Observer.h \
    source/PointerList.h \
    source/ObserverData.h \
    source/AbstractSubjectFilter.h \
    source/ObserverProperty.h \
    source/ActivityPolicyFilter.h \
    source/ObjectManager.h \
    source/QtilitiesPropertyChangeEvent.h \
    source/ObserverMimeData.h \
    source/QtilitiesCore.h \
    source/IObjectManager.h \
    source/SubjectTypeFilter.h \
    source/IExportable.h \
    source/IFactory.h \
    source/ObserverRelationalTable.h \
    source/Factory.h \
    source/IModificationNotifier.h \
    source/IContext.h \
    source/IContextManager.h \
    source/ContextManager.h \
    source/IObjectBase.h

SOURCES += source/Observer.cpp \
    source/ActivityPolicyFilter.cpp \
    source/ObjectManager.cpp \
    source/QtilitiesPropertyChangeEvent.cpp \
    source/QtilitiesCore.cpp \
    source/SubjectTypeFilter.cpp \
    source/ObserverData.cpp \
    source/ObserverProperty.cpp \
    source/ObserverRelationalTable.cpp \
    source/ContextManager.cpp