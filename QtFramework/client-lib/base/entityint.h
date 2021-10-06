#ifndef ENTITYINT_H
#define ENTITYINT_H

#include <QJsonObject>
#include <entity.h>

namespace Displax {

class EntityInt: public Entity
{
    Q_OBJECT
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int min READ min NOTIFY minChanged)
    Q_PROPERTY(int max READ max NOTIFY maxChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit EntityInt(QObject* parent = nullptr);
    explicit EntityInt(const QJsonObject& proto_entity, EntityContext* context, QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    int value() const;
    void setValue(int value);

    int min() const;
    int max() const;

    QString type() const override;
    void setType(const QString& type);

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

signals:
    void valueChanged();
    void minChanged();
    void maxChanged();
    void typeChanged();

private:
    int m_value, m_old_value;
    int m_min;
    int m_max;
    QString m_type;
};

} // namespace Displax

#endif // ENTITYINT_H
