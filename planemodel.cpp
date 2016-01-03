#include "planemodel.h"

PlaneModel::PlaneModel()
{

}


int PlaneModel::rowCount(const QModelIndex &parent) const
{
    return _planes.count();
}

QVariant PlaneModel::data(const QModelIndex &index, int role) const
{
    if(role != Qt::DisplayRole || index.column()!=0 || index.row()<0 || index.row() >= _planes.count()) {
        return QVariant();
    }
    return QVariant::fromValue(_planes.at(index.row()));
}

void PlaneModel::addPlane(PlaneInfo *plane)
{
    beginInsertRows(QModelIndex(),_planes.count(),_planes.count());
    _planes.append(plane);
    endInsertRows();
}
