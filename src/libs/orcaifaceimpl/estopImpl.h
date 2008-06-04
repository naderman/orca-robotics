#ifndef ORCA_ESTOP_IMPL_H
#define ORCA_ESTOP_IMPL_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/estop.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl
{

//!
//! @brief EStopImpl needs a pointer to a class implementing these.
//!
class EStopNonStandardImpl {
public:
    virtual ~EStopNonStandardImpl() {}

    virtual void activateEStop()=0;
    virtual void keepAlive()=0;
    virtual double getRequiredKeepAlivePeriodSec()=0;
    virtual void setToOperatingMode()=0;
};
    
//!
//! Implements the EStopIface interface: Handles remote calls.
//!
class EStopImpl : public IceUtil::Shared
{
friend class EStopI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    EStopImpl( EStopNonStandardImpl   &eStopNonStandardImpl,
               const std::string      &interfaceTag, 
               const orcaice::Context &context );
    //! constructor using interfaceName
    EStopImpl( EStopNonStandardImpl   &eStopNonStandardImpl,
               const orcaice::Context &context,
               const std::string      &interfaceName );
    ~EStopImpl();
    
    // local functions
    //! may throw gbxutilacfr::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::EStopData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::EStopData internalGetData() const;
    void internalSubscribe(const ::orca::EStopConsumerPrx&);
    void internalUnsubscribe(const ::orca::EStopConsumerPrx& );

    gbxiceutilacfr::Store<orca::EStopData> dataStore_;

    orca::EStopConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx       topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string        interfaceName_;
    const std::string        topicName_;

    EStopNonStandardImpl &eStopNonStandardImpl_;
    
    orcaice::Context         context_;
};
typedef IceUtil::Handle<EStopImpl> EStopImplPtr;

}

#endif
