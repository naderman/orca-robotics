/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_LOCALISE3D_H
#define ORCAIFACEIMPL_LOCALISE3D_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/localise3d.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Localise3d, orca::Localise3dPrx,
                    orca::Localise3dConsumer, orca::Localise3dConsumerPrx,
                    orca::Localise3dData, orca::VehicleGeometryDescriptionPtr> Localise3dImpl;
typedef IceUtil::Handle<Localise3dImpl> Localise3dImplPtr;

typedef StoringConsumerImpl<orca::Localise3d, orca::Localise3dPrx,
                    orca::Localise3dConsumer, orca::Localise3dConsumerPrx,
                    orca::Localise3dData> StoringLocalise3dConsumerImpl;
typedef IceUtil::Handle<StoringLocalise3dConsumerImpl> StoringLocalise3dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Localise3d, orca::Localise3dPrx,
                        orca::Localise3dConsumer, orca::Localise3dConsumerPrx,
                        orca::Localise3dData> BufferedLocalise3dConsumerImpl;
typedef IceUtil::Handle<BufferedLocalise3dConsumerImpl> BufferedLocalise3dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Localise3d, orca::Localise3dPrx,
                        orca::Localise3dConsumer,
                        orca::Localise3dConsumerPrx,
                        orca::Localise3dData> NotifyingLocalise3dConsumerImpl;
typedef IceUtil::Handle<NotifyingLocalise3dConsumerImpl> NotifyingLocalise3dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Localise3d, orca::Localise3dPrx,
                        orca::Localise3dConsumer, orca::Localise3dConsumerPrx,
                        orca::Localise3dData> PrintingLocalise3dConsumerImpl;
typedef IceUtil::Handle<PrintingLocalise3dConsumerImpl> PrintingLocalise3dConsumerImplPtr;

}

#endif
