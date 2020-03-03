QT -= gui
QT += network quick qml

TEMPLATE = lib
DEFINES += DPX_CLIENT_LIB

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += communication base

SOURCES += \
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

