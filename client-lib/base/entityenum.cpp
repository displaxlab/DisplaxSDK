#include "entityenum.h"
#include <QJsonArray>

namespace Displax {

EntityEnum::EntityEnum(QObject* parent)
    : Entity(parent)
{}

EntityEnum::EntityEnum(const QJsonObject& proto_entity, EntityContext* context, QObject* parent)
    : EntityEnum(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

QString EntityEnum::value() const
{
    return m_value;
}

void EntityEnum::setValue(const QString& value)
{
    if(m_value == value)
        return;

    if(!m_all_values.contains(value))
        return;

    m_old_value = m_value;
    m_value = value;
    setDirty(true);
    if(isValid())
        context()->activateEntity(this);
    emit valueChanged();
}

QString EntityEnum::type() const
{
    return m_type;
}

void EntityEnum::setType(const QString& type)
{
    if(m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void EntityEnum::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("type"))
        setType(proto_entity.value("type").toString());
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
    if(proto_entity.contains("values"))
    {
        m_all_values.clear();
        auto arr = proto_entity.value("values").toArray();
        for(auto i = arr.constBegin(), e = arr.constEnd(); i != e; ++i)
            m_all_values.append(i->toString());
        emit allValuesChanged();
    }
}

QStringList EntityEnum::allValues() const
{
    return m_all_values;
}

QVariant EntityEnum::allValuesQml()
{
    return QVariant::fromValue(m_all_values);
}

QJsonValue Displax::EntityEnum::toJson() const
{
    if(isInitialised())
        return QJsonValue(m_value);
    return QJsonValue();
}

void EntityEnum::fromJson(const QJsonValue& json)
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

void EntityEnum::cancelChange()
{
    m_value = m_old_value;
    emit valueChanged();
}

} // namespace Displax
