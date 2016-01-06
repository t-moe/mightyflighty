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
    connect(provider,SIGNAL(newPlane(PlaneInfo*)),this,SLOT(addPlane(PlaneInfo*)));
    connect(provider,SIGNAL(planeRemoved(PlaneInfo*)),this,SLOT(removePlane(PlaneInfo*)));
    //TODO: add already available planes to list?
    //TODO: add removeProvider Method?
}

QVariant PlaneModel::providers()
{
    return QVariant::fromValue(_providers); //QVariant needed, otherwise QObjectList wont be accepted as model?!
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
