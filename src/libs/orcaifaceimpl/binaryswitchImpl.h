#ifndef ORCA_BINARYSWITCH_IMPL_H
#define ORCA_BINARYSWITCH_IMPL_H

#include <memory>
#include <orca/binaryswitch.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>
#include <orcaice/topichandler.h>

namespace gbxutilacfr { class Stoppable; }

namespace orcaifaceimpl
{

//!
//! @brief BinarySwitch needs a pointer to a callback object implementing these.
//!
class AbstractBinarySwitchCallback {
public:
    virtual ~AbstractBinarySwitchCallback() {}

    virtual void setData( const orca::BinarySwitchData &data )=0;
};

//!
//! Implements the BinarySwitchIface interface: Handles remote calls.
//!
class BinarySwitchImpl : public IceUtil::Shared
{
friend class BinarySwitchI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    BinarySwitchImpl( AbstractBinarySwitchCallback &callback,
                      const std::string            &interfaceTag, 
                      const orcaice::Context       &context );
    //! constructor using interfaceName
    BinarySwitchImpl( AbstractBinarySwitchCallback &callback,
                      const orcaice::Context       &context,
                      const std::string            &interfaceName );
    ~BinarySwitchImpl();
    
    // local functions
    //! may throw gbxutilacfr::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the activity (e.g. thread) was stopped.
    void initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface
    void localSet( const orca::BinarySwitchData& data )
    {
        dataStore_.set( data );
    }

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::BinarySwitchData &data );

private:
    void init();

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::BinarySwitchData internalGetData() const;
    IceStorm::TopicPrx internalSubscribe(const orca::BinarySwitchConsumerPrx& subscriber);

    gbxiceutilacfr::Store<orca::BinarySwitchData> dataStore_;

    typedef orcaice::TopicHandler<orca::BinarySwitchConsumerPrx,orca::BinarySwitchData> BinarySwitchTopicHandler;
    std::auto_ptr<BinarySwitchTopicHandler> topicHandler_;

    AbstractBinarySwitchCallback &callback_;

    Ice::ObjectPtr ptr_;
    const std::string interfaceName_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<BinarySwitchImpl> BinarySwitchImplPtr;

}

#endif
