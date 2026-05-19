#ifndef DPXCOMCONNECTION_H
#define DPXCOMCONNECTION_H

#include <dpxcomstream.h>
#include <dpxcomurl.h>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>
#include <displaxclientlib_global.h>

namespace Displax {

class DPXComConnection: public QObject
{
    Q_OBJECT

public:
    struct ID: public QPair<QString, QString>
    {
        ID() = default;
        inline ID(const QString& a, const QString& b)
            : QPair<QString, QString>(a, b)
        {}
        inline bool operator==(const ID& rhs) const
        {
            return (first == rhs.first && second == rhs.second)
                   || (first == rhs.second && second == rhs.first);
        }
        inline bool operator!=(const ID& rhs) const { return !((*this) == rhs); }
    };

    explicit DPXComConnection(const DPXComUrl& url, QObject* parent = nullptr);

    QHostAddress localAddress() const;
    quint16 localPort() const;
    QString peerName() const;
    QHostAddress peerAddress() const;
    quint16 peerPort() const;
    QUrl requestUrl() const;
    DPXComUrl url() const;

    QAbstractSocket::SocketState state() const;

    QByteArray readAll();
    quint64 write(const QByteArray& data);
    void flush();

    QJsonObject readJson();
    void writeJson(const QJsonObject& obj);

    DPXComStream::EncodeType encodeType() const;
    void setEncodeType(DPXComStream::EncodeType t);

    ID id() const;
    void setId(const ID& id);

    bool isLocal() const;

    bool operator==(const DPXComUrl& url) const;

signals:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState);
    void error(QAbstractSocket::SocketError);
    void readyRead();
    void aboutToClose();

public slots:
    void connectToHost(const QHostAddress& address, const quint16 port);
    void disconnectFromHost();

private slots:
    void readReadyData();

protected:
    DPXComUrl m_url;
    ID m_id;
    QTcpSocket m_socket;
    DPXComStream::Reader m_reader;
    DPXComStream::Writer m_writer;
};

bool operator==(const DPXComUrl& lhs, const DPXComConnection& rhs);

} // namespace Displax

#endif // DPXCOMCONNECTION_H
