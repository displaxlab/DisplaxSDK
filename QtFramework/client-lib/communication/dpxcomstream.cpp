#include "dpxcomstream.h"
#include <QCborValue>
#include <QChar>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <qcborstream.h>

namespace DPXComStream {

struct CborHeader
{
    static constexpr const quint64 g_tag = 0x4331435653585044UL; // DPXSVC1C

    quint64 tag = 0;
    quint32 payload = 0;

    bool isValid() const { return tag == g_tag; }
};

struct DecoderUnknow: public Reader::Decoder
{
    DecoderUnknow() = default;
    virtual ~DecoderUnknow() = default;

    void appendData(const QByteArray& data) override { Q_UNUSED(data); }

    EncodeType encodeType() const override { return EncodeType::Unknown; }

    QJsonObject decode() override { return QJsonObject(); }
};

struct DecoderJson: public Reader::Decoder
{
    DecoderJson() = default;
    virtual ~DecoderJson() = default;

    void appendData(const QByteArray& data) override { m_data.append(data); }

    EncodeType encodeType() const override { return EncodeType::Json; }

    QJsonObject decode() override
    {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(m_data, &error);
        switch(error.error)
        {
            case QJsonParseError::NoError: m_data.clear(); break;
            case QJsonParseError::GarbageAtEnd:
                {
                    auto d = m_data.left(error.offset);
                    m_data = m_data.mid(error.offset);
                    doc = QJsonDocument::fromJson(d, &error);
                }
                break;
            case QJsonParseError::UnterminatedString:
            case QJsonParseError::TerminationByNumber:
            case QJsonParseError::IllegalValue:
            case QJsonParseError::UnterminatedObject:
            case QJsonParseError::UnterminatedArray:
            case QJsonParseError::MissingNameSeparator: break;
            case QJsonParseError::MissingValueSeparator:
            case QJsonParseError::IllegalNumber:
            case QJsonParseError::IllegalEscapeSequence:
            case QJsonParseError::IllegalUTF8String:
            case QJsonParseError::MissingObject:
            case QJsonParseError::DeepNesting:
            case QJsonParseError::DocumentTooLarge:
            default: m_data.clear(); qCritical() << "ComDecoderJson corrupted data...";
        }
        if(doc.isNull())
            return QJsonObject();
        return doc.object();
    }

    QByteArray m_data;
};

struct DecoderCbor: public Reader::Decoder
{
    DecoderCbor() = default;
    virtual ~DecoderCbor() = default;

    void appendData(const QByteArray& data) override { m_data.append(data); }

    EncodeType encodeType() const override { return EncodeType::Cbor; }

    QJsonObject decode() override
    {
        if(m_data.isEmpty())
            return QJsonObject();

        const CborHeader* h = reinterpret_cast<const CborHeader*>(m_data.constData());
        if(!h->isValid())
        {
            qCritical() << "ComDecoderCbor corrupted data...";
            m_data.clear();
            return QJsonObject();
        }

        if(m_data.size() - sizeof(CborHeader) < h->payload)
            return QJsonObject();

        QCborStreamReader reader(m_data.sliced(sizeof(CborHeader), h->payload));
        if(reader.isTag() && reader.toTag() == QCborKnownTags::Signature)
            reader.next();

        const auto obj = QCborValue::fromCbor(reader).toJsonValue().toObject();
        m_data = m_data.mid(sizeof(CborHeader) + h->payload);
        return obj;
    }

    QByteArray m_data;
};

struct EncoderUnknow: public Writer::Encoder
{
    EncoderUnknow() = default;
    virtual ~EncoderUnknow() = default;

    EncodeType encodeType() const override { return EncodeType::Unknown; }

    QByteArray encode(const QJsonObject& obj) override
    {
        Q_UNUSED(obj);
        return QByteArray();
    }
};

struct EncoderJson: public Writer::Encoder
{
    EncoderJson() = default;
    virtual ~EncoderJson() = default;

    EncodeType encodeType() const override { return EncodeType::Json; }

    QByteArray encode(const QJsonObject& doc) override
    {
        return QJsonDocument(doc).toJson(QJsonDocument::Compact);
    }
};

struct EncoderCbor: public Writer::Encoder
{
    EncoderCbor() = default;
    virtual ~EncoderCbor() = default;

    EncodeType encodeType() const override { return EncodeType::Cbor; }

