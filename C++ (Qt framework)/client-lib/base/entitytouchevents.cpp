#include "entitytouchevents.h"
#include <QLineF>
#include <QPointF>

namespace Displax {

EntityTouchEvents::EntityTouchEvents(QObject* parent)
    : Entity(parent)
{}

EntityTouchEvents::EntityTouchEvents(const QJsonObject& proto_entity,
                                     Displax::EntityContext* context, QObject* parent)
    : EntityTouchEvents(parent)
{
    updateProtoEntity(proto_entity);
    setContext(context);
}

QJsonValue EntityTouchEvents::toJson() const
{
    if(isInitialised())
    {
        QJsonObject obj;
        obj.insert("actions", m_set_property);
        m_set_property = QJsonArray();
        return obj;
    }
    return QJsonValue();
}

void EntityTouchEvents::fromJson(const QJsonValue& json)
{
    setDirty(false);
    if(json != m_value)
    {
        m_value = json;
        emit valueChanged();
    }
    if(!isInitialised())
    {
        m_set_property = QJsonArray();
        setInitialised(true);
    }
}

void EntityTouchEvents::cancelChange() {}

QJsonValue EntityTouchEvents::value() const
{
    return m_value;
}

QString EntityTouchEvents::type() const
{
    return m_type;
}

void EntityTouchEvents::setType(const QString& type)
{
    if(m_type == type)
        return;

    m_type = type;
    emit typeChanged();
}

void EntityTouchEvents::updateProtoEntity(const QJsonObject& proto_entity)
{
    if(proto_entity.contains("type"))
        setType(proto_entity.value("type").toString());
    if(proto_entity.contains("name"))
        setName(proto_entity.value("name").toString());
}

void EntityTouchEvents::setEntityProperty(const QJsonObject& obj)
{
    if(!isInitialised())
        return;

    m_set_property.append(obj);
    setDirty(true);
    if(isValid())
        context()->activateEntity(this);
}

inline bool circuleIntersects(const QJsonObject& a, const QJsonArray& arr, qreal radius)
{
    QPointF c0(a.value("x").toDouble(), a.value("y").toDouble());
    int id = a.value("identifier").toInt();
    qreal a_radius = a.value("radius").toDouble();
    for(auto i = arr.constBegin(), e = arr.constEnd(); i != e; ++i)
    {
        QJsonObject b = i->toObject();
        if(id == b.value("identifier").toInt())
            continue;

        QPointF c1(b.value("x").toDouble(), b.value("y").toDouble());
        qreal distance = QLineF(c0, c1).length();
        if(radius < 0.0f)
        {
            qreal b_radius = b.value("radius").toDouble();
            if(distance <= b_radius + a_radius)
                return true;
        }
        else if(distance <= 2 * radius)
            return true;
    }
    return false;
}

QJsonArray EntityTouchEvents::intersects(qreal radius)
{
    QJsonArray ret_arr;
    QJsonArray arr = m_value.toObject().value("entities").toArray();
    for(auto i = arr.constBegin(), e = arr.constEnd(); i != e; ++i)
        if(circuleIntersects(i->toObject(), arr, radius))
            ret_arr.append(*i);

    return ret_arr;
}

QJsonArray EntityTouchEvents::isInside(qreal x, qreal y, qreal radius)
{
    QJsonArray ret_arr;
    QPointF c0(x, y);
    QJsonArray arr = m_value.toObject().value("entities").toArray();
    for(auto i = arr.constBegin(), e = arr.constEnd(); i != e; ++i)
    {
        QJsonObject b = i->toObject();
        QPointF c1(b.value("x").toDouble(), b.value("y").toDouble());
        if(QLineF(c0, c1).length() < radius)
            ret_arr.append(b);
    }
    return ret_arr;
}

} // namespace Displax
