#include <QGuiApplication>
#include <dpxclientlib.h>
#include <Qt3DQuickExtras/qt3dquickwindow.h>
#include <Qt3DQuick/QQmlAspectEngine>
#include <QQmlEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Quick::Qt3DQuickWindow view;

    // Register Displax Types
    Displax::registerQmlTypes();

    view.engine()->qmlEngine()->rootContext()->setContextProperty("_window", &view);
    view.setSource(QUrl("qrc:/main.qml"));
    view.show();

    return app.exec();
}
