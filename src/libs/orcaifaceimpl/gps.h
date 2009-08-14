/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_GPS_H
#define ORCAIFACEIMPL_GPS_H

#include <orca/gps.h>
#include <orcaifaceutil/gps.h>

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Gps, orca::GpsConsumer,
                    orca::GpsData, orca::GpsDescription> GpsImpl;
typedef IceUtil::Handle<GpsImpl> GpsImplPtr;

typedef StoringConsumerImpl<orca::Gps, orca::GpsConsumer,
                    orca::GpsData> StoringGpsConsumerImpl;
typedef IceUtil::Handle<StoringGpsConsumerImpl> StoringGpsConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Gps, orca::GpsConsumer,
                        orca::GpsData> BufferedGpsConsumerImpl;
typedef IceUtil::Handle<BufferedGpsConsumerImpl> BufferedGpsConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Gps, orca::GpsConsumer,
                        orca::GpsData> NotifyingGpsConsumerImpl;
typedef IceUtil::Handle<NotifyingGpsConsumerImpl> NotifyingGpsConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Gps, orca::GpsConsumer,
                        orca::GpsData> PrintingGpsConsumerImpl;
typedef IceUtil::Handle<PrintingGpsConsumerImpl> PrintingGpsConsumerImplPtr;

}

#endif
