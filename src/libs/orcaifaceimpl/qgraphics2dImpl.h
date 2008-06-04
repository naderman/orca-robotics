#ifndef ORCA_QGRAPHICS2D_IMPL_H
#define ORCA_QGRAPHICS2D_IMPL_H

// include defnition of Ice runtime
#include <Ice/Ice.h>
#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/qgraphics2d.h>

// utilities
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxiceutilacfr { class Thread; }

namespace orcaifaceimpl
{
    
//!
//! Implements the QGraphics2dIface interface: Handles remote calls.
//!
class QGraphics2dImpl : public IceUtil::Shared
{
friend class QGraphics2dI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    QGraphics2dImpl( const std::string       &interfaceTag, 
                     const orcaice::Context  &context );
    //! constructor using interfaceName
    QGraphics2dImpl( const orcaice::Context  &context,
                     const std::string       &interfaceName );
    ~QGraphics2dImpl();
    
    // local functions
    //! may throw gbxutilacfr::Exceptions
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName="", int retryInterval=2 );

    //! A local call which sets the data reported by the interface, 
    //! and sends it through IceStorm
    void localSetAndSend( const orca::QGraphics2dData &data );

private:
    // remote call implementations, mimic (but do not inherit) the orca interface
    ::orca::QGraphics2dData internalGetData() const;
    void internalSubscribe(const ::orca::QGraphics2dConsumerPrx&);
    void internalUnsubscribe(const ::orca::QGraphics2dConsumerPrx& );

    gbxiceutilacfr::Store<orca::QGraphics2dData> dataStore_;

    orca::QGraphics2dConsumerPrx consumerPrx_;
    IceStorm::TopicPrx           topicPrx_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;

    const std::string        interfaceName_;
    const std::string        topicName_;
    orcaice::Context         context_;
};
typedef IceUtil::Handle<QGraphics2dImpl> QGraphics2dImplPtr;

}

#endif
