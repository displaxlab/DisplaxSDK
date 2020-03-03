#ifndef DPXCOMPEERMANAGER_H
#define DPXCOMPEERMANAGER_H

#include <dpxcomconnection.h>
#include <QList>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QUdpSocket>

namespace Displax {

class DPXComManager;

class DPXComPeerManager: public QObject
{
    Q_OBJECT

public:
    explicit DPXComPeerManager(DPXComManager* manager);
    void start(QHostAddress::SpecialAddress address);
    void stop();

signals:
    void newConnection(DPXComConnection* connection);

private slots:
    void sendBroadcastDatagram();
    void readBroadcastDatagram();
    void establishConnection();

private:
    void updateAddresses();
    bool isLocalHostAddress(const QHostAddress& address) const;

    DPXComManager* m_manager;
    QList<QHostAddress> m_broadcast_addresses;
    QList<QHostAddress> m_ip_addresses;
    QUdpSocket m_broadcast_socket;
    QTimer m_broadcast_timer;
    QByteArray m_datagram;
};

} // namespace Displax

#endif // DPXCOMPEERMANAGER_H
