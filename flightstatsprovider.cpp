#include "flightstatsprovider.h"
#include <QNetworkRequest>
#include <QDebug>
#include <QGeoCoordinate>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include "planeinfo.h"

FlightstatsProvider::FlightstatsProvider(QNetworkAccessManager *manager)
{
    _manager = manager;
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(respFinished(QNetworkReply*)));
}

void FlightstatsProvider::makeSingleRequest()
{
    QGeoCoordinate center(46.8,8.1);

    //Get an api key from flighstats here: https://developer.flightstats.com/getting-started
    //(Free for limited use, evaluation)
    QString appId = "<APPID>";
    QString appKey = "<APPKEY>";
    float radius = 200; //maximum 200 according to docu


    //API-Doc see: https://developer.flightstats.com/api-docs/flightstatus/v2/flightsNear
    QString uri = "https://api.flightstats.com/flex/flightstatus/rest/v2/json/flightsNear/%1/%2/%3?appId=%4&appKey=%5";
    uri = uri.arg(center.latitude()).arg(center.longitude()).arg(radius);
    uri = uri.arg(appId,appKey);

   _manager->get(QNetworkRequest(QUrl(uri)));



}

void FlightstatsProvider::start()
{
    makeSingleRequest();
    _timerId = startTimer(60000); //re-request every 60s
}

void FlightstatsProvider::stop()
{
    killTimer(_timerId);
}

QList<PlaneInfo *> FlightstatsProvider::planes() const
{
    return _planes.values();
}

void FlightstatsProvider::respFinished(QNetworkReply* repl)
{
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
            QString flightId = flightPosition["flightId"].toString();
            double heading = flightPosition["heading"].toDouble();
            QVariantMap lastPos = flightPosition["positions"].toList().last().toMap();
            double lat = lastPos["lat"].toDouble();
            double lon = lastPos["lon"].toDouble();
            QGeoCoordinate cord(lat,lon);

            if(!_planes.contains(flightId)) {
                PlaneInfo* pi = new PlaneInfo(callSign,cord,heading);
                _planes[flightId] = pi;
                  qDebug() << "Plane added:"<< pi->callSign();
                emit newPlane(pi);
            } else {
                PlaneInfo* pi = _planes[flightId];
                oldPlanes.removeOne(pi);
                if(pi->heading() != heading) pi->setHeading(heading);
                if(pi->currentCoordinate() != cord) pi->setCurrentCoordinate(cord);
            }
        }

        QMutableHashIterator<QString, PlaneInfo*> it (_planes);

        while(it.hasNext()) {
            PlaneInfo* pi = it.next().value();
            if(oldPlanes.contains(pi)) {
                it.remove();
                qDebug() << "Plane removed:"<< pi->callSign();
                emit planeRemoved(pi);
            }
        }
    }
}

void FlightstatsProvider::timerEvent(QTimerEvent *event)
{
    QObject::timerEvent(event);
    makeSingleRequest();
}


QObject *FlightstatsProvider::qobject()
{
    return this;
}
