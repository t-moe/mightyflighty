#ifndef PLANEMODEL_H
#define PLANEMODEL_H

#include <QAbstractListModel>
#include "planeinfo.h"

class PlaneModel : public QAbstractListModel
{
public:
    PlaneModel();

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    void addPlane(PlaneInfo* plane);

private:
    QVector<PlaneInfo*> _planes;
};

#endif // PLANEMODEL_H
