#include "dpxclientlib.h"
#include <QQmlEngine>
#include <sessionmanager.h>

void Displax::registerQmlTypes()
{
    const int major = 1;
    const int minor = 0;
    qmlRegisterUncreatableType<Displax::Session>("com.displax.sdk", major, minor, "Session",
                                                 "Can not instantiate Session in qml");
    qmlRegisterType<Displax::SessionManager>("com.displax.sdk", major, minor, "SessionManager");
    qmlRegisterType<Displax::Device>("com.displax.sdk", major, minor, "Device");
}
