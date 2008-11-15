#ifndef ORCA_PATHFOLLOWER2D_IMPL_H
#define ORCA_PATHFOLLOWER2D_IMPL_H

#include <memory>
#include <orca/pathfollower2d.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>
#include <orcaice/topichandler.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl
{

//!
//! @brief PathFollower2dImpl needs a pointer to a callback object implementing these.
//!
class AbstractPathFollowerCallback {
public:
    virtual ~AbstractPathFollowerCallback() {}

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
    PathFollower2dImpl( AbstractPathFollowerCallback    &callback,
                        const std::string       &interfaceTag, 
                        const orcaice::Context  &context );
    //! constructor using interfaceName
    PathFollower2dImpl( AbstractPathFollowerCallback    &callback,
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
    void init();

    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::PathFollower2dData internalGetData() const;
    IceStorm::TopicPrx internalSubscribe(const orca::PathFollower2dConsumerPrx& subscriber);

    gbxiceutilacfr::Store<orca::PathFollower2dData> dataStore_;

    typedef orcaice::TopicHandler<orca::PathFollower2dConsumerPrx,orca::PathFollower2dData> PathFollower2dTopicHandler;
    std::auto_ptr<PathFollower2dTopicHandler> topicHandler_;

    AbstractPathFollowerCallback &callback_;

    Ice::ObjectPtr ptr_;
    const std::string interfaceName_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<PathFollower2dImpl> PathFollower2dImplPtr;

}

#endif
