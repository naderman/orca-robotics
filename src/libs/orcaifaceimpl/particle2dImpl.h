#ifndef ORCA_PARTICLE2D_IMPL_H
#define ORCA_PARTICLE2D_IMPL_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/particle2d.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxsickacfr { namespace gbxiceutilacfr { class Thread; } }

namespace orcaifaceimpl
{
    
//!
//! Implements the Particle2dIface interface: Handles remote calls.
//!
class Particle2dImpl : public IceUtil::Shared
{
friend class Particle2dI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    Particle2dImpl( const std::string       &interfaceTag, 
                    const orcaice::Context  &context );
    //! constructor using interfaceName
    Particle2dImpl( const orcaice::Context  &context,
                    const std::string       &interfaceName );
    ~Particle2dImpl();
    
    // local functions
    //! may throw gbxsickacfr::gbxutilacfr::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxsickacfr::gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::Particle2dData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::Particle2dData internalGetData() const;
    void internalSubscribe(const ::orca::Particle2dConsumerPrx&);
    void internalUnsubscribe(const ::orca::Particle2dConsumerPrx& );

    gbxsickacfr::gbxiceutilacfr::Store<orca::Particle2dData> dataStore_;

    orca::Particle2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx       topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string        interfaceName_;
    const std::string        topicName_;
    orcaice::Context         context_;
};
typedef IceUtil::Handle<Particle2dImpl> Particle2dImplPtr;

}

#endif
