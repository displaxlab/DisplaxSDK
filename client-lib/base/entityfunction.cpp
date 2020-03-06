#include "entityfunction.h"

namespace Displax {

QString EntityFunction::type() const
{
    return "Function";
}

void EntityFunction::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("command"))
    {
        m_command = proto_entity.value("command").toString();
        emit commandChanged();
    }
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

EntityFunction::EntityFunction(QObject* parent)
    : Entity(parent)
{}

EntityFunction::EntityFunction(const QJsonObject& proto_entity, EntityContext* context,
                               QObject* parent)
    : Entity(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

QJsonValue EntityFunction::toJson() const
{
    if(isInitialised() && m_parameters.isNull())
        return QJsonValue::fromVariant(m_parameters);
    return QJsonValue();
}

void EntityFunction::fromJson(const QJsonValue& json)
{
    setDirty(false);
    QString v = json.toString();
    if(v != m_result)
    {
        m_result = v;
        emit resultChanged();
    }
    setInitialised(true);
}

void EntityFunction::cancelChange()
{
    m_result = "";
    emit resultChanged();
}

QString EntityFunction::command() const
{
    return m_command;
}

QVariant EntityFunction::parameters() const
{
    return m_parameters;
}

void EntityFunction::setParameters(const QVariant& parameters)
{
    if(m_parameters == parameters)
        return;

    m_parameters = parameters;
    emit parametersChanged();
}

void EntityFunction::execute()
{
    if(isValid())
        context()->activateEntity(this);
}

QVariant EntityFunction::result() const
{
    return m_result;
}

} // namespace Displax
