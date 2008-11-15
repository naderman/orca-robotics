/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_FEATUREMAP2D_H
#define ORCAIFACEIMPL_FEATUREMAP2D_H

#include <orcaifaceimpl/providerImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/featuremap2d.h>

namespace orcaifaceimpl {

typedef ProviderImpl<orca::FeatureMap2d, orca::FeatureMap2dPrx,
                    orca::FeatureMap2dConsumer, orca::FeatureMap2dConsumerPrx,
                    orca::FeatureMap2dData> FeatureMap2dImpl;
typedef IceUtil::Handle<FeatureMap2dImpl> FeatureMap2dImplPtr;

typedef StoringConsumerImpl<orca::FeatureMap2d, orca::FeatureMap2dPrx,
                    orca::FeatureMap2dConsumer, orca::FeatureMap2dConsumerPrx,
                    orca::FeatureMap2dData> StoringFeatureMap2dConsumerImpl;
typedef IceUtil::Handle<StoringFeatureMap2dConsumerImpl> StoringFeatureMap2dConsumerImplPtr;

typedef BufferedConsumerImpl<orca::FeatureMap2d, orca::FeatureMap2dPrx,
                        orca::FeatureMap2dConsumer, orca::FeatureMap2dConsumerPrx,
                        orca::FeatureMap2dData> BufferedFeatureMap2dConsumerImpl;
typedef IceUtil::Handle<BufferedFeatureMap2dConsumerImpl> BufferedFeatureMap2dConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::FeatureMap2d, orca::FeatureMap2dPrx,
                        orca::FeatureMap2dConsumer,
                        orca::FeatureMap2dConsumerPrx,
                        orca::FeatureMap2dData> NotifyingFeatureMap2dConsumerImpl;
typedef IceUtil::Handle<NotifyingFeatureMap2dConsumerImpl> NotifyingFeatureMap2dConsumerImplPtr;

typedef PrintingConsumerImpl<orca::FeatureMap2d, orca::FeatureMap2dPrx,
                        orca::FeatureMap2dConsumer, orca::FeatureMap2dConsumerPrx,
                        orca::FeatureMap2dData> PrintingFeatureMap2dConsumerImpl;
typedef IceUtil::Handle<PrintingFeatureMap2dConsumerImpl> PrintingFeatureMap2dConsumerImplPtr;

}

#endif
