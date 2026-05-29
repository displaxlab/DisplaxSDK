#include <QCoreApplication>
#include <sessionmanager.h>
#include "devicesettings.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    // Instantiate SessionManager to handle and manage services comunications.
    Displax::SessionManager sm;
    DeviceSettings settings;

    sm.connect(&sm, &Displax::SessionManager::sessionInserted, &settings,
               &DeviceSettings::sessionInserted);

    // Queue SessionManager to discovery local service and connect to it.
    sm.start();
    return a.exec();
}
