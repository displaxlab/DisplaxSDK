#ifndef ACTION_H
#define ACTION_H

#include <displaxclientlib_global.h>
#include <QJsonObject>
#include <QString>
#include <QTimer>
#include <entity.h>

namespace Displax {

class DPX_EXPORT Action: public QObject
{
    Q_OBJECT

public:
    explicit Action(Entity* e, quint32 id);
    virtual ~Action() = default;

    QJsonObject activate();
    void deactivate(const QJsonValue& value);
    bool isActive() const;

    Entity* entity();
    QString name() const;
    EntityContext* context();

    int id() const;
    void setId(int id);

public slots:
    void timeoutCallback();

private:
    EntityContext* m_context;
    QString m_name;
    QTimer m_timer;
    int m_id;
};

} // namespace Displax

#endif // ACTION_H
