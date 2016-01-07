#include "planeinfo.h"

PlaneInfo::PlaneInfo(QString callSign, QGeoCoordinate coordinate, double heading)
{
    _callSign = callSign;
    _currentCoordinate = coordinate;
    _heading = heading;
    _speed = 0;
    _altitude = 0;
    _dataRequested = false;
    _additionalData = QVariantMap();
}

double PlaneInfo::heading() const
{
    return _heading;
}

void PlaneInfo::setHeading(double heading)
{
    _heading = heading;
    emit headingChanged();
}

QGeoCoordinate PlaneInfo::currentCoordinate() const
{
    return _currentCoordinate;
}

void PlaneInfo::setCurrentCoordinate(const QGeoCoordinate &currentCoordinate)
{
    _currentCoordinate = currentCoordinate;
    emit coordinateChanged();
}

QString PlaneInfo::callSign() const
{
    return _callSign;
}

int PlaneInfo::altitude() const
{
    return _altitude;
}

void PlaneInfo::setAltitude(int altitude)
{
    _altitude = altitude;
    emit altitudeChanged();
}

int PlaneInfo::speed() const
{
    return _speed;
}

void PlaneInfo::setSpeed(int speed)
{
    _speed = speed;
    emit speedChanged();
}

QVariantMap PlaneInfo::additionalData()
{
    if(!_dataRequested) {
        _dataRequested = true;
        emit additionalDataRequested();
    }
    return _additionalData;
}

void PlaneInfo::setAdditionalData(const QVariantMap &additionalData)
{
    _additionalData = additionalData;
    emit additionalDataChanged();
}


