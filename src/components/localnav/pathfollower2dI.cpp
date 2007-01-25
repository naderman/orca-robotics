/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "pathfollower2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace localnav {

PathFollower2dI::PathFollower2dI( const std::string      &ifaceTag,
                                  const Clock            &clock,
                                  const orcaice::Context &context ) 
    : clock_(clock),
      ifaceTag_(ifaceTag),
      context_(context)
{
    // We're inactive on initialization
    wpIndexProxy_.set( -1 );

    // But enabled
    enabledProxy_.set( true );
}

void
PathFollower2dI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    if ( !topicPrx_ )
    {
        topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
            ( context_, consumerPrx_, ifaceTag_ );
    }

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

orca::PathFollower2dData
PathFollower2dI::getData( const ::Ice::Current& ) const
{
    orca::PathFollower2dData data;
    if ( pathProxy_.isEmpty() )
    {
        throw orca::DataNotExistException( "No path has been set" );
    }
    pathProxy_.get( data );
    return data;
}

void
PathFollower2dI::setData( const ::orca::PathFollower2dData& data, bool activateImmediately, const ::Ice::Current& )
{
    // Sanity check
    std::string insanityReason;
    if ( !orcaice::isSane( data, insanityReason ) )
    {
        cout<<"TRACE(pathfollower2dI.cpp): Received dodgy path: " << orcaice::toString(data) << endl;
        throw orca::MalformedParametersException( insanityReason );
    }

    cout<<"TRACE(pathfollower2dI.cpp): Received new path: " << orcaice::toVerboseString(data) << endl;
    cout<<"TRACE(pathfollower2dI.cpp): activateImmediately: " << activateImmediately << endl;
    pathProxy_.set( data );
    newPathArrivedProxy_.set( true );
    if ( activateImmediately )
        activateNow();
}

double
PathFollower2dI::timeSinceActivate( const orca::Time &activationTime )
{
    double timeSince = orcaice::timeDiffAsDouble( clock_.time(), activationTime );
    return timeSince;
}

void
PathFollower2dI::activateNow( const ::Ice::Current& )
{
    // cout << "TRACE(pathfollower2dI.cpp): activateNow called" << endl;
    orca::Time now = clock_.time();
    activationTimeProxy_.set( now );
    activationArrivedProxy_.set( true );

    localSetWaypointIndex( 0 );

    // Try to push to IceStorm.
    try {
        consumerPrx_->setActivationTime( now,
                                         timeSinceActivate(now) );
    }
    catch ( Ice::Exception &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm." );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer()->print( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer()->print( "PathFollower2dI: Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setActivationTime( now,
                                             timeSinceActivate(now) );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer()->print( "PathFollower2dI: Re-connection to IceStorm failed." );
        }
    }
    catch ( ... )
    {
        context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
    }
}

int
PathFollower2dI::getWaypointIndex( const ::Ice::Current& ) const
{
    int ret;
    wpIndexProxy_.get( ret );
    return ret;
}

bool
PathFollower2dI::getAbsoluteActivationTime(::orca::Time &time, const Ice::Current&) const
{
    int wpIndex;
    wpIndexProxy_.get( wpIndex );
    if ( wpIndex != -1 )
    {
        activationTimeProxy_.get( time );
        return true;
    }
    else
        return false;
}
    
bool
PathFollower2dI::getRelativeActivationTime(double &secondsSinceActivation, const Ice::Current&) const
{
    int wpIndex;
    wpIndexProxy_.get( wpIndex );
    if ( wpIndex != -1 )
    {
        orca::Time timeActivated;
        activationTimeProxy_.get( timeActivated );
        secondsSinceActivation = orcaice::timeDiffAsDouble( clock_.time(), timeActivated );
        return true;
    }
    else
        return false;
}

void 
PathFollower2dI::setEnabled( bool enabled, const ::Ice::Current& )
{
    enabledProxy_.set( enabled );
    // let the consumers know that the enabled state changed
    consumerPrx_->setEnabledState( enabled );
}
bool 
PathFollower2dI::enabled(const ::Ice::Current&) const
{
    bool enabled;
    enabledProxy_.get( enabled );
    return enabled;
}

void
PathFollower2dI::subscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"subscribe()"<<endl;
    assert ( topicPrx_ != 0 );
    cout<<"TRACE(pathfollower2dI.cpp): topicPrx_: " << topicPrx_->ice_toString() << endl;
    IceStorm::QoS qos;
    qos["reliability"] = "twoway";
    topicPrx_->subscribe( qos, subscriber );
}

void
PathFollower2dI::unsubscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    assert ( topicPrx_ != 0 );
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}

void 
PathFollower2dI::localSetWaypointIndex( int index )
{
    wpIndexProxy_.set( index );
    // Try to push to IceStorm.

    try {
        consumerPrx_->setWaypointIndex( index );
    }
    catch ( Ice::Exception &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm." );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer()->print( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer()->print( "PathFollower2dI: Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setWaypointIndex( index );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer()->print( "PathFollower2dI: Re-connection to IceStorm failed." );
        }
    }
    catch ( ... )
    {
        context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
    }
}

// void
// PathFollower2dI::localSetActivationTime( const orca::Time &activationTime )
// {
// //     cout<<"TRACE(pathfollower2dI.cpp): activationTime: " << orcaice::toString(activationTime) << endl;

// //     activationTimeProxy_.set( activationTime );
//     // Try to push to IceStorm.
//     try {
//         consumerPrx_->setActivationTime( activationTime, 
//                                          orcaice::timeDiffAsDouble(orcaice::getNow(),activationTime) );
//     }
//     catch ( Ice::Exception &e )
//     {
//         // This could happen if IceStorm dies.
//         // If we're running in an IceBox and the IceBox is shutting down, 
//         // this is expected (our co-located IceStorm is obviously going down).
//         context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm." );

//         // If IceStorm just re-started for some reason though, we want to try to re-connect
//         try
//         {
//             context_.tracer()->print( "Re-connecting to IceStorm..." );
//             topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
//                 ( context_, consumerPrx_, ifaceTag_ );
//             context_.tracer()->print( "PathFollower2dI: Re-connected to IceStorm." );

//             // try again to push that bit of info
//             consumerPrx_->setActivationTime( activationTime, 
//                                              orcaice::timeDiffAsDouble(orcaice::getNow(),activationTime) );
//         }
//         catch ( ... )
//         {
//             // ignore it -- we'll try again next push.
//             context_.tracer()->print( "PathFollower2dI: Re-connection to IceStorm failed." );
//         }
//     }
//     catch ( ... )
//     {
//         context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
//     }
// }

void 
PathFollower2dI::localSetData( const orca::PathFollower2dData &path )
{
    pathProxy_.set( path );
    try {
        consumerPrx_->setData( path );
    }
    catch ( Ice::Exception &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm." );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer()->print( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer()->print( "PathFollower2dI: Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setData( path );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer()->print( "PathFollower2dI: Re-connection to IceStorm failed." );
        }
    }
    catch ( ... )
    {
        context_.tracer()->warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
    }
}

bool
PathFollower2dI::localIsEnabled() const
{
    bool enabled;
    enabledProxy_.get( enabled );
    return enabled;
}

}
