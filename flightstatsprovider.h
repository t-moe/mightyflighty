#ifndef FLIGHTSTATSPROVIDER_H
#define FLIGHTSTATSPROVIDER_H

#include <QObject>
#include <QHash>
#include "abstractprovider.h"
#include <QSettings>


class FlightstatsProvider : public AbstractProvider
{
    Q_OBJECT
    Q_PROPERTY(QString appKey READ appKey WRITE setAppKey NOTIFY appKeyChanged)
    Q_PROPERTY(QString appId READ appId WRITE setAppId NOTIFY appIdChanged)
public:
    explicit FlightstatsProvider(class QNetworkAccessManager* manager, class QQmlApplicationEngine* eng);

    QString name() const Q_DECL_OVERRIDE;

    void setEnabled(bool en) Q_DECL_OVERRIDE;
    bool enabled() const Q_DECL_OVERRIDE;

    class QQuickItem *configItem();
    QList<class PlaneInfo*> planes() const;

    QString appId() const;
    void setAppId(const QString &apiId);

    QString appKey() const;
    void setAppKey(const QString &appKey);


signals:
    void appKeyChanged();
    void appIdChanged();

public slots:

private slots:
    void respFinished(class QNetworkReply* repl);
    void additionalDataRequested();

protected:
    void timerEvent(QTimerEvent *event);
private:
    QSettings _settings;
    QString _appId;
    QString _appKey;
    QHash<int,PlaneInfo*> _planes;
    class QNetworkAccessManager* _manager;
    int _timerId;
    QQuickItem* _configItem;
    bool _enabled;

    void makeSingleRequest();
    void makeDetailsRequest(int flightnumber);


};

#endif // FLIGHTSTATSPROVIDER_H
