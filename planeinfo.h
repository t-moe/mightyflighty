#ifndef PLANEINFO_H
#define PLANEINFO_H

#include <QObject>
#include <QGeoCoordinate>
#include <QVariantMap>

class PlaneInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QGeoCoordinate CurrentCoordinate MEMBER _currentCoordinate NOTIFY coordinateChanged())
    Q_PROPERTY(QString CallSign MEMBER _callSign CONSTANT)
    Q_PROPERTY(double Heading MEMBER _heading NOTIFY headingChanged())
    Q_PROPERTY(int Altitude READ altitude WRITE setAltitude NOTIFY altitudeChanged())
    Q_PROPERTY(int Speed READ speed WRITE setSpeed NOTIFY speedChanged())
    Q_PROPERTY(QVariantMap AdditionalData READ additionalData WRITE setAdditionalData NOTIFY additionalDataChanged())
public:
    explicit PlaneInfo(QString callSign, QGeoCoordinate coordinate, double heading);

    double heading() const;
    void setHeading(double heading);

    QGeoCoordinate currentCoordinate() const;
    void setCurrentCoordinate(const QGeoCoordinate &currentCoordinate);

    QString callSign() const;

    int altitude() const;
    void setAltitude(int altitude);

    int speed() const;
    void setSpeed(int speed);


    QVariantMap additionalData();
    void setAdditionalData(const QVariantMap &additionalData);

    bool isAdditionalDataRequested() const;

private:
    bool _dataRequested;
    QVariantMap _additionalData;
    QGeoCoordinate _currentCoordinate;
    QString _callSign;
    double _heading;
    int _altitude;
    int _speed;

signals:
    void coordinateChanged();
    void headingChanged();
    void altitudeChanged();
    void speedChanged();
    void additionalDataChanged();
    void additionalDataRequested();
    void aboutToBeRemoved();

};

Q_DECLARE_METATYPE(PlaneInfo*)

#endif // PLANEINFO_H
