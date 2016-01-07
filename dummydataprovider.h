#ifndef DUMMYDATAPROVIDER_H
#define DUMMYDATAPROVIDER_H

#include "abstractprovider.h"
#include <QSettings>
class DummyDataProvider : public AbstractProvider
{
public:
    DummyDataProvider();

    // AbstractProvider interface
public:
    QString name() const Q_DECL_OVERRIDE;
    void setEnabled(bool en) Q_DECL_OVERRIDE;
    bool enabled() const Q_DECL_OVERRIDE;
    QList<PlaneInfo *> planes() const Q_DECL_OVERRIDE;
    QQuickItem *configItem() Q_DECL_OVERRIDE;
 private:
    bool _enabled;
    QSettings _settings;
    QHash<int,PlaneInfo*> _planes;
    void readData();
};

#endif // DUMMYDATAPROVIDER_H
