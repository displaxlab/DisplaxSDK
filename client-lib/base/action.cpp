/*!
    \class Action

    \brief The Action class provides a convenient interface to syncronize entities with service.

    \reentrant

    The Action handle midle protocol communication with local and remote property, synchronize them.

    \sa Entity
*/

#include "action.h"
#include <QJsonObject>
#include "entity.h"

namespace Displax {

Action::Action(Entity* e, quint32 id)
    : QObject()
    , m_context(e->context())
    , m_name(e->name())
    , m_id(id)
{}

/*!
  Build JSON package to remote service with new value.
  */
QJsonObject Action::activate()
{
    QJsonObject obj;
    Entity* e = m_context->getEntity(m_name);
    if(e)
    {
        obj.insert("key", m_name);
        obj.insert("id", m_id);
        QJsonValue v = e->toJson();
        if(!v.isNull())
        {
            if(e->type() == "Function")
                obj.insert("arguments", v);
            else
                obj.insert("value", v);
        }
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeoutCallback()));
        m_timer.setInterval(e->timeout());
        m_timer.setSingleShot(true);
        m_timer.start();
    }
    return obj;
}

/*!
  Convert JSON package from remote service and deliver to correspond entity.
  */
void Action::deactivate(const QJsonValue& value)
{
    m_timer.stop();
    Entity* e = m_context->getEntity(m_name);
    if(e)
        e->fromJson(value);
}

/*!
  Communication in transit.
  */
bool Action::isActive() const
{
    return m_timer.isActive();
}

Entity* Action::entity()
{
    return m_context->getEntity(m_name);
}

int Action::id() const
{
    return m_id;
}

void Action::setId(int id)
{
    m_id = id;
}

void Action::timeoutCallback()
{
    m_timer.stop();
    Entity* e = m_context->getEntity(m_name);
    if(e)
        e->cancelChange();
    m_context->cancelAction(this);
}

EntityContext* Action::context()
{
    return m_context;
}

QString Action::name() const
{
    return m_name;
}

} // namespace Displax
