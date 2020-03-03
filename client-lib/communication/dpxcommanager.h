#ifndef DPXCOMMANAGER_H
#define DPXCOMMANAGER_H

#include <dpxcomconnection.h>
#include <dpxcompeermanager.h>
#include <QList>
#include <QObject>

namespace Displax {

class DPXComManager: public QObject
{
    Q_OBJECT

public:
    typedef QPair<QHostAddress, QString> HostId;
    explicit DPXComManager(QObject* parent = nullptr);
    virtual ~DPXComManager();

    QHostAddress::SpecialAddress listenning() const;

    bool isPartialEqual(const DPXComUrl& url) const;
    bool hasConnection(const DPXComUrl& url) const;
    bool hasConnection(const DPXComConnection::ID& id) const;
    void addConnection(DPXComConnection* connection);
    void removeConnection(DPXComConnection* connection);

    DPXComConnection* getConnection(const DPXComUrl& url);
    DPXComConnection* getPartialUrlConnection(const DPXComUrl& url);

    QList<DPXComConnection*>* connections();

    QString id() const;

signals:
    void addedConnection(DPXComConnection* connection);
    void removedConnection(DPXComConnection* connection);

public slots:
    void start(QHostAddress::SpecialAddress address = QHostAddress::Any);
    void stop();

private slots:
    void connectionClosed();

private:
    void generateId();

    QList<DPXComConnection*> m_connections;
    DPXComPeerManager m_peer_manager;
    QString m_id;
    QHostAddress::SpecialAddress m_listen_address;
};

} // namespace Displax

#endif // DPXCOMCONNECTIONMANAGER_H
