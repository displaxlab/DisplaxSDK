#include "main.h"
#include <QCoreApplication>
#include <QTextStream>

/*!
  This example show a very basic connection to local DisplaxService (service).
  First, instantiate SessionManager to handle and manage services comunications.
  PrinfInfo, linked to SessionManager, will print info about local service and devices if exists.
  */

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    Displax::SessionManager sm;
    PrintInfo pf;

    // Link SessionManager to PrintInfo
    sm.connect(&sm, SIGNAL(sessionInserted(Displax::Session*)), &pf, SLOT(sessionInserted(Displax::Session*)));

    // Queue SessionManager to discovery local service and connect to it.
    sm.start();
    pf.start();

    return a.exec();
}

/*!
 Configure to timeout connection to service.
 */
void PrintInfo::start()
{
    m_message = "No service found";
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer.start(2000);
}

void PrintInfo::timeout()
{
    QTextStream out(stdout);
    out << m_message << "\n";
    QCoreApplication::quit();
}

/*!
  Local service found and connected.
  Wait for devices.
  */
void PrintInfo::sessionInserted(Displax::Session* s)
{
    m_timer.stop();
    m_session = s;
    connect(s, SIGNAL(devicesChanged()), this, SLOT(devicesChanged()));
    QTextStream out(stdout);
    out << "Local session found\n";
    m_message = "No device found";
    m_timer.start(200);
}

/*
  Devices found. Print: product id, product name and serial number for each device.
  */
void PrintInfo::devicesChanged()
{
    QTextStream out(stdout);
    const auto devices = m_session->devices();
    for(auto i = devices->constBegin(), e = devices->constEnd(); i != e; ++i)
        out << i.value()->productId() << " " << i.value()->productName() << " "
            << i.value()->serialNumber() << "\n";

    QCoreApplication::quit();
}
