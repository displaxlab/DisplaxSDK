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
    , m_reader(&m_socket)
    , m_writer(&m_socket)
{
    connect(&m_socket, &QTcpSocket::connected, this, &DPXComConnection::connected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &DPXComConnection::disconnected);
    connect(&m_socket, &QTcpSocket::stateChanged, this, &DPXComConnection::stateChanged);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &DPXComConnection::error);
    connect(&m_socket, &QTcpSocket::readyRead, this, &DPXComConnection::readReadyData);
    connect(&m_socket, &QTcpSocket::aboutToClose, this, &DPXComConnection::aboutToClose);
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
    return QUrl::fromUserInput(peerAddress().toString() % ":" % QString::number(peerPort()));
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

void DPXComConnection::abort()
{
    m_socket.abort();
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

void DPXComConnection::connectToHost(const QHostAddress& address, const quint16 port)
{
    m_socket.connectToHost(address, port);
}

void DPXComConnection::disconnectFromHost()
{
    m_socket.disconnectFromHost();
}

QJsonObject DPXComConnection::readJson()
{
    if(m_reader.hasItems())
        return m_reader.dequeue();
    return QJsonObject();
}

void DPXComConnection::writeJson(const QJsonObject& obj)
{
    m_writer.enqueue(obj);
}

DPXComStream::EncodeType DPXComConnection::encodeType() const
{
    return m_writer.encodeType();
}

void DPXComConnection::setEncodeType(DPXComStream::EncodeType t)
{
    m_writer.setEncodeType(t);
}

void DPXComConnection::readReadyData()
{
    m_reader.reparse();
    if(m_writer.encodeType() == DPXComStream::Unknown)
        m_writer.setEncodeType(m_reader.encodeType());

    if(m_reader.hasItems())
        emit readyRead();
}

} // namespace Displax
