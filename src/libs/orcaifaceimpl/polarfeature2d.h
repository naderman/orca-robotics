/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_POLARFEATURE2D_H
#define ORCAIFACEIMPL_POLARFEATURE2D_H

#include <orcaifaceimpl/providerWithDescriptionImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/polarfeature2d.h>

namespace orcaifaceimpl {

typedef ProviderWithDescriptionImpl<orca::PolarFeature2d, orca::PolarFeature2dConsumer,
                    orca::PolarFeature2dData, orca::PolarFeature2dDescription> PolarFeature2dImpl;
typedef IceUtil::Handle<PolarFeature2dImpl> PolarFeature2dImplPtr;

typedef StoringConsumerImpl<orca::PolarFeature2d, orca::PolarFeature2dConsumer,
                    orca::PolarFeature2dData> StoringPolarFeature2dConsumerImpl;
typedef IceUtil::Handle<StoringPolarFeature2dConsumerImpl> StoringPolarFeature2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::PolarFeature2d, orca::PolarFeature2dConsumer,
                        orca::PolarFeature2dData> BufferedPolarFeature2dConsumerImpl;
typedef IceUtil::Handle<BufferedPolarFeature2dConsumerImpl> BufferedPolarFeature2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::PolarFeature2d, orca::PolarFeature2dConsumer,
                        orca::PolarFeature2dData> NotifyingPolarFeature2dConsumerImpl;
typedef IceUtil::Handle<NotifyingPolarFeature2dConsumerImpl> NotifyingPolarFeature2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::PolarFeature2d, orca::PolarFeature2dConsumer,
                        orca::PolarFeature2dData> PrintingPolarFeature2dConsumerImpl;
typedef IceUtil::Handle<PrintingPolarFeature2dConsumerImpl> PrintingPolarFeature2dConsumerImplPtr;

}

#endif
