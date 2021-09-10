#ifndef DEVICESETTINGS_H
#define DEVICESETTINGS_H

#include <QObject>
#include <QTimer>
#include <entitysettings.h>
#include <session.h>

class DeviceSettings: public QObject
{
    Q_OBJECT

    enum State
    {
        Begin,
        Download,
        SetDefault,
        Upload,
        End,
        Fail
    };

public:
    explicit DeviceSettings(QObject* parent = nullptr);

public slots:
    void sessionInserted(Displax::Session* session);

private slots:
    void deviceInserted(Displax::Device* device);
    void runStateOperation();
    void downloadFinished(const QByteArray& data);
    void fail();

private:
    State m_state;
    QByteArray m_data;
    Displax::EntitySettings m_settings_handle;
};

#endif // DEVICESETTINGS_H
