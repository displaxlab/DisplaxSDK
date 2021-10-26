#include "devicereset.h"
#include <QCoreApplication>
#include <QTextStream>
#include <entityfunction.h>

DeviceReset::DeviceReset(int timeout, QObject* parent)
    : QObject(parent)
{
    // After timeout, in milliseconds, quit application.
    QTimer::singleShot(timeout, QCoreApplication::instance(), SLOT(quit()));
}

void DeviceReset::sessionInserted(Displax::Session* session)
{
    connect(session, SIGNAL(deviceInserted(Displax::Device*)), this,
            SLOT(deviceInserted(Displax::Device*)));
    connect(session, SIGNAL(deviceRemoved(Displax::Device*)), this,
            SLOT(deviceRemoved(Displax::Device*)));
}

void DeviceReset::deviceInserted(Displax::Device* device)
{
    QTextStream out(stdout);
    // Device already processed?
    if(m_devices_already_reset.contains(device->serialNumber()))
    {
        m_devices_already_reset[device->serialNumber()] = Restarted;
        out << "Device " << device->serialNumber() << " restarted\n";
        return;
    }

    // Mark device to restart
    m_devices_already_reset[device->serialNumber()] = Marked;
    out << "Device " << device->serialNumber() << " marked to restart\n";

    // Find proto entity for command device reset
    QJsonArray proto = device->protoEntities();
    for(auto i = proto.begin(), e = proto.end(); i != e; ++i)
    {
        auto obj = i->toObject();
        auto type = obj.value("type").toString();
        auto name = obj.value("name").toString();
        if(type == "Function" && (name == "dreset" || name == "reset"))
        {
            // Build Entity Function device reset and execute
            // To create an entity we must indicate proto entity (obj) and context (device).
            Displax::EntityFunction func(obj, device, this);
            func.execute();
        }
    }
}

void DeviceReset::deviceRemoved(Displax::Device* device)
{
    if(m_devices_already_reset.contains(device->serialNumber()))
    {
        // The device is restarting
        QTextStream out(stdout);
        m_devices_already_reset[device->serialNumber()] = Restarting;
        out << "Device " << device->serialNumber() << " is restarting\n";
    }
}
