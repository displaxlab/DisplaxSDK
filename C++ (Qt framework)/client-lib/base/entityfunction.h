#ifndef ENTITYFUNCTION_H
#define ENTITYFUNCTION_H

#include <QJsonObject>
#include <QVariant>
#include <entity.h>

namespace Displax {

class EntityFunction: public Entity
{
    Q_OBJECT
    Q_PROPERTY(QString command READ command NOTIFY commandChanged)
    Q_PROPERTY(QVariant parameters READ parameters WRITE setParameters NOTIFY parametersChanged)
    Q_PROPERTY(QVariant result READ result NOTIFY resultChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

public:
    explicit EntityFunction(QObject* parent = nullptr);
    explicit EntityFunction(const QJsonObject& proto_entity, EntityContext* context,
                            QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    QString command() const;

    QVariant parameters() const;
    void setParameters(const QVariant& parameters);

    Q_INVOKABLE void execute();

    QVariant result() const;

    QString type() const override;

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

signals:
    void commandChanged();
    void parametersChanged();
    void resultChanged();
    void typeChanged();

private:
    QString m_command;
    QVariant m_parameters;
    QVariant m_result;
};

} // namespace Displax

#endif // ENTITYFUNCTION_H
