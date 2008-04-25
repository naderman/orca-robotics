/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include "pathfollower2dI.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace localnav {

namespace {
    
    bool isDodgy( const orca::PathFollower2dData &pathData, std::string &reason )
    {
        std::stringstream ss;
        bool dodgy=false;
        for ( unsigned int i=0; i < pathData.path.size(); i++ )
        {
            const orca::Waypoint2d &wp = pathData.path[i];

            if ( wp.distanceTolerance < 1e-3 )
            {
                ss << "Waypoint " << i << ": tiny distance tolerance: " 
                   << wp.distanceTolerance << "m" << endl;
                dodgy = true;
            }
            if ( wp.headingTolerance < 1e-3 )
            {
                ss << "Waypoint " << i << ": tiny heading tolerance: " 
                   << wp.headingTolerance*180.0/M_PI << "deg" << endl;
                dodgy = true;
            }
            if ( wp.maxApproachSpeed < 1e-3 )
            {
                ss << "Waypoint " << i << ": tiny maxApproachSpeed: " 
                   << wp.maxApproachSpeed << "m/s" << endl;
                dodgy = true;
            }
            if ( wp.maxApproachTurnrate < 1e-3 )
            {
                ss << "Waypoint " << i << ": tiny maxApproachTurnrate: " 
                   << wp.maxApproachTurnrate*180.0/M_PI << "deg/s" << endl;
                dodgy = true;
            }
        }
        reason = ss.str();
        return dodgy;
    }

}

PathFollower2dI::PathFollower2dI( const std::string      &ifaceTag,
                                  const Clock            &clock,
                                  const orcaice::Context &context ) 
    : clock_(clock),
      ifaceTag_(ifaceTag),
      context_(context)
{
    // We're inactive on initialization
    wpIndexStore_.set( -1 );

    // But enabled
    enabledStore_.set( true );
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
    if ( pathStore_.isEmpty() )
    {
        throw orca::DataNotExistException( "No path has been set" );
    }
    pathStore_.get( data );
    return data;
}

void
PathFollower2dI::setData( const ::orca::PathFollower2dData& data, bool activateImmediately, const ::Ice::Current& )
{
    // Sanity check
    std::string reason;
    if ( !orcaobj::isSane( data, reason ) )
    {
        stringstream ss;
        ss << "PathFollower2dI: Received screwy path: " << orcaobj::toString(data) << endl << reason;
        context_.tracer().warning( ss.str() );
        throw orca::MalformedParametersException( reason );
    }

    if ( isDodgy( data, reason ) )
    {
        stringstream ss;
        ss << "Received dodgy path: " << orcaobj::toVerboseString(data) << endl << reason;
        context_.tracer().warning( ss.str() );
    }

    cout<<"TRACE(pathfollower2dI.cpp): Received new path: " << orcaobj::toVerboseString(data) << endl;
    cout<<"TRACE(pathfollower2dI.cpp): activateImmediately: " << activateImmediately << endl;
    pathStore_.set( data );
    newPathArrivedStore_.set( true );
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
    activationTimeStore_.set( now );
    activationArrivedStore_.set( true );

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
        context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm." );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer().print( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer().print( "PathFollower2dI: Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setActivationTime( now,
                                             timeSinceActivate(now) );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer().print( "PathFollower2dI: Re-connection to IceStorm failed." );
        }
    }
    catch ( ... )
    {
        context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
    }
}

int
PathFollower2dI::getWaypointIndex( const ::Ice::Current& ) const
{
    int ret;
    wpIndexStore_.get( ret );
    return ret;
}

bool
PathFollower2dI::getAbsoluteActivationTime(::orca::Time &time, const Ice::Current&) const
{
    int wpIndex;
    wpIndexStore_.get( wpIndex );
    if ( wpIndex != -1 )
    {
        activationTimeStore_.get( time );
        return true;
    }
    else
        return false;
}
    
