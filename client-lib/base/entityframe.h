#ifndef ENTITYFRAME_H
#define ENTITYFRAME_H

#include <QImage>
#include <QJsonObject>
#include <entity.h>

namespace Displax {

class EntityFrame: public Entity
{
    Q_OBJECT
    Q_PROPERTY(QImage value READ value NOTIFY valueChanged)
    Q_PROPERTY(QSize size READ size NOTIFY sizeChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

public:
    explicit EntityFrame(QObject* parent = nullptr);
    explicit EntityFrame(const QJsonObject& proto_entity, EntityContext* context, QObject* parent);

    QJsonValue toJson() const override;
    void fromJson(const QJsonValue& json) override;
    void cancelChange() override;

    QImage value() const;
    QSize size() const;

    QString type() const override;

    Q_INVOKABLE void updateProtoEntity(const QJsonObject& proto_entity);

signals:
    void valueChanged();
    void sizeChanged();
    void typeChanged();

private:
    QImage m_value;
    QString m_type;
};

} // namespace Displax

#endif // ENTITYFRAME_H
