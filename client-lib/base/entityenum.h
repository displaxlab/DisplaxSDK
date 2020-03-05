#ifndef ENTITYENUM_H
#define ENTITYENUM_H

#include <QJsonObject>
#include <entity.h>
#include <QStringListModel>

namespace Displax {

class EntityEnum: public Entity
{
    Q_OBJECT
    Q_PROPERTY(QString value READ value WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(QVariant allValues READ allValuesQml NOTIFY allValuesChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)

public:
    explicit EntityEnum(QObject* parent = nullptr);
    explicit EntityEnum(const QJsonObject& proto_entity, EntityContext* context, QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    QString value() const;
    void setValue(const QString& value);

    QString type() const override;
    void setType(const QString& type);

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

    QStringList allValues() const;
    QVariant allValuesQml();

signals:
    void valueChanged();
    void allValuesChanged();
    void typeChanged();

private:
    QString m_value, m_old_value;
    QString m_type;
    QStringList m_all_values;
};

} // namespace Displax

#endif // ENTITYENUM_H
