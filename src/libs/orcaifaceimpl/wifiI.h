#ifndef ORCA_WIFI_I_H
#define ORCA_WIFI_I_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/wifi.h>

// utilities
#include <orcaice/proxy.h>
#include <orcaice/context.h>

namespace orcaifaceimpl
{
    
//!
//! Implements the WifiI interface: Handles remote calls.
//!
class WifiI : virtual public ::orca::Wifi
{
public:

    WifiI( const std::string       &ifaceTag, 
           const orcaice::Context  &context );

    // remote functions

    virtual ::orca::WifiData getData(const Ice::Current&) const;

    virtual void subscribe(const ::orca::WifiConsumerPrx&,
               const Ice::Current&);

    virtual void unsubscribe(const ::orca::WifiConsumerPrx&,
                 const Ice::Current&);

    // local functions
    // may throw orcaice::Exceptions
    void initInterface();
    // A local call which sets the data reported by the interface, 
    // and sends it through IceStorm
    void localSetAndSend( const orca::WifiData &data );

private:

    orcaice::Proxy<orca::WifiData> dataProxy_;

    orca::WifiConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx       topicPrx_;

    const std::string        ifaceTag_;
    orcaice::Context         context_;
};

}

#endif
