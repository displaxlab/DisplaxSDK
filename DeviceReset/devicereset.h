#ifndef DEVICERESET_H
#define DEVICERESET_H

#include <QHash>
#include <QObject>
#include <QTimer>
#include <session.h>

class DeviceReset: public QObject
{
    Q_OBJECT

public:
    enum Status
    {
        Marked,
        Restarting,
        Restarted
    };

    explicit DeviceReset(int timeout = 2000, QObject* parent = nullptr);

public slots:
    void sessionInserted(Displax::Session* session);
    void deviceInserted(Displax::Device* device);
    void deviceRemoved(Displax::Device* device);

private:
    QHash<QString, int> m_devices_already_reset;
};

#endif // DEVICERESET_H
