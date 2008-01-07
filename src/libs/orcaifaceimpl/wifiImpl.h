#ifndef ORCA_WIFI_IMPL_H
#define ORCA_WIFI_IMPL_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/wifi.h>

// utilities
#include <hydroiceutil/store.h>
#include <orcaice/context.h>

namespace orcaifaceimpl
{
    
//!
//! Implements the WifiIface interface: Handles remote calls.
//!
class WifiImpl : public IceUtil::Shared
{
friend class WifiI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    WifiImpl( const std::string       &interfaceTag, 
              const orcaice::Context  &context );
    //! constructor using interfaceName
    WifiImpl( const orcaice::Context  &context,
              const std::string       &interfaceName );
    ~WifiImpl();
    
    // local functions
    //! may throw hydroutil::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroiceutil::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::WifiData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::WifiData internalGetData() const;
    void internalSubscribe(const ::orca::WifiConsumerPrx&);
    void internalUnsubscribe(const ::orca::WifiConsumerPrx& );

    hydroiceutil::Store<orca::WifiData> dataStore_;

    orca::WifiConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx       topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string        interfaceName_;
    const std::string        topicName_;
    orcaice::Context         context_;
};
typedef IceUtil::Handle<WifiImpl> WifiImplPtr;

}

#endif
