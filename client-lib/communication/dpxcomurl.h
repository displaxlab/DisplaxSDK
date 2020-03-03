#ifndef DPXCOMURL_H
#define DPXCOMURL_H

#include <QHostAddress>
#include <QString>
#include <QUrl>
#include <displaxclientlib_global.h>

namespace Displax {

class DPXComUrl
{
public:
    DPXComUrl();
    DPXComUrl(const QString& str);
    DPXComUrl(const QUrl& url);
    DPXComUrl(const QHostAddress& address, quint16 port);

    QUrl toUrl() const;
    QHostAddress hostAddress() const;
    quint16 port() const;
    QString toString() const;

    inline bool isValid() const;

    bool isPartialEqual(const DPXComUrl& rhs);
    inline bool operator==(const DPXComUrl& rhs) const;
    inline bool operator!=(const DPXComUrl& rhs) const;

    operator QString() const;

private:
    QString m_address;
    int m_port;
    QString m_path;
};

bool DPXComUrl::isValid() const
{
    return (!QHostAddress(m_address).isNull() && m_port > 0) || toUrl().isValid();
}

inline bool DPXComUrl::operator==(const DPXComUrl& rhs) const
{
    return (QHostAddress(m_address).isEqual(QHostAddress(rhs.m_address)) && m_port == rhs.m_port
            && m_path == rhs.m_path);
}

inline bool DPXComUrl::operator!=(const DPXComUrl& rhs) const
{
    return !(*this == rhs);
}

} // namespace Displax

#endif // DPXCOMURL_H
