#ifndef ORCA_PATHFOLLOWER2D_IMPL_H
#define ORCA_PATHFOLLOWER2D_IMPL_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/pathfollower2d.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl
{

class PathFollowerCallback {
public:
    virtual ~PathFollowerCallback() {}

    virtual void setData( const orca::PathFollower2dData &path, bool activateImmediately )=0;
    virtual void activateNow()=0;
    virtual int  getWaypointIndex() const=0;
    virtual bool getAbsoluteActivationTime( orca::Time &activationTime ) const=0;
    virtual bool getRelativeActivationTime( double &secondsSinceActivation ) const=0;
    virtual void setEnabled( bool enabled )=0;
    virtual bool enabled() const=0;
};

//!
//! Implements the PathFollower2dIface interface: Handles remote calls.
//!
class PathFollower2dImpl : public IceUtil::Shared
{
friend class PathFollower2dI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    PathFollower2dImpl( PathFollowerCallback    &callback,
                        const std::string       &interfaceTag, 
                        const orcaice::Context  &context );
    //! constructor using interfaceName
    PathFollower2dImpl( PathFollowerCallback    &callback,
                        const orcaice::Context  &context,
                        const std::string       &interfaceName );
    ~PathFollower2dImpl();
    
    // local functions
    //! may throw gbxutilacfr::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::PathFollower2dData &data );

    // Local calls to the Impl which get translated into remote calls on subscribers
    void localSetWaypointIndex( int index );
    void localSetActivationTime( orca::Time absoluteTime, double relativeTime );
    void localSetEnabledState( bool enabledState );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PathFollower2dData internalGetData() const;
    void internalSubscribe(const ::orca::PathFollower2dConsumerPrx&);
    void internalUnsubscribe(const ::orca::PathFollower2dConsumerPrx& );

    gbxiceutilacfr::Store<orca::PathFollower2dData> dataStore_;

    orca::PathFollower2dConsumerPrx    consumerPrx_;
    IceStorm::TopicPrx       topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string        interfaceName_;
    const std::string        topicName_;

    PathFollowerCallback &callback_;

    orcaice::Context         context_;
};
typedef IceUtil::Handle<PathFollower2dImpl> PathFollower2dImplPtr;

}

#endif
