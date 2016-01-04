#include "planeinfo.h"

PlaneInfo::PlaneInfo(QString callSign, QGeoCoordinate coordinate, double heading)
{
    _callSign = callSign;
    _currentCoordinate = coordinate;
    _heading = heading;
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

