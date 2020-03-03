/*!
    \class DPXComConnection

    \brief The DPXComConnection class provides the base functionality to connect to Displax Service

    \reentrant

    DPXComConnection establish connection between our client and service server.

*/

#include "dpxcomconnection.h"

namespace Displax {

DPXComConnection::DPXComConnection(const DPXComUrl& url, QObject* parent)
    : QObject(parent)
    , m_url(url)
    , m_socket()
{
    connect(&m_socket, SIGNAL(connected()), this, SIGNAL(connected()));
    connect(&m_socket, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    connect(&m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this,
            SIGNAL(stateChanged(QAbstractSocket::SocketState)));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this,
            SIGNAL(error(QAbstractSocket::SocketError)));
    connect(&m_socket, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
    connect(&m_socket, SIGNAL(aboutToClose()), this, SIGNAL(aboutToClose()));
}

QHostAddress DPXComConnection::localAddress() const
{
    return m_socket.localAddress();
}

quint16 DPXComConnection::localPort() const
{
    return m_socket.localPort();
}

QString DPXComConnection::peerName() const
{
    return m_socket.peerName();
}

QHostAddress DPXComConnection::peerAddress() const
{
    return m_url.hostAddress();
}

quint16 DPXComConnection::peerPort() const
{
    return m_url.port();
}

QUrl DPXComConnection::requestUrl() const
{
    return QUrl::fromUserInput(peerAddress().toString() + ":" + QString::number(peerPort()));
}

DPXComUrl DPXComConnection::url() const
{
    return m_url;
}

QAbstractSocket::SocketState DPXComConnection::state() const
{
    return m_socket.state();
}

QByteArray DPXComConnection::readAll()
{
    return m_socket.readAll();
}

quint64 DPXComConnection::write(const QByteArray& data)
{
    return m_socket.write(data);
}

void DPXComConnection::flush()
{
    m_socket.flush();
}

DPXComConnection::ID DPXComConnection::id() const
{
    return m_id;
}

void DPXComConnection::setId(const ID& id)
{
    m_id = id;
}

bool DPXComConnection::isLocal() const
{
    const QString url = m_url.toString();
    return url.contains("127.0.0.1") || url.contains("10.0.0") || url.contains("localhost");
}

bool DPXComConnection::operator==(const DPXComUrl& url) const
{
    return m_url == url;
}

bool operator==(const DPXComUrl& lhs, const DPXComConnection& rhs)
{
    return rhs == lhs;
}

void DPXComConnection::connectToHost(const QHostAddress& address, quint16 port)
{
    m_socket.connectToHost(address, port);
}

void DPXComConnection::disconnectFromHost()
{
    m_socket.disconnectFromHost();
}

} // namespace Displax
