/*!
    \class SessionManager

    \brief The SessionManager class handle new remote services.

    \reentrant

    SessionManager pay attention to new remote services.
    If discovery a new one create a Session to represent it.

    \sa Session
*/

#include "sessionmanager.h"
#include <dpxcommanager.h>

namespace Displax {

SessionManager::SessionManager(QObject* parent)
    : QObject(parent)
    , m_com_manager(new DPXComManager(this))
{}

void SessionManager::addConnection(DPXComConnection* connection)
{
    if(!m_sessions.contains(connection))
    {
        Session* session = new Session(connection);
        m_sessions[session->connection()] = session;
        m_sessions_qml = m_sessions.values();
        emit sessionInserted(session);
        emit sessionsChanged();
    }
}

void SessionManager::removeConnection(DPXComConnection* connection)
{
    if(m_sessions.contains(connection))
    {
        auto s = m_sessions.take(connection);
        s->invalidate();
        m_sessions_qml = m_sessions.values();
        emit sessionRemoved(s);
        emit sessionsChanged();
    }
}

/*!
  Stop remote service discovery. This also cancel active sessions.
  */
void SessionManager::stop()
{
    disconnect(m_com_manager, &DPXComManager::addedConnection, this, &SessionManager::addConnection);

    disconnect(m_com_manager, &DPXComManager::removedConnection, this,
               &SessionManager::removeConnection);

    for(auto& s: m_sessions)
    {
        s->invalidate();
        emit sessionRemoved(s);
    }

    m_sessions.clear();
    m_sessions_qml.clear();
    emit sessionsChanged();

    m_com_manager->stop();
}

/*!
  Start remote service discovery.
  */
void SessionManager::start(bool local)
{
    connect(m_com_manager, &DPXComManager::addedConnection, this, &SessionManager::addConnection);
    connect(m_com_manager, &DPXComManager::removedConnection, this,
            &SessionManager::removeConnection);

    m_com_manager->start(local ? QHostAddress::LocalHost : QHostAddress::Any);
}

QList<Session*> SessionManager::sessions()
{
    return m_sessions.values();
}

QQmlListProperty<Session> SessionManager::sessionsQml()
{
    return QQmlListProperty<Session>(this, &m_sessions_qml);
}

} // namespace Displax
