#include "flightstatsprovider.h"
#include <QNetworkRequest>
#include <QDebug>
#include <QGeoCoordinate>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QQmlApplicationEngine>
#include <QtQuick>
#include "planeinfo.h"

FlightstatsProvider::FlightstatsProvider(QNetworkAccessManager *manager, QQmlApplicationEngine* engine)
{
    _manager = manager;
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(respFinished(QNetworkReply*)));

    _timerId=-1;

    _appId = _settings.value("FlightstatsAppId").toString();
    _appKey = _settings.value("FlightstatsAppKey").toString();
    setEnabled(_settings.value("FlightstatsWasEnabled",false).toBool()); //will cause a request if enabled=true!


    QQmlComponent comp(engine);
    comp.loadUrl(QUrl("qrc:///flightstatsconf.qml"));
    _configItem =  qobject_cast<QQuickItem*>(comp.create());
    _configItem->childItems().first()->setProperty("model",QVariant::fromValue(this));
}

QString FlightstatsProvider::name() const
{
    return "FlightStats.com API";
}

void FlightstatsProvider::setEnabled(bool en)
{
    if(en!=_enabled) {
        _enabled = en;
        _settings.setValue("FlightstatsWasEnabled",en);
        if(en) {
            makeSingleRequest();
            _timerId = startTimer(60000); //re-request every 60s
        } else {
            if(_timerId!=-1) {
                killTimer(_timerId);
                _timerId = -1;
            }
            QMutableHashIterator<int, PlaneInfo*> it (_planes);
            while(it.hasNext()) {
                PlaneInfo* pi = it.next().value();
                it.remove();
                emit planeRemoved(pi);
            }
        }
        emit enabledChanged();
    }
}

bool FlightstatsProvider::enabled() const
{
    return _enabled;
}

void FlightstatsProvider::makeSingleRequest()
{
    QGeoCoordinate center(46.8,8.1); //Switzerland
    //QGeoCoordinate center(40,-74); //New York (where they have routes with waypoints)
    float radius = 200; //maximum 200 according to docu


    //API-Doc see: https://developer.flightstats.com/api-docs/flightstatus/v2/flightsNear
    QString uri = "https://api.flightstats.com/flex/flightstatus/rest/v2/json/flightsNear/%1/%2/%3?appId=%4&appKey=%5&extendedOptions=useHttpErrors";
    uri = uri.arg(center.latitude()).arg(center.longitude()).arg(radius);
    uri = uri.arg(_appId,_appKey);

   _manager->get(QNetworkRequest(QUrl(uri)));

}

void FlightstatsProvider::makeDetailsRequest(int flightnumber) {
    //API-Doc see: https://developer.flightstats.com/api-docs/flightstatus/v2/flighttrackresponse
    QString uri = "https://api.flightstats.com/flex/flightstatus/rest/v2/json/flight/track/%1?appId=%2&appKey=%3&includeFlightPlan=true&maxPositions=1&extendedOptions=useInlinedReferences";
    uri = uri.arg(flightnumber);
    uri = uri.arg(_appId,_appKey);
    _manager->get(QNetworkRequest(QUrl(uri)));
}

QQuickItem *FlightstatsProvider::configItem()
{
    return _configItem;
}

QList<PlaneInfo *> FlightstatsProvider::planes() const
{
    return _planes.values();
}

