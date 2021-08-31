QT -= gui
QT += network quick qml

TEMPLATE = lib
DEFINES += DPX_CLIENT_LIB

CONFIG += c++11
CONFIG += staticlib

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += communication base

SOURCES += \
    base/entitybool.cpp \
    base/entitycontext.cpp \
    base/entityenum.cpp \
    base/entityfloat.cpp \
    base/entityframe.cpp \
    base/entityfunction.cpp \
    base/entityint.cpp \
    base/entityjsonvalue.cpp \
    base/entitysettings.cpp \
    base/entitystring.cpp \
    base/entitytouchevents.cpp \
    dpxclientlib.cpp \
    communication/dpxcomconnection.cpp \
    communication/dpxcommanager.cpp \
    communication/dpxcompeermanager.cpp \
    communication/dpxcomurl.cpp \
    base/device.cpp \
    base/entity.cpp \
    base/action.cpp \
    base/session.cpp \
    base/sessionmanager.cpp

HEADERS += \
    base/entitybool.h \
    base/entityenum.h \
    base/entityfloat.h \
    base/entityframe.h \
    base/entityfunction.h \
    base/entityint.h \
    base/entityjsonvalue.h \
    base/entitysettings.h \
    base/entitystring.h \
    base/entitytouchevents.h \
    displaxclientlib_global.h \
    dpxclientlib.h \
    communication/dpxcomconnection.h \
    communication/dpxcommanager.h \
    communication/dpxcompeermanager.h \
    communication/dpxcomurl.h \
    base/device.h \
    base/entity.h \
    base/entitycontext.h \
    base/action.h \
    base/session.h \
    base/sessionmanager.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    readme.md
