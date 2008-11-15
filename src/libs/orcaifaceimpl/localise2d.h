/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_LOCALISE2D_H
#define ORCAIFACEIMPL_LOCALISE2D_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/localise2d.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::Localise2d, orca::Localise2dPrx,
                    orca::Localise2dConsumer, orca::Localise2dConsumerPrx,
                    orca::Localise2dData, orca::VehicleGeometryDescriptionPtr> Localise2dImpl;
typedef IceUtil::Handle<Localise2dImpl> Localise2dImplPtr;

typedef StoringConsumerImpl<orca::Localise2d, orca::Localise2dPrx,
                    orca::Localise2dConsumer, orca::Localise2dConsumerPrx,
                    orca::Localise2dData> StoringLocalise2dConsumerImpl;
typedef IceUtil::Handle<StoringLocalise2dConsumerImpl> StoringLocalise2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Localise2d, orca::Localise2dPrx,
                        orca::Localise2dConsumer, orca::Localise2dConsumerPrx,
                        orca::Localise2dData> BufferedLocalise2dConsumerImpl;
typedef IceUtil::Handle<BufferedLocalise2dConsumerImpl> BufferedLocalise2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Localise2d, orca::Localise2dPrx,
                        orca::Localise2dConsumer,
                        orca::Localise2dConsumerPrx,
                        orca::Localise2dData> NotifyingLocalise2dConsumerImpl;
typedef IceUtil::Handle<NotifyingLocalise2dConsumerImpl> NotifyingLocalise2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Localise2d, orca::Localise2dPrx,
                        orca::Localise2dConsumer, orca::Localise2dConsumerPrx,
                        orca::Localise2dData> PrintingLocalise2dConsumerImpl;
typedef IceUtil::Handle<PrintingLocalise2dConsumerImpl> PrintingLocalise2dConsumerImplPtr;

}

#endif
