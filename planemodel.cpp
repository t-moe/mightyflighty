#include "planemodel.h"


PlaneModel::PlaneModel()
{

}


int PlaneModel::rowCount(const QModelIndex &) const
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

void PlaneModel::addProvider(AbstractProvider *provider)
{
    _providers.append(provider);
    connect(provider->qobject(),SIGNAL(newPlane(PlaneInfo*)),this,SLOT(addPlane(PlaneInfo*)));
    connect(provider->qobject(),SIGNAL(planeRemoved(PlaneInfo*)),this,SLOT(removePlane(PlaneInfo*)));
    provider->start();
}

void PlaneModel::addPlane(PlaneInfo *plane)
{
    beginInsertRows(QModelIndex(),_planes.count(),_planes.count());
    _planes.append(plane);
    endInsertRows();
}

void PlaneModel::removePlane(PlaneInfo *plane)
{
    for(int i=0; i<_planes.count();i++) {
        if(_planes[i]==plane) {
            beginRemoveRows(QModelIndex(),i,i);
            _planes.removeAt(i);
            endRemoveRows();
            return;
        }
    }
}
