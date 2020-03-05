#ifndef ENTITYBOOL_H
#define ENTITYBOOL_H

#include <QJsonObject>
#include <entity.h>

namespace Displax {

class EntityBool: public Entity
{
    Q_OBJECT
    Q_PROPERTY(bool value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit EntityBool(QObject* parent = nullptr);
    explicit EntityBool(const QJsonObject& proto_entity, EntityContext* context, QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    bool value() const;
    void setValue(bool value);

    QString type() const override;
    void setType(const QString& type);

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

signals:
    void valueChanged();
    void typeChanged();

private:
    bool m_value, m_old_value;
    QString m_type;
};

} // namespace Displax

#endif // ENTITYBOOL_H
