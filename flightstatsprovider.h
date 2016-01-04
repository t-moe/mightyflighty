#ifndef FLIGHTSTATSPROVIDER_H
#define FLIGHTSTATSPROVIDER_H

#include <QObject>
#include <QHash>
#include "abstractprovider.h"


class FlightstatsProvider : public QObject, public AbstractProvider
{
    Q_OBJECT
public:
    explicit FlightstatsProvider(class QNetworkAccessManager* manager);



    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

    QList<class PlaneInfo*> planes() const;
    QObject *qobject() Q_DECL_OVERRIDE;

signals:
    void newPlane(class PlaneInfo* pi) Q_DECL_OVERRIDE;
    void planeRemoved(class PlaneInfo* pi) Q_DECL_OVERRIDE;

public slots:

private slots:
    void respFinished(class QNetworkReply* repl);

protected:
    void timerEvent(QTimerEvent *event);
private:
    QHash<QString,class PlaneInfo*> _planes;
    class QNetworkAccessManager* _manager;
    int _timerId;

    void makeSingleRequest();


};

#endif // FLIGHTSTATSPROVIDER_H
