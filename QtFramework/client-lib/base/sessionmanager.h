#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QHash>
#include <QObject>
#include <QQmlListProperty>
#include <session.h>

namespace Displax {

class DPXComConnection;

class DPX_EXPORT SessionManager: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Displax::Session> sessions READ sessionsQml NOTIFY sessionsChanged)

public:
    explicit SessionManager(QObject* parent = nullptr);
    virtual ~SessionManager() = default;

    QList<Session*> sessions();
    QQmlListProperty<Session> sessionsQml();

signals:
    void sessionsChanged();
    void sessionInserted(Displax::Session*);
    void sessionRemoved(Displax::Session*);

public slots:
    void stop();
    void start(bool local = true);

private slots:
    void addConnection(DPXComConnection* connection);
    void removeConnection(DPXComConnection* connection);

private:
    class DPXComManager* m_com_manager;
    QHash<DPXComConnection*, Session*> m_sessions;
    QList<Session*> m_sessions_qml;
};

} // namespace Displax

#endif // SESSIONMANAGER_H
