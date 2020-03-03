#include "dpxcommanager.h"
#include <QDateTime>
#include <QDir>
#include <QNetworkInterface>
#include <QSettings>
#include <QStandardPaths>
#include <QTime>

namespace Displax {

DPXComManager::DPXComManager(QObject* parent)
    : QObject(parent)
    , m_peer_manager(this)
{}

DPXComManager::~DPXComManager()
{
    stop();
}

QHostAddress::SpecialAddress DPXComManager::listenning() const
{
    return m_listen_address;
}

bool DPXComManager::isPartialEqual(const DPXComUrl& url) const
{
    for(auto it = m_connections.constBegin(), end = m_connections.constEnd(); it != end; ++it)
        if((*it)->url().isPartialEqual(url))
            return true;
    return false;
}

void DPXComManager::start(QHostAddress::SpecialAddress address)
{
    m_listen_address = address;
    generateId();
    m_peer_manager.start(address);
}

void DPXComManager::stop()
{
    m_peer_manager.stop();
    for(auto i = m_connections.begin(), e = m_connections.end(); i != e; ++i)
        removeConnection(*i);
}

bool DPXComManager::hasConnection(const DPXComUrl& url) const
{
    for(auto it = m_connections.constBegin(), end = m_connections.constEnd(); it != end; ++it)
        if(*(*it) == url)
            return true;
    return false;
}

bool DPXComManager::hasConnection(const DPXComConnection::ID& id) const
{
    for(auto it = m_connections.constBegin(), end = m_connections.constEnd(); it != end; ++it)
        if((*it)->id() == id)
            return true;
    return false;
}

void DPXComManager::addConnection(DPXComConnection* connection)
{
    if(!m_connections.contains(connection))
    {
        m_connections << connection;
        connect(connection, SIGNAL(disconnected()), this, SLOT(connectionClosed()));
        emit addedConnection(connection);
    }
}

void DPXComManager::removeConnection(DPXComConnection* connection)
{
    if(m_connections.contains(connection))
    {
        m_connections.removeOne(connection);
        connection->disconnectFromHost();
        emit removedConnection(connection);
        // TODO: fix this hard nail
        QTimer::singleShot(1000, connection, SLOT(deleteLater()));
    }
}

DPXComConnection* DPXComManager::getConnection(const DPXComUrl& url)
{
    foreach(auto var, m_connections)
        if(*var == url)
            return var;
    return nullptr;
}

DPXComConnection* DPXComManager::getPartialUrlConnection(const DPXComUrl& url)
{
    for(auto it = m_connections.constBegin(), end = m_connections.constEnd(); it != end; ++it)
        if((*it)->url().isPartialEqual(url))
            return (*it);
    return nullptr;
}

QList<DPXComConnection*>* DPXComManager::connections()
{
    return &m_connections;
}

void DPXComManager::connectionClosed()
{
    DPXComConnection* c = qobject_cast<DPXComConnection*>(QObject::sender());
    if(c)
        removeConnection(c);
}

void DPXComManager::generateId()
{
    m_id = "Displax";
    m_id += "DesktopConnect";
    m_id += QString::number(QDateTime::currentMSecsSinceEpoch());
    m_id += "0000";
    foreach(QNetworkInterface interface, QNetworkInterface::allInterfaces())
        foreach(QNetworkAddressEntry entry, interface.addressEntries())
            if(entry.broadcast() != QHostAddress::Null && entry.ip() != QHostAddress::LocalHost)
                m_id += entry.ip().toString();
    m_id += QString::number(reinterpret_cast<quint64>(this));
}

QString DPXComManager::id() const
{
    return m_id;
}

} // namespace Displax
