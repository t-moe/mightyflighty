#ifndef PLANEMODEL_H
#define PLANEMODEL_H

#include <QAbstractListModel>
#include "planeinfo.h"
#include "abstractprovider.h"

class PlaneModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QVariant providers READ providers)
public:
    PlaneModel();

public:
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    void addProvider(AbstractProvider *provider);
    QVariant providers();

public slots:
    void addPlane(PlaneInfo* plane);
    void removePlane(PlaneInfo* plane);

private:
    QVector<PlaneInfo*> _planes;
    QObjectList _providers;
};


#endif // PLANEMODEL_H
