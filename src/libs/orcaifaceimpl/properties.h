/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAIFACEIMPL_PROPERTIES_H
#define ORCAIFACEIMPL_PROPERTIES_H

#include <orcaifaceimpl/propertiesImpl.h>
#include <orcaifaceimpl/storingconsumerImpl.h>
#include <orcaifaceimpl/bufferedconsumerImpl.h>
#include <orcaifaceimpl/notifyingconsumerImpl.h>
#include <orcaifaceimpl/printingconsumerImpl.h>

#include <orca/properties.h>

namespace orcaifaceimpl {

typedef StoringConsumerImpl<orca::Properties, orca::PropertiesPrx,
                    orca::PropertiesConsumer, orca::PropertiesConsumerPrx,
                    orca::PropertiesData> StoringPropertiesConsumerImpl;
typedef IceUtil::Handle<StoringPropertiesConsumerImpl> StoringPropertiesConsumerImplPtr;

typedef BufferedConsumerImpl<orca::Properties, orca::PropertiesPrx,
                        orca::PropertiesConsumer, orca::PropertiesConsumerPrx,
                        orca::PropertiesData> BufferedPropertiesConsumerImpl;
typedef IceUtil::Handle<BufferedPropertiesConsumerImpl> BufferedPropertiesConsumerImplPtr;

typedef NotifyingConsumerImpl<orca::Properties, orca::PropertiesPrx,
                        orca::PropertiesConsumer,
                        orca::PropertiesConsumerPrx,
                        orca::PropertiesData> NotifyingPropertiesConsumerImpl;
typedef IceUtil::Handle<NotifyingPropertiesConsumerImpl> NotifyingPropertiesConsumerImplPtr;

typedef PrintingConsumerImpl<orca::Properties, orca::PropertiesPrx,
                        orca::PropertiesConsumer, orca::PropertiesConsumerPrx,
                        orca::PropertiesData> PrintingPropertiesConsumerImpl;
typedef IceUtil::Handle<PrintingPropertiesConsumerImpl> PrintingPropertiesConsumerImplPtr;

}

#endif
