#ifndef ENTITYSTRING_H
#define ENTITYSTRING_H

#include <QJsonObject>
#include <entity.h>

namespace Displax {

class EntityString: public Entity
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(bool isWritable READ isWritable NOTIFY isWritableChanged)

public:
    explicit EntityString(QObject* parent = nullptr);
    explicit EntityString(const QJsonObject& proto_entity, EntityContext* context, QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    QString value() const;
    void setValue(const QString& value);

    QString type() const override;
    void setType(const QString& type);

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

    bool isWritable() const;

signals:
    void valueChanged();
    void typeChanged();
    void isWritableChanged();

private:
    QString m_value, m_old_value;
    QString m_type;
    bool m_is_writable;
};

} // namespace Displax

#endif // ENTITYSTRING_H
