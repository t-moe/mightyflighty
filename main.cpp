#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "planemodel.h"

int main(int argc, char *argv[])
{
    #ifdef IS_EMBEDDED
    //Set platform to elgs if no other platform is specified via command line arguments
    bool platformOverride=false;
    for(int i=0; i<argc; i++) {
        if(QString(argv[i]).contains("-platform")) {
            platformOverride= true;
            break;
        }
    }

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wwrite-strings"
    if(!platformOverride) {

        argv[argc++] = "-platform";
        argv[argc++] = "eglfs";
    }
    #pragma GCC diagnostic pop
    #endif


    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    #ifdef IS_EMBEDDED
    //Add correct qml import path for embedded bbb
    engine.addImportPath("/usr/local/Qt5.5.1/qml/");
    #endif


    PlaneModel planes;
    planes.addPlane(new PlaneInfo("LX1234",QGeoCoordinate(46.9108846,7.4955513)));
    planes.addPlane(new PlaneInfo("EX2836",QGeoCoordinate(47.4501756,8.5612913)));
    engine.rootContext()->setContextProperty("planeModel",&planes);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
