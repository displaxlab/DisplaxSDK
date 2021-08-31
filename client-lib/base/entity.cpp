/*!
    \class Entity

    \brief The Entity class provides a local representation of its counterpart in the remote service.

    \reentrant

    This Entity define the basic type struct to deal with remote service.
    For each type it's necessary to specificate the conversions methods and provide get/set value
    methods.
*/

#include "entity.h"
#include <QUuid>

namespace Displax {

Entity::Entity(QObject* parent)
    : QObject(parent)
    , m_id(QUuid::createUuid().toString())
    , m_context(nullptr)
    , m_dirty(true)
    , m_initialised(false)
    , m_notify(true)
    , m_timeout(3000)
{}

Entity::~Entity()
{
    if(m_context && !m_name.isEmpty())
        m_context->removeEntity(this);
}

QString Entity::id() const
{
    return m_id;
}

/*!
  Return name entity.
  */
QString Entity::name() const
{
    return m_name;
}

/*!
  Set name entity.
  If valid inform remote service.
  */
void Entity::setName(const QString& name)
{
    if(m_name != name)
    {
        if(m_context && !m_name.isEmpty())
            m_context->removeEntity(this);

        setInitialised(false);
        m_name = name;

        if(m_context && !m_name.isEmpty())
            m_context->insertEntity(this);

        emit nameChanged();
    }
}

EntityContext* Entity::context() const
{
    return m_context;
}

/*!
  Set new context.
  If has a old one, remove from it.
  If valid then insert to new one.
  */
void Entity::setContext(EntityContext* ctx)
{
    if(m_context != ctx)
    {
        if(m_context && !m_name.isEmpty())
            m_context->removeEntity(this);

        setInitialised(false);
        m_context = ctx;

        if(m_context && !m_name.isEmpty())
            m_context->insertEntity(this);

        emit contextChanged();
    }
}

bool Entity::isDirty() const
{
    return m_dirty;
}

void Entity::setDirty(bool dirty)
{
    if(m_dirty != dirty)
    {
        m_dirty = dirty;
        emit dirtyChanged();
    }
}

/*!
  If you subclassing you can set new context without inform it.
  This is unsafe. Be careful if you use this.
  */
void Entity::setContextUnsafe(EntityContext* ctx)
{
    m_context = ctx;
}

/*!
  If you subclassing you can set new name without inform the context.
  This is unsafe. Be careful if you use this.
  */
void Entity::setNameUnsafe(const QString& name)
{
    m_name = name;
}

bool Entity::isNotify() const
{
    return m_notify;
}

/*!
  To receive notifications if remote has changes is necessary to tell them to notify us.
  By default this is active.
  */
void Entity::setNotify(bool notify)
{
    if(m_notify != notify)
    {
        if(m_notify && !m_name.isEmpty() && m_context)
            m_context->inactivateNotify(this);
        m_notify = notify;
        if(m_notify && !m_name.isEmpty() && m_context)
            m_context->activateNotify(this);
        emit notifyChanged();
    }
}

bool Entity::isValid() const
{
    return m_context && !m_name.isEmpty();
}

/*!
  Return true if is initialised with remote service.
  */
bool Entity::isInitialised() const
{
    return m_initialised;
}

void Entity::setInitialised(bool initialised)
{
    if(m_initialised != initialised)
    {
        m_initialised = initialised;
        emit initialisedChanged();
    }
}

int Entity::timeout()
{
    return m_timeout;
}

void Entity::setTimeout(int interval)
{
    m_timeout = interval;
}

} // namespace Displax
