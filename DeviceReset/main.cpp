#include <QCoreApplication>
#include <devicereset.h>
#include <sessionmanager.h>

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    // Instantiate SessionManager to handle and manage services comunications.
    Displax::SessionManager sm;
    DeviceReset device_reset;
    // Link SessionManager to DeviceReset
    sm.connect(&sm, SIGNAL(sessionInserted(Displax::Session*)), &device_reset,
               SLOT(sessionInserted(Displax::Session*)));

    // Queue SessionManager to discovery local service and connect to it.
    sm.start();
    return a.exec();
}
