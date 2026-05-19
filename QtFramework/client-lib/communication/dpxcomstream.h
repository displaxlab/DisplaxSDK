#ifndef DPXCOMSTREAM_H
#define DPXCOMSTREAM_H

#include <QIODevice>
#include <QJsonObject>
#include <QQueue>

namespace DPXComStream {

enum EncodeType
{
    Unknown = 0,
    QtBinJson, // Legacy, do not remove
    Json,
    Cbor
};

class Reader
{
public:
    struct Decoder
    {
        virtual ~Decoder() = default;

        virtual void appendData(const QByteArray& data) = 0;
        virtual DPXComStream::EncodeType encodeType() const = 0;
        virtual QJsonObject decode() = 0;
    };

    Reader(QIODevice* device);
    virtual ~Reader();

    QIODevice* device() const;
    void reparse();

    EncodeType encodeType() const;
    bool hasItems() const;
    QJsonObject dequeue();

    static QJsonObject decode(const QByteArray& a, EncodeType* type = nullptr);

private:
    static inline bool isQtBinJson(const QByteArray& d);
    static inline bool isJson(const QByteArray& d);
    static inline bool isCbor(const QByteArray& d);

    Decoder* m_handler = nullptr;
    QIODevice* m_device = nullptr;
    QQueue<QJsonObject> m_queue;
};

class Writer
{
public:
    struct Encoder
    {
        virtual ~Encoder() = default;

        virtual QByteArray encode(const QJsonObject& obj) = 0;
        virtual EncodeType encodeType() const = 0;
    };

    Writer(QIODevice* device);
    virtual ~Writer();

    QIODevice* device() const;

    void setEncodeType(const EncodeType type);
    EncodeType encodeType() const;
    void enqueue(const QJsonObject& obj);

    static QByteArray encode(const QJsonObject& o, const EncodeType t = EncodeType::Json);

private:
    Encoder* m_handler = nullptr;
    QIODevice* m_device = nullptr;
};

} // namespace DPXComStream

#endif // DPXCOMSTREAM_H
