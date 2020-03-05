#ifndef ENTITY_H
#define ENTITY_H

#include <displaxclientlib_global.h>
#include <QJsonValue>
#include <QObject>
#include <entitycontext.h>

namespace Displax {

class DPX_EXPORT Entity: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool initialised READ isInitialised NOTIFY initialisedChanged)
    Q_PROPERTY(bool dirty READ isDirty NOTIFY dirtyChanged)
    Q_PROPERTY(bool notify READ isNotify WRITE setNotify NOTIFY notifyChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Displax::EntityContext* context READ context WRITE setContext NOTIFY contextChanged)

public:
    explicit Entity(QObject* parent = nullptr);
    virtual ~Entity();

    virtual QJsonValue toJson() const = 0;
    virtual void fromJson(const QJsonValue& json) = 0;
    virtual void cancelChange() = 0;

    QString id() const;
    virtual QString type() const = 0;

    QString name() const;
    virtual void setName(const QString& name);

    EntityContext* context() const;
    virtual void setContext(EntityContext* ctx);

    bool isDirty() const;
    bool isInitialised() const;

    int timeout();
    void setTimeout(int interval);

    bool isNotify() const;
    void setNotify(bool notify);

    bool isValid() const;

signals:
    void dirtyChanged();
    void initialisedChanged();
    void nameChanged();
    void contextChanged();
    void notifyChanged();

protected:
    void setInitialised(bool initialised);
    void setDirty(bool dirty);
    void setContextUnsafe(EntityContext* ctx);
    void setNameUnsafe(const QString& name);

private:
    const QString m_id;
    QString m_name;
    EntityContext* m_context;
    bool m_dirty;
    bool m_initialised;
    bool m_notify;
    int m_timeout;
};

} // namespace Displax

#endif // ENTITY_H
