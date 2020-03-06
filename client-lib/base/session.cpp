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
#include <QTimer>
#include <device.h>
#include <entity.h>

namespace Displax {

Session::Session(DPXComConnection* parent)
    : EntityContext(parent)
    , m_connection(parent)
    , m_valid(true)
    , m_id(0)
{
    connect(m_connection, SIGNAL(readyRead()), this, SLOT(readyRead()));
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

    QJsonObject notifications;
    notifications.insert("key", "notifications");

    if(!m_register_notifies.isEmpty())
    {
        QJsonArray arr;
        foreach(auto const& ctx, m_register_notifies.keys())
        {
            foreach(auto const& key, m_register_notifies[ctx])
            {
                QJsonObject obj;
                obj.insert("key", key);
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
        foreach(auto const& ctx, m_unregister_notifies.keys())
        {
            foreach(auto const& key, m_unregister_notifies[ctx])
            {
                QJsonObject obj;
                obj.insert("key", key);
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
    m_data_received.append(m_connection->readAll());
    while(!m_data_received.isEmpty() && m_valid)
    {
        QJsonDocument doc = QJsonDocument::fromBinaryData(m_data_received);
        if(doc.isNull())
            break;
        parseNewData(doc);
        int size = 0;
        doc.rawData(&size);
        m_data_received = m_data_received.right(m_data_received.size() - size);
    }
}

void Session::parseNewData(const QJsonDocument& doc)
{
    if(!doc.isEmpty())
    {
        QJsonObject obj = doc.object();
        //qDebug() << "parseNewData" << obj;

        if(!obj.contains("error"))
        {
            if(obj.contains("reactions"))
                parseReactions(obj.value("reactions").toObject());

            if(obj.contains("notifications"))
                parseNotifications(obj.value("notifications").toObject());

            if(obj.contains("cli"))
                emit receiveCli(obj.value("cli").toArray());
        }
    }
    else
    {
        //qDebug() << "error on communication" << doc;
    }
}

void Session::sendActions()
{
    if(!m_pending_actions.isEmpty() && m_valid)
    {
        QJsonObject actions;
        QJsonObject obj;
        QJsonDocument doc;

        actions.insert("list", m_pending_actions);
        obj.insert("actions", actions);
        doc.setObject(obj);
        m_connection->write(doc.toBinaryData());

        m_pending_actions = QJsonArray();

        //qDebug() << "send session actions" << obj;
    }
}

void Session::sendCli(QJsonArray arr)
{
    if(!arr.isEmpty() && m_valid)
    {
        QJsonObject actions;
        QJsonObject objAction;
        QJsonDocument doc;

        actions.insert("parser", arr);
        objAction.insert("cli", actions);

        doc.setObject(objAction);

        m_connection->write(doc.toBinaryData());

        //qDebug() << "send obj actions" << objAction;
    }
}

void Session::invalidate()
{
    if(m_valid)
    {
        m_valid = false;
        emit validChanged();
    }
}

void Session::removeDevice(const QString& serial_number)
{
    if(!m_valid)
        return;

    if(m_devices.contains(serial_number))
    {
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
}

void Session::parseReactions(const QJsonObject& reactions)
{
    QJsonArray arr = reactions.value("list").toArray();
    for(auto i = arr.constBegin(), e = arr.constEnd(); i < e; ++i)
    {
        QJsonObject o = i->toObject();
        if(o.contains("error"))
            continue;

        Action* a = m_active_actions.take(o.value("id").toInt());
        if(a)
        {
            a->deactivate(o.value("value"));
            a->deleteLater();
        }
        else if(o.value("key").toString() == "devices_all")
        {
            parseDevicesAll(o);
        }
        else if(o.value("key").toString() == "version")
        {
            parseVersion(o.value("value"));
        }
    }
}

void Session::parseNotifications(const QJsonObject& notifications)
{
    QJsonArray arr = notifications.value("list").toArray();
    for(auto i = arr.constBegin(), e = arr.constEnd(); i != e; ++i)
    {
        QJsonObject o = i->toObject();
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
            auto e = getEntity(o.value("key").toString());
            if(e)
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
    return QQmlListProperty<Device>(this, m_devices_qml);
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
    {
        m_devices[serial_number]->updateJsonData(data);
    }
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

    QJsonArray json_devices = o.value("value").toArray();
    QStringList contexts = m_devices.keys();

    if(json_devices.isEmpty() && contexts.isEmpty())
        return;

    for(const auto obj : o.value("value").toArray())
        contexts.removeOne(obj.toObject().value("serial_number").toString());

    for(const auto& key : contexts)
        removeDevice(key);

    for(const auto d : json_devices)
        updateDevice(d.toObject());

    m_devices_qml = m_devices.values();

    emit devicesChanged();
}

void Session::parseVersion(const QJsonValue& v)
{
    QString value = v.toString().split("\n").first();
    QRegExp rxlen("[\\w ]*(\\d+).(\\d+).(\\d+)");
    int pos = rxlen.indexIn(value);
    if(pos > -1)
    {
        int version_major = rxlen.cap(1).toInt();
        m_version = (rxlen.cap(1) + "." + rxlen.cap(2) + "." + rxlen.cap(3));
        if(version_major == 1)
        {
            firstRequest();
            m_valid = true;
        }
        else
            invalidate();
    }
}

} // namespace Displax
