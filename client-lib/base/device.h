#ifndef DEVICE_H
#define DEVICE_H

#include <displaxclientlib_global.h>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <entitycontext.h>

namespace Displax {

class DPX_EXPORT Device: public EntityContext
{
    Q_OBJECT
    Q_PROPERTY(int productId READ productId NOTIFY productIdChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString productName READ productName NOTIFY productNameChanged)
    Q_PROPERTY(QString serialNumber READ serialNumber NOTIFY serialNumberChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(QJsonArray protoEntities READ protoEntities NOTIFY protoEntitiesChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

public:
    explicit Device(Session* parent = nullptr);
    virtual ~Device();

    void insertEntity(Entity* e) override;
    bool containsEntity(const QString& name) override;
    Entity* getEntity(const QString& name) override;
    void removeEntity(Entity* e) override;
    void activateEntity(Entity*) override;
    void activateNotify(Entity*) override;
    void inactivateNotify(Entity*) override;
    void cancelAction(Action* a) override;

    Session* session();
    const Session* session() const;

    int productId() const;
    QString type() const;
    bool isValid() const;
    QString productName() const;
    QString serialNumber() const;
    QString version() const;
    QJsonArray protoEntities() const;
    Q_INVOKABLE bool hasEntity(const QString& name) const;
    Q_INVOKABLE QJsonObject getProtoEntity(const QString& name) const;

    void updateJsonData(const QJsonObject& obj);

    void invalidate();

signals:
    void productIdChanged();
    void typeChanged();
    void validChanged();
    void productNameChanged();
    void serialNumberChanged();
    void versionChanged();
    void protoEntitiesChanged();

private:
    int m_product_id;
    QString m_type;
    QString m_product_name;
    QString m_serial_number;
    QString m_version;
    QJsonArray m_proto_entities;
    bool m_valid;
    QHash<QString, Entity*> m_entities;
};

} // namespace Displax

#endif // DEVICE_H
