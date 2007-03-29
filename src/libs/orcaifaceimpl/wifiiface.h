#ifndef ORCA_WIFI_IFACE_H
#define ORCA_WIFI_IFACE_H

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
//! Implements the WifiIface interface: Handles remote calls.
//!
class WifiIface : public IceUtil::Shared
{
public:

    WifiIface( const std::string       &ifaceTag, 
               const orcaice::Context  &context );
    ~WifiIface();

    // remote functions

    ::orca::WifiData getData() const;
    
    void subscribe(const ::orca::WifiConsumerPrx&);
    
    void unsubscribe(const ::orca::WifiConsumerPrx& );
    
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

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string        ifaceTag_;
    orcaice::Context         context_;
};
typedef IceUtil::Handle<WifiIface> WifiIfacePtr;

}

#endif
