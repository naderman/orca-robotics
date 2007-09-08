#ifndef GPSINTERFACES_H
#define GPSINTERFACES_H

#include "genericImpl.h"
#include <orca/gps.h>

namespace gps {

    typedef GenericImpl<orca::Gps,
                         orca::GpsData,
                         orca::GpsConsumerPrx,
                         orca::GpsDescription>          GpsImpl;
    typedef IceUtil::Handle<GpsImpl> GpsImplPtr;

    typedef GenericImpl<orca::GpsMapGrid,
                         orca::GpsMapGridData,
                         orca::GpsMapGridConsumerPrx,
                         orca::GpsDescription>          GpsMapGridImpl;
    typedef IceUtil::Handle<GpsMapGridImpl> GpsMapGridImplPtr;

    typedef GenericImpl<orca::GpsTime,
                         orca::GpsTimeData,
                         orca::GpsTimeConsumerPrx,
                         orca::GpsDescription> GpsTimeImpl;
    typedef IceUtil::Handle<GpsTimeImpl> GpsTimeImplPtr;

}


#endif
