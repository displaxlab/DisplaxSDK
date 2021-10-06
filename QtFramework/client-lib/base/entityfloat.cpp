#include "entityfloat.h"
#include <limits>

namespace Displax {

EntityFloat::EntityFloat(QObject* parent)
    : Entity(parent)
    , m_value(0)
    , m_old_value(0)
    , m_min(std::numeric_limits<float>::min())
    , m_max(std::numeric_limits<float>::max())
{}

EntityFloat::EntityFloat(const QJsonObject& proto_entity, EntityContext* context, QObject* parent)
    : EntityFloat(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

float EntityFloat::value() const
{
    return m_value;
}

void EntityFloat::setValue(float value)
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

QString EntityFloat::type() const
{
    return m_type;
}

void EntityFloat::setType(const QString& type)
{
    if(m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void EntityFloat::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("type"))
        setType(proto_entity.value("type").toString());
    if(proto_entity.contains("domain"))
    {
        QStringList domain = proto_entity.value("domain").toString().split("..");
        if(domain.size() == 2)
        {
            m_min = QString(domain.at(0)).replace("[", "").toDouble();
            m_max = QString(domain.at(1)).replace("]", "").toDouble();
            emit minChanged();
            emit maxChanged();
        }
    }
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

float EntityFloat::min() const
{
    return m_min;
}

float EntityFloat::max() const
{
    return m_max;
}

QJsonValue Displax::EntityFloat::toJson() const
{
    if(isInitialised())
        return QJsonValue(m_value);
    return QJsonValue();
}

void EntityFloat::fromJson(const QJsonValue& json)
{
    setDirty(false);
    float v = json.toDouble();
    if(v != m_value)
    {
        m_value = v;
        emit valueChanged();
    }
    setInitialised(true);
}

void EntityFloat::cancelChange()
{
    m_value = m_old_value;
    emit valueChanged();
}

} // namespace Displax

