#ifndef MAIN_H
#define MAIN_H

#include <QObject>
#include <QTimer>
#include <sessionmanager.h>

class PrintInfo: public QObject
{
    Q_OBJECT

public:
    PrintInfo() = default;
    virtual ~PrintInfo() = default;

    void start();

public slots:
    void timeout();
    void sessionInserted(Displax::Session* s);
    void devicesChanged();

private:
    QTimer m_timer;
    QString m_message;
    Displax::Session* m_session = nullptr;
};

#endif // MAIN_H
