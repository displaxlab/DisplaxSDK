#ifndef ENTITYCONTEXT_H
#define ENTITYCONTEXT_H

#include <QObject>
#include <QString>
#include <displaxclientlib_global.h>

namespace Displax {

class DPX_EXPORT EntityContext: public QObject
{
    Q_OBJECT

public:
    EntityContext(QObject* parent = nullptr);
    virtual ~EntityContext() = default;

    virtual void insertEntity(Entity* e) = 0;
    virtual bool containsEntity(const QString& namme) = 0;
    virtual Entity* getEntity(const QString& name) = 0;
    virtual void removeEntity(Entity* e) = 0;

    virtual void activateEntity(Entity*) = 0;

    virtual void activateNotify(Entity*) = 0;
    virtual void inactivateNotify(Entity*) = 0;

    virtual void cancelAction(Action*) = 0;
};

} // namespace Displax

#endif // ENTITYCONTEXT_H
