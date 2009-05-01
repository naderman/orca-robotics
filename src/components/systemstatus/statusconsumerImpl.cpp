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

using namespace std;
namespace systemstatus {

StatusConsumerImpl::StatusConsumerImpl( const orca::FQComponentName& fqCompName, 
                        const Config& config, 
                        const orcaice::Context& context ) :
    orcasystemstatusutil::StatusConsumerImpl( fqCompName, config, context )
{
    // initialize the store, so that we have something to return right away

    // it's a bit cheating, we initialize to the same default value as the base class.
    // we cannot call the event function from the default constructor, because the derived
    // will not be initialized yet.
    orca::EstimatedComponentStatus status;
    status.name = fqCompName;
    status.estimatedState = orca::EstCompInactive;
    status.reportedStatus.clear();
    estCompStatus_.set( status );

    resubscribeRequest_.set( false );
}

orca::EstimatedComponentStatus 
StatusConsumerImpl::estimatedStatus() const
{
    orca::EstimatedComponentStatus status;
    estCompStatus_.get( status );
    return status;
}

bool 
StatusConsumerImpl::isResubscribeRequested( bool turnOffIfOn )
{
    bool isResubRequested;
    resubscribeRequest_.get( isResubRequested );

    // here we can miss a repeat request
    // due to non-atomic get and set.
    // it doesn't matter, we are going to subscribe anyway.
    if ( isResubRequested && turnOffIfOn )
        resubscribeRequest_.set( false );
    
    return isResubRequested;
}

void
StatusConsumerImpl::estimateChangedEvent( const orca::EstimatedComponentStatus& status )
{
    context_.tracer().debug( orcaice::toString(status.name) + " : status estimated changed." );
    estCompStatus_.set( status );
}

void
StatusConsumerImpl::resubscribeRequestedEvent( const orca::EstimatedComponentStatus& status )
{
    context_.tracer().debug( orcaice::toString(status.name) + " : resubscription requested." );
    resubscribeRequest_.set( true );
}

} // namespace
