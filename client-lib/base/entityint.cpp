#include "entityint.h"
#include <limits>

namespace Displax {

EntityInt::EntityInt(QObject* parent)
    : Entity(parent)
    , m_value(0)
    , m_old_value(0)
    , m_min(std::numeric_limits<int>::min())
    , m_max(std::numeric_limits<int>::max())
{}

EntityInt::EntityInt(const QJsonObject& proto_entity, EntityContext* context, QObject* parent)
    : EntityInt(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

int EntityInt::value() const
{
    return m_value;
}

void EntityInt::setValue(int value)
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

QString EntityInt::type() const
{
    return m_type;
}

void EntityInt::setType(const QString& type)
{
    if(m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void EntityInt::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("type"))
        setType(proto_entity.value("type").toString());
    if(proto_entity.contains("domain"))
    {
        QStringList domain = proto_entity.value("domain").toString().split("..");
        if(domain.size() == 2)
        {
            m_min = QString(domain.at(0)).replace("[", "").toInt();
            m_max = QString(domain.at(1)).replace("]", "").toInt();
            emit minChanged();
            emit maxChanged();
        }
    }
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

int EntityInt::min() const
{
    return m_min;
}

int EntityInt::max() const
{
    return m_max;
}

QJsonValue Displax::EntityInt::toJson() const
{
    if(isInitialised())
        return QJsonValue(m_value);
    return QJsonValue();
}

void EntityInt::fromJson(const QJsonValue& json)
{
    setDirty(false);
    int v = json.toInt();
    if(v != m_value)
    {
        m_value = v;
        emit valueChanged();
    }
    setInitialised(true);
}

void EntityInt::cancelChange()
{
    m_value = m_old_value;
    emit valueChanged();
}

} // namespace Displax
