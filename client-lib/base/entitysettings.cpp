#include "entitysettings.h"
#include <QJsonObject>

namespace Displax {

EntitySettings::EntitySettings(QObject* parent)
    : Entity(parent)
{}

EntitySettings::EntitySettings(const QJsonObject& proto_entity, EntityContext* context,
                               QObject* parent)
    : Entity(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

QJsonValue EntitySettings::toJson() const
{
    return m_value;
}

void EntitySettings::fromJson(const QJsonValue& json)
{
    if(!isInitialised())
    {
        setInitialised(true);
        return;
    }

    QString tmp = m_value.toObject().value("action").toString();
    QByteArray data(QByteArray::fromBase64(json.toString().toUtf8()));

    setDirty(false);
    m_value = QJsonValue();
    if(data.size() > 0 && tmp == "download")
        emit downloadFinished(data);
    emit finished();
}

void EntitySettings::cancelChange()
{
    emit operationFail();
}

QString EntitySettings::type() const
{
    return "DeviceSettings";
}

void EntitySettings::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

void EntitySettings::upload(const QByteArray& data)
{
    QJsonObject obj;
    obj.insert("action", "upload");
    obj.insert("data", QString(data.toBase64()));
    m_value = obj;
    if(isValid())
        context()->activateEntity(this);
}

void EntitySettings::download()
{
    QJsonObject obj;
    obj.insert("action", "download");
    m_value = obj;
    if(isValid())
        context()->activateEntity(this);
}

void EntitySettings::savePermanent()
{
    QJsonObject obj;
    obj.insert("action", "save");
    m_value = obj;
    if(isValid())
        context()->activateEntity(this);
}

void EntitySettings::setDefault()
{
    QJsonObject obj;
    obj.insert("action", "default");
    m_value = obj;
    if(isValid())
        context()->activateEntity(this);
}

} // namespace Displax
