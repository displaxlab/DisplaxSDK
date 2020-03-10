#ifndef ENTITYSETTINGS_H
#define ENTITYSETTINGS_H

#include <QObject>
#include <entity.h>

namespace Displax {

class EntitySettings: public Entity
{
    Q_OBJECT
public:
    explicit EntitySettings(QObject* parent = nullptr);
    EntitySettings(const QJsonObject& proto_entity, EntityContext* context, QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    QString type() const override;

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

signals:
    void downloadFinished(const QByteArray& data);
    void finished();
    void operationFail();

public slots:
    void upload(const QByteArray& data);
    void download();
    void savePermanent();
    void setDefault();

private:
    QJsonValue m_value;
};

} // namespace Displax

#endif // ENTITYSETTINGS_H
