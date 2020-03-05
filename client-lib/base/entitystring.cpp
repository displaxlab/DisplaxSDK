#include "entitystring.h"

namespace Displax {

EntityString::EntityString(QObject* parent)
    : Entity(parent)
{}

EntityString::EntityString(const QJsonObject& proto_entity, EntityContext* context, QObject* parent)
    : EntityString(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

QString EntityString::value() const
{
    return m_value;
}

void EntityString::setValue(const QString& value)
{
    if(m_value == value)
        return;

    m_old_value = m_value;
    m_value = value;
    setDirty(true);
    if(isValid())
        context()->activateEntity(this);
    emit valueChanged();
}

QString EntityString::type() const
{
    return m_type;
}

void EntityString::setType(const QString& type)
{
    if(m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void EntityString::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("type"))
        setType(proto_entity.value("type").toString());
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
    if(proto_entity.contains("access_mode"))
        m_is_writable = proto_entity.value("access_mode").toString().contains("write");
}

bool EntityString::isWritable() const
{
    return m_is_writable;
}

QJsonValue Displax::EntityString::toJson() const
{
    if(isInitialised())
        return QJsonValue(m_value);
    return QJsonValue();
}

void EntityString::fromJson(const QJsonValue& json)
{
    setDirty(false);
    QString v = json.toString();
    if(v != m_value)
    {
        m_value = v;
        emit valueChanged();
    }
    setInitialised(true);
}

void EntityString::cancelChange()
{
    m_value = m_old_value;
    emit valueChanged();
}

} // namespace Displax
