#include "entityframe.h"

namespace Displax {

EntityFrame::EntityFrame(QObject* parent)
    : Entity(parent)
{}

EntityFrame::EntityFrame(const QJsonObject& proto_entity, EntityContext* context, QObject* parent)
    : Entity(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

QJsonValue EntityFrame::toJson() const
{
    return QJsonValue();
}

void EntityFrame::fromJson(const QJsonValue& json)
{
    setDirty(false);

    QSize tmp_size = m_value.size();

    QByteArray ba = QByteArray::fromBase64(json.toString().toUtf8());
    m_value.loadFromData(ba);

    if(!m_value.isNull())
        emit valueChanged();

    if(tmp_size != m_value.size())
        emit sizeChanged();

    setInitialised(true);
}

void EntityFrame::cancelChange() {}

QImage EntityFrame::value() const
{
    return m_value;
}

QSize EntityFrame::size() const
{
    return m_value.size();
}

QString EntityFrame::type() const
{
    return "Frame";
}

void EntityFrame::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

} // namespace Displax
