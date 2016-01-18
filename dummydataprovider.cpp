#include "dummydataprovider.h"
#include <QJsonDocument>
#include <QJsonObject>

DummyDataProvider::DummyDataProvider()
{
    setEnabled(_settings.value("DummyWasEnabled",false).toBool());
}


QString DummyDataProvider::name() const
{
    return "Dummy Data Provider";
}

void DummyDataProvider::setEnabled(bool en)
{
    if(en!=_enabled) {
        _enabled = en;
        _settings.setValue("DummyWasEnabled",en);
        if(en) {
            readData();
        } else {
            QMutableHashIterator<int, PlaneInfo*> it (_planes);
            while(it.hasNext()) {
                PlaneInfo* pi = it.next().value();
                pi->aboutToBeRemoved();
                it.remove();
                emit planeRemoved(pi);
            }
        }
        emit enabledChanged();
    }
}

bool DummyDataProvider::enabled() const
{
    return _enabled;
}

QList<PlaneInfo *> DummyDataProvider::planes() const
{
    return _planes.values();
}

QQuickItem *DummyDataProvider::configItem()
{
    return nullptr;
}

void DummyDataProvider::readData()
{
    QFile f(":/dummydata1.json");
    f.open(QFile::ReadOnly);
    QJsonDocument jsondoc = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonobj= jsondoc.object();
    QVariantMap variantmap =jsonobj.toVariantMap();
    QVariantList flightPositions = variantmap["flightPositions"].toList();
    for(int i=0; i<flightPositions.count();i++) {
        QVariantMap flightPosition = flightPositions.at(i).toMap();
        QString callSign = flightPosition["callsign"].toString();
        int flightId = flightPosition["flightId"].toInt();
        double heading = 180+flightPosition["heading"].toDouble();
        QVariantMap lastPos = flightPosition["positions"].toList().last().toMap();
        double lat = lastPos["lat"].toDouble();
        double lon = lastPos["lon"].toDouble();
        int speed = lastPos["speedMph"].toInt() * 1.60934;
        int alt = lastPos["altitudeFt"].toInt() * 0.3048;
        QGeoCoordinate cord(lat,lon);

        Q_ASSERT(!_planes.contains(flightId));

        PlaneInfo* pi = new PlaneInfo(callSign,cord,heading);
        pi->setAltitude(alt);
        pi->setSpeed(speed);
        _planes[flightId] = pi;
        qDebug() << "Plane added:"<< pi->callSign() << pi->currentCoordinate();
        emit newPlane(pi);
    }
}
