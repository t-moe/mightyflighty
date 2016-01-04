#ifndef ABSTRACTPROVIDER_H
#define ABSTRACTPROVIDER_H

#include "planeinfo.h"

class AbstractProvider {
 public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual class QObject* qobject() =0;
    virtual QList<PlaneInfo*> planes() const =0;

public: /* Signal Section */
    virtual void newPlane(PlaneInfo* pi) = 0;
    virtual void planeRemoved(PlaneInfo* pi) = 0;



};


#endif // ABSTRACTPROVIDER_H
