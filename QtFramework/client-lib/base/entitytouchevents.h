#ifndef ENTITYTOUCHEVENTS_H
#define ENTITYTOUCHEVENTS_H

#include <QJsonObject>
#include <QJsonArray>
#include <entity.h>

namespace Displax {

class EntityTouchEvents: public Entity
{
    Q_OBJECT
    Q_PROPERTY(QJsonValue value READ value NOTIFY valueChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit EntityTouchEvents(QObject* parent = nullptr);
    explicit EntityTouchEvents(const QJsonObject& proto_entity, EntityContext* context,
                             QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    QJsonValue value() const;

    QString type() const override;
    void setType(const QString& type);

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);
    Q_INVOKABLE void setEntityProperty(const QJsonObject& obj);
    Q_INVOKABLE QJsonArray intersects(qreal radius = -1.0);
    Q_INVOKABLE QJsonArray isInside(qreal x, qreal y, qreal radius);

signals:
    void valueChanged();
    void typeChanged();

private:
    QJsonValue m_value;
    mutable QJsonArray m_set_property;
    QString m_type;
};

} // namespace Displax

#endif // ENTITYTOUCHEVENTS_H
