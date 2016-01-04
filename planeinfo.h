#ifndef PLANEINFO_H
#define PLANEINFO_H

#include <QObject>
#include <QGeoCoordinate>

class PlaneInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate CurrentCoordinate MEMBER _currentCoordinate NOTIFY coordinateChanged())
    Q_PROPERTY(QString CallSign MEMBER _callSign NOTIFY callSignChanged())
    Q_PROPERTY(double Heading MEMBER _heading NOTIFY headingChanged)
public:
    explicit PlaneInfo(QString callSign, QGeoCoordinate coordinate, double heading);

    double heading() const;
    void setHeading(double heading);

    QGeoCoordinate currentCoordinate() const;
    void setCurrentCoordinate(const QGeoCoordinate &currentCoordinate);

    QString callSign() const;

private:
    QGeoCoordinate _currentCoordinate;
    QString _callSign;
    double _heading;

signals:
    void callSignChanged();
    void coordinateChanged();
    void headingChanged();

};

Q_DECLARE_METATYPE(PlaneInfo*)

#endif // PLANEINFO_H
