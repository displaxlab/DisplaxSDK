#ifndef ENTITYFLOAT_H
#define ENTITYFLOAT_H

#include <QJsonObject>
#include <entity.h>

namespace Displax {

class EntityFloat: public Entity
{
    Q_OBJECT
    Q_PROPERTY(float value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(float min READ min NOTIFY minChanged)
    Q_PROPERTY(float max READ max NOTIFY maxChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit EntityFloat(QObject* parent = nullptr);
    explicit EntityFloat(const QJsonObject& proto_entity, EntityContext* context, QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    float value() const;
    void setValue(float value);

    float min() const;
    float max() const;

    QString type() const override;
    void setType(const QString& type);

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

signals:
    void valueChanged();
    void minChanged();
    void maxChanged();
    void typeChanged();

private:
    float m_value, m_old_value;
    float m_min;
    float m_max;
    QString m_type;
};

} // namespace Displax

#endif // ENTITYFLOAT_H
