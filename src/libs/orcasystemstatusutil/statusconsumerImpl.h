/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCASYSTEMSTATUSUTIL_STATUS_CONSUMER_IMPL_H
#define ORCASYSTEMSTATUSUTIL_STATUS_CONSUMER_IMPL_H

#include <orcaice/context.h>
#include <orcaifaceimpl/consumerImpl.h>
#include <orca/systemstatus.h>
#include <IceUtil/Mutex.h>

namespace orcasystemstatusutil {

/*!
Generic implementation of StatusConsumer which receives updates from the component, analyses data but leaves
it up to the derived class to take action.

This implementation provides an event-driven interface. The derived class reimplements the *Event functions
which are called when someting happens.

@verbatim
Entry { enteredInactiveEvent( estimatedState ); }
Inactive
{
    received
    Reporting
    { receivedEvent( estimatedState ); }
}

Entry { enteredReportingEvent( estimatedState ); }
Reporting
{
    timedOut
    Stale
    { timedOutEvent( estimatedState ); }

    received
    nil
    { receivedEvent( estimatedState ); }
}

Entry { enteredStaleEvent( estimatedState ); }
Stale
{
    received
    Reporting
    { receivedEvent( estimatedState ); }

    gaveUp
    Inactive
    { gaveUpEvent( estimatedState ); }
}
@endverbatim
*/
class StatusConsumerImpl : 
    public orcaifaceimpl::ConsumerImpl<orca::Status,orca::StatusPrx,orca::StatusConsumer,orca::StatusConsumerPrx,orca::StatusData>
{
public: 

    struct Config
    {
        Config()
        {
             resubscribeTimeoutInactive=0;
             resubscribeIntervalInactive=15; 
             resubscribeTimeoutReporting=45;
             resubscribeIntervalReporting=5; 
             resubscribeTimeoutStale=0;
             resubscribeIntervalStale=10;
             staleTimeout=60;
             giveUpTimeout=-1;
        }
        // If we don't hear for longer than this timeout [sec], try to re-subscribe.
        // State dependent.
        int resubscribeTimeoutInactive;
        int resubscribeTimeoutReporting;
        int resubscribeTimeoutStale;
        // Don't try to resubscribe more frequently than this interval [sec]
        // State dependent.
        int resubscribeIntervalInactive;
        int resubscribeIntervalReporting;
        int resubscribeIntervalStale;
        // If we don't hear for longer than this timeout [sec], flag an Stale.
        int staleTimeout;
        // If we don't hear for longer than this timeout [sec], flag an Inactive.
        // TODO:
        // assert that if (giveUpTimeout>0), then (giveUpTimeout>staleTimeout)
        int giveUpTimeout;
    };

    //! Platform and component names are different from the ones in the context!
    StatusConsumerImpl( const orca::FQComponentName& fqCompName, 
                        const Config& config, 
                        const orcaice::Context& context );

    //! Contains remote calls. Thread-safe.
    void subscribe();

    //! This function must be called periodically to detect staleness of component status information.
    //! sends internal events if the status becomes stale.
    //!
    //! Contains remote calls. does not block until the next status update
    //! but can decide to resubscribe. Thread-safe.
    void checkStatus();


    // Polling interface

    //! Non-blocking local calls only. Simply returns the current estimate.
    //! Thread-safe. Do not call from event handlers!
    orca::EstimatedComponentStatus estimatedStatus() const;

    //! Returns the name of the component which is being monitored. 
    //! Thread-safe. Do not call from event handlers!
    orca::FQComponentName name() const;

//     bool isResubscribeRequested() const;

    // from orcaifaceimpl::ConsumerImpl
    // Not part of API of this class.
    //
    // sends internal events about status changes.
    virtual void dataEvent( const orca::StatusData& data );

protected:

    // Event-driven interface to be used by the derived classes

    // generated events

    //! This event handler is called every time status is received.
    virtual void statusRecievedEvent( const orca::FQComponentName& name, const orca::StatusData& ) {};

    //! This event handler is called every time status is received and estimated
    //! state has changed.
    virtual void estimateChangedEvent( const orca::EstimatedComponentStatus& ) {};

    //! This event handler is called every time a resubscription is needed.
    virtual void resubscribeRequestedEvent( const orca::EstimatedComponentStatus& ) {};

private: 
    // internal state machine transitions
    // not thread safe!
    void received();
    void timedOut();
    void gaveUp();

    // internal actions 
    void checkStatus_Inactive( int timeSinceLastReceiveSec, int timeSinceLastSubscribeSec );
    void checkStatus_Reporting( int timeSinceLastReceiveSec, int timeSinceLastSubscribeSec );
    void checkStatus_Stale( int timeSinceLastReceiveSec, int timeSinceLastSubscribeSec );

    // not using Store because we want read/write/evaluate acess to all this data to be atomic.
    IceUtil::Mutex  dataMutex_;
    orca::EstimatedComponentStatus estCompStatus_;
    // this is different from the time stamp in statusData_ because it's recorded with the local clock.
    IceUtil::Time lastRecieveTime_;
    IceUtil::Time lastSubscribeTime_;

    Config config_;
};
typedef IceUtil::Handle<StatusConsumerImpl> StatusConsumerImplPtr;

}

#endif
