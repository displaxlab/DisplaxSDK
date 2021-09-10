#ifndef SESSION_H
#define SESSION_H

#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QQmlListProperty>
#include <action.h>
#include <device.h>

namespace Displax {

class DPXComConnection;

class DPX_EXPORT Session: public EntityContext
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString version READ version NOTIFY versionChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(bool isLocal READ isLocal NOTIFY isLocalChanged)
    Q_PROPERTY(QQmlListProperty<Displax::Device> devices READ devicesQml NOTIFY devicesChanged)

public:
    explicit Session(DPXComConnection* connection);
    virtual ~Session();

    void insertEntity(Entity* e) override;
    bool containsEntity(const QString& namme) override;
    Entity* getEntity(const QString& name) override;
    void removeEntity(Entity* e) override;
    void activateEntity(Entity* e) override;
    void activateNotify(Entity* e) override;
    void inactivateNotify(Entity* e) override;
    void cancelAction(Action*) override;

    void activateEntity(Entity* e, Device* device);
    void activateNotify(Entity* e, Device* device);
    void inactivateNotify(Entity* e, Device* device);

    QString url() const;
    QString version() const;
    bool isValid() const;
    bool isLocal() const;
    DPXComConnection* connection() const;

    QHash<QString, Device*>* devices();
    QQmlListProperty<Device> devicesQml();

    Q_INVOKABLE Displax::Device* device(const QString& serialNumber);
    Q_INVOKABLE void sendCli(QJsonArray arr);

    void invalidate();

signals:
    void urlChanged();
    void versionChanged();
    void validChanged();
    void isLocalChanged();
    void devicesChanged();
    void deviceInserted(Displax::Device* device);
    void deviceRemoved(Displax::Device* device);
    void receiveCli(const QJsonArray& arr);

private slots:
    void readyRead();
    void sendActions();
    void sendNotifications();
    void firstRequest();

private:
    void parseDevicesAll(const QJsonObject& o);
    void parseVersion(const QJsonValue& v);
    void parseNewData(const QJsonDocument& doc);
    void parseReactions(const QJsonObject& reactions);
    void parseNotifications(const QJsonObject& notifications);
    void requestDevicesAll();
    void requestProtocolVersion();
    void updateDevice(const QJsonObject& data);
    void removeDevice(const QString& serialNumber);

    DPXComConnection* m_connection;
    QHash<QString, Device*> m_devices;
    QList<Device*> m_devices_qml;
    bool m_valid;
    int m_id;
    QString m_version;
    QJsonArray m_pending_actions;
    QHash<quint32, Action*> m_active_actions;
    QHash<QString, QSet<QString>> m_register_notifies;
    QHash<QString, QSet<QString>> m_unregister_notifies;
    QHash<QString, Entity*> m_entities;
    QByteArray m_data_received;
};

} // namespace Displax

#endif // SESSION_H