    QByteArray encode(const QJsonObject& obj) override
    {
        QByteArray ba;
        {
            QCborStreamWriter writer(&ba);
            auto o = QCborValue::fromJsonValue(obj);
            o.toCbor(writer);
        }
        CborHeader h;
        h.tag = h.g_tag;
        h.payload = ba.size();
        ba.prepend(reinterpret_cast<char*>(&h), sizeof(CborHeader));
        return ba;
    }
};

Reader::Reader(QIODevice* device)
    : m_handler(new DPXComStream::DecoderUnknow())
    , m_device(device)
{
    Q_ASSERT(device != nullptr);
}

Reader::~Reader()
{
    delete m_handler;
}

QIODevice* Reader::device() const
{
    return m_device;
}

void Reader::reparse()
{
    if(m_handler->encodeType() != Unknown)
    {
        m_handler->appendData(m_device->readAll());
        do
        {
            auto obj = m_handler->decode();
            if(obj.isEmpty())
                return;
            m_queue.enqueue(obj);
        }
        while(true);
    }

    if(m_device->bytesAvailable() < 8)
        return;

    QByteArray data = m_device->readAll();

    if(isJson(data))
    {
        delete m_handler;
        m_handler = new DPXComStream::DecoderJson();
    }
    else if(isCbor(data))
    {
        delete m_handler;
        m_handler = new DPXComStream::DecoderCbor();
    }
    else
    {
        qCritical() << "No ComStream decoder supported...";
    }
    m_handler->appendData(data);
    reparse();
}

EncodeType Reader::encodeType() const
{
    return m_handler->encodeType();
}

bool Reader::hasItems() const
{
    return !m_queue.isEmpty();
}

QJsonObject Reader::dequeue()
{
    return m_queue.dequeue();
}

QJsonObject Reader::decode(const QByteArray& a, EncodeType* type)
{
    if(a.size() < 8)
    {
        if(type)
            *type = EncodeType::Unknown;
        return QJsonObject();
    }

    if(isJson(a))
    {
        if(type)
            *type = EncodeType::Json;
        DPXComStream::DecoderJson decoder;
        decoder.appendData(a);
        return decoder.decode();
    }

    if(isCbor(a))
    {
        if(type)
            *type = EncodeType::Cbor;
        DPXComStream::DecoderCbor decoder;
        decoder.appendData(a);
        return decoder.decode();
    }

    if(type)
        *type = EncodeType::Unknown;
    return QJsonObject();
}

bool Reader::isQtBinJson(const QByteArray& d)
{
    return d[0] == 'q' && d[1] == 'b' && d[2] == 'j' && d[3] == 's' && d[4] == 1 && d[5] == 0
           && d[6] == 0 && d[7] == 0;
}

bool Reader::isJson(const QByteArray& d)
{
    if(d[0] != '{' && d[0] != '[')
        return false;

    for(int i = 0; i < d.size(); ++i)
        if(QChar(d[i]).isNonCharacter())
            return false;

    return true;
}

bool Reader::isCbor(const QByteArray& d)
{
    return d.startsWith("DPXSVC1C");
}

Writer::Writer(QIODevice* device)
    : m_handler(new DPXComStream::EncoderUnknow())
    , m_device(device)
{
    Q_ASSERT(device != nullptr);
}

Writer::~Writer()
{
    delete m_handler;
}

QIODevice* Writer::device() const
{
    return m_device;
}

void Writer::setEncodeType(const EncodeType type)
{
    if(m_handler->encodeType() == type)
        return;

    switch(type)
    {
        case Json:
            delete m_handler;
            m_handler = new DPXComStream::EncoderJson();
            break;
        case Cbor:
            delete m_handler;
            m_handler = new DPXComStream::EncoderCbor();
            break;
        default: delete m_handler; m_handler = new DPXComStream::EncoderUnknow();
    }
}

EncodeType Writer::encodeType() const
{
    return m_handler->encodeType();
}

void Writer::enqueue(const QJsonObject& obj)
{
    auto v = m_handler->encode(obj);
    if(v.isEmpty())
        return;
    m_device->write(v);
}

QByteArray Writer::encode(const QJsonObject& o, const EncodeType t)
{
    if(t == Json)
        return EncoderJson().encode(o);
    if(t == Cbor)
        return EncoderCbor().encode(o);
    return QByteArray();
}

} // namespace DPXComStream
