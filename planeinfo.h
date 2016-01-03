#ifndef PLANEINFO_H
#define PLANEINFO_H

#include <QObject>
#include <QGeoCoordinate>

class PlaneInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate CurrentCoordinate MEMBER _currentCoordinate NOTIFY coordinateChanged())
    Q_PROPERTY(QString FlightNumber MEMBER _flightNumber NOTIFY flightNumberChanged())
public:
    explicit PlaneInfo(QString flightNumber, QGeoCoordinate coordinate);

private:
    QGeoCoordinate _currentCoordinate;
    QString _flightNumber;

signals:
    void flightNumberChanged();
    void coordinateChanged();

};

Q_DECLARE_METATYPE(PlaneInfo*)

#endif // PLANEINFO_H
