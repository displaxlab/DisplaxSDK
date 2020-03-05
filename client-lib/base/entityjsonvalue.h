#ifndef ENTITYJSONVALUE_H
#define ENTITYJSONVALUE_H

#include <QJsonObject>
#include <entity.h>

namespace Displax {

class EntityJsonValue: public Entity
{
    Q_OBJECT
    Q_PROPERTY(QJsonValue value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit EntityJsonValue(QObject* parent = nullptr);
    explicit EntityJsonValue(const QJsonObject& proto_entity, EntityContext* context,
                             QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    QJsonValue value() const;
    void setValue(const QJsonValue& value);

    QString type() const override;
    void setType(const QString& type);

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

signals:
    void valueChanged();
    void typeChanged();

private:
    QJsonValue m_value, m_old_value;
    QString m_type;
};

} // namespace Displax

#endif // ENTITYJSONVALUE_H
