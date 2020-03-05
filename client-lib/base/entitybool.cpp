#include "entitybool.h"

namespace Displax {

EntityBool::EntityBool(QObject* parent)
    : Entity(parent)
{}

EntityBool::EntityBool(const QJsonObject& proto_entity, EntityContext* context, QObject* parent)
    : EntityBool(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

bool EntityBool::value() const
{
    return m_value;
}

void EntityBool::setValue(bool value)
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

QString EntityBool::type() const
{
    return m_type;
}

void EntityBool::setType(const QString& type)
{
    if(m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void EntityBool::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("type"))
        setType(proto_entity.value("type").toString());
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

QJsonValue Displax::EntityBool::toJson() const
{
    if(isInitialised())
        return QJsonValue(m_value);
    return QJsonValue();
}

void EntityBool::fromJson(const QJsonValue& json)
{
    setDirty(false);

    bool v = json.toBool();
    if(v != m_value)
    {
        m_value = v;
        emit valueChanged();
    }

    setInitialised(true);
}

void EntityBool::cancelChange()
{
    m_value = m_old_value;
    emit valueChanged();
}

} // namespace Displax
