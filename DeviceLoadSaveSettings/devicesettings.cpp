#include "devicesettings.h"
#include <QCoreApplication>
#include <QJsonObject>
#include <QTextStream>

DeviceSettings::DeviceSettings(QObject* parent)
    : QObject(parent)
    , m_state(Begin)
{
    connect(&m_settings_handle, SIGNAL(finished()), this, SLOT(runStateOperation()));
    connect(&m_settings_handle, SIGNAL(downloadFinished(const QByteArray&)), this,
            SLOT(downloadFinished(const QByteArray&)));
}

void DeviceSettings::sessionInserted(Displax::Session* session)
{
    connect(session, SIGNAL(deviceInserted(Displax::Device*)), this,
            SLOT(deviceInserted(Displax::Device*)));
}

/*!
  Take first device and download settings
  */
void DeviceSettings::deviceInserted(Displax::Device* device)
{
    // We already have device context
    if(m_settings_handle.context())
        return;

    // Get settings proto entity
    auto proto = device->getProtoEntity("device_settings");
    if(proto.isEmpty())
        return;

    // Set proto entity and context
    m_settings_handle.updateProtoEntity(proto);
    m_settings_handle.setContext(device);

    // Proceed to download
    runStateOperation();
}

/*!
  Execute state machine.
  In this example download settings, set default settings and then upload the
  previous settings. At end the settings should be the same.
  */
void DeviceSettings::runStateOperation()
{
    QTextStream out(stdout);
    switch(m_state)
    {
        case Begin:
        {
            m_state = Download;
            m_settings_handle.download();
            out << "Downloading settings\n";
            break;
        }
        case Download:
        {
            m_state = SetDefault;
            m_settings_handle.setDefault();
            out << "Set default settings\n";
            break;
        }
        case SetDefault:
        {
            m_state = Upload;
            m_settings_handle.upload(m_data);
            out << "Uploading settings\n";
            break;
        }
        case Upload:
        {
            m_state = End;
            out << "Finished\n";
            QCoreApplication::quit();
            break;
        }
        case Fail:
        {
            out << "Operation fail\n";
            QCoreApplication::quit();
            break;
        }
        default:
        {
            out << "Undefined state\n";
            QCoreApplication::quit();
        }
    }
}

/*!
  Save settings data
  */
void DeviceSettings::downloadFinished(const QByteArray& data)
{
    m_data = data;
}

void DeviceSettings::fail()
{
    m_state = Fail;
    runStateOperation();
}
