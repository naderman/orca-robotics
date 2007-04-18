#ifndef GPSINTERFACES_H
#define GPSINTERFACES_H

#include "genericiface.h"
#include <orca/gps.h>

namespace gps {

    typedef GenericIface<orca::Gps,
                         orca::GpsData,
                         orca::GpsConsumerPrx,
                         orca::GpsDescription>          GpsIface;
    typedef IceUtil::Handle<GpsIface> GpsIfacePtr;

    typedef GenericIface<orca::GpsMapGrid,
                         orca::GpsMapGridData,
                         orca::GpsMapGridConsumerPrx,
                         orca::GpsDescription>          GpsMapGridIface;
    typedef IceUtil::Handle<GpsMapGridIface> GpsMapGridIfacePtr;

    typedef GenericIface<orca::GpsTime,
                         orca::GpsTimeData,
                         orca::GpsTimeConsumerPrx,
                         orca::GpsDescription> GpsTimeIface;
    typedef IceUtil::Handle<GpsTimeIface> GpsTimeIfacePtr;

}


#endif
