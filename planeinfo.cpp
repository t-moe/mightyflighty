#include "planeinfo.h"

PlaneInfo::PlaneInfo(QString flightNumber, QGeoCoordinate coordinate)
{
    _flightNumber = flightNumber;
    _currentCoordinate = coordinate;
}

