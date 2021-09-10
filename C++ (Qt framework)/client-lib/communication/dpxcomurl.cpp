#include "dpxcomurl.h"

/*!
    \class DPXComUrl

    \brief The DPXComUrl class provides a convenient interface for working with URLs.

    \reentrant
    \ingroup skinservice
    \inmodule SkinService

    It can parse and construct URLs in both encoded and unencoded form (from QUrl). It also support TPC or UDP port.

    \sa SkinService, DPXComConnection, DPXComManager
*/

/*!
    \fn bool DPXComUrl::isValid() const

    Checks if url is valid.
*/

/*!
    \fn bool DPXComUrl::operator==(const DPXComUrl& rhs) const

    Returns \c true if \a rhs is equivalent to url; otherwise returns \c false.
*/

/*!
    \fn bool DPXComUrl::operator!=(const DPXComUrl& rhs) const

    Returns \c true if \a rhs isn't equivalent to url; otherwise returns \c false.
*/

namespace Displax {

/*! Create a new empty url. */
DPXComUrl::DPXComUrl()
    : m_address("")
    , m_port(-1)
    , m_path("")
{}

/*! Create a new url with the given \a str (QString). */
DPXComUrl::DPXComUrl(const QString& str)
    : m_address("")
    , m_port(-1)
    , m_path("")
{
    QUrl url = QUrl(str);
    if(url.isValid())
    {
        m_address = url.host().isNull() ? url.path() : url.host();
        m_port = url.port();
        m_path = url.path();
    }
    else
    {
        if(str.contains(":"))
        {
            QString last = str.section(":", -1);
            m_address = str.chopped(last.size() + 1);
            m_port = last.toInt();
        }
    }
}

/*! Create a new url with the given \a url (QUrl). */
DPXComUrl::DPXComUrl(const QUrl& url)
    : m_address(url.host())
    , m_port(url.port())
    , m_path(url.path())
{}

/*! Create a new url with the given \a address and \a port. */
DPXComUrl::DPXComUrl(const QHostAddress& address, quint16 port)
    : m_address(address.toString())
    , m_port(port)
    , m_path("")
{}

/*! Returns url converted to QUrl. */
QUrl DPXComUrl::toUrl() const
{
    return QUrl::fromUserInput(toString());
}

/*! Returns the host address if exist. */
QHostAddress DPXComUrl::hostAddress() const
{
    return QHostAddress(m_address);
}

/*! Returns the address port if exist. */
quint16 DPXComUrl::port() const
{
    return m_port;
}

/*! Returns the url in string format. */
QString DPXComUrl::toString() const
{
    QString str = m_address;
    if(m_port > 0)
        str += ":" + QString::number(m_port);
    if(!m_path.isEmpty())
        str += "/" + m_path;
    return str;
}

/*! Allow compare partial address, only ipv6 */
bool DPXComUrl::isPartialEqual(const DPXComUrl& rhs)
{
    return QHostAddress(m_address).isEqual(QHostAddress(rhs.m_address));
}

/*! Allow native convertion to QString. */
DPXComUrl::operator QString() const
{
    return toString();
}

} // namespace Displax
