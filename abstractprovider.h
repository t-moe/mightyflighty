#ifndef ABSTRACTPROVIDER_H
#define ABSTRACTPROVIDER_H

#include "planeinfo.h"
#include <QQuickItem>


class AbstractProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QQuickItem* configItem READ configItem)
 public:
    virtual QString name() const = 0;
    virtual void setEnabled(bool en) =0;
    virtual bool enabled() const = 0;
    virtual QList<PlaneInfo*> planes() const =0;
    virtual QQuickItem* configItem() = 0;

signals: /* Signal Section */
     void newPlane(PlaneInfo* pi);
     void planeRemoved(PlaneInfo* pi);
     void enabledChanged();

};


#endif // ABSTRACTPROVIDER_H
