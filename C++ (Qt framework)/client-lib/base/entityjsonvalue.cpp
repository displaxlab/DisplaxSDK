#include "entityjsonvalue.h"

namespace Displax {

EntityJsonValue::EntityJsonValue(QObject* parent)
    : Entity(parent)
{}

EntityJsonValue::EntityJsonValue(const QJsonObject& proto_entity, EntityContext* context,
                                 QObject* parent)
    : EntityJsonValue(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

QJsonValue EntityJsonValue::value() const
{
    return m_value;
}

void EntityJsonValue::setValue(const QJsonValue& value)
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

QString EntityJsonValue::type() const
{
    return m_type;
}

void EntityJsonValue::setType(const QString& type)
{
    if(m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void EntityJsonValue::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("type"))
        setType(proto_entity.value("type").toString());
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

QJsonValue Displax::EntityJsonValue::toJson() const
{
    if(isInitialised())
        return m_value;
    return QJsonValue();
}

void EntityJsonValue::fromJson(const QJsonValue& json)
{
    setDirty(false);
    if(json != m_value)
    {
        m_value = json;
        emit valueChanged();
    }
    setInitialised(true);
}

void EntityJsonValue::cancelChange()
{
    m_value = m_old_value;
    emit valueChanged();
}

} // namespace Displax
