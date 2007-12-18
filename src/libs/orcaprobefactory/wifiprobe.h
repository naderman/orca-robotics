/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_WIFI_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_WIFI_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/wifi.h>

namespace orcaprobefactory
{

class WifiProbe : public orca::WifiConsumer, public orcaprobe::InterfaceProbe
{

public:

    WifiProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
               const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );

    // from consumer
    virtual void setData(const orca::WifiData& data, const Ice::Current&);
    
private:

    int loadGetData( orcacm::OperationData& data );
    int loadGetDescription( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    orca::WifiConsumerPrx callbackPrx_;
    orcacm::OperationData subscribeOperationData_;
};

} // namespace

#endif
