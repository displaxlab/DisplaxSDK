/*!
    \class Session

    \brief The Session class represents a remote service.

    \reentrant

    This is the heart of Displax Client library. This represent a session on remote service.
    A local machine service, here, is also treated a remote service.

    This represent a view of one service context.

    \sa Device
    \sa SessionManager
*/

#include "session.h"
#include <dpxcomconnection.h>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QRegularExpression>
#include <QTimer>
#include <device.h>
#include <entity.h>

namespace Displax {

Session::Session(DPXComConnection* parent)
    : EntityContext(parent)
    , m_connection(parent)
{
    connect(m_connection, &DPXComConnection::readyRead, this, &Session::readyRead);
    requestProtocolVersion();
}

Session::~Session()
{
    for(auto i = m_active_actions.constBegin(), e = m_active_actions.constEnd(); i != e; ++i)
        delete i.value();

    for(auto i = m_devices.begin(), e = m_devices.end(); i != e; ++i)
        delete i.value();
}

void Session::insertEntity(Entity* e)
{
    if(!isValid() || m_entities.contains(e->name()))
        return;

    m_entities.insert(e->name(), e);
    activateEntity(e);
    if(e->isNotify())
        activateNotify(e);
}

Entity* Session::getEntity(const QString& name)
{
    return m_entities.value(name);
}

void Session::removeEntity(Entity* e)
{
    if(isValid() && e->isNotify())
        inactivateNotify(e);
    m_entities.remove(e->name());
}

bool Session::containsEntity(const QString& name)
{
    return m_entities.contains(name);
}

/*!
  Force update entity, related to session, to remote service.
  */
void Session::activateNotify(Entity* e)
{
    if(!m_valid || !containsEntity(e->name()))
        return;

    if(m_unregister_notifies["session"].contains(e->name()))
        m_unregister_notifies["session"].remove(e->name());
    else
        m_register_notifies["session"].insert(e->name());

    metaObject()->invokeMethod(this, "sendNotifications", Qt::QueuedConnection);
}

/*!
  Indicate to remote service to notify me if remote entity has changes.
  */
void Session::inactivateNotify(Entity* e)
{
    if(!m_valid || !containsEntity(e->name()))
        return;

    if(m_register_notifies["session"].contains(e->name()))
        m_register_notifies["session"].remove(e->name());
    else
        m_unregister_notifies["session"].insert(e->name());

    metaObject()->invokeMethod(this, "sendNotifications", Qt::QueuedConnection);
}

void Session::cancelAction(Action* a)
{
    m_active_actions.take(a->id());
    a->deleteLater();
}

/*!
  This method is directed to device implementation.
  */
void Session::activateEntity(Entity* e, Device* device)
{
    if(!m_valid)
        return;

    Action* a = new Action(e, ++m_id);
    QJsonObject jo = a->activate();
    jo.insert("device", device->serialNumber());
    m_pending_actions.append(jo);
    m_active_actions.insert(a->id(), a);

    metaObject()->invokeMethod(this, "sendActions", Qt::QueuedConnection);
}

/*!
  This method is directed to device implementation.
  */
void Session::activateNotify(Entity* e, Device* device)
{
    if(!m_valid)
        return;

    if(m_unregister_notifies[device->serialNumber()].contains(e->name()))
        m_unregister_notifies[device->serialNumber()].remove(e->name());
    else
        m_register_notifies[device->serialNumber()].insert(e->name());

    metaObject()->invokeMethod(this, "sendNotifications", Qt::QueuedConnection);
}

/*!
  This method is directed to device implementation.
  */
void Session::inactivateNotify(Entity* e, Device* device)
{
    if(!m_valid)
        return;

    if(m_register_notifies[device->serialNumber()].contains(e->name()))
        m_register_notifies[device->serialNumber()].remove(e->name());
    else
        m_unregister_notifies[device->serialNumber()].insert(e->name());

    metaObject()->invokeMethod(this, "sendNotifications", Qt::QueuedConnection);
}

void Session::sendNotifications()
{
    if(!m_valid)
        return;

    if(m_register_notifies.isEmpty() && m_unregister_notifies.isEmpty())
        return;

    QJsonObject notifications({{"key", "notifications"}});

    if(!m_register_notifies.isEmpty())
    {
        QJsonArray arr;
        for(const auto& ctx: m_register_notifies.keys())
        {
            for(const auto& key: m_register_notifies[ctx])
            {
                QJsonObject obj({{"key", key}});
                if(ctx != "session")
                    obj.insert("device", ctx);
                arr.append(obj);
            }
        }
        notifications.insert("register", arr);
        m_register_notifies.clear();
    }

    if(!m_unregister_notifies.isEmpty())
    {
        QJsonArray arr;
        for(const auto& ctx: m_unregister_notifies.keys())
        {
            for(const auto& key: m_unregister_notifies[ctx])
            {
                QJsonObject obj({{"key", key}});
                if(ctx != "session")
                    obj.insert("device", ctx);
                arr.append(obj);
            }
        }
        notifications.insert("unregister", arr);
        m_unregister_notifies.clear();
    }

    m_pending_actions.append(notifications);
    metaObject()->invokeMethod(this, "sendActions", Qt::QueuedConnection);
}

QString Session::url() const
{
    return m_connection->url().toString();
}

void Session::activateEntity(Entity* p)
{
    if(!m_valid)
        return;

    Action* a = new Action(p, ++m_id);
    m_pending_actions.append(a->activate());
    m_active_actions.insert(a->id(), a);

    metaObject()->invokeMethod(this, "sendActions", Qt::QueuedConnection);
}

void Session::readyRead()
{
    while(m_valid)
    {
        const QJsonObject obj = m_connection->readJson();
        if(obj.isEmpty())
            break;
        parseNewData(obj);
    }
}

void Session::parseNewData(const QJsonObject& obj)
{
    if(obj.contains("error"))
        return;

    if(obj.contains("reactions"))
        parseReactions(obj.value("reactions").toObject());

    if(obj.contains("notifications"))
        parseNotifications(obj.value("notifications").toObject());

    if(obj.contains("cli"))
        emit receiveCli(obj.value("cli").toArray());
}

void Session::sendActions()
{
    if(!m_valid || m_pending_actions.isEmpty())
        return;

    const QJsonObject actions({{"list", m_pending_actions}});
    m_connection->writeJson(QJsonObject({{"actions", actions}}));

    m_pending_actions = QJsonArray();
}

void Session::sendCli(QJsonArray arr)
{
    if(!m_valid || arr.isEmpty())
        return;

    const QJsonObject actions({{"parser", arr}});
    m_connection->writeJson(QJsonObject({{"cli", actions}}));
}

void Session::invalidate()
{
    if(!m_valid)
        return;

    m_valid = false;
    emit validChanged();
}

void Session::removeDevice(const QString& serial_number)
{
    if(!m_valid)
        return;

    if(!m_devices.contains(serial_number))
        return;

    Device* device = m_devices.take(serial_number);
    m_devices_qml = m_devices.values();

    auto i = m_active_actions.begin();
    while(i != m_active_actions.end())
    {
        auto entity = i.value()->entity();
        if(entity == nullptr || (entity && entity->context() == device))
        {
            auto tmp = i.value();
            i = m_active_actions.erase(i);
            delete tmp;
        }
        else
            ++i;
    }

    device->invalidate();
    emit deviceRemoved(device);
    emit devicesChanged();
    device->deleteLater();
}

void Session::parseReactions(const QJsonObject& reactions)
{
    const QJsonArray arr = reactions.value("list").toArray();
    for(auto i = arr.constBegin(), e = arr.constEnd(); i < e; ++i)
    {
        const QJsonObject o = i->toObject();
        if(o.contains("error"))
            continue;

        if(Action* a = m_active_actions.take(o.value("id").toInt()); a)
        {
            a->deactivate(o.value("value"));
            a->deleteLater();
        }
        else if(o.value("key").toString() == "devices_all")
            parseDevicesAll(o);
        else if(o.value("key").toString() == "version")
            parseVersion(o.value("value"));
    }
}

void Session::parseNotifications(const QJsonObject& notifications)
{
    const QJsonArray arr = notifications.value("list").toArray();
    for(auto i = arr.constBegin(), e = arr.constEnd(); i != e; ++i)
    {
        const QJsonObject o = i->toObject();
        if(o.value("key") == "devices_list")
            requestDevicesAll();
        else if(o.contains("device"))
        {
            if(m_devices.contains(o.value("device").toString()))
            {
                auto device = m_devices[o.value("device").toString()];
                auto e = device->getEntity(o.value("key").toString());
                if(e)
                    e->fromJson(o.value("value"));
            }
        }
        else
        {
            if(auto e = getEntity(o.value("key").toString()); e)
                e->fromJson(o.value("value"));
        }
    }
}

void Session::requestDevicesAll()
{
    QJsonObject o;
    o.insert("key", "devices_all");
    o.insert("id", ++m_id);
    m_pending_actions.append(o);
    metaObject()->invokeMethod(this, "sendActions", Qt::QueuedConnection);
}

void Session::requestProtocolVersion()
{
    QJsonObject o;
    o.insert("key", "version");
    o.insert("id", ++m_id);
    m_pending_actions.append(o);
    metaObject()->invokeMethod(this, "sendActions", Qt::QueuedConnection);
}

void Session::firstRequest()
{
    {
        QJsonObject o;
        o.insert("key", "devices_all");
        o.insert("id", ++m_id);
        m_pending_actions.append(o);
    }
    {
        QJsonArray arr_noty;
        QJsonObject obj_n, o;
        o.insert("key", "devices_list");
        arr_noty.append(o);
        obj_n.insert("register", arr_noty);
        obj_n.insert("key", "notifications");
        m_pending_actions.append(obj_n);
    }
    metaObject()->invokeMethod(this, "sendActions", Qt::QueuedConnection);
}

bool Session::isLocal() const
{
    return m_connection->isLocal();
}

bool Session::isValid() const
{
    return m_valid;
}

QString Session::version() const
{
    return m_version;
}

QHash<QString, Device*>* Session::devices()
{
    return &m_devices;
}

QQmlListProperty<Device> Session::devicesQml()
{
    return QQmlListProperty<Device>(this, &m_devices_qml);
}

DPXComConnection* Session::connection() const
{
    return m_connection;
}

Device* Session::device(const QString& serial_number)
{
    return m_devices.value(serial_number);
}

void Session::updateDevice(const QJsonObject& data)
{
    const QString serial_number = data.value("serial_number").toString();
    if(m_devices.contains(serial_number))
        m_devices[serial_number]->updateJsonData(data);
    else
    {
        auto device = new Device(this);
        m_devices[serial_number] = device;
        device->updateJsonData(data);
        emit deviceInserted(device);
    }
}

void Session::parseDevicesAll(const QJsonObject& o)
{
    if(o.value("key") != "devices_all")
        return;

    const QJsonArray json_devices = o.value("value").toArray();
    QStringList contexts = m_devices.keys();

    if(json_devices.isEmpty() && contexts.isEmpty())
        return;

    for(const auto obj: std::as_const(json_devices))
        contexts.removeOne(obj.toObject().value("serial_number").toString());

    for(auto& key: std::as_const(contexts))
        removeDevice(key);

    for(auto& d: std::as_const(json_devices))
        updateDevice(d.toObject());

    m_devices_qml = m_devices.values();

    emit devicesChanged();
}

void Session::parseVersion(const QJsonValue& v)
{
    static const QRegularExpression rxlen("[\\w ]*(\\d+).(\\d+).(\\d+)");
    const QString value = v.toString().split("\n").first();
    const QRegularExpressionMatch match = rxlen.match(value);

    if(!match.hasMatch())
        return;

    const int version_major = match.captured(1).toInt();
    m_version = (match.captured(1) + "." + match.captured(2) + "." + match.captured(3));
    if(version_major == 1)
    {
        firstRequest();
        m_valid = true;
    }
    else
        invalidate();
}

} // namespace Displax
