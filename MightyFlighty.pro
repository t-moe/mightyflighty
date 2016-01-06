TEMPLATE = app

QT += qml quick location
CONFIG += c++11

SOURCES +=  main.cpp \
            planemodel.cpp \
            planeinfo.cpp \
            flightstatsprovider.cpp \
            abstractprovider.cpp
HEADERS += \
            planemodel.h \
            planeinfo.h \
            flightstatsprovider.h \
            abstractprovider.h


RESOURCES += qml.qrc

linux-arm-gnueabihf-g++ {
    DEFINES+= IS_EMBEDDED=1
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)


