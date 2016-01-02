#include <QGuiApplication>
#include <QQmlApplicationEngine>

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

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
