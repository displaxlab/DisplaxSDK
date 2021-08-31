#include "dpxclientlib.h"
#include <QQmlEngine>
#include <entity.h>
#include <entitybool.h>
#include <entityenum.h>
#include <entityfloat.h>
#include <entityframe.h>
#include <entityfunction.h>
#include <entityint.h>
#include <entityjsonvalue.h>
#include <entitysettings.h>
#include <entitystring.h>
#include <entitytouchevents.h>
#include <sessionmanager.h>

void Displax::registerQmlTypes()
{
    const int major = 1;
    const int minor = 0;
    qmlRegisterUncreatableType<Displax::Session>("com.displax.sdk", major, minor, "Session",
                                                 "Can not instantiate Session in qml");
    qmlRegisterType<Displax::SessionManager>("com.displax.sdk", major, minor, "SessionManager");
    qmlRegisterType<Displax::Device>("com.displax.sdk", major, minor, "Device");

    qmlRegisterUncreatableType<Displax::Entity>("com.displax.sdk", major, minor, "Entity",
                                                "Can not instantiate Entity in qml");
    qmlRegisterUncreatableType<Displax::EntityContext>("com.displax.sdk", major, minor,
                                                       "EntityContext",
                                                       "Can not instantiate Entity in qml");
    qmlRegisterType<Displax::EntityBool>("com.displax.sdk", major, minor, "EntityBool");
    qmlRegisterType<Displax::EntityInt>("com.displax.sdk", major, minor, "EntityInt");
    qmlRegisterType<Displax::EntityFloat>("com.displax.sdk", major, minor, "EntityFloat");
    qmlRegisterType<Displax::EntityString>("com.displax.sdk", major, minor, "EntityString");
    qmlRegisterType<Displax::EntityJsonValue>("com.displax.sdk", major, minor, "EntityJsonValue");
    qmlRegisterType<Displax::EntityFrame>("com.displax.sdk", major, minor, "EntityFrame");
    qmlRegisterType<Displax::EntityFunction>("com.displax.sdk", major, minor, "EntityFunction");
    qmlRegisterType<Displax::EntityEnum>("com.displax.sdk", major, minor, "EntityEnum");
    qmlRegisterType<Displax::EntitySettings>("com.displax.sdk", major, minor, "EntitySettings");
    qmlRegisterType<Displax::EntityTouchEvents>("com.displax.sdk", major, minor,
                                                "EntityTouchEvents");
}