void FlightstatsProvider::respFinished(QNetworkReply* repl)
{
    if(repl->error()!=QNetworkReply::NoError) {
        qDebug() << repl->errorString() << QString(repl->readAll());
    } else {
        if(repl->url().path().contains("flightsNear")) {
            QJsonParseError err;
            QJsonDocument jsondoc = QJsonDocument::fromJson(repl->readAll(),&err);
            if(err.error == QJsonParseError::NoError) {
                QJsonObject jsonobj= jsondoc.object();
                QVariantMap variantmap =jsonobj.toVariantMap();
                QVariantList flightPositions = variantmap["flightPositions"].toList();

                QList<PlaneInfo*> oldPlanes = planes();

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

                    if(!_planes.contains(flightId)) {
                        PlaneInfo* pi = new PlaneInfo(callSign,cord,heading);
                        pi->setAltitude(alt);
                        pi->setSpeed(speed);
                        connect(pi,SIGNAL(additionalDataRequested()),this,SLOT(additionalDataRequested()));
                        _planes[flightId] = pi;
                          qDebug() << "Plane added:" << pi->callSign() << pi->currentCoordinate();
                        emit newPlane(pi);
                    } else {
                        PlaneInfo* pi = _planes[flightId];
                        oldPlanes.removeOne(pi);
                        if(pi->heading() != heading) pi->setHeading(heading);
                        if(pi->currentCoordinate() != cord) pi->setCurrentCoordinate(cord);
                        if(pi->altitude() != alt) pi->setAltitude(alt);
                        if(pi->speed() != speed) pi->setSpeed(speed);
                    }
                }

                QMutableHashIterator<int, PlaneInfo*> it (_planes);

                while(it.hasNext()) {
                    PlaneInfo* pi = it.next().value();
                    if(oldPlanes.contains(pi)) {
                        it.remove();
                        qDebug() << "Plane removed:"<< pi->callSign();
                        emit planeRemoved(pi);
                        delete pi;
                    }
                }
            }
        } else {
            QJsonParseError err;
            QJsonDocument jsondoc = QJsonDocument::fromJson(repl->readAll(),&err);
            if(err.error == QJsonParseError::NoError) {
                QJsonObject jsonobj= jsondoc.object();
                QVariantMap variantmap =jsonobj.toVariantMap()["flightTrack"].toMap();
                int flightId = variantmap["flightId"].toInt();
                if(_planes.contains(flightId)) {
                    QVariantMap result;
                    QVariantMap dep = variantmap["departureAirport"].toMap();
                    QVariantMap arr = variantmap["arrivalAirport"].toMap();
                    if(!dep.isEmpty()) result["departureAirport"] = QString("%1 (%2)").arg(dep["fs"].toString(),dep["name"].toString());
                    if(!arr.isEmpty()) result["arrivalAirport"] = QString("%1 (%2)").arg(arr["fs"].toString(),arr["name"].toString());

                    QVariantList waypoints = variantmap["waypoints"].toList();
                    if(!waypoints.isEmpty()) {
                        QVariantList pts;
                        for(int i=0; i<waypoints.length(); i++) {
                            QVariantMap waypoint = waypoints[i].toMap();
                            pts << QVariant::fromValue(QGeoCoordinate(waypoint["lat"].toDouble(),waypoint["lon"].toDouble()));
                        }
                        result["route"] = QVariant::fromValue(pts);
                    }

                    _planes[flightId]->setAdditionalData(result);
                }
            }
        }
    }
}

void FlightstatsProvider::additionalDataRequested()
{

    QHashIterator<int, PlaneInfo*> it (_planes);
    while(it.hasNext()) {
        it.next();
        if(sender()==it.value()) {
            makeDetailsRequest(it.key());
            break;
        }
    }
}

void FlightstatsProvider::timerEvent(QTimerEvent *event)
{
    QObject::timerEvent(event);
    makeSingleRequest();
}

QString FlightstatsProvider::appKey() const
{
    return _appKey;
}

void FlightstatsProvider::setAppKey(const QString &appKey)
{
    if(_appKey==appKey) return;
    _appKey = appKey;
    _settings.setValue("FlightstatsAppKey",appKey);
    emit appKeyChanged();
}

QString FlightstatsProvider::appId() const
{
    return _appId;
}

void FlightstatsProvider::setAppId(const QString &appId)
{
     if(_appId==appId) return;
    _appId = appId;
    _settings.setValue("FlightstatsAppId",appId);
    emit appIdChanged();
}
