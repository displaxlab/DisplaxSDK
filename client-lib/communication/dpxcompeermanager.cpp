#include "dpxcompeermanager.h"
#include <dpxcomconnection.h>
#include <dpxcommanager.h>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtNetwork>

namespace Displax {

constexpr static const quint32 broadcast_port = 45001;

DPXComPeerManager::DPXComPeerManager(DPXComManager* manager)
    : m_manager(manager)
{}

void DPXComPeerManager::start(QHostAddress::SpecialAddress address)
{
    QJsonObject obj;
    obj.insert("frame_type", "discovery");
    obj.insert("server_port", 0);
    obj.insert("type", "DesktopConnect");
    obj.insert("id", m_manager->id());
    QJsonDocument doc(obj);
    m_datagram = doc.toBinaryData();

    updateAddresses();

    m_broadcast_socket.bind(address, 0, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(&m_broadcast_socket, SIGNAL(readyRead()), this, SLOT(readBroadcastDatagram()));
    connect(&m_broadcast_timer, SIGNAL(timeout()), this, SLOT(sendBroadcastDatagram()));
    sendBroadcastDatagram();
    m_broadcast_timer.start(2000);
}

void DPXComPeerManager::stop()
{
    m_broadcast_timer.stop();
    m_broadcast_socket.close();
}

void DPXComPeerManager::sendBroadcastDatagram()
{
    bool valid_broadcast_addresses = true;
    foreach(QHostAddress address, m_broadcast_addresses)
        if(m_broadcast_socket.writeDatagram(m_datagram, address, broadcast_port) == -1)
            valid_broadcast_addresses = false;

    if(!valid_broadcast_addresses)
        updateAddresses();
}

void DPXComPeerManager::readBroadcastDatagram()
{
    while(m_broadcast_socket.hasPendingDatagrams())
    {
        QHostAddress sender_ip;
        quint16 sender_port;
        QByteArray datagram;
        datagram.resize(m_broadcast_socket.pendingDatagramSize());
        if(m_broadcast_socket.readDatagram(datagram.data(), datagram.size(), &sender_ip,
                                           &sender_port)
           == -1)
            continue;

        QJsonDocument doc = QJsonDocument::fromBinaryData(datagram);
        if(doc.isEmpty() && !doc.isObject())
            continue;

        QJsonObject obj = doc.object();
        if(!obj.contains("type") || obj.value("type").toString() != "Service")
            continue;

        if(!obj.contains("id") || obj.value("id").toString() == m_manager->id())
            continue;

        if(!obj.contains("server_port"))
            continue;

        quint16 sender_server_port = obj.value("server_port").toInt();

        if(isLocalHostAddress(sender_ip))
            continue;

        if(!m_manager->hasConnection(DPXComUrl(sender_ip, sender_server_port))
           && !m_manager->hasConnection(
               DPXComConnection::ID(obj.value("id").toString(), m_manager->id())))
        {
            auto connection = new DPXComConnection(DPXComUrl(sender_ip, sender_server_port),
                                                   m_manager);
            connection->setId(DPXComConnection::ID(obj.value("id").toString(), m_manager->id()));
            connect(connection, SIGNAL(connected()), this, SLOT(establishConnection()));
            connection->connectToHost(sender_ip, sender_server_port);
            m_manager->addConnection(connection);
        }
    }
}

void DPXComPeerManager::establishConnection()
{
    DPXComConnection* c = dynamic_cast<DPXComConnection*>(QObject::sender());
    if(c)
    {
        QJsonObject obj;
        obj.insert("frame_type", "connection_info");
        obj.insert("server_port", 0);
        obj.insert("type", "DesktopConnect");
        obj.insert("id", m_manager->id());
        obj.insert("communication_port", c->localPort());
        QJsonDocument doc(obj);
        QByteArray datagram = doc.toBinaryData();
        m_broadcast_socket.writeDatagram(datagram, c->peerAddress(), broadcast_port);
    }
}

void DPXComPeerManager::updateAddresses()
{
    m_broadcast_addresses.clear();
    m_ip_addresses.clear();
    m_broadcast_addresses << QHostAddress("127.0.0.1");
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
        foreach(QNetworkAddressEntry entry, interface.addressEntries())
            if(entry.broadcast() != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost)
            {
                m_broadcast_addresses << entry.broadcast();
                m_ip_addresses << entry.ip();
            }
}

bool DPXComPeerManager::isLocalHostAddress(const QHostAddress& address) const
{
    foreach(QHostAddress local_address, m_ip_addresses)
        if(address.isEqual(local_address))
            return true;
    return false;
}

} // namespace Displax