bool
PathFollower2dI::getRelativeActivationTime(double &secondsSinceActivation, const Ice::Current&) const
{
    int wpIndex;
    wpIndexStore_.get( wpIndex );
    if ( wpIndex != -1 )
    {
        orca::Time timeActivated;
        activationTimeStore_.get( timeActivated );
        secondsSinceActivation = orcaice::timeDiffAsDouble( clock_.time(), timeActivated );
        return true;
    }
    else
        return false;
}

void 
PathFollower2dI::setEnabled( bool enabled, const ::Ice::Current& )
{
    enabledStore_.set( enabled );
    // let the consumers know that the enabled state changed
    consumerPrx_->setEnabledState( enabled );
}
bool 
PathFollower2dI::enabled(const ::Ice::Current&) const
{
    bool enabled;
    enabledStore_.get( enabled );
    return enabled;
}

void
PathFollower2dI::subscribe( const ::orca::PathFollower2dConsumerPrx& subscriber, const ::Ice::Current& )
{
    cout<<"subscribe()"<<endl;
    assert ( topicPrx_ != 0 );
    cout<<"TRACE(pathfollower2dI.cpp): topicPrx_: " << topicPrx_->ice_toString() << endl;
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway());
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer().debug( ss.str(), 2 );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e;
        context_.tracer().warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
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
    wpIndexStore_.set( index );
    // Try to push to IceStorm.

    try {
        consumerPrx_->setWaypointIndex( index );
    }
    catch ( Ice::Exception &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm." );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer().print( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer().print( "PathFollower2dI: Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setWaypointIndex( index );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer().print( "PathFollower2dI: Re-connection to IceStorm failed." );
        }
    }
    catch ( ... )
    {
        context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
    }
}

// void
// PathFollower2dI::localSetActivationTime( const orca::Time &activationTime )
// {
// //     cout<<"TRACE(pathfollower2dI.cpp): activationTime: " << orcaice::toString(activationTime) << endl;

// //     activationTimeStore_.set( activationTime );
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
//         context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm." );

//         // If IceStorm just re-started for some reason though, we want to try to re-connect
//         try
//         {
//             context_.tracer().print( "Re-connecting to IceStorm..." );
//             topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
//                 ( context_, consumerPrx_, ifaceTag_ );
//             context_.tracer().print( "PathFollower2dI: Re-connected to IceStorm." );

//             // try again to push that bit of info
//             consumerPrx_->setActivationTime( activationTime, 
//                                              orcaice::timeDiffAsDouble(orcaice::getNow(),activationTime) );
//         }
//         catch ( ... )
//         {
//             // ignore it -- we'll try again next push.
//             context_.tracer().print( "PathFollower2dI: Re-connection to IceStorm failed." );
//         }
//     }
//     catch ( ... )
//     {
//         context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
//     }
// }

void 
PathFollower2dI::localSetData( const orca::PathFollower2dData &path )
{
    pathStore_.set( path );
    try {
        consumerPrx_->setData( path );
    }
    catch ( Ice::Exception &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down, 
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm." );

        // If IceStorm just re-started for some reason though, we want to try to re-connect
        try
        {
            context_.tracer().print( "Re-connecting to IceStorm..." );
            topicPrx_ = orcaice::connectToTopicWithTag<orca::PathFollower2dConsumerPrx>
                ( context_, consumerPrx_, ifaceTag_ );
            context_.tracer().print( "PathFollower2dI: Re-connected to IceStorm." );

            // try again to push that bit of info
            consumerPrx_->setData( path );
        }
        catch ( ... )
        {
            // ignore it -- we'll try again next push.
            context_.tracer().print( "PathFollower2dI: Re-connection to IceStorm failed." );
        }
    }
    catch ( ... )
    {
        context_.tracer().warning( "PathFollower2dI: Failed push to IceStorm: unknown exception" );
    }
}

bool
PathFollower2dI::localIsEnabled() const
{
    bool enabled;
    enabledStore_.get( enabled );
    return enabled;
}

}
