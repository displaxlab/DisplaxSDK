/*!
    \class Device

    \brief The Device class represent remote device context.

    \reentrant

    This class represent a physical device on remote service machine.
    At base provide serial number, product id, product name, type and firmware version.
    Extended entities can been provide throw proto entities.

    \sa Session
*/

#include "device.h"
#include <entity.h>
#include <session.h>

namespace Displax {

Device::Device(Session* parent)
    : EntityContext(parent)
    , m_product_id(0)
    , m_type("None")
    , m_valid(false)
{
    if(parent)
        connect(parent, SIGNAL(validChanged()), this, SIGNAL(validChanged()));
}

Device::~Device()
{
    auto i = m_entities.begin();
    while(i != m_entities.end())
    {
        auto e = i.value();
        i = m_entities.erase(i);
        e->setContext(nullptr);
    }
}

/*!
  Insert generic entity and activate with remote service.
  This entity must be build throw proto entities.
  If entity notify is active (by default) also activate notify on remote changes.
  */
void Device::insertEntity(Entity* e)
{
    if(!isValid() || m_entities.contains(e->name()))
        return;

    m_entities.insert(e->name(), e);
    session()->activateEntity(e, this);
    if(e->isNotify())
        session()->activateNotify(e, this);
}

bool Device::containsEntity(const QString& name)
{
    return m_entities.contains(name);
}

Entity* Device::getEntity(const QString& name)
{
    if(m_entities.contains(name))
        return m_entities.value(name);
    return nullptr;
}

void Device::removeEntity(Entity* e)
{
    if(isValid() && e->isNotify())
        session()->inactivateNotify(e, this);
    m_entities.remove(e->name());
}

/*!
  Force update entity, related to device, to remote service.
  */
void Device::activateEntity(Entity* e)
{
    if(isValid() && m_entities.contains(e->name()))
        session()->activateEntity(e, this);
}

/*!
  Indicate to remote service to notify me if remote entity has changes.
  */
void Device::activateNotify(Entity* e)
{
    if(isValid() && m_entities.contains(e->name()))
        session()->activateNotify(e, this);
}

/*!
  Disable remote change notify for entity.
  */
void Device::inactivateNotify(Entity* e)
{
    if(isValid() && m_entities.contains(e->name()))
        session()->inactivateNotify(e, this);
}

void Device::cancelAction(Action* a)
{
    if(isValid())
        session()->cancelAction(a);
}

Session* Device::session()
{
    return static_cast<Session*>(parent());
}

const Session* Device::session() const
{
    return static_cast<const Session*>(parent());
}

int Device::productId() const
{
    return m_product_id;
}

QString Device::productName() const
{
    return m_product_name;
}

QString Device::serialNumber() const
{
    return m_serial_number;
}

QString Device::type() const
{
    return m_type;
}

bool Device::isValid() const
{
    return m_type != "None" && (session() && session()->isValid()) && m_valid;
}

void Device::updateJsonData(const QJsonObject& obj)
{
    QStringList keys = obj.keys();

    if(keys.contains("product_id"))
    {
        int pi = obj.value("product_id").toInt();
        if(m_product_id != pi)
        {
            m_product_id = pi;
            emit productIdChanged();
        }
    }

    if(keys.contains("product_name"))
    {
        QString pn = obj.value("product_name").toString();
        if(m_product_name != pn)
        {
            m_product_name = pn;
            emit productNameChanged();
        }
    }

    if(keys.contains("version"))
    {
        QString v = obj.value("version").toString();
        if(m_version != v)
        {
            m_version = v;
            emit versionChanged();
        }
    }

    if(keys.contains("device_type"))
    {
        QString t = obj.value("device_type").toString();
        if(m_type != t)
        {
            m_type = t;
            emit typeChanged();
        }
    }

    if(keys.contains("serial_number"))
    {
        QString sn = obj.value("serial_number").toString();
        if(m_serial_number != sn)
        {
            m_serial_number = sn;
            emit serialNumberChanged();
        }
    }

    if(keys.contains("views"))
    {
        QJsonArray v = obj.value("views").toArray().first().toObject().value("value").toArray();
        if(v != m_proto_entities)
        {
            m_proto_entities = v;
            emit protoEntitiesChanged();
        }
    }

    m_valid = true;
    emit validChanged();
}

void Device::invalidate()
{
    if(m_valid)
    {
        m_valid = false;
        emit validChanged();
    }
}

QString Device::version() const
{
    return m_version;
}

/*!
  Return a list with proto entities.
  */
QJsonArray Device::protoEntities() const
{
    return m_proto_entities;
}

bool Device::hasEntity(const QString& name) const
{
    for(auto i = m_proto_entities.begin(), e = m_proto_entities.end(); i != e; ++i)
    {
        QJsonObject o = i->toObject();
        if(o.contains("name") && o.value("name") == name)
            return true;
    }
    return false;
}

/*!
  Return a proto entity.
  A proto entity is a JSON Object with entity declaration. Typical contain type (ex: int, string,
  enum, etc), name, and other usefull information.
  */
QJsonObject Device::getProtoEntity(const QString& name) const
{
    for(auto i = m_proto_entities.begin(), e = m_proto_entities.end(); i != e; ++i)
    {
        QJsonObject o = i->toObject();
        if(o.contains("name") && o.value("name") == name)
            return o;
    }
    return QJsonObject();
}

} // namespace Displax
