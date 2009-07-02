/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "statusconsumerImpl.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaice/convertutils.h>
#include <orcaifaceutil/systemstatus.h>

using namespace std;
namespace orcasystemstatusutil {

StatusConsumerImpl::StatusConsumerImpl( const orca::FQComponentName& fqCompName, 
                        const Config& config, 
                        const orcaice::Context& context ) :
    orcaifaceimpl::ConsumerImpl<orca::Status,orca::StatusConsumer,orca::StatusData>(context),
    config_(config)
{
    estCompStatus_.name = fqCompName;
    estCompStatus_.estimatedState = orca::EstCompInactive;
    estCompStatus_.reportedStatus.clear();
}

void
StatusConsumerImpl::subscribe()
{
    // the base class will figure out that we want to talk to an Admin interface.
//     std::string proxyStr = string("status@")+config_.platformName+"/"+config_.componentName;
    string proxyStr = orcaice::toString( orcaice::toInterface( estCompStatus_.name, "status" ) );
    //
    // remote call
    //
    this->subscribeWithString( proxyStr );

    IceUtil::Mutex::Lock lock(dataMutex_);
    lastSubscribeTime_ = IceUtil::Time::now();
}

void
StatusConsumerImpl::dataEvent( const orca::StatusData& data ) 
{
    IceUtil::Mutex::Lock lock(dataMutex_);

    lastRecieveTime_ = IceUtil::Time::now();

    switch ( estCompStatus_.reportedStatus.size() )
    {
    case 0 :
        estCompStatus_.reportedStatus.push_back( data.compStatus );
        break;
    case 1 :
        estCompStatus_.reportedStatus[0] = data.compStatus;
        break;
    default :
        stringstream ss;
        ss << "Optional sequence of illegal size, should be 0 or 1, we have " << estCompStatus_.reportedStatus.size();
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // TODO: check the there's at least one subsystem

    // some clients may take special action when status is received
    statusRecievedEvent( estCompStatus_.name, data );

    // state machine action
    received();
}

void
StatusConsumerImpl::checkStatus()
{
    IceUtil::Mutex::Lock lock(dataMutex_);

    IceUtil::Time timeSinceLastReceive = IceUtil::Time::now() - lastRecieveTime_;
    IceUtil::Time timeSinceLastSubscribe = IceUtil::Time::now() - lastSubscribeTime_;

    switch ( estCompStatus_.estimatedState )
    {
    case orca::EstCompInactive : {
        checkStatus_Inactive( timeSinceLastReceive.toSeconds(), timeSinceLastSubscribe.toSeconds() );
        return;
    }
    case orca::EstCompReporting : {
        checkStatus_Reporting( timeSinceLastReceive.toSeconds(), timeSinceLastSubscribe.toSeconds() );
        return;
    }
    case orca::EstCompStale : {
        checkStatus_Stale( timeSinceLastReceive.toSeconds(), timeSinceLastSubscribe.toSeconds() );
        return;
    }
    }
}

void
StatusConsumerImpl::checkStatus_Inactive( int timeSinceLastReceiveSec, int timeSinceLastSubscribeSec )
{
    if ( timeSinceLastReceiveSec > config_.resubscribeTimeoutInactive &&
         timeSinceLastSubscribeSec > config_.resubscribeIntervalInactive )
    {
        stringstream ss;
        ss << "StatusConsumerImpl("<<orcaice::toString(estCompStatus_.name)<<"): Resubscribing.";
        context_.tracer().debug( ss.str() );

        // let the derive class do something useful
        resubscribeRequestedEvent( estCompStatus_ );
    }
}

void
StatusConsumerImpl::checkStatus_Reporting( int timeSinceLastReceiveSec, int timeSinceLastSubscribeSec )
{    
    if ( timeSinceLastReceiveSec > config_.staleTimeout )
    {
        // state machine
        timedOut();
        return;
    }

    if ( timeSinceLastReceiveSec > config_.resubscribeTimeoutReporting &&
         timeSinceLastSubscribeSec > config_.resubscribeIntervalReporting )
    {
        stringstream ss;
        ss << "StatusConsumerImpl("<<orcaice::toString(estCompStatus_.name)<<"): Resubscribing.";
        context_.tracer().debug( ss.str() );

        // let the derive class do something useful
        resubscribeRequestedEvent( estCompStatus_ );
    }
}

void
StatusConsumerImpl::checkStatus_Stale( int timeSinceLastReceiveSec, int timeSinceLastSubscribeSec )
{    
    if ( config_.giveUpTimeout > 0 && timeSinceLastReceiveSec > config_.giveUpTimeout )
    {
        // state machine
        gaveUp();
        return;
    }

    if ( timeSinceLastReceiveSec > config_.resubscribeTimeoutStale &&
         timeSinceLastSubscribeSec > config_.resubscribeIntervalStale )
    {
        stringstream ss;
        ss << "StatusConsumerImpl("<<orcaice::toString(estCompStatus_.name)<<"): Resubscribing.";
        context_.tracer().debug( ss.str() );

        // let the derive class do something useful
        resubscribeRequestedEvent( estCompStatus_ );
    }
}

orca::EstimatedComponentStatus 
StatusConsumerImpl::estimatedStatus() const
{
    IceUtil::Mutex::Lock lock(dataMutex_);

    return estCompStatus_;
}

orca::FQComponentName 
StatusConsumerImpl::name() const
{
    IceUtil::Mutex::Lock lock(dataMutex_);

    return estCompStatus_.name;
}

// state machine

void 
StatusConsumerImpl::received()
{
    if ( estCompStatus_.estimatedState != orca::EstCompReporting ) {
        estCompStatus_.estimatedState = orca::EstCompReporting;
    }

    // we don't check if the status has actually changed.
    // in the case of stable system we wouldn't save much by optimizing this out because
    // frequency of status updates is slow (30sec).
    estimateChangedEvent( estCompStatus_ );
}

void 
StatusConsumerImpl::timedOut()
{
    if ( estCompStatus_.estimatedState == orca::EstCompReporting ) {
        estCompStatus_.estimatedState = orca::EstCompStale;

        // state change event
        estimateChangedEvent( estCompStatus_ );

        return;
    }
}

void 
StatusConsumerImpl::gaveUp()
{
    if ( estCompStatus_.estimatedState == orca::EstCompStale ) {
        estCompStatus_.estimatedState = orca::EstCompInactive;

        // state change event
        estimateChangedEvent( estCompStatus_ );

        return;
    }

//     stringstream ss;
//     ss << "Internal state machine error: state=" << ifaceutil::toString(estCompStatus_.estimatedState)
//        << " action=gaveUp";
//     throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
}

} // namespace
