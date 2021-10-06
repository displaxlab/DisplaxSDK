#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <dpxclientlib.h>
#include <QQuickStyle>
#include <frameviewer.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QQuickStyle::setStyle("Material");
    // Register Displax Types
    Displax::registerQmlTypes();
    qmlRegisterType<FrameViewer>("com.displax.DeviceFrameTouch", 1, 0, "FrameViewer");
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
